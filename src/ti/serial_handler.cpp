/* Copyright © EagleSat II - Embry Riddle Aeronautical University - All rights reserved - 2022 */
//
// Created by Hayden Roszell on 3/17/22.
//

#include <chrono>
#include <thread>
#include <iostream>
#include <dirent.h>
#include <cstring>
#include <vector>
#include <termios.h>
#include <boost/asio.hpp>
#include <sys/ioctl.h>
#include "serial_handler.h"
#include "helper.h"

using namespace boost::asio;
using namespace estts;
using namespace std::this_thread; // sleep_for, sleep_until
using namespace std::chrono; // nanoseconds, system_clock, seconds

/**
* @brief Base constructor that initializes port and baud, opens specified port
* as serial port, and configures it using Termios.
* @return None
*/
serial_handler::serial_handler() : io(), serial(io) {
#ifndef __ESTTS_OS_LINUX__
    this->port = estts::ti_serial::TI_SERIAL_ADDRESS;
#else
    while (find_serial_port() != ES_OK) {
        SPDLOG_WARN("Couldn't find serial device to attach to.");
        sleep_until(system_clock::now() + seconds(1));
    }
#endif
    this->baud = 115200;
    failures = 0;
    delimiter_timeout_ms = 400;

    sync_buf = new unsigned char[MAX_SERIAL_READ];

    SPDLOG_DEBUG("Opening serial port {} with {} baud", port, baud);

    if (ES_OK != initialize_serial_port()) {
        SPDLOG_WARN("Failed to initialize serial port {}", port);
        sleep_until(system_clock::now() + seconds(1));
    }
}

/**
 * @brief Initializes serial terminal port using Termios and Boost
 * @return #ES_OK if port configures successfully, or #ES_UNSUCCESSFUL if not
 */
estts::Status serial_handler::initialize_serial_port() {
    boost::system::error_code error;

    if (serial.is_open()) {
        SPDLOG_DEBUG("initialize_serial_port: serial port was open; closing before init");
        serial.cancel();
        serial.close();
    }

    serial.open(port, error);
    if (error) {
        SPDLOG_ERROR("Failed to open serial port {} - {}", port, error.message());
        return ES_UNSUCCESSFUL;
    }
    SPDLOG_INFO("Opened serial port");

    struct termios tty{};
    if (tcgetattr(serial.lowest_layer().native_handle(), &tty) != 0) {
        SPDLOG_ERROR("Error %i from tcgetattr: %s\n", errno, strerror(errno));
        SPDLOG_INFO("Did you mean to use TI Dev Mode? See README.md");
        return ES_UNSUCCESSFUL;
    }

    // Initialize Terminos structure
    tty.c_lflag &= ~ICANON;  // Disable UNIX Canonical mode (\n != terminator)
    tty.c_lflag &= ~ECHO;    // Disable echo
    tty.c_lflag &= ~ECHOE;   // Disable erasure
    tty.c_lflag &= ~ECHONL;  // Disable new-line echo
    tty.c_lflag &= ~ISIG;    // Disable interpretation of INTR, QUIT and SUSP
    // tty.c_iflag &= ~(IXON | IXOFF | IXANY); // Disable software flow control
    // tty.c_iflag &= ~(IGNBRK | BRKINT | PARMRK | ISTRIP | INLCR | IGNCR | ICRNL); // Disable any special handling of received bytes

    // Save tty settings, also check for error
    if (tcsetattr(serial.lowest_layer().native_handle(), TCSANOW, &tty) != 0) {
        SPDLOG_ERROR("Error {} from tcsetattr: {}", errno, strerror(errno));
        return ES_UNSUCCESSFUL;
    }

    try {
        serial.set_option(boost::asio::serial_port_base::baud_rate(baud));
        serial.set_option(boost::asio::serial_port_base::character_size(8));
        serial.set_option(boost::asio::serial_port_base::flow_control(boost::asio::serial_port_base::flow_control::none));
        serial.set_option(boost::asio::serial_port_base::parity(boost::asio::serial_port_base::parity::none));
        serial.set_option(boost::asio::serial_port_base::stop_bits(boost::asio::serial_port_base::stop_bits::one));
    } catch (boost::system::system_error::exception &e) {
        SPDLOG_ERROR("Failed to configure serial port with baud {} - {}", baud, e.what());
        return ES_UNSUCCESSFUL;
    }

    SPDLOG_INFO("Initialized serial port");

    return estts::ES_OK;
}

estts::Status serial_handler::handle_failure() {
    failures++;
    if (failures > 3) {
        SPDLOG_ERROR("Serial handler failure. Re-initializing serial device at address {}", port);
        initialize_serial_port();
        failures = 0;
    }
    sleep_until(system_clock::now() + seconds(1));
    return ES_OK;
}

/**
 * @brief Takes argument for a pointer to an unsigned char
 * and transmits it across the open serial port.
 * @param data Unsigned char * containing bytes to be written
 * @param size Size of data being transmitted
 * @return Returns -1 if write failed, or the number of bytes written if call succeeded
 */
size_t serial_handler::write_serial_uc(unsigned char *data, int size) {
    if (!serial.is_open()) {
        SPDLOG_ERROR("Serial port {} not open", port);
        return -1;
    }
    size_t written = 0;
    boost::system::error_code error;
    // written = serial.write_some(buffer(data,size), error);
    written = write(serial, buffer(data,size), error);
    if (error) {
        SPDLOG_ERROR("Failed to write to serial port - {}", error.message());
        handle_failure();
        return -1;
    }

    tx_trace_timestamp = high_resolution_clock::now();

    SPDLOG_TRACE("{}", get_write_trace_msg(data, written, port));

    last_transmission_byte_count = written;

    return written;
}

/**
 * @brief Reads available data from open serial port
 * @return Returns nullptr if nothing was read, or the data read
 * from serial port if read was successful (and data was available).
 *
 * CRITICAL NOTE: delete MUST be called when done with the value returned. If this
 * is not done, a memory leak will be created. To avoid this issue, use read_serial_s
 */
unsigned char *serial_handler::read_serial_uc() {
    // Clear cache buf
    cache.clear();

    if (!serial.is_open()) {
        SPDLOG_ERROR("Serial port {} not open", port);
        return nullptr;
    }
    size_t n = 0;
    boost::system::error_code error;
    n = serial.read_some(buffer(sync_buf, MAX_SERIAL_READ), error);
    if (error) {
        SPDLOG_ERROR("Failed to read from serial port - {}", error.message());
        handle_failure();
        return nullptr;
    }

    SPDLOG_TRACE("{}",get_read_trace_msg(sync_buf, n, port));

    sync_buf[n] = '\0';
    cache << sync_buf;
    return sync_buf;
}

/**
 * @brief Writes string to open serial port
 * @param data String argument
 * @return Number of bytes transferred across open serial port
 */
estts::Status serial_handler::write_serial_s(const std::string &data) {
    if (!serial.is_open()) {
        SPDLOG_ERROR("Serial port {} not open", port);
        return estts::ES_UNINITIALIZED;
    }
    // Cast string to const unsigned char *, then cast away const to pass
    // to method that writes unsigned char
    auto csc_data = const_cast<unsigned char *>(reinterpret_cast<const unsigned char *>(data.c_str()));
    if (this->write_serial_uc(csc_data, (int) data.length()) < 0) return estts::ES_UNSUCCESSFUL;
    return estts::ES_OK;
}

/**
 * @brief Reads available data from serial port and returns data as string
 * @return Returns translated string of received data
 */
std::string serial_handler::read_serial_s() {
    if (!serial.is_open()) {
        SPDLOG_ERROR("Serial port {} not open", port);
        return "";
    }
    // Read serial data
    auto read = this->read_serial_uc();
    if (read == nullptr) {
        return "";
    }
    // Type cast unsigned char (auto) to a char *
    // Then call std::string constructor
    std::string string_read(reinterpret_cast<char const *>(read));
    return string_read;
}

serial_handler::~serial_handler() {
    serial.close();
}

void serial_handler::clear_serial_fifo() {
    while (check_serial_bytes_avail() > 0)
        read_serial_uc(check_serial_bytes_avail());
}

void serial_handler::clear_serial_fifo(const std::function<estts::Status(std::string)> &cb) {
    cb(read_serial_s(check_serial_bytes_avail()));
}

int serial_handler::check_serial_bytes_avail() {
    int value = -1;
    if (0 != ioctl(serial.lowest_layer().native_handle(), FIONREAD, &value)) {
        SPDLOG_ERROR("Failed to get bytes available - {}",
                     boost::system::error_code(errno, boost::asio::error::get_system_category()).message());
        handle_failure();
    }
    return value;
}

std::function<void(boost::system::error_code, size_t)> serial_handler::get_generic_async_read_lambda(const std::function<Status(char *, size_t)>& estts_callback) {
    return [estts_callback, this] (const boost::system::error_code& error, std::size_t bytes_transferred) {
        if (error) {
            spdlog::error("Async read failed - {}", error.message().c_str());
        }
        std::stringstream temp;
        for (char i : async_buf) {
            if (i != '\r')
                temp << i;
        }
        spdlog::info("Async callback lambda - Got back -> {}", temp.str());

        estts_callback(this->async_buf, bytes_transferred);

        return estts::ES_OK;
    };
}

/**
 * read_to_delimeter reads from serial port 1 byte at a time until delimiter is found. If the serial device
 * stops sending data, the function times out and returns what it collected.
 * @param delimiter Unsigned char delimiter that function will read to
 * @return String containing collected data from serial device
 */
std::string serial_handler::read_to_delimeter(const unsigned char * delimiter, int size) {
    if (!serial.is_open()) {
        SPDLOG_ERROR("Serial port {} not open", port);
        return "";
    }

    // Goal: read to delimiter, but if delimiter_timeout_ms elapses with no delimiter, return

    // Clear cache buf
    cache.clear();

    std::chrono::time_point<std::chrono::high_resolution_clock> last_received_timepoint = high_resolution_clock::now();
    std::stringstream read_buf;
    int consecutive_delimeter_chars_found = 0;
    bool delim_found = false;
    for (;;) {
        if (check_serial_bytes_avail() > 0) {
            if (internal_read_serial_uc(1) == 1) {
                last_received_timepoint = high_resolution_clock::now();
                // TODO this needs a better workaround.
                //if (sync_buf[0] == '\0')
                //    read_buf << "0";
                //else
                    read_buf << sync_buf[0];

                if (sync_buf[0] == delimiter[consecutive_delimeter_chars_found]) {
                    consecutive_delimeter_chars_found++;
                    if (consecutive_delimeter_chars_found == size) {
                        delim_found = true;
                        break;
                    }
                } else {
                    consecutive_delimeter_chars_found = 0;
                }
            }
        }

        if (duration_cast<milliseconds>(high_resolution_clock::now() - last_received_timepoint).count() > delimiter_timeout_ms) {
            SPDLOG_TRACE("Delimiter not found within {} milliseconds.", delimiter_timeout_ms);
            break;
        }
    }

    if (delim_found) {
        auto uc = const_cast<unsigned char *>(reinterpret_cast<const unsigned char *>(read_buf.str().c_str()));
        SPDLOG_TRACE("Delimiter found. {}", get_read_trace_msg(uc, read_buf.str().size(), port));
        return read_buf.str().substr(0, read_buf.str().size() - consecutive_delimeter_chars_found);
    }

    return read_buf.str();
}

/**
 * read_to_delimeter reads from serial port 1 byte at a time until delimiter is found. If the serial device
 * stops sending data, the function times out and returns what it collected.
 * @param delimiter Unsigned char delimiter that function will read to
 * @return String containing collected data from serial device
 */
std::string serial_handler::read_to_delimeter(const unsigned char delimiter) {
    if (!serial.is_open()) {
        SPDLOG_ERROR("Serial port {} not open", port);
        return "";
    }

    // Goal: read to delimiter, but if delimiter_timeout_ms elapses with no delimiter, return

    // Clear cache buf
    cache.clear();

    std::chrono::time_point<std::chrono::high_resolution_clock> last_received_timepoint = high_resolution_clock::now();
    std::stringstream read_buf;
    for (;;) {
        if (check_serial_bytes_avail() > 0) {
            if (internal_read_serial_uc(1) == 1) {
                last_received_timepoint = high_resolution_clock::now();
                read_buf << sync_buf[0];

                if (sync_buf[0] == delimiter)
                    break;
            }
        }

        if (duration_cast<milliseconds>(high_resolution_clock::now() - last_received_timepoint).count() > delimiter_timeout_ms) {
            SPDLOG_TRACE("Delimiter not found within {} milliseconds.", delimiter_timeout_ms);
            break;
        }
    }
    if (!read_buf.str().empty()) {
        auto uc = const_cast<unsigned char *>(reinterpret_cast<const unsigned char *>(read_buf.str().c_str()));
        SPDLOG_TRACE("Delimiter found. {}", get_read_trace_msg(uc, read_buf.str().size(), port));
    }

    return read_buf.str();
}

size_t serial_handler::internal_read_serial_uc(int bytes) {
    boost::system::error_code error;
    auto n = read(serial, buffer(sync_buf, bytes), error);
    if (error) {
        SPDLOG_ERROR("Failed to read from serial port - {}", error.message());
        handle_failure();
    }
    if (n != bytes)
        SPDLOG_WARN("read() didn't return error but failed to read 1 byte");
    return n;
}

unsigned char *serial_handler::read_serial_uc(int bytes) {
    // Clear cache buf
    cache.clear();

    if (bytes < 1)
        return nullptr;

    if (!serial.is_open()) {
        SPDLOG_ERROR("Serial port {} not open", port);
        return nullptr;
    }
    SPDLOG_TRACE("Reading {} bytes from {}", bytes, port);

    auto n = internal_read_serial_uc(bytes);

    for (int i = 0; i < n; i++)
        if (sync_buf[i] == '\0')
            sync_buf[i] = ' ';

    // Add null terminator at the end of transmission for easier processing by parent class(s)
    sync_buf[n] = '\0';

    SPDLOG_TRACE("{}",get_read_trace_msg(sync_buf, n, port));

    cache << sync_buf;
    return sync_buf;
}

std::string serial_handler::read_serial_s(int bytes) {
    if (!serial.is_open()) {
        SPDLOG_ERROR("Serial port {} not open", port);
        return "";
    }
    // Read serial data
    auto read = this->read_serial_uc(bytes);
    if (read == nullptr) {
        return "";
    }
    // Type cast unsigned char (auto) to a char *
    // Then call std::string constructor
    std::string string_read(reinterpret_cast<char const *>(read));

    return string_read;
}

Status serial_handler::find_serial_port() {
    std::string serial_dir = "/dev/serial/by-id";
    std::stringstream temp_sym;
    DIR * d = opendir(serial_dir.c_str()); // open the path
    if (d == nullptr) return estts::ES_UNSUCCESSFUL; // if was not able to open path, return
    struct dirent * dir;
    while ((dir = readdir(d)) != nullptr) {
        std::string temp = dir->d_name;
        if (dir->d_type == DT_LNK && temp.find(estts::ti_serial::SERIAL_DEVICE_SN) != std::string::npos) {
            // Basically if we get here whatever is inside by-id is a symlink pointing to the /dev
            // location we want to connect to. We need to get the absolute path.
            temp_sym << serial_dir << "/" << dir->d_name;
            SPDLOG_INFO("Found serial device mounted at {}", temp_sym.str());
            break;
        }
    }
    if (temp_sym.str().empty()) {
        SPDLOG_ERROR("Didn't find serial port to attach to. Ensure that device is plugged in and mounted.");
        return ES_NOTFOUND;
    }
    this->port = temp_sym.str();
    closedir(d);
    return ES_OK;
}

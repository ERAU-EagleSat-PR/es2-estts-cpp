//
// Created by Hayden Roszell on 3/17/22.
//

#include <termios.h>
#include <boost/asio.hpp>
#include <sys/ioctl.h>
#include "ti_serial_handler.h"

using namespace boost::asio;
using namespace estts;

ti_serial_handler::ti_serial_handler() : io(), serial(io) {
    SPDLOG_DEBUG("Detecting open serial ports");
    port = "";
}

/**
* @brief Base constructor that initializes port and baud, opens specified port
* as serial port, and configures it using Terminos.
* @param port Serial port (EX "/dev/cu.usbmodem")
* @param baud Serial baud rate (EX 115200)
* @return None
*/
ti_serial_handler::ti_serial_handler(const char *port, int baud) : io(), serial(io) {
    SPDLOG_DEBUG("Opening serial port {} with {} baud", port, baud);
    this->port = port;
    try {
        serial.open(port);
    } catch (boost::system::system_error::exception &e) {
        SPDLOG_ERROR("Failed to open serial port {} - {}", port, e.what());
        throw std::runtime_error(e.what());
    }

    if (ES_OK != initialize_serial_port(baud)) {
        SPDLOG_ERROR("Failed to initialize serial port {}", port);
        throw std::runtime_error("Failed to initialize serial port.");
    }

}

/**
 * @brief Initializes serial terminal port using Terminos
 * @return #ES_OK if port configures successfully, or #ES_UNSUCCESSFUL if not
 */
estts::Status ti_serial_handler::initialize_serial_port(int baud) {
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
    tty.c_iflag &= ~(IXON | IXOFF | IXANY); // Disable software flow control
    tty.c_iflag &= ~(IGNBRK | BRKINT | PARMRK | ISTRIP | INLCR | IGNCR | ICRNL); // Disable any special handling of received bytes

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
        throw std::runtime_error(e.what());
    }

    return estts::ES_OK;
}

/**
 * @brief Takes argument for a pointer to an unsigned char
 * and transmits it across the open serial port.
 * @param data Unsigned char * containing bytes to be written
 * @param size Size of data being transmitted
 * @return Returns -1 if write failed, or the number of bytes written if call succeeded
 */
ssize_t ti_serial_handler::write_serial_uc(unsigned char *data, int size) {
    if (!serial.is_open()) {
        SPDLOG_ERROR("Serial port {} not open", port);
        return -1;
    }
    size_t written = 0;
    try {
        written = serial.write_some(buffer(data,size));
    } catch (boost::system::system_error::exception &e) {
        SPDLOG_ERROR("Failed to write to serial port - {}", e.what());
        return -1;
    }
    std::stringstream temp;
    for (int i = 0; i < size; i ++) {
        if (data[i] != '\r')
            temp << data[i];
    }
    SPDLOG_TRACE("Wrote '{}' (size={}) to {}", temp.str(), written, port);

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
unsigned char *ti_serial_handler::read_serial_uc() {
    // Clear cache buf
    cache.clear();

    if (!serial.is_open()) {
        SPDLOG_ERROR("Serial port {} not open", port);
        return nullptr;
    }
    auto buf = new unsigned char[MAX_SERIAL_READ];
    size_t n = 0;
    try {
        n = serial.read_some(buffer(buf, MAX_SERIAL_READ));
    } catch (boost::system::system_error::exception &e) {
        SPDLOG_ERROR("Failed to read from serial port - {}", e.what());
        return nullptr;
    }
    std::stringstream temp;
    for (int i = 0; i < n; i ++) {
        if (buf[i] != '\r')
            temp << buf[i];
    }
    SPDLOG_TRACE("Read '{}' (size={}) from {}", temp.str(), n, port);
    // Add null terminator at the end of transmission for easier processing by parent class(s)
    buf[n] = '\0';
    cache << buf;
    return buf;
}

/**
 * @brief Writes string to open serial port
 * @param data String argument
 * @return Number of bytes transferred across open serial port
 */
estts::Status ti_serial_handler::write_serial_s(const std::string &data) {
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
std::string ti_serial_handler::read_serial_s() {
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
    delete read;
    return string_read;
}

ti_serial_handler::~ti_serial_handler() {
    serial.close();
}

void ti_serial_handler::clear_serial_fifo() {
    SPDLOG_TRACE("Clearing serial FIFO buffer");
    if (check_serial_bytes_avail() > 0)
        read_serial_s();
}

int ti_serial_handler::check_serial_bytes_avail() {
    int value = 0;
    if (0 != ioctl(serial.lowest_layer().native_handle(), FIONREAD, &value)) {
        SPDLOG_ERROR("Failed to get bytes available - {}",
                     boost::system::error_code(errno, boost::asio::error::get_system_category()).message());
    }
    return value;
}

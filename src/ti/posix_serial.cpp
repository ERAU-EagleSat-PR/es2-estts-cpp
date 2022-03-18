//
// Created by Hayden Roszell on 3/17/22.
//

#include "posix_serial.h"

#include <fcntl.h> // Contains file controls like O_RDWR
#include <cerrno> // Error integer and strerror() function
#include <unistd.h> // write(), read(), close()
#include <sys/ioctl.h>
#include <termios.h> // Contains POSIX terminal control definitions
#include <sys/types.h>
#include "posix_serial.h"


using namespace estts;

// Reference https://www.cmrr.umn.edu/~strupp/serial.html

/**
 * @brief Base constructor that initializes port and baud, opens specified port
 * as serial port, and configures it using Terminos.
 * @param port Serial port (EX "/dev/cu.usbmodem")
 * @param baud Serial baud rate (EX 115200)
 * @return None
 */
posix_serial::posix_serial(const char *port, int baud) {
    this->baud = baud;
    this->port = port;
    serial_port = -1; // Serial port initialized in open_port() method
    // Attempt to open serial port

#ifdef __TI_DEV_MODE__
    SPDLOG_DEBUG("Initializing transmission interface in dev mode. No serial port will be opened.");
    serial_port = 0;
#else
    SPDLOG_DEBUG("Opening serial port {} with {} baud", port, baud);
    if (ES_OK != open_port()) {
        SPDLOG_ERROR("Failed to open serial port {}", port);
        throw std::runtime_error("Failed to open serial port.");
    }

    // Attempt to initialize serial port
    if (ES_OK != initialize_serial_port()) {
        SPDLOG_ERROR("Failed to open serial port {}", port);
        throw std::runtime_error("Failed to initialize serial port.");
    }
#endif
}

/**
 * @brief Initializes serial terminal port using Terminos
 * @return #ES_OK if port configures successfully, or #ES_UNSUCCESSFUL if not
 */
estts::Status posix_serial::initialize_serial_port() {
    struct termios tty{};
#ifndef __ESTTS_OS_LINUX__
    if (tcgetattr(serial_port, &tty) != 0) {
        SPDLOG_ERROR("Error %i from tcgetattr: %s\n", errno, strerror(errno));
        SPDLOG_INFO("Did you mean to use TI Dev Mode? See README.md");
        return ES_UNSUCCESSFUL;
    }
    // Initialize Terminos structure
    tty.c_cflag &= ~PARENB;  // Disable parity bit (IE clear parity bit)
    tty.c_cflag &= ~CSTOPB;  // 1 stop bit (IE clear stop field)
    tty.c_cflag &= ~CSIZE;   // Clear size bit
    tty.c_cflag |= CS8;      // 8 data bits
    tty.c_cflag &= ~CRTSCTS; // Disable hardware flow control
    tty.c_lflag &= ~ICANON;  // Disable UNIX Canonical mode (\n != terminator)
    tty.c_lflag &= ~ECHO;    // Disable echo
    tty.c_lflag &= ~ECHOE;   // Disable erasure
    tty.c_lflag &= ~ECHONL;  // Disable new-line echo
    tty.c_lflag &= ~ISIG;    // Disable interpretation of INTR, QUIT and SUSP
    tty.c_iflag &= ~(IXON | IXOFF | IXANY); // Disable software flow control
    tty.c_iflag &= ~(IGNBRK | BRKINT | PARMRK | ISTRIP | INLCR | IGNCR |
                     ICRNL); // Disable any special handling of received bytes

    // Set baud rate
    cfsetispeed(&tty, baud);
    cfsetospeed(&tty, baud);

    // Set non-blocking
    // todo make separate constructor determine if serial should read blocking/nonblocking
    fcntl(serial_port, F_SETFL, FNDELAY);

    // Save tty settings, also check for error
    if (tcsetattr(serial_port, TCSANOW, &tty) != 0) {
        SPDLOG_ERROR("Error {} from tcsetattr: {}", errno, strerror(errno));
        return ES_UNSUCCESSFUL;
    }
#else
        SPDLOG_INFO("Initializing serial handler with compatibility for Linux kernel");
    if (tcgetattr(serial_port, &tty) != 0) {
        spdlog::error("Error %i from tcgetattr: %s\n", errno, strerror(errno));
        SPDLOG_INFO("Did you mean to use TI Dev Mode? See README.md");
        return ES_UNSUCCESSFUL;
    }

    // Initialize Terminos structure
    tty.c_cflag &= ~PARENB;  // Disable parity bit (IE clear parity bit)
    tty.c_cflag &= ~CSTOPB;  // 1 stop bit (IE clear stop field)
    tty.c_cflag &= ~CSIZE;   // Clear size bit
    tty.c_cflag |= CS8;      // 8 data bits
    tty.c_cflag &= ~CRTSCTS; // Disable hardware flow control
    tty.c_cflag |= CREAD | CLOCAL; // Turn on READ and ignore control lines

    tty.c_lflag &= ~ICANON;  // Disable UNIX Canonical mode (\n != terminator)
    tty.c_lflag &= ~ECHO;    // Disable echo
    tty.c_lflag &= ~ECHOE;   // Disable erasure
    tty.c_lflag &= ~ECHONL;  // Disable new-line echo
    tty.c_lflag &= ~ISIG;    // Disable interpretation of INTR, QUIT and SUSP

    tty.c_iflag &= ~(IXON | IXOFF | IXANY); // Disable software flow control
    tty.c_iflag &= ~(IGNBRK | BRKINT | PARMRK | ISTRIP | INLCR | IGNCR |
                     ICRNL); // Disable any special handling of received bytes

    tty.c_oflag &= ~OPOST;
    tty.c_oflag &= ~ONLCR;

    tty.c_cc[VTIME] = 100; // Set timeout of 10.0 seconds
    tty.c_cc[VMIN] = 0;

    // Set baud rate
    cfsetispeed(&tty, B115200);
    cfsetospeed(&tty, B115200);

    cfmakeraw(&tty);

    // Save tty settings, also check for error
    if (tcsetattr(serial_port, TCSANOW, &tty) != 0) {
        spdlog::error("Error {} from tcsetattr: {}", errno, strerror(errno));
        return ES_UNSUCCESSFUL;
    }
    ioctl(serial_port, TIOCEXCL); // Disallow other processes from using this serial port
#endif
    SPDLOG_DEBUG("Successfully initialized port {} with fd {}", port, serial_port);
    return ES_OK;
}

/**
 * Opens the port initialized by constructor using open() system call. Doesn't assign
 * controlling terminal to port
 * @return #ES_OK if port opens successfully, or #ES_UNSUCCESSFUL if not
 */
estts::Status posix_serial::open_port() {
#ifndef __ESTTS_OS_LINUX__
    // Open port stored in object with read/write
    serial_port = open(this->port, O_RDWR | O_NOCTTY | O_NDELAY);
#else
    // Open port stored in object with read/write
    serial_port = open(this->port, O_RDWR);
#endif
    // Check for errors
    if (serial_port < 0) {
        SPDLOG_ERROR("Error {} from open: {}", errno, strerror(errno));
        return ES_UNSUCCESSFUL;
    }
    SPDLOG_DEBUG("Successfully opened port {} with fd {}", port, serial_port);
    return ES_OK;
}

/**
 * @brief Takes argument for a pointer to an unsigned char
 * and transmits it across the open serial port.
 * @param data Unsigned char * containing bytes to be written
 * @param size Size of data being transmitted
 * @return Returns -1 if write failed, or the number of bytes written if call succeeded
 */
ssize_t posix_serial::write_serial_uc(unsigned char *data, int size) const {
    // If serial port isn't open, error out
    if (serial_port < 0) {
        return -1;
    }
    ssize_t written = write(serial_port, data, size);
    if (written < 1) {
        return -1;
    }
    if (data[size] == '\r')
        SPDLOG_TRACE("Wrote '{}' (size={}) to {}", data, written, port);
    else {
        std::stringstream temp;
        for (int i = 0; i < size; i ++) {
            if (data[i] != '\r')
                temp << data[i];
        }
        SPDLOG_TRACE("Wrote '{}' (size={}) to {}", temp.str(), written, port);
    }
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
unsigned char *posix_serial::read_serial_uc() {
    // Clear cache buf
    cache.clear();
    SPDLOG_TRACE("Reading serial - uc");

    // If serial port isn't open, error out
    if (serial_port < 0) {
        SPDLOG_ERROR("Serial port not open! - serial_port = {}", serial_port);
        return nullptr;
    }
    auto buf = new unsigned char[MAX_SERIAL_READ];
    ssize_t n = read(serial_port, buf, MAX_SERIAL_READ);
    if (n < 1) {
        return nullptr;
    }
    if (buf[n] == '\r')
        SPDLOG_TRACE("Read '{}' (size={}) from {}", buf, n, port);
    else {
        std::stringstream temp;
        for (int i = 0; i < n; i ++) {
            if (buf[i] != '\r')
                temp << buf[i];
        }
        SPDLOG_TRACE("Read '{}' (size={}) from {}", temp.str(), n, port);
    }
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
estts::Status posix_serial::write_serial_s(const std::string &data) const {
    // If serial port isn't open, error out
    if (serial_port < 0) return estts::ES_UNINITIALIZED;
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
std::string posix_serial::read_serial_s() {
    SPDLOG_TRACE("Reading serial - s");
    // If serial port isn't open, error out
    if (serial_port < 0) {
        SPDLOG_ERROR("Serial port not open! - serial_port = {}", serial_port);
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

posix_serial::~posix_serial() {
    close(serial_port);
}

void posix_serial::clear_serial_fifo() {
    SPDLOG_TRACE("Clearing serial FIFO buffer");
    if (check_serial_bytes_avail() > 0)
        read_serial_s();
}

int posix_serial::check_serial_bytes_avail() const {
    int bytes;
    ioctl(serial_port, FIONREAD, &bytes);
    return bytes;
}
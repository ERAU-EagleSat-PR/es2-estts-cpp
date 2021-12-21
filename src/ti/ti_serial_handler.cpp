//
// Created by Hayden Roszell on 12/10/21.
//

#include <iostream>
#include "ti_serial_handler.h"
#include "spdlog/spdlog.h"

using namespace estts;

// Reference https://www.cmrr.umn.edu/~strupp/serial.html

/**
 * @brief Base constructor that initializes port and baud, opens specified port
 * as serial port, and configures it using Terminos. NOTE: After calling constructor,
 * program MUST check if successful_init is true.
 * @param port Serial port (EX "/dev/cu.usbmodem")
 * @param baud Serial baud rate (EX 115200)
 * @return None
 */
ti_serial_handler::ti_serial_handler(const char * port, int baud) {
    spdlog::debug("Constructor for TI serial handler called on {}", fmt::ptr(this));
    this->baud = baud;
    this->port = port;
    serial_port = -1; // Serial port initialized in open_port() method
    // Todo - Better exception handling
    // Attempt to open serial port
    if (ES_OK != open_port()) {
        successful_init = false;
        throw std::runtime_error("Failed to open serial port.");
    }

    // Attempt to initialize serial port
    if (ES_OK != initialize_serial_port()) {
        successful_init = false;
        throw std::runtime_error("Failed to initialize serial port.");
    }
    successful_init = true;
}

/**
 * @brief Initializes serial terminal port using Terminos
 * @return #ES_OK if port configures successfully, or #ES_UNSUCCESSFUL if not
 */
estts::Status ti_serial_handler::initialize_serial_port() const {
    struct termios tty{};
    if(tcgetattr(serial_port, &tty) != 0) {
        printf("Error %i from tcgetattr: %s\n", errno, strerror(errno)); // TODO implement logging class
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
    tty.c_iflag &= ~(IGNBRK|BRKINT|PARMRK|ISTRIP|INLCR|IGNCR|ICRNL); // Disable any special handling of received bytes

    // Set baud rate
    cfsetispeed(&tty, baud);
    cfsetospeed(&tty, baud);

    // Set non-blocking
    // todo make separate constructor determine if serial should read blocking/nonblocking
    fcntl(serial_port, F_SETFL, FNDELAY);

    // Save tty settings, also check for error
    if (tcsetattr(serial_port, TCSANOW, &tty) != 0) {
        printf("Error %i from tcsetattr: %s\n", errno, strerror(errno));
        return ES_UNSUCCESSFUL;
    }
    return ES_OK;
}

/**
 * Opens the port initialized by constructor using open() system call. Doesn't assign
 * controlling terminal to port
 * @return #ES_OK if port opens successfully, or #ES_UNSUCCESSFUL if not
 */
estts::Status ti_serial_handler::open_port() {
    // Open port stored in object with read/write
    serial_port = open(this->port, O_RDWR | O_NOCTTY | O_NDELAY);

    // Check for errors
    if (serial_port < 0) {
        // printf("Error %i from open: %s\n", errno, strerror(errno)); // TODO implement logging class
        spdlog::error("Error %i from open: %s", errno, strerror(errno));
        return ES_UNSUCCESSFUL;
    }
    return ES_OK;
}

/**
 * @brief Takes argument for a pointer to an unsigned char
 * and transmits it across the open serial port.
 * @param data Unsigned char * containing bytes to be written
 * @param size Size of data being transmitted
 * @return Returns -1 if write failed, or the number of bytes written if call succeeded
 */
ssize_t ti_serial_handler::write_serial_uc(unsigned char * data, int size) const {
    // If serial port isn't open, error out
    if (serial_port < 0) {
        return -1;
    }
    ssize_t written = write(serial_port, data, size);
    if (written < 1) {
        return -1;
    }
    return written;
}

/**
 * @brief Reads available data from open serial port
 * @return Returns nullptr if nothing was read, or the data read
 * from serial port if read was successful (and data was available).
 *
 * CRITICAL NOTE: delete MUST be called when done with the value returned. If this
 * is not done, a memory leak will be created.
 */
unsigned char * ti_serial_handler::read_serial_uc() const {
    // If serial port isn't open, error out
    if (serial_port < 0) {
        return nullptr;
    }
    auto buf = new unsigned char [MAX_SERIAL_READ];
    ssize_t n = read(serial_port, buf, sizeof(buf));
    if (n < 1) {
        return nullptr;
    }
    // Add null terminator at the end of transmission for easier processing by parent class(s)
    buf[n] = '\0';
    return buf;
}

/**
 * @brief Writes string to open serial port
 * @param data String argument
 * @return Number of bytes transferred across open serial port
 */
ssize_t ti_serial_handler::write_serial_s(const std::string& data) const {
    // If serial port isn't open, error out
    if (serial_port < 0) {
        return -1;
    }
    // Cast string to const unsigned char *, then cast away const to pass
    // to method that writes unsigned char
    auto csc_data = const_cast<unsigned char *>(reinterpret_cast<const unsigned char *>(data.c_str()));
    return this->write_serial_uc(csc_data, (int)data.length());
}

/**
 * @brief Reads available data from serial port and returns data as string
 * @return Returns translated string of received data
 */
std::string ti_serial_handler::read_serial_s() const {
    // If serial port isn't open, error out
    if (serial_port < 0) {
        return "";
    }
    // Read serial data
    auto read = this->read_serial_uc();
    if (read == nullptr) {
        return "";
    }
    // Type cast unsigned char (auto) to a char *
    // Then call std::string constructor
    std::string string_read(reinterpret_cast<char const*>(read));
    delete read;
    return string_read;
}

ti_serial_handler::~ti_serial_handler() {
    close(serial_port);
}

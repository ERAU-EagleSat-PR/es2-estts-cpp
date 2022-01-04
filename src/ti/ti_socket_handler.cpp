//
// Created by Hayden Roszell on 1/3/22.
//

#include <chrono>
#include <thread>
#include "ti_socket_handler.h"
#include <arpa/inet.h>
#include <unistd.h>
#include "spdlog/spdlog.h"

/**
 * @brief Base constructor that initializes address and port, opens specified port
 * as socket, and configures it. NOTE:
 * @param address Server address to connect to (EX "127.0.0.1")
 * @param port Server port to connect to (EX 8080)
 * @return None
 */
ti_socket_handler::ti_socket_handler(const char *address, int port) {
#ifdef __TI_DEV_MODE__
    sock = -1;
    serv_addr = {0};
    this->port = port;
    spdlog::debug("Opening socket at port {} for TI emulation", port);
    this->address = address;
    if (estts::ES_OK != open_socket()) {
        spdlog::error("Failed to open socket.");
        throw std::runtime_error("Failed to open socket.");
    }
    if (estts::ES_OK != configure_socket()) {
        spdlog::error("Failed to configure socket.");
        spdlog::warn("Is the ESTTS server running? See documentation for more.");
        throw std::runtime_error("Failed to configure socket.");
    }
    spdlog::debug("Socket configuration complete.");
#endif
}

estts::Status ti_socket_handler::open_socket() {
    // Creating socket file descriptor
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        spdlog::error("Error {} from socket(): {}", errno, strerror(errno));
        return estts::ES_UNSUCCESSFUL;
    }
    return estts::ES_OK;
}

/**
 * @brief Initializes POSIX client socket using socket.h and in.h. Attempts to complete
 * a bare-bones handshake with open socket.
 * @return #ES_OK if sock configures successfully, or #ES_UNSUCCESSFUL if not
 */
estts::Status ti_socket_handler::configure_socket() {
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(port);

    // Convert IPv4 and IPv6 addresses from text to binary form
    if (inet_pton(AF_INET, address, &serv_addr.sin_addr) <= 0) {
        spdlog::error("Invalid address / Address not supported");
        return estts::ES_UNSUCCESSFUL;
    }

    spdlog::debug("Attempting to connect to socket at address {}:{}", address, port);

    if (connect(sock, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
        spdlog::error("Error {} from connect(): {}", errno, strerror(errno));
        return estts::ES_UNSUCCESSFUL;
    }
    spdlog::trace("Connection succeeded. Attempting handshake.");

    char *hello = "es2-estts-cpp";
    char buffer[estts::ti_socket::TI_SOCKET_BUF_SZ] = {0};

    if (send(sock, hello, strlen(hello), 0) != strlen(hello)) {
        spdlog::error("Failed to transmit to socket");
        return estts::ES_UNSUCCESSFUL;
    }
    auto r = read(sock, buffer, estts::ti_socket::TI_SOCKET_BUF_SZ);
    if (r != strlen(hello)) {
        spdlog::error("Failed to read from socket");
        return estts::ES_UNSUCCESSFUL;
    }
    spdlog::trace("Handshake succeeded - transmitted {}, got back {}", hello, buffer);

    using namespace std::this_thread; // sleep_for, sleep_until
    using namespace std::chrono; // nanoseconds, system_clock, seconds
    sleep_until(system_clock::now() + seconds(2));
    return estts::ES_OK;
}

/**
 * @brief Takes argument for a pointer to an unsigned char
 * and transmits it across the open socket.
 * @param data Unsigned char * containing bytes to be written
 * @param size Size of data being transmitted
 * @return Returns -1 if write failed, or the number of bytes written if call succeeded
 */
ssize_t ti_socket_handler::write_socket_uc(unsigned char *data, int size) const {
    // If serial port isn't open, error out
    if (sock < 0) {
        return -1;
    }
    ssize_t written = send(sock, data, size, 0);
    if (written < 1) {
        return -1;
    }
    spdlog::trace("Wrote '{}' (size={}) to {}", data, written, port);
    return written;
}

/**
 * @brief Reads available data from open socket
 * @return Returns nullptr if nothing was read, or the data read
 * from socket if read was successful (and data was available).
 *
 * CRITICAL NOTE: delete MUST be called when done with the value returned. If this
 * is not done, a memory leak will be created. To avoid this issue, use read_socket_s
 */
unsigned char *ti_socket_handler::read_socket_uc() const {
    // If socket isn't open, error out
    if (sock < 0) {
        return nullptr;
    }
    // Allocate heap space for receive buffer
    auto buf = new unsigned char[estts::ti_socket::TI_SOCKET_BUF_SZ];
    // Use read system call to read data in sock to buf
    auto r = read(sock, buf, estts::ti_socket::TI_SOCKET_BUF_SZ);
    if (r < 1) {
        // Can't receive a negative number of bytes ;)
        return nullptr;
    }
    // Add null terminator at the end of transmission for easier processing by parent class(s)
    buf[r] = '\0';
    spdlog::trace("Read '{}' from {}", buf, port);
    return buf;
}

/**
 * @brief Writes string to open socket
 * @param data String argument
 * @return Number of bytes transferred across open socket
 */
estts::Status ti_socket_handler::write_socket_s(const std::string &data) const {
    // If serial port isn't open, error out
    if (sock < 0) return estts::ES_UNINITIALIZED;
    // Cast string to const unsigned char *, then cast away const to pass
    // to method that writes unsigned char
    auto csc_data = const_cast<unsigned char *>(reinterpret_cast<const unsigned char *>(data.c_str()));
    if (this->write_socket_uc(csc_data, (int) data.length()) < 0) return estts::ES_UNSUCCESSFUL;
    return estts::ES_OK;
}

/**
 * @brief Reads available data from socket and returns data as string
 * @return Returns translated string of received data
 */
std::string ti_socket_handler::read_socket_s() const {
    // If serial port isn't open, error out
    if (sock < 0) {
        return "";
    }
    // Read serial data
    auto read = this->read_socket_uc();
    if (read == nullptr) {
        return "";
    }
    // Type cast unsigned char (auto) to a char *
    // Then call std::string constructor
    std::string string_read(reinterpret_cast<char const *>(read));
    delete read;
    return string_read;
}

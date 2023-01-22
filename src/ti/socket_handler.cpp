/* Copyright © EagleSat II - Embry Riddle Aeronautical University - All rights reserved - 2022 */
//
// Created by Hayden Roszell on 1/3/22.
//

#include <sstream>
#include <chrono>
#include <thread>
#include <fcntl.h>
#include <sys/ioctl.h>
#include "socket_handler.h"
#include <arpa/inet.h>
#include <unistd.h>
#include "helper.h"

using namespace std::this_thread; // sleep_for, sleep_until
using namespace std::chrono; // nanoseconds, system_clock, seconds


/**
 * @brief Base constructor that initializes address and port, opens specified port
 * as socket, and configures it. NOTE:
 * @param address Server address to connect to (EX "127.0.0.1")
 * @param port Server port to connect to (EX 8080)
 * @return None
 */
socket_handler::socket_handler() {
    // Initialize all variables
    sock = -1;
    serv_addr = {0};
    failures = 0;
    address = "";
    port = 0;
    serv_addr = {0};

    sync_buf = new unsigned char[estts::ti_socket::TI_SOCKET_BUF_SZ];
}

estts::Status socket_handler::open_socket() {
    // Creating socket file descriptor
    if (sock > 0) {
        close(sock);
    }
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
estts::Status socket_handler::configure_socket() {
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(port);

    // Convert IPv4 and IPv6 addresses from text to binary form
    if (inet_pton(AF_INET, address, &serv_addr.sin_addr) <= 0) {
        spdlog::error("Invalid address / Address not supported");
        return estts::ES_UNSUCCESSFUL;
    }

    SPDLOG_DEBUG("Attempting to connect to socket at address {}:{}", address, port);

    if (connect(sock, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
        spdlog::error("Error {} from connect(): {}", errno, strerror(errno));
        return estts::ES_UNSUCCESSFUL;
    }
    int flags = fcntl(sock, F_GETFL, 0);
    if (0 != fcntl(sock, F_SETFL, flags & ~O_NONBLOCK))
        return estts::ES_UNSUCCESSFUL;
    SPDLOG_TRACE("Connection succeeded.");

    return estts::ES_OK;
}

estts::Status socket_handler::handle_failure() {
    failures++;
    if (failures > 3) {
        SPDLOG_ERROR("Socket handler failure. Re-initializing socket at address {}:{}", address, port);
        init_socket_handle(address, port);
        failures = 0;
    }
    sleep_until(system_clock::now() + seconds(1));
    return estts::ES_OK;
}

/**
 * @brief Takes argument for a pointer to an unsigned char
 * and transmits it across the open socket.
 * @param data Unsigned char * containing bytes to be written
 * @param size Size of data being transmitted
 * @return Returns -1 if write failed, or the number of bytes written if call succeeded
 */
ssize_t socket_handler::write_socket_uc(unsigned char *data, int size) {
    // If serial port isn't open, error out
    if (sock < 0) {
        return -1;
    }
    ssize_t written = -1;
    try {
        written = send(sock, data, size, 0);
    } catch (std::exception &e) {
        handle_failure();
        return -1;
    }

    if (written < 1) {
        handle_failure();
        return -1;
    }

    SPDLOG_TRACE("{}",get_write_trace_msg(data, written, endpoint));

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
unsigned char *socket_handler::read_socket_uc() {
    // If socket isn't open, error out
    if (sock < 0) {
        return nullptr;
    }
    // Allocate heap space for receive buffer
    // auto buf = new unsigned char[estts::ti_socket::TI_SOCKET_BUF_SZ];

    // Use read system call to read data in sock to buf
    ssize_t n = -1;
    try {
        n = read(sock, sync_buf, estts::ti_socket::TI_SOCKET_BUF_SZ);
    } catch (std::exception &e) {
        handle_failure();
        return nullptr;
    }
    if (n < 1) {
        // Can't receive a negative number of bytes ;)
        handle_failure();
        return nullptr;
    }

    // COSMOS is really bad at prepending zeros... hacky workaround because this isn't worth my time
    for (int i = 0; i < n; i++)
        if (sync_buf[i] == '\0' || sync_buf[i] == '`')
            sync_buf[i] = '0';

    sync_buf[n] = '\0';

    SPDLOG_TRACE("{}",get_read_trace_msg(sync_buf, n, endpoint));

    // Add null terminator at the end of transmission for easier processing by parent class(s)
    sync_buf[n] = '\0';
    return sync_buf;
}

void pad_zeros(unsigned char * buf, ssize_t size) {
    int start_index = 0;
    int end_index = 0;
    bool inversion_required = false;
    // First, record the last non-null index
    for (int i = 0; i < size; i++) {
        if (buf[i] == '\0' && !inversion_required) {
            start_index = i;
            inversion_required = true;
        }
        if (buf[i] != '\0' && inversion_required) {
            end_index = i;
            for (int j = start_index; j <= end_index; j++) {
                buf[(end_index) - j] = buf[j];
            }
            inversion_required = false;
        }
    }
}

/**
 * @brief Writes string to open socket
 * @param data String argument
 * @return Number of bytes transferred across open socket
 */
estts::Status socket_handler::write_socket_s(const std::string &data) {
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
std::string socket_handler::read_socket_s() {
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

    return string_read;
}

int socket_handler::check_sock_bytes_avail() const {
    int count;
    ioctl(sock, FIONREAD, &count);
    return count;
}

estts::Status socket_handler::init_socket_handle(const char *address_, int port_) {
    this->address = address_;
    this->port = port_;
    SPDLOG_DEBUG("Opening socket at {}:{}", address_, port_);
    if (estts::ES_OK != open_socket()) {
        SPDLOG_ERROR("Failed to open socket.");
        return estts::ES_UNINITIALIZED;
    }
    if (estts::ES_OK != configure_socket()) {
        SPDLOG_ERROR("Failed to configure socket.");
        return estts::ES_UNINITIALIZED;
    }
    SPDLOG_DEBUG("Socket configuration complete.");

    std::stringstream temp;
    temp << address << ":" << port;
    this->endpoint = temp.str();

    return estts::ES_OK;
}

socket_handler::~socket_handler() {
    if (sync_buf) delete sync_buf;
}

//
// Created by Hayden Roszell on 1/3/22.
//

#include "ti_socket_handler.h"
#include <arpa/inet.h>
#include <unistd.h>
#include "spdlog/spdlog.h"

ti_socket_handler::ti_socket_handler(const char * address, int port) {
#ifdef __TI_DEV_MODE__
    this->port = port;
    spdlog::debug("Opening socket at port {} for TI emulation", port);
    this->address = address;
    if (estts::ES_OK != open_socket()) {
        spdlog::error("Failed to open socket.");
        throw std::runtime_error("Failed to open socket.");
    }
    if (estts::ES_OK != configure_socket()){
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

estts::Status ti_socket_handler::configure_socket() {
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(port);

    // Convert IPv4 and IPv6 addresses from text to binary form
    if(inet_pton(AF_INET, address, &serv_addr.sin_addr)<=0)
    {
        spdlog::error("Invalid address / Address not supported");
        return estts::ES_UNSUCCESSFUL;
    }

    spdlog::debug("Attempting to connect to socket at address {}:{}", address, port);

    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    {
        spdlog::error("Error {} from connect(): {}", errno, strerror(errno));
        return estts::ES_UNSUCCESSFUL;
    }
    spdlog::trace("Connection succeeded. Attempting handshake.");

    char * hello = "es2-estts-cpp";
    char buffer[estts::ti_socket::TI_SOCKET_BUF_SZ] = {0};

    if (send(sock , hello , strlen(hello) , 0 ) != strlen(hello)) {
        spdlog::error("Failed to transmit to socket");
        return estts::ES_UNSUCCESSFUL;
    }
    auto r = read(sock , buffer, estts::ti_socket::TI_SOCKET_BUF_SZ);
    if (r != strlen(hello)) {
        spdlog::error("Failed to read from socket");
        return estts::ES_UNSUCCESSFUL;
    }
    spdlog::trace("Handshake succeeded - transmitted {}, got back {}", hello, buffer);
    return estts::ES_OK;
}
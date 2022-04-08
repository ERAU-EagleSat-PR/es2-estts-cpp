/* Copyright © EagleSat II - Embry Riddle Aeronautical University - All rights reserved - 2022 */
//
// Created by Hayden Roszell on 1/3/22.
//

#ifndef ESTTS_SOCKET_HANDLER_H
#define ESTTS_SOCKET_HANDLER_H

#include <string>
#include <sys/socket.h>
#include <netinet/in.h>
#include "constants.h"

class socket_handler {
private:
    unsigned char * sync_buf;

    const char *address;

    std::string endpoint;

    struct sockaddr_in serv_addr;

    estts::Status open_socket();

    estts::Status configure_socket();

protected:

    int check_sock_bytes_avail() const;

public:

    int sock, port;

    socket_handler(const char *address, int port);

    ~socket_handler();

    std::string read_socket_s() const;

    unsigned char * read_socket_uc() const;

    estts::Status write_socket_s(const std::string &data) const;

    ssize_t write_socket_uc(unsigned char *data, int size) const;

    estts::Status init_socket_handle();
};


#endif //ESTTS_SOCKET_HANDLER_H

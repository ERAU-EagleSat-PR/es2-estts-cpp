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
    int failures;

    unsigned char * sync_buf;

    const char *address;

    std::string endpoint;

    struct sockaddr_in serv_addr;

    estts::Status open_socket();

    estts::Status configure_socket();

    estts::Status handle_failure();

protected:

    int check_sock_bytes_avail() const;

public:

    int sock, port;

    socket_handler();

    ~socket_handler();

    std::string read_socket_s();

    unsigned char * read_socket_uc();

    estts::Status write_socket_s(const std::string &data);

    ssize_t write_socket_uc(unsigned char *data, int size);

    estts::Status init_socket_handle(const char *address_, int port_);
};


#endif //ESTTS_SOCKET_HANDLER_H

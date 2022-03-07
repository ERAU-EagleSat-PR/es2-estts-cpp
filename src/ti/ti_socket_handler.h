//
// Created by Hayden Roszell on 1/3/22.
//

#ifndef ESTTS_TI_SOCKET_HANDLER_H
#define ESTTS_TI_SOCKET_HANDLER_H

#include <string>
#include <sys/socket.h>
#include <netinet/in.h>
#include "constants.h"

class ti_socket_handler {
private:
    const char *address;

    struct sockaddr_in serv_addr;

    estts::Status open_socket();

    estts::Status configure_socket();

protected:

    int check_sock_bytes_avail() const;

    ssize_t write_socket_uc(unsigned char *data, int size) const;

public:

    int sock, port;

    ti_socket_handler(const char *address, int port);

    std::string read_socket_s() const;

    unsigned char * read_socket_uc() const;

    estts::Status write_socket_s(const std::string &data) const;

    estts::Status init_socket_handle();
};


#endif //ESTTS_TI_SOCKET_HANDLER_H

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
    int sock, port;
    struct sockaddr_in serv_addr;

    estts::Status open_socket();

    estts::Status configure_socket();

protected:

    ssize_t write_socket_uc(unsigned char *data, int size) const;

    unsigned char *read_socket_uc() const;

    estts::Status write_socket_s(const std::string &data) const;

    std::string read_socket_s() const;

    int check_sock_bytes_avail() const;

public:
    ti_socket_handler(const char *address, int port);
};


#endif //ESTTS_TI_SOCKET_HANDLER_H

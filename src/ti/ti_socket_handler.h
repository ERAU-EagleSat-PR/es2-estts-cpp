//
// Created by Hayden Roszell on 1/3/22.
//

#ifndef ESTTS_TI_SOCKET_HANDLER_H
#define ESTTS_TI_SOCKET_HANDLER_H

#include <sys/socket.h>
#include <netinet/in.h>
#include "constants.h"

class ti_socket_handler {
private:
    const char * address;
    int sock, port;
    struct sockaddr_in serv_addr;
    estts::Status open_socket();
    estts::Status configure_socket();

public:
    ti_socket_handler(const char * address, int port);
};


#endif //ESTTS_TI_SOCKET_HANDLER_H

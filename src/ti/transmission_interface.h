//
// Created by Hayden Roszell on 12/21/21.
//

#ifndef ESTTS_TRANSMISSION_INTERFACE_H
#define ESTTS_TRANSMISSION_INTERFACE_H


#include "ti_esttc.h"
#include "ti_socket_handler.h"

class transmission_interface : virtual public ti_esttc, virtual public ti_socket_handler {
private:
    estts::Status initialize_ti();
    estts::Status check_ti_health();
public:
    explicit transmission_interface(const char *address);
    estts::Status transmit(const std::string& value);
    std::string receive();
};


#endif //ESTTS_TRANSMISSION_INTERFACE_H

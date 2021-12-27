//
// Created by Hayden Roszell on 12/21/21.
//

#ifndef ESTTS_TRANSMISSION_INTERFACE_H
#define ESTTS_TRANSMISSION_INTERFACE_H


#include "ti_esttc.h"

class transmission_interface : virtual public ti_esttc {
private:
    estts::Status initialize_ti();
    estts::Status check_ti_health();
public:
    explicit transmission_interface(const char *port);
    estts::Status transmit(const std::string& value);
};


#endif //ESTTS_TRANSMISSION_INTERFACE_H

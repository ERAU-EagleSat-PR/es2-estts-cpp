//
// Created by Hayden Roszell on 12/21/21.
//

#ifndef ESTTS_TRANSMISSION_INTERFACE_H
#define ESTTS_TRANSMISSION_INTERFACE_H


#include "ti_esttc.h"

class transmission_interface {
private:
    ti_esttc esttc_handle;
public:
    transmission_interface();
};


#endif //ESTTS_TRANSMISSION_INTERFACE_H

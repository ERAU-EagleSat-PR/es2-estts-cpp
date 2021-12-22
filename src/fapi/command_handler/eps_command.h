//
// Created by Hayden Roszell on 12/21/21.
//

#ifndef ESTTS_EPS_COMMAND_H
#define ESTTS_EPS_COMMAND_H


#include <constants.h>
#include "fapi_command_handler.h"

namespace eps {
    struct vitals {
        double battery_voltage;
        double brownouts;
        double charge_time_mins;
    };
}

class eps_command : virtual public fapi_command_handler {
private:
    const unsigned char eps_address = estts::ES_EPS;
public:
    eps::vitals get_vitals();
};


#endif //ESTTS_EPS_COMMAND_H

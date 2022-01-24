//
// Created by Hayden Roszell on 12/21/21.
//

#ifndef ESTTS_EPS_COMMAND_H
#define ESTTS_EPS_COMMAND_H


#include <constants.h>
#include "fapi_command_handler.h"
#include "communication_handler.h"

class eps_command : virtual public fapi_command_handler {
public:
    explicit eps_command(transmission_interface *ti);

    estts::Status get_vitals(communication_handler * telem_handle);
};


#endif //ESTTS_EPS_COMMAND_H

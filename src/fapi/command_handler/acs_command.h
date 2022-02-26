//
// Created by Hayden Roszell on 12/21/21.
//

#ifndef ESTTS_ACS_COMMAND_H
#define ESTTS_ACS_COMMAND_H

#include "constants.h"

class acs_command {
public:
    estts::Status get_current_position();
    std::string deploy_magnetometer_boom(const estts::dispatch_fct &dispatch);
    std::string enable_acs(const estts::dispatch_fct &dispatch);
    std::string power_acs(const estts::dispatch_fct &dispatch);
    std::string set_ctrl_mode(const estts::dispatch_fct &dispatch);
    std::string set_est_mode(const estts::dispatch_fct &dispatch);
};


#endif //ESTTS_ACS_COMMAND_H

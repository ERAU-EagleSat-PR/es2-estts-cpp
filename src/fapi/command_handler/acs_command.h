//
// Created by Hayden Roszell on 12/21/21.
//

#ifndef ESTTS_ACS_COMMAND_H
#define ESTTS_ACS_COMMAND_H

#include "constants.h"

class acs_command {
public:
    estts::Status get_current_position();
};


#endif //ESTTS_ACS_COMMAND_H

//
// Created by Hayden Roszell on 12/21/21.
//

#ifndef ESTTS_FAPI_COMMAND_HANDLER_H
#define ESTTS_FAPI_COMMAND_HANDLER_H

#include <functional>
#include <vector>
#include "transmission_interface.h"
#include "constants.h"

class command_handler {
private:
    transmission_interface *ti;

    estts::Status await_response();

    estts::Status map_telemetry_to_dispatched(const std::vector<estts::telemetry_object *> &telem);
protected:
    std::vector<estts::dispatched_command *> dispatched;

    explicit command_handler();

    estts::Status init_command_handler(transmission_interface *ti);

    ~command_handler();

    estts::Status execute(const std::vector<estts::waiting_command *> &commands);
};


#endif //ESTTS_FAPI_COMMAND_HANDLER_H

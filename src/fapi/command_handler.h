//
// Created by Hayden Roszell on 12/21/21.
//

#ifndef ESTTS_FAPI_COMMAND_HANDLER_H
#define ESTTS_FAPI_COMMAND_HANDLER_H

#include <deque>
#include <functional>
#include <vector>
#include "transmission_interface.h"
#include "constants.h"

class command_handler {
private:
    typedef struct {
        std::string serial_number;
        estts::Status response_code;
    } completed;

    transmission_interface *ti;

    estts::Status await_response();

    estts::Status map_telemetry_to_dispatched(const std::vector<estts::telemetry_object *> &telem);
protected:
    std::vector<estts::dispatched_command *> dispatched;

    std::vector<completed *> completed_cache;

    explicit command_handler();

    estts::Status init_command_handler(transmission_interface *ti);

    ~command_handler();

    estts::Status execute(const std::deque<estts::waiting_command *> &commands);

    estts::Status execute(estts::waiting_command * command);
};


#endif //ESTTS_FAPI_COMMAND_HANDLER_H

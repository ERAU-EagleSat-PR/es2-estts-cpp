//
// Created by Hayden Roszell on 12/21/21.
//

#ifndef ESTTS_FAPI_COMMAND_HANDLER_H
#define ESTTS_FAPI_COMMAND_HANDLER_H

#include <vector>
#include "transmission_interface.h"
#include "constants.h"

class fapi_command_handler {
private:
    transmission_interface * ti;
    std::vector<estts::telemetry_object *> telemetry;
protected:
    explicit fapi_command_handler(transmission_interface * ti);
    ~fapi_command_handler();
    static int get_timestamp();
    estts::Status send_command(const std::vector<estts::command_object *>& command);
    estts::Status await_response();
    std::vector<estts::telemetry_object *> get_telemetry_response() {return telemetry;}
};


#endif //ESTTS_FAPI_COMMAND_HANDLER_H

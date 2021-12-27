//
// Created by Hayden Roszell on 12/21/21.
//

#include <vector>
#include "spdlog/spdlog.h"
#include "eps_command.h"
#include "constants.h"

eps::vitals * eps_command::get_vitals() {
    spdlog::info("Getting EagleSat II EPS Vitals");
    std::vector<estts::command_object *> command;
    auto temp = new estts::command_object;
    command.push_back(temp);

    command[0]->address = estts::es2_endpoint::ES_EPS;
    command[0]->commandID = estts::es2_commands::eps::EPS_GET_HEALTH;
    command[0]->method = estts::es2_commands::method::ES_READ;
    command[0]->sequence = 1;
    command[0]->timeStamp = get_timestamp();

    if (this->send_command(command) != estts::ES_OK) {
        return nullptr;
    }

    return new eps::vitals();
}

eps_command::eps_command(transmission_interface * ti) : fapi_command_handler(ti) {

}

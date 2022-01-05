//
// Created by Hayden Roszell on 12/21/21.
//

#include <vector>

#include "eps_command.h"
#include "constants.h"

eps::vitals *eps_command::get_vitals() {
    SPDLOG_INFO("Getting EagleSat II EPS Vitals");
    std::vector<estts::command_object *> command;
    auto temp = new estts::command_object;
    command.push_back(temp);

    command[0]->address = estts::es2_endpoint::ES_EPS;
    command[0]->commandID = estts::es2_commands::eps::EPS_GET_HEALTH;
    command[0]->method = estts::es2_commands::method::ES_READ;
    command[0]->sequence = 01;
    command[0]->timeStamp = 8456;

    if (this->send_command(command) != estts::ES_OK) {
        SPDLOG_WARN("Failed to get vitals.");
        return nullptr;
    }

    auto telem = this->get_telemetry_response();

    // Make sure that the telemetry received was valid
    if (telem.empty()) {
        SPDLOG_WARN("Telemetry response vector empty.");
        return nullptr;
    }

    if (estts::ES_OK != this->validate_response_code(telem[0]->response_code)) {
        SPDLOG_WARN("EPS Get Vitals command returned response code {}", telem[0]->response_code);
        return nullptr;
    }
    SPDLOG_DEBUG("EPS Get Vitals succeeded with response code {}", telem[0]->response_code);

    // todo validate command id, address, etc.

    auto vitals = new eps::vitals;

    vitals->battery_voltage = 8.4;
    vitals->brownouts = 0;
    vitals->charge_time_mins = 24;

    SPDLOG_INFO("EPS Vitals: battery voltage: {} - brownouts: {} - charge time (min) {}", vitals->battery_voltage, vitals->brownouts, vitals->charge_time_mins);

    return vitals;
}

eps_command::eps_command(transmission_interface *ti) : fapi_command_handler(ti) {

}

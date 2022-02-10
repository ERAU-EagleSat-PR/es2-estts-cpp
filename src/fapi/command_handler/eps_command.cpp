//
// Created by Hayden Roszell on 12/21/21.
//

#include <vector>
#include "eps_command.h"

std::string eps_command::get_eps_vitals(const estts::dispatch_fct& dispatch, const std::function<estts::Status(estts::es2_telemetry::eps::vitals *)>& telem_callback) {
    SPDLOG_INFO("Getting EagleSat II EPS Vitals");
    std::vector<estts::command_object *> command;
    auto temp = new estts::command_object;
    command.push_back(temp);

    command[0]->address = estts::es2_endpoint::ES_EPS;
    command[0]->commandID = estts::es2_commands::eps::EPS_GET_HEALTH;
    command[0]->method = estts::es2_commands::method::ES_READ;
    command[0]->sequence = 01;
    command[0]->timeStamp = 8456;



    // This callback is expected by the dispatcher/command handler, which will filter and construct telemetry objects
    // and pass them to this function. Once this process is complete, this function should know how to decode
    // the telemetry object into the expected structure (in this case, an EPS Vitals structure), and pass the vitals to
    // the callback function passed as an argument. This process keeps logic specific to EPS in the EPS
    // command function, and changes the execution of the function to the command handler, where it should be. However, from
    // the perspective of this EPS get vitals function, the specific time when the callback is called doesn't matter in any way,
    // because now the responsibility of handling the telemetry is pushed to the future when it's actually available.
    auto eps_telem_decomposition_callback = [telem_callback] (const std::vector<estts::telemetry_object *>& telem) -> estts::Status {
        if (telem.empty()) {
            return estts::ES_UNINITIALIZED;
        }
        // TODO do something with the telem vector passed to this function
        auto vitals = new estts::es2_telemetry::eps::vitals;

        vitals->battery_voltage = 9.0;
        vitals->brownouts = 0;
        vitals->charge_time_mins = 24;

        spdlog::info("EPS Vitals: battery voltage: {} - brownouts: {} - charge time (min) {}", vitals->battery_voltage, vitals->brownouts, vitals->charge_time_mins);

        telem_callback(vitals);
    };

    // We're expecting the dispatcher to call the telemetry decomposition function. As seen above, the telemetry
    // decomposition knows how to handle a telemetry object of the EPS vitals type (which is matched by the address/command
    // in the command configuration), and calls the callback function passed in by the calling body. Note that when this function
    // returns, it is expected that the entire telemetry collection and storage process is handled by the callbacks.
    // This means that at no point does this function need to run again, because the context is implied by the lambdas.

    // Note that our callback model allows this function to return with no repercussions, and using the unique command
    // serial number, we can fetch the command status at any time.
    return dispatch(command, eps_telem_decomposition_callback);
}

std::string eps_command::get_eps_voltage(const estts::dispatch_fct &dispatch) {
    std::vector<estts::command_object *> command;
    auto temp = new estts::command_object;

    temp->address = estts::es2_endpoint::ES_EPS;
    temp->commandID = estts::es2_commands::eps::EPS_GET_VOLTAGE;
    temp->method = estts::es2_commands::method::ES_READ;
    temp->sequence = 01;
    temp->timeStamp = 8456;

    SPDLOG_INFO("Attempting to get EPS battery voltage");

    command.push_back(temp);

    auto eps_telem_decomposition_callback = [] (const std::vector<estts::telemetry_object *>& telem) -> estts::Status {
        if (telem.empty()) {
            return estts::ES_UNINITIALIZED;
        }
        spdlog::info("Got back battery voltage - it worked");
        return estts::ES_OK;
    };

    return dispatch(command, eps_telem_decomposition_callback);
}

eps_command::eps_command() = default;
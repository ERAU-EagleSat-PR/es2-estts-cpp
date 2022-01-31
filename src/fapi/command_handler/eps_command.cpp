//
// Created by Hayden Roszell on 12/21/21.
//

#include <vector>
#include "eps_command.h"

estts::Status eps_command::get_eps_vitals(const estts::dispatch_fct& dispatch, const std::function<estts::Status(estts::es2_telemetry::eps::vitals *)>& telem_callback) {
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
    // the telemetry object into the expected structure (in this case, an EPS Vitals structure), and call the callback
    // function passed as an argument with the vitals as argument. This process keeps logic specific to EPS in the EPS
    // command function, and changes the execution of the function to the command handler, where it should be.
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
    auto sn = dispatch(command, eps_telem_decomposition_callback);

    // todo validate command id, address, etc.

    // EVERYTHING below this needs to be in its own function. Pass that new function as a pointer to dispatch(), and have the
    // new function take argument for the callback function. That way, this function returns, and the callback is called
    // when the telemetry comes back. IE, when the callback is called, the deconstructed Vitals object will be uploaded to
    // the database and local state file automatically. Then, the returned serial number can be evaluated to determine
    // if the command executed successfully.

    return estts::ES_OK;
}

eps_command::eps_command() = default;

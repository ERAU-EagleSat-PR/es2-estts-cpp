//
// Created by Hayden Roszell on 1/23/22.
//

#include <functional>
#include "constants.h"
#include "communication_handler.h"

int main() {

    spdlog::set_level(spdlog::level::trace); // This setting is missed in the wiki

    /******************************************************************************************************************/
    auto telemetry = new communication_handler();
    // telemetry->communication_init();
    // Test telemetry handler without using the pipe
    auto eps_telem = new estts::es2_telemetry::eps::vitals;

    eps_telem->battery_voltage = 8.4;
    eps_telem->charge_time_mins = 20;
    eps_telem->brownouts = 1;

    if (estts::ES_OK != telemetry->store_eps_vitals(eps_telem))
        SPDLOG_WARN("Something went wrong here..");

    SPDLOG_INFO("Recent battery voltage: {}", telemetry->get_recent_battery_voltage());
    delete eps_telem;
    delete telemetry;
    /******************************************************************************************************************/
    // test communication handler for its function abstractions
    auto comm_handle = new communication_handler();
    auto sn = comm_handle->get_eps_vitals(comm_handle->dispatch_lambda(), [comm_handle](estts::es2_telemetry::eps::vitals *vitals) -> estts::Status {
        return comm_handle->store_eps_vitals(vitals);
    });
    comm_handle->await_dispatcher();
    delete comm_handle;

    /******************************************************************************************************************/
    // Test communication handler for basic automation abilities
    auto auto_comm = new communication_handler();
    auto_comm->autonomous_communication_init();

    delete auto_comm;

    return 0;
}
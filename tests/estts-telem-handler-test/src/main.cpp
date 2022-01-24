//
// Created by Hayden Roszell on 1/23/22.
//

#include <functional>
#include <thread>
#include "command_dispatcher.h"
#include "eps_command.h"
#include "constants.h"
#include "telemetry_handler.h"

int main() {

    spdlog::set_level(spdlog::level::debug); // This setting is missed in the wiki

    auto schedule = new command_dispatcher();
    auto telemetry = new telemetry_handler();
    auto ti = new transmission_interface();
    auto eps = new eps_command(ti);

    /******************************************************************************************************************/
    // Test telemetry handler without using the pipe
    auto eps_telem = new estts::es2_telemetry::eps::vitals;

    eps_telem->battery_voltage = 8.4;
    eps_telem->charge_time_mins = 20;
    eps_telem->brownouts = 1;

    if (estts::ES_OK != telemetry->store_eps_vitals(eps_telem))
        SPDLOG_WARN("Something went wrong here..");

    SPDLOG_INFO("Recent battery voltage: {}", telemetry->get_recent_battery_voltage());
    delete eps_telem;
    /******************************************************************************************************************/

    /******************************************************************************************************************/
    // Test telemetry handler using command pipe

    auto sn = schedule->schedule_command([eps, telemetry] () -> estts::Status { return eps->get_vitals(telemetry); });

    SPDLOG_INFO("Command status for {}: {}", sn, schedule->get_command_status(sn));

    schedule->await_completion();

    SPDLOG_INFO("Command status for {}: {}", sn, schedule->get_command_status(sn));

    SPDLOG_INFO("Recent battery voltage returned by command with SN {}: {}", sn, telemetry->get_recent_battery_voltage());

    // delete ti;
    // delete eps;
    delete schedule;

    return 0;
}
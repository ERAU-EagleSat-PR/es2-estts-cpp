//
// Created by Hayden Roszell on 12/26/21.
//

#include <functional>
#include <thread>
#include "communication_handler.h"
#include "command_dispatcher.h"
#include "eps_command.h"
#include "constants.h"

using namespace std::this_thread; // sleep_for, sleep_until
using namespace std::chrono; // nanoseconds, system_clock, seconds

estts::Status test() {
    return estts::ES_OK;
}

int main() {

    spdlog::set_level(spdlog::level::debug); // This setting is missed in the wiki

    auto schedule = new command_dispatcher();
    auto telemetry = new communication_handler();
    auto ti = new transmission_interface();
    auto eps = new eps_command(ti);

    auto sn = schedule->schedule_command([eps, telemetry] () -> estts::Status { return eps->get_vitals(telemetry); });

    SPDLOG_INFO("Command status for {}: {}", sn, schedule->get_command_status(sn));

    // Schedule a few more
    sn = schedule->schedule_command([eps, telemetry] () -> estts::Status { return eps->get_vitals(telemetry); });

    SPDLOG_INFO("Command status for {}: {}", sn, schedule->get_command_status(sn));

    sn = schedule->schedule_command([eps, telemetry] () -> estts::Status { return eps->get_vitals(telemetry); });

    SPDLOG_INFO("Command status for {}: {}", sn, schedule->get_command_status(sn));

    sn = schedule->schedule_command([eps, telemetry] () -> estts::Status { return eps->get_vitals(telemetry); });

    SPDLOG_INFO("Command status for {}: {}", sn, schedule->get_command_status(sn));

    // Test that scheduler is thread safe
    if (schedule->schedule_command([eps, telemetry] () -> estts::Status { return eps->get_vitals(telemetry); }).empty()) {
        SPDLOG_ERROR("Failed to schedule command");
    }

    schedule->await_completion();

    SPDLOG_INFO("Command status for {}: {}", sn, schedule->get_command_status(sn));

    delete ti;
    delete eps;
    delete schedule;

    return 0;
}
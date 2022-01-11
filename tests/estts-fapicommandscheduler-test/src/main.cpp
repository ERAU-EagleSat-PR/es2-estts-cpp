//
// Created by Hayden Roszell on 12/26/21.
//

#include <thread>
#include "command_scheduler.h"
#include "eps_command.h"
#include "constants.h"

using namespace std::this_thread; // sleep_for, sleep_until
using namespace std::chrono; // nanoseconds, system_clock, seconds

int main() {

    spdlog::set_level(spdlog::level::trace); // This setting is missed in the wiki

    auto schedule = new command_scheduler();

    auto ti = new transmission_interface();
    auto eps = new eps_command(ti);

    // Bind eps object to method using lambda
    schedule->schedule_command([eps] { eps->get_vitals(); });

    // Schedule a few more
    schedule->schedule_command([eps] { eps->get_vitals(); });

    schedule->schedule_command([eps] { eps->get_vitals(); });

    schedule->schedule_command([eps] { eps->get_vitals(); });

    schedule->await_completion();

    // Test that scheduler is thread safe
    if (estts::ES_OK != schedule->schedule_command([eps] { eps->get_vitals(); })) {
        SPDLOG_ERROR("Failed to schedule command");
    }

    delete ti;
    delete eps;
    delete schedule;

    return 0;
}
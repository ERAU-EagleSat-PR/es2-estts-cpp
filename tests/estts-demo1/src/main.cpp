//
// Created by Hayden Roszell on 10/28/21.
//

#include <functional>
#include <chrono>
#include "constants.h"
#include "communication_handler.h"

using namespace std::this_thread; // sleep_for, sleep_until
using namespace std::chrono; // nanoseconds, system_clock, seconds

int main() {
    spdlog::set_level(spdlog::level::trace); // This setting is missed in the wiki

    /******************************************************************************************************************/

    auto dispatch = new session_manager();
    auto command = new estts::command_object;
    command->address = estts::es2_endpoint::ES_EPS;
    command->commandID = estts::es2_commands::eps::EPS_GET_BATTERY_VOLTAGE;
    command->method = estts::es2_commands::method::ES_READ;
    command->sequence = 01;
    command->timeStamp = 8456;

    sleep_until(system_clock::now() + seconds (5));

    for (int i = 0; i < 10; i++) {
        dispatch->schedule_command(command, nullptr);
    }

    sleep_until(system_clock::now() + seconds (20));

    for (int i = 0; i < 4; i++) {
        dispatch->schedule_command(command, nullptr);
    }

    dispatch->await_completion();

    sleep_until(system_clock::now() + seconds (200));

    /******************************************************************************************************************/


    return 0;
}
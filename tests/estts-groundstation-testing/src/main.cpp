//
// Created by Hayden Roszell on 10/28/21.
//

#include <functional>
#include <chrono>
#include <iostream>
#include "constants.h"
#include "transmission_interface.h"
#include "socket_handler.h"
#include <unistd.h>
#include <iostream>
#include "cosmos_handler.h"

using namespace std;

using namespace std::this_thread; // sleep_for, sleep_until
using namespace std::chrono; // nanoseconds, system_clock, seconds

int main() {
    spdlog::set_level(spdlog::level::trace); // This setting is missed in the wiki
    spdlog::set_pattern("[%T] [thread %t] [%^%l%$] [%@] %v");

    auto ti = new transmission_interface();

    sleep_until(system_clock::now() + seconds (1));

    ti->write_serial_s("ES+R2200\r");

    cout << ti->read_serial_s();

    //ti->request_obc_session();

    //ti->end_obc_session("asdf");

    sleep_until(system_clock::now() + seconds (500));

    return 0;
}
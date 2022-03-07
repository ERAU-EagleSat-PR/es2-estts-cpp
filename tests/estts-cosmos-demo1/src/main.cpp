//
// Created by Hayden Roszell on 10/28/21.
//

#include <functional>
#include <chrono>
#include "constants.h"
#include "communication_handler.h"
#include "transmission_interface.h"
#include "ti_socket_handler.h"
#include <unistd.h>
#include <iostream>
#include "cosmos_handler.h"

using namespace std::this_thread; // sleep_for, sleep_until
using namespace std::chrono; // nanoseconds, system_clock, seconds

int main() {
    spdlog::set_level(spdlog::level::trace); // This setting is missed in the wiki
    /*
    auto ti = new transmission_interface();

    ti->transmit(reinterpret_cast<const unsigned char *>("ES+R220A\r"), 9);

    std::string resp;
    do {}
    while ((resp = ti->receive()).empty());
    SPDLOG_INFO("Got back: {}", resp);

     */


    auto cosmos = new cosmos_handler();
    if (estts::ES_OK != cosmos->cosmos_init())
        return -1;

    sleep_until(system_clock::now() + seconds (500));

    return 0;
}
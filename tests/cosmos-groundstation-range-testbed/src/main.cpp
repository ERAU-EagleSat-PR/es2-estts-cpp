//
// Created by Hayden Roszell on 6/6/22.
//
#include <chrono>
#include <thread>
#include <sstream>
#include "constants.h"
#include "socket_handler.h"

using namespace std;
using namespace std::this_thread; // sleep_for, sleep_until
using namespace std::chrono; // nanoseconds, system_clock, seconds

int main() {
    spdlog::set_level(spdlog::level::trace);

    auto sock = new socket_handler(estts::cosmos::COSMOS_SERVER_ADDR, estts::cosmos::COSMOS_GROUNDSTATION_CMD_TELEM_PORT);
    if (estts::ES_OK != sock->init_socket_handle())
        return -1;
    string base = "ES+W6901";

    for (int i = 0; i < 1000; i++) {
        stringstream tlm;
        tlm << base;
        if (i % 2 == 0)
            tlm << "0";
        else
            tlm << "1";
        if (estts::ES_OK != sock->write_socket_s(tlm.str())) {
            printf("Something went wrong....");
            return -1;
        }
        sleep_until(system_clock::now() + seconds (5));
    }
    return 0;
}
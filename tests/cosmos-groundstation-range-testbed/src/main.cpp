//
// Created by Hayden Roszell on 6/6/22.
//
#include <chrono>
#include <thread>
#include <sstream>
#include "constants.h"
#include "socket_handler.h"
#include "obc_filesystem.h"
#include <iostream>
#include <fstream>
#include "serial_handler.h"

using namespace std;
using namespace std::this_thread; // sleep_for, sleep_until
using namespace std::chrono; // nanoseconds, system_clock, seconds
using namespace estts;

int main() {
    spdlog::set_level(spdlog::level::trace);

    auto sock = new socket_handler(cosmos::COSMOS_SERVER_ADDR, cosmos::COSMOS_PRIMARY_CMD_TELEM_PORT);
    auto telem_sock = new socket_handler(cosmos::COSMOS_SERVER_ADDR, cosmos::COSMOS_PRIMARY_AX25_TELEM_PORT);

    while (sock->init_socket_handle() != ES_OK) {
        SPDLOG_WARN("Socket handler init failed. Retry in 1 second.");
        sleep_until(system_clock::now() + seconds(1));
    }
    while (telem_sock->init_socket_handle() != ES_OK) {
        SPDLOG_WARN("Socket handler init failed. Retry in 1 second.");
        sleep_until(system_clock::now() + seconds(1));
    }

    for (;;) {
        auto temp_string = sock->read_socket_s();
        if (!temp_string.empty()) {
            cout << temp_string << endl;
        }
    }


    return 0;
}
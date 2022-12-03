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
#include "helper.h"
#include "obc_filesystem.h"

using namespace std;
using namespace std::this_thread; // sleep_for, sleep_until
using namespace std::chrono; // nanoseconds, system_clock, seconds
using namespace estts;

int main(int argc, char *argv[]) {
    spdlog::set_level(spdlog::level::trace);

    //publish_file_to_git("telem.csv", "KAJFKSJFLSDJLF");

    for (int i = 1; i < argc; i += 2) {
        // --log-level trace --cosmos-server-addr 172.19.35.150 --working-log-dir /home/parallels/telemetry
        if (strcmp(argv[i], "--log-level") == 0) {
            SPDLOG_INFO("Argument {} is {}", argv[i], argv[i+1]);
        } else if (strcmp(argv[i], "--cosmos-server-addr") == 0) {
            SPDLOG_INFO("Argument {} is {}", argv[i], argv[i+1]);
        } else if (strcmp(argv[i], "--working-log-dir") == 0) {
            SPDLOG_INFO("Argument {} is {}", argv[i], argv[i+1]);
        } else {
            SPDLOG_WARN("Unrecognized program argument: {}", argv[i]);
        }
    }

    return 0;
}
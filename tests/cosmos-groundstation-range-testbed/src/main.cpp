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

int main() {
    spdlog::set_level(spdlog::level::trace);

    publish_file_to_git("telem.csv", "KAJFKSJFLSDJLF");

    return 0;
}
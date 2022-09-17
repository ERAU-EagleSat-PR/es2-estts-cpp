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

int main() {
    spdlog::set_level(spdlog::level::trace);

    auto interface = new serial_handler();

    interface->write_serial_s("ES+R2200\r");

    std::cout << interface->read_to_delimeter('\r') << std::endl;

    return 0;
}
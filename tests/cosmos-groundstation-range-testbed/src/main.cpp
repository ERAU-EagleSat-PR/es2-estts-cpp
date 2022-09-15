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

using namespace std;
using namespace std::this_thread; // sleep_for, sleep_until
using namespace std::chrono; // nanoseconds, system_clock, seconds

int main() {
    spdlog::set_level(spdlog::level::trace);

    std::ofstream file;
    file.open("/opt/poopfuckballshitcum.txt", std::ios::in | std::ios::out | std::ios::app);
    if (file.is_open()) {
        file << "AWESXRCTYGUHIJO:UYGTFR^TYGIJKOIJUTFRD%ESRFTYBUHNIJKMJNIUBYTFRD%RFTYBUHNIJMKOJNITFRD%E$SDE%RFTYBUNJB";
        file.close();
        std::cout << "Wrote" << std::endl;
    } else {
        std::cout << "Not wrote" << std::endl;
    }

    return 0;
}
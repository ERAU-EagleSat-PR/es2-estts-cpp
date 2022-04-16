//
// Created by Cody Park on 2/4/22.
//
#include <iostream>
#include <serial_handler.h>
#include "transmission_interface.h"
using std::cout;
using std::endl;
using std::string;

int main() {
    spdlog::set_level(spdlog::level::trace); // This setting is missed in the wiki
    spdlog::set_pattern("[%T] [thread %t] [%^%l%$] [%@] %v");

    auto ti = new transmission_interface();

    // Test response holders
    string a1 = "";
    string a2 = "";
    string a3 = "";
    string a4 = "";

    // ti->enable_pipe(); SUCCESS


    ti->read_scw(a1, a2, a3, a4);

    cout << "a1 = " << a1
                << ", a2 = " << a2
                << ", a3 = " << a3
                << ", a4 = " << a4
                << endl;


}

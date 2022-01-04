//
// Created by Hayden Roszell on 12/26/21.
//

#include "spdlog/spdlog.h"
#include "eps_command.h"

int main() {
    spdlog::set_level(spdlog::level::trace);
    auto ti = new transmission_interface("127.0.0.1");
    auto eps_test = new eps_command(ti);
    eps_test->get_vitals();
    delete ti;
    delete eps_test;
    return 0;
}
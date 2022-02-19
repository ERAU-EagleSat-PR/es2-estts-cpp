//
// Created by Hayden Roszell on 2/4/22.
//

#include <functional>
#include "constants.h"
#include "communication_handler.h"

int main() {
    spdlog::set_level(spdlog::level::trace); // This setting is missed in the wiki

    /******************************************************************************************************************/
    auto comm_handle = new communication_handler();

    auto sn = comm_handle->get_eps_voltage(comm_handle->dispatch_lambda());

    comm_handle->await_dispatcher();
    delete comm_handle;
}
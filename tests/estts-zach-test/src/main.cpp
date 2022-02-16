

#include <functional>
#include "constants.h"
#include "communication_handler.h"
#include "eps_command.h"

int main() {

    spdlog::set_level(spdlog::level::trace); // This setting is missed in the wiki

    /******************************************************************************************************************/
    // test communication handler for its function abstractions
    auto comm_handle = new communication_handler();

    auto sn = comm_handle->get_eps_batteryCurrent(comm_handle->dispatch_lambda());
    comm_handle->await_dispatcher();
    delete comm_handle;

    /******************************************************************************************************************/



    return 0;
}
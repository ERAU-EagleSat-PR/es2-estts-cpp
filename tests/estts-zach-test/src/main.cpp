

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

    /******************************************************************************************************************/
    // test communication handler for its function abstractions
    auto comm_handle2 = new communication_handler();

    auto sn2 = comm_handle2->get_eps_5Vbus_current(comm_handle2->dispatch_lambda());
    comm_handle2->await_dispatcher();
    delete comm_handle2;

    /******************************************************************************************************************/

    /******************************************************************************************************************/
    // test communication handler for its function abstractions
    auto comm_handle3 = new communication_handler();

    auto sn3 = comm_handle3->get_eps_3Vbus_current(comm_handle3->dispatch_lambda());
    comm_handle3->await_dispatcher();
    delete comm_handle3;

    /******************************************************************************************************************/



    return 0;
}
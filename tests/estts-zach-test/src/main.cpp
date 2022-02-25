

#include <functional>
#include "constants.h"
#include "communication_handler.h"
#include "eps_command.h"

int main() {

    spdlog::set_level(spdlog::level::trace); // This setting is missed in the wiki

    /******************************************************************************************************************/
    // test communication handler for its function abstractions
    // test battery current function
    auto comm_handle = new communication_handler();

    auto sn = comm_handle->get_eps_batteryCurrent(comm_handle->dispatch_lambda());
    comm_handle->await_dispatcher();
    delete comm_handle;

    /******************************************************************************************************************/

    /******************************************************************************************************************/
    // test communication handler for its function abstractions
    // test 5 volt bus current function
    auto comm_handle2 = new communication_handler();

    auto sn2 = comm_handle2->get_eps_5Vbus_current(comm_handle2->dispatch_lambda());
    comm_handle2->await_dispatcher();
    delete comm_handle2;

    /******************************************************************************************************************/

    /******************************************************************************************************************/
    // test communication handler for its function abstractions
    // test 3.3 volt bus current function
    auto comm_handle3 = new communication_handler();

    auto sn3 = comm_handle3->get_eps_3Vbus_current(comm_handle3->dispatch_lambda());
    comm_handle3->await_dispatcher();
    delete comm_handle3;

    /******************************************************************************************************************/

    /******************************************************************************************************************/
    // test communication handler for its function abstractions
    // test temperature sensor 5 function
    auto comm_handle4 = new communication_handler();

    auto sn4 = comm_handle4->get_eps_temp_sensor5(comm_handle4->dispatch_lambda());
    comm_handle4->await_dispatcher();
    delete comm_handle4;

    /******************************************************************************************************************/

    /******************************************************************************************************************/
    // test communication handler for its function abstractions
    // test temperature sensor 6 function
    auto comm_handle5 = new communication_handler();

    auto sn5 = comm_handle5->get_eps_temp_sensor6(comm_handle5->dispatch_lambda());
    comm_handle5->await_dispatcher();
    delete comm_handle5;

    /******************************************************************************************************************/

    /******************************************************************************************************************/
    // test communication handler for its function abstractions
    // test temperature sensor 6 function
    auto comm_handle6 = new communication_handler();

    auto sn6 = comm_handle6->get_eps_temp_sensor7(comm_handle6->dispatch_lambda());
    comm_handle6->await_dispatcher();
    delete comm_handle6;

    /******************************************************************************************************************/

    return 0;
}
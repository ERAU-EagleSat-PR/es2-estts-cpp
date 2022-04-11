

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

    /******************************************************************************************************************/
    // test communication handler for its function abstractions
    // test battery temp sensor 1
    auto comm_handle7 = new communication_handler();

    auto sn7 = comm_handle7->get_eps_battery_temp_sensor1(comm_handle7->dispatch_lambda());
    comm_handle7->await_dispatcher();
    delete comm_handle7;

    /******************************************************************************************************************/

    /******************************************************************************************************************/
    // test communication handler for its function abstractions
    // test battery temp sensor 2
    auto comm_handle8 = new communication_handler();

    auto sn8 = comm_handle8->get_eps_battery_temp_sensor2(comm_handle8->dispatch_lambda());
    comm_handle8->await_dispatcher();
    delete comm_handle8;

    /******************************************************************************************************************/

    /******************************************************************************************************************/
    // test communication handler for its function abstractions
    // test battery temp sensor 3
    auto comm_handle9 = new communication_handler();

    auto sn9 = comm_handle9->get_eps_battery_temp_sensor3(comm_handle9->dispatch_lambda());
    comm_handle9->await_dispatcher();
    delete comm_handle9;

    /******************************************************************************************************************/

    /******************************************************************************************************************/
    // test communication handler for its function abstractions
    // test battery temp sensor 4
    auto comm_handle10 = new communication_handler();

    auto sn10 = comm_handle10->get_eps_battery_temp_sensor4(comm_handle10->dispatch_lambda());
    comm_handle10->await_dispatcher();
    delete comm_handle10;

    /******************************************************************************************************************/

    return 0;
}
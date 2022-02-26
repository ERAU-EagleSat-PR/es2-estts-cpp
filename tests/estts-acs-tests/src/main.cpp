//
// Created by Taylor Hancock on 02/11/2022.
//

#include <functional>
#include "constants.h"
#include "communication_handler.h"

int main() {

    spdlog::set_level(spdlog::level::trace); // This setting is missed in the wiki
    /******************************************************************************************************************/
    // test communication handler for its function abstractions
    auto comm_handle = new communication_handler();
    //comm_handle->deploy_magnetometer_boom(comm_handle->dispatch_lambda());
    //comm_handle->enable_acs(comm_handle->dispatch_lambda());
    //comm_handle->power_acs(comm_handle->dispatch_lambda());
    //comm_handle->set_ctrl_mode(comm_handle->dispatch_lambda());
    //comm_handle->set_est_mode(comm_handle->dispatch_lambda());

    /*
    auto sn = comm_handle->get_eps_vitals(comm_handle->dispatch_lambda(), [comm_handle](estts::es2_telemetry::eps::vitals *vitals) -> estts::Status {
        return comm_handle->store_eps_vitals(vitals);
    });
     */

    comm_handle->await_dispatcher();
    delete comm_handle;

    /******************************************************************************************************************/

    return 0;
}
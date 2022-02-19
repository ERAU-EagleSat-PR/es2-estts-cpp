//
// Created by Hayden Roszell on 1/12/22.
//

#ifndef ESTTS_TELEMETRY_HANDLER_H
#define ESTTS_TELEMETRY_HANDLER_H

#include <functional>
#include <thread>
#include <vector>
#include <nlohmann/json.hpp>
#include "transmission_interface.h"
#include "session_manager.h"
#include "constants.h"
#include "eps_command.h"
#include "acs_command.h"
#include "obc_command.h"
#include "mde_command.h"
#include "crp_command.h"

class communication_handler : virtual public eps_command, acs_command, obc_command, mde_command, crp_command {
private:
    std::vector<std::string> command_serial_number_cache;

    transmission_interface *ti;

    session_manager *dispatch;

    std::thread communication_worker;

    nlohmann::json state;
    std::string filename;

    estts::Status read_current_state();

    estts::Status write_current_state();

    [[noreturn]] void start_communication_automation();

public:
    void await_dispatcher() {
        dispatch->await_completion();
    }

    std::function<std::string(estts::command_object *,std::function<estts::Status(std::vector<estts::telemetry_object *>)>)> dispatch_lambda() {
        return [this](estts::command_object * command, std::function<estts::Status(
                std::vector<estts::telemetry_object *>)> decomp_callback) -> std::string {
            return this->dispatch->schedule_command(command, decomp_callback);
        };
    }

    communication_handler();

    ~communication_handler();

    estts::Status autonomous_communication_init();

    estts::Status store_eps_vitals(estts::es2_telemetry::eps::vitals *vitals);

public:
    double get_recent_battery_voltage();
};


#endif //ESTTS_TELEMETRY_HANDLER_H

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
#include "command_dispatcher.h"
#include "constants.h"
#include "eps_command.h"
#include "acs_command.h"
#include "obc_command.h"
#include "mde_command.h"
#include "crp_command.h"

class communication_handler : virtual public eps_command, virtual public acs_command, virtual public obc_command, virtual public mde_command, virtual public crp_command {
private:
    std::vector<std::string> command_serial_number_cache;

    command_dispatcher *dispatch;

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

    std::function<std::string(std::vector<estts::command_object *>,
                              std::function<estts::Status(std::vector<estts::telemetry_object *>)>)> dispatch_lambda() {
        return [this](const std::vector<estts::command_object *> &command, std::function<estts::Status(
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

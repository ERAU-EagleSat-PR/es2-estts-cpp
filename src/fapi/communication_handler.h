//
// Created by Hayden Roszell on 1/12/22.
//

#ifndef ESTTS_TELEMETRY_HANDLER_H
#define ESTTS_TELEMETRY_HANDLER_H

#include <nlohmann/json.hpp>
#include "transmission_interface.h"
#include "command_dispatcher.h"
#include "constants.h"

class communication_handler {
private:
    transmission_interface * ti;
    command_dispatcher * dispatch;

    nlohmann::json state;
    std::string filename;

    estts::Status read_current_state();
    estts::Status write_current_state();
public:
    communication_handler();
    ~communication_handler();

    estts::Status store_eps_vitals(estts::es2_telemetry::eps::vitals * vitals);

public:
    double get_recent_battery_voltage();
};


#endif //ESTTS_TELEMETRY_HANDLER_H

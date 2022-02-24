//
// Created by Hayden Roszell on 12/21/21.
//

#ifndef ESTTS_EPS_COMMAND_H
#define ESTTS_EPS_COMMAND_H

#include <functional>
#include <string>
#include <constants.h>

class eps_command {
public:
    explicit eps_command();

    std::string get_eps_vitals(const estts::dispatch_fct &dispatch, const std::function<estts::Status(
            estts::es2_telemetry::eps::vitals *)> &telem_callback);

    std::string get_eps_voltage(const estts::dispatch_fct &dispatch);

    std::string get_eps_batteryCurrent(const estts::dispatch_fct &dispatch);

    std::string get_eps_5Vbus_current(const estts::dispatch_fct &dispatch);

    std::string get_eps_3Vbus_current(const estts::dispatch_fct &dispatch);
};


#endif //ESTTS_EPS_COMMAND_H


//
// Created by Hayden Roszell on 1/12/22.
//

#include <sys/stat.h>
#include <string>
#include <fstream>
#include "communication_handler.h"

using nlohmann::json;

json telem_template = {
    {"latest_downlink_timestamp", ""},
    {"eps", {
        {"latest_downlink_timestamp", ""},
        {"battery_voltage", 0.0},
        {"brownouts", 0.0},
        {"charge_time_min", 0.0},
    }},
    {"acs", {
        {"latest_downlink_timestamp", ""},
        {"", ""}
    }},
    {"obc", {
        {"latest_downlink_timestamp", ""},
        {"safe_mode_time", 0.0},
        {"current_mode", "nominal"},
        {"schedule_faults", 0}
    }},
    {"crp", {
        {"latest_downlink_timestamp", ""},
        {"running", false},
        {"camera1_brightness", 0},
        {"camera2_brightness", 0},
        {"camera3_brightness", 0},
        {"camera4_brightness", 0},
        {"camera5_brightness", 0},
        {"camera6_brightness", 0},
        {"camera7_brightness", 0},
        {"camera8_brightness", 0}
    }},
    {"mde", {
        {"latest_downlink_timestamp", ""},
        {"running", false}
    }}
};

inline bool test_file_exists(const std::string& name) {
    struct stat buffer{};
    return (stat (name.c_str(), &buffer) == 0);
}

communication_handler::communication_handler() {
    filename = estts::telem_handler::TELEM_HANDLER_STATE_FILE;
    if (!test_file_exists(filename)) {
        std::ofstream o(filename);
        o << std::setw(4) << telem_template << std::endl;
    }
    read_current_state();
}

estts::Status communication_handler::store_eps_vitals(estts::es2_telemetry::eps::vitals * vitals) {
    read_current_state();
    state["eps"]["battery_voltage"] = vitals->battery_voltage;
    state["eps"]["brownouts"] = vitals->brownouts;
    state["eps"]["charge_time_min"] = vitals->charge_time_mins;
    write_current_state();
    return estts::ES_OK;
}

double communication_handler::get_recent_battery_voltage() {

    return state["eps"]["battery_voltage"].get<double>();
}

estts::Status communication_handler::read_current_state() {
    std::ifstream i(filename);
    i >> state;
    return estts::ES_OK;
}

estts::Status communication_handler::write_current_state() {
    std::ofstream o(filename);
    o << std::setw(4) << state << std::endl;
    return estts::ES_OK;
}

communication_handler::~communication_handler() {
    write_current_state();
}

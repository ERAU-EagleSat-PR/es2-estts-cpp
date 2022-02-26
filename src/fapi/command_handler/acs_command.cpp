//
// Created by Hayden Roszell on 12/21/21.
//

#include "acs_command.h"

estts::Status acs_command::get_current_position() {


    return estts::ES_UNINITIALIZED;
}

std::string acs_command::deploy_magnetometer_boom(const estts::dispatch_fct &dispatch) {

    auto temp = new estts::command_object;

    temp->address = estts::es2_endpoint::ES_ACS;
    temp->commandID = estts::es2_commands::acs::ACS_DEP_MAG_BOOM;
    temp->method = estts::es2_commands::method::ES_WRITE;
    temp->sequence = 01;
    temp->timeStamp = 1729;

    SPDLOG_INFO("Attempting to Deploy Magnetometer Boom (in theory)");

    auto acs_dep_magnet_boom_callback = [] (const std::vector<estts::telemetry_object *>& telem) -> estts::Status {
        if (telem.empty()) {
            return estts::ES_UNINITIALIZED;
        }
        spdlog::info("Deployed Magnetometer Boom (hypothetically)!");
        return estts::ES_OK;
    };

    return dispatch(temp, acs_dep_magnet_boom_callback);
}

std::string acs_command::enable_acs(const estts::dispatch_fct &dispatch) {

    auto temp = new estts::command_object;

    temp->address = estts::es2_endpoint::ES_ACS;
    temp->commandID = estts::es2_commands::acs::ACS_ENABLE;
    temp->method = estts::es2_commands::method::ES_WRITE;
    temp->sequence = 01;
    temp->timeStamp = 1729;

    SPDLOG_INFO("Attempting to Enable ACS");


    auto acs_enable_callback = [] (const std::vector<estts::telemetry_object *>& telem) -> estts::Status {
        if (telem.empty()) {
            return estts::ES_UNINITIALIZED;
        }
        spdlog::info("ACS Enabled!");
        return estts::ES_OK;
    };

    return dispatch(temp, acs_enable_callback);
}

std::string acs_command::power_acs(const estts::dispatch_fct &dispatch) {

    auto temp = new estts::command_object;

    temp->address = estts::es2_endpoint::ES_ACS;
    temp->commandID = estts::es2_commands::acs::ACS_POWER;
    temp->method = estts::es2_commands::method::ES_WRITE;
    temp->sequence = 01;
    temp->timeStamp = 1729;

    SPDLOG_INFO("Attempting to Power ACS");

    auto acs_power_callback = [] (const std::vector<estts::telemetry_object *>& telem) -> estts::Status {
        if (telem.empty()) {
            return estts::ES_UNINITIALIZED;
        }
        spdlog::info("ACS Powered!");
        return estts::ES_OK;
    };

    return dispatch(temp, acs_power_callback);
}

std::string acs_command::set_ctrl_mode(const estts::dispatch_fct &dispatch) {

    auto temp = new estts::command_object;

    temp->address = estts::es2_endpoint::ES_ACS;
    temp->commandID = estts::es2_commands::acs::ACS_SET_CTRL_MODE;
    temp->method = estts::es2_commands::method::ES_WRITE;
    temp->sequence = 01;
    temp->timeStamp = 1729;

    SPDLOG_INFO("Attempting to Set Control Mode");

    auto acs_power_callback = [] (const std::vector<estts::telemetry_object *>& telem) -> estts::Status {
        if (telem.empty()) {
            return estts::ES_UNINITIALIZED;
        }
        spdlog::info("Control Mode Set!");
        return estts::ES_OK;
    };

    return dispatch(temp, acs_power_callback);
}

std::string acs_command::set_est_mode(const estts::dispatch_fct &dispatch) {

    auto temp = new estts::command_object;

    temp->address = estts::es2_endpoint::ES_ACS;
    temp->commandID = estts::es2_commands::acs::ACS_SET_EST_MODE;
    temp->method = estts::es2_commands::method::ES_WRITE;
    temp->sequence = 01;
    temp->timeStamp = 1729;

    SPDLOG_INFO("Attempting to Set Estimation Mode");

    auto acs_power_callback = [] (const std::vector<estts::telemetry_object *>& telem) -> estts::Status {
        if (telem.empty()) {
            return estts::ES_UNINITIALIZED;
        }
        spdlog::info("Estimation Mode Set!");
        return estts::ES_OK;
    };

    return dispatch(temp, acs_power_callback);
}

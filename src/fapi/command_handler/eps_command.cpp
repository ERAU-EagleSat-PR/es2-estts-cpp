//
// Created by Hayden Roszell on 12/21/21.
//

#include <vector>
#include "eps_command.h"

std::string eps_command::get_eps_vitals(const estts::dispatch_fct& dispatch, const std::function<estts::Status(estts::es2_telemetry::eps::vitals *)>& telem_callback) {
    SPDLOG_INFO("Getting EagleSat II EPS Vitals");
    auto command = new estts::command_object;

    command->address = estts::es2_endpoint::ES_EPS;
    command->commandID = estts::es2_commands::eps::EPS_GET_HEALTH;
    command->method = estts::es2_commands::method::ES_READ;
    command->sequence = 01;
    command->timeStamp = 8456;



    // This callback is expected by the dispatcher/command handler, which will filter and construct telemetry objects
    // and pass them to this function. Once this process is complete, this function should know how to decode
    // the telemetry object into the expected structure (in this case, an EPS Vitals structure), and pass the vitals to
    // the callback function passed as an argument. This process keeps logic specific to EPS in the EPS
    // command function, and changes the execution of the function to the command handler, where it should be. However, from
    // the perspective of this EPS get vitals function, the specific time when the callback is called doesn't matter in any way,
    // because now the responsibility of handling the telemetry is pushed to the future when it's actually available.
    auto eps_telem_decomposition_callback = [telem_callback] (const std::vector<estts::telemetry_object *>& telem) -> estts::Status {
        if (telem.empty()) {
            return estts::ES_UNINITIALIZED;
        }
        // TODO do something with the telem vector passed to this function
        auto vitals = new estts::es2_telemetry::eps::vitals;

        vitals->battery_voltage = 9.0;
        vitals->brownouts = 0;
        vitals->charge_time_mins = 24;

        spdlog::info("EPS Vitals: battery voltage: {} - brownouts: {} - charge time (min) {}", vitals->battery_voltage, vitals->brownouts, vitals->charge_time_mins);

        telem_callback(vitals);
    };

    // We're expecting the dispatcher to call the telemetry decomposition function. As seen above, the telemetry
    // decomposition knows how to handle a telemetry object of the EPS vitals type (which is matched by the address/command
    // in the command configuration), and calls the callback function passed in by the calling body. Note that when this function
    // returns, it is expected that the entire telemetry collection and storage process is handled by the callbacks.
    // This means that at no point does this function need to run again, because the context is implied by the lambdas.

    // Note that our callback model allows this function to return with no repercussions, and using the unique command
    // serial number, we can fetch the command status at any time.
    return dispatch(command, eps_telem_decomposition_callback);
}

std::string eps_command::get_eps_batteryVoltage(const estts::dispatch_fct &dispatch) {
    auto temp = new estts::command_object;

    temp->address = estts::es2_endpoint::ES_EPS;
    temp->commandID = estts::es2_commands::eps::EPS_GET_BATTERY_VOLTAGE;
    temp->method = estts::es2_commands::method::ES_READ;
    temp->sequence = 01;
    temp->timeStamp = 8456;

    SPDLOG_INFO("Attempting to get EPS battery voltage");

    auto eps_telem_decomposition_callback = [] (const std::vector<estts::telemetry_object *>& telem) -> estts::Status {
        if (telem.empty()) {
            return estts::ES_UNINITIALIZED;
        }
        auto eps_voltage = new estts::es2_telemetry::eps::eps_voltage;

        eps_voltage->battery_voltage = 9.0;

        spdlog::info("EPS battery voltage : {}", eps_voltage->battery_voltage);
        spdlog::info("Got back battery voltage - it worked");
        return estts::ES_OK;
    };

    return dispatch(temp, eps_telem_decomposition_callback);
}


std::string eps_command::get_eps_batteryCurrent(const estts::dispatch_fct &dispatch) {
    auto command = new estts::command_object;

    command->address = estts::es2_endpoint::ES_EPS;
    command->commandID = estts::es2_commands::eps::EPS_GET_BATTERY_CURRENT;
    command->method = estts::es2_commands::method::ES_READ;
    command->sequence = 01;
    command->timeStamp = 8456;

    SPDLOG_INFO("Attempting to get EPS battery current");


    auto eps_telem_decomposition_callback = [] (const std::vector<estts::telemetry_object *>& telem) -> estts::Status {
        if (telem.empty()) {
            return estts::ES_UNINITIALIZED;
        }
        // TODO do something with the telem vector passed to this function
        auto eps_current = new estts::es2_telemetry::eps::eps_current;

        eps_current->battery_current = 9.0;

        spdlog::info("EPS battery current : {}", eps_current->battery_current);

        spdlog::info("Got back battery current - it worked");
    };

    return dispatch(command, eps_telem_decomposition_callback);
}


std::string eps_command::get_eps_5Vbus_current(const estts::dispatch_fct &dispatch) {
    auto temp = new estts::command_object;

    temp->address = estts::es2_endpoint::ES_EPS;
    temp->commandID = estts::es2_commands::eps::EPS_GET_5VBUS_CURRENT;
    temp->method = estts::es2_commands::method::ES_READ;
    temp->sequence = 01;
    temp->timeStamp = 8467;

    SPDLOG_INFO("Attempting to get EPS 5V bus current");




    auto eps_telem_decomposition_callback = [] (const std::vector<estts::telemetry_object *>& telem) -> estts::Status {
        if (telem.empty()) {
            return estts::ES_UNINITIALIZED;
        }
        // TODO do something with the telem vector passed to this function
        auto eps_5Vbus_current = new estts::es2_telemetry::eps::eps_5Vbus_current;

        eps_5Vbus_current->bus_current = 9.0;

        spdlog::info("EPS 5V bus current : {}", eps_5Vbus_current->bus_current);

        spdlog::info("Got back 5V bus current - it worked");
    };

    return dispatch(temp, eps_telem_decomposition_callback);
}


std::string eps_command::get_eps_3Vbus_current(const estts::dispatch_fct &dispatch) {

    auto temp = new estts::command_object;

    temp->address = estts::es2_endpoint::ES_EPS;
    temp->commandID = estts::es2_commands::eps::EPS_GET_3VBUS_CURRENT;
    temp->method = estts::es2_commands::method::ES_READ;
    temp->sequence = 01;
    temp->timeStamp = 8489;

    SPDLOG_INFO("Attempting to get EPS 3.3V bus current");




    auto eps_telem_decomposition_callback = [] (const std::vector<estts::telemetry_object *>& telem) -> estts::Status {
        if (telem.empty()) {
            return estts::ES_UNINITIALIZED;
        }
        // TODO do something with the telem vector passed to this function
        auto eps_3Vbus_current = new estts::es2_telemetry::eps::eps_3Vbus_current;

        eps_3Vbus_current->bus_current = 9.0;

        spdlog::info("EPS 3.3V bus current : {}", eps_3Vbus_current->bus_current);

        spdlog::info("Got back 3.3V bus current - it worked");
    };

    return dispatch(temp, eps_telem_decomposition_callback);
}


std::string eps_command::get_eps_temp_sensor5(const estts::dispatch_fct &dispatch) {

    auto temp = new estts::command_object;

    temp->address = estts::es2_endpoint::ES_EPS;
    temp->commandID = estts::es2_commands::eps::EPS_GET_TEMP_SENSOR5;
    temp->method = estts::es2_commands::method::ES_READ;
    temp->sequence = 01;
    temp->timeStamp = 8330;

    SPDLOG_INFO("Attempting to get EPS external temperature sensor 5");



    auto eps_telem_decomposition_callback = [] (const std::vector<estts::telemetry_object *>& telem) -> estts::Status {
        if (telem.empty()) {
            return estts::ES_UNINITIALIZED;
        }
        auto eps_externalTemp_sensor5 = new estts::es2_telemetry::eps::eps_externalTemp_sensor5;

        eps_externalTemp_sensor5->external_temperature = 1.5;

        spdlog::info("EPS external temperature sensor #5 reads : {}", eps_externalTemp_sensor5->external_temperature);
        spdlog::info("Got back the external temperature sensor - it worked");
        return estts::ES_OK;
    };

    return dispatch(temp, eps_telem_decomposition_callback);
}


std::string eps_command::get_eps_temp_sensor6(const estts::dispatch_fct &dispatch) {

    auto temp = new estts::command_object;

    temp->address = estts::es2_endpoint::ES_EPS;
    temp->commandID = estts::es2_commands::eps::EPS_GET_TEMP_SENSOR6;
    temp->method = estts::es2_commands::method::ES_READ;
    temp->sequence = 01;
    temp->timeStamp = 8987;

    SPDLOG_INFO("Attempting to get EPS external temperature sensor 6");



    auto eps_telem_decomposition_callback = [] (const std::vector<estts::telemetry_object *>& telem) -> estts::Status {
        if (telem.empty()) {
            return estts::ES_UNINITIALIZED;
        }
        auto eps_externalTemp_sensor6 = new estts::es2_telemetry::eps::eps_externalTemp_sensor6;

        eps_externalTemp_sensor6->external_temperature = 1.5;

        spdlog::info("EPS external temperature sensor #6 reads : {}", eps_externalTemp_sensor6->external_temperature);
        spdlog::info("Got back the external temperature sensor - it worked");
        return estts::ES_OK;
    };

    return dispatch(temp, eps_telem_decomposition_callback);
}


std::string eps_command::get_eps_temp_sensor7(const estts::dispatch_fct &dispatch) {

    auto temp = new estts::command_object;

    temp->address = estts::es2_endpoint::ES_EPS;
    temp->commandID = estts::es2_commands::eps::EPS_GET_TEMP_SENSOR7;
    temp->method = estts::es2_commands::method::ES_READ;
    temp->sequence = 01;
    temp->timeStamp = 8987;

    SPDLOG_INFO("Attempting to get EPS external temperature sensor 7");



    auto eps_telem_decomposition_callback = [] (const std::vector<estts::telemetry_object *>& telem) -> estts::Status {
        if (telem.empty()) {
            return estts::ES_UNINITIALIZED;
        }
        auto eps_externalTemp_sensor7 = new estts::es2_telemetry::eps::eps_externalTemp_sensor7;

        eps_externalTemp_sensor7->external_temperature = 1.5;

        spdlog::info("EPS external temperature sensor #7 reads : {}", eps_externalTemp_sensor7->external_temperature);
        spdlog::info("Got back the external temperature sensor - it worked");
        return estts::ES_OK;
    };

    return dispatch(temp, eps_telem_decomposition_callback);
}


std::string eps_command::get_eps_battery_temp_sensor1(const estts::dispatch_fct &dispatch) {

    auto temp = new estts::command_object;

    temp->address = estts::es2_endpoint::ES_EPS;
    temp->commandID = estts::es2_commands::eps::EPS_GET_BATTERY_TEMP_SENSOR1;
    temp->method = estts::es2_commands::method::ES_READ;
    temp->sequence = 01;
    temp->timeStamp = 8278;

    SPDLOG_INFO("Attempting to get EPS battery temp sensor 1");



    auto eps_telem_decomposition_callback = [] (const std::vector<estts::telemetry_object *>& telem) -> estts::Status {
        if (telem.empty()) {
            return estts::ES_UNINITIALIZED;
        }
        auto eps_batteryTemp_sensor1 = new estts::es2_telemetry::eps::eps_batteryTemp_sensor1;

        eps_batteryTemp_sensor1->battery_temperature = 1.5;

        spdlog::info("EPS battery temp sensor 1 reads : {}", eps_batteryTemp_sensor1->battery_temperature);
        spdlog::info("Got back the battery temp sensor - it worked");
        return estts::ES_OK;
    };

    return dispatch(temp, eps_telem_decomposition_callback);
}


std::string eps_command::get_eps_battery_temp_sensor2(const estts::dispatch_fct &dispatch) {

    auto temp = new estts::command_object;

    temp->address = estts::es2_endpoint::ES_EPS;
    temp->commandID = estts::es2_commands::eps::EPS_GET_BATTERY_TEMP_SENSOR2;
    temp->method = estts::es2_commands::method::ES_READ;
    temp->sequence = 01;
    temp->timeStamp = 8672;

    SPDLOG_INFO("Attempting to get EPS battery temp sensor 2");



    auto eps_telem_decomposition_callback = [] (const std::vector<estts::telemetry_object *>& telem) -> estts::Status {
        if (telem.empty()) {
            return estts::ES_UNINITIALIZED;
        }
        auto eps_batteryTemp_sensor2 = new estts::es2_telemetry::eps::eps_batteryTemp_sensor2;

        eps_batteryTemp_sensor2->battery_temperature = 1.5;

        spdlog::info("EPS battery temp sensor 2 reads : {}", eps_batteryTemp_sensor2->battery_temperature);
        spdlog::info("Got back the battery temp sensor - it worked");
        return estts::ES_OK;
    };

    return dispatch(temp, eps_telem_decomposition_callback);
}


std::string eps_command::get_eps_battery_temp_sensor3(const estts::dispatch_fct &dispatch) {

    auto temp = new estts::command_object;

    temp->address = estts::es2_endpoint::ES_EPS;
    temp->commandID = estts::es2_commands::eps::EPS_GET_BATTERY_TEMP_SENSOR3;
    temp->method = estts::es2_commands::method::ES_READ;
    temp->sequence = 01;
    temp->timeStamp = 8782;

    SPDLOG_INFO("Attempting to get EPS battery temp sensor 3");



    auto eps_telem_decomposition_callback = [] (const std::vector<estts::telemetry_object *>& telem) -> estts::Status {
        if (telem.empty()) {
            return estts::ES_UNINITIALIZED;
        }
        auto eps_batteryTemp_sensor3 = new estts::es2_telemetry::eps::eps_batteryTemp_sensor3;

        eps_batteryTemp_sensor3->battery_temperature = 1.5;

        spdlog::info("EPS battery temp sensor 3 reads : {}", eps_batteryTemp_sensor3->battery_temperature);
        spdlog::info("Got back the battery temp sensor - it worked");
        return estts::ES_OK;
    };

    return dispatch(temp, eps_telem_decomposition_callback);
}


std::string eps_command::get_eps_battery_temp_sensor4(const estts::dispatch_fct &dispatch) {

    auto temp = new estts::command_object;

    temp->address = estts::es2_endpoint::ES_EPS;
    temp->commandID = estts::es2_commands::eps::EPS_GET_BATTERY_TEMP_SENSOR4;
    temp->method = estts::es2_commands::method::ES_READ;
    temp->sequence = 01;
    temp->timeStamp = 8674;

    SPDLOG_INFO("Attempting to get EPS battery temp sensor 4");



    auto eps_telem_decomposition_callback = [] (const std::vector<estts::telemetry_object *>& telem) -> estts::Status {
        if (telem.empty()) {
            return estts::ES_UNINITIALIZED;
        }
        auto eps_batteryTemp_sensor4 = new estts::es2_telemetry::eps::eps_batteryTemp_sensor4;

        eps_batteryTemp_sensor4->battery_temperature = 1.5;

        spdlog::info("EPS battery temp sensor 4 reads : {}", eps_batteryTemp_sensor4->battery_temperature);
        spdlog::info("Got back the battery temp sensor - it worked");
        return estts::ES_OK;
    };

    return dispatch(temp, eps_telem_decomposition_callback);
}

eps_command::eps_command() = default;
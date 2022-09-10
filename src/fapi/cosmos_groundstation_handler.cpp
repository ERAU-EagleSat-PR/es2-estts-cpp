/* Copyright © EagleSat II - Embry Riddle Aeronautical University - All rights reserved - 2022 */

#include "sstream"
#include "cosmos_groundstation_handler.h"
#include "socket_handler.h"
#include "constants.h"

using namespace std::this_thread; // sleep_for, sleep_until
using namespace std::chrono; // nanoseconds, system_clock, seconds
using namespace estts;
using namespace estts::endurosat;

std::function<Status(std::string)> get_groundstation_command_callback_lambda(const std::string& command, socket_handler * sock);
std::function<Status(std::string)> get_groundstation_telemetry_callback_lambda(socket_handler * sock);
std::function<Status(std::string)> get_groundstation_session_transmit_func(groundstation_manager * gm);
std::function<std::string()> get_groundstation_session_receive_func(groundstation_manager * gm);
std::function<Status()> get_groundstation_session_start_session_func();
std::function<Status()> get_groundstation_session_end_session_func();

cosmos_groundstation_handler::cosmos_groundstation_handler() {
    gm = nullptr;
    sock = new socket_handler(estts::cosmos::COSMOS_SERVER_ADDR, estts::cosmos::COSMOS_GROUNDSTATION_CMD_TELEM_PORT);
}

void cosmos_groundstation_handler::groundstation_cosmos_worker() {
    auto config = new session_config;
    config->receive_func = get_groundstation_session_receive_func(gm);
    config->end_session_func = get_groundstation_session_end_session_func();
    config->start_session_func = get_groundstation_session_start_session_func();
    config->transmit_func = get_groundstation_session_transmit_func(gm);
    config->priority = 1;
    config->satellite_range_required_for_execution = false;
    config->endpoint = GROUND_STATION;
    auto command_handle = gm->generate_session_manager(config);
    if (command_handle == nullptr)
        throw std::runtime_error("Primary COSMOS worker failed to request a session manager.");

    std::string command;
    for (;;) {
        command = sock->read_socket_s();
        if (not command.empty()) {
            command_handle->schedule_command(command, get_groundstation_command_callback_lambda(command, sock));
        }
    }
}

estts::Status cosmos_groundstation_handler::cosmos_groundstation_init(groundstation_manager * temp_gm) {
    // todo this needs to handle failure
    sock->init_socket_handle();
    this->gm = temp_gm;

    gm->set_groundstation_telemetry_callback(get_groundstation_telemetry_callback_lambda(sock));

    cosmos_worker = std::thread(&cosmos_groundstation_handler::groundstation_cosmos_worker, this);
    SPDLOG_TRACE("Created groundstation COSMOS worker thread with ID {}", std::hash<std::thread::id>{}(cosmos_worker.get_id()));

    return estts::ES_OK;
}

/**
 * Function that returns a function pointer that takes argument for a command received by COSMOS and the local socket
 * handler for dealing with any responses. This function is passed as the command callback to the schedule_command function
 * @param command String command passed by COSMOS
 * @param sock Pointer to socket handler used to handle the command response.
 * @return Function pointer with form std::function<estts::Status(std::string)>
 */
std::function<estts::Status(std::string)> get_groundstation_command_callback_lambda(const std::string& command, socket_handler *sock) {
    return [command, sock] (const std::string& telem) -> estts::Status {
        if (telem.empty() || sock == nullptr) {
            return estts::ES_UNINITIALIZED;
        }
        std::stringstream temp;
        temp << command << telem;
        sock->write_socket_s(temp.str());
        return estts::ES_OK;
    };
}

/**
 * Function that returns a function pointer that takes argument for the local socket
 * handler for dealing with telemetry received by ESTTS.
 * @param sock Pointer to socket handler used to handle the telemetry response.
 * @return Function pointer with form std::function<estts::Status(std::string)>
 */
std::function<estts::Status(std::string)> get_groundstation_telemetry_callback_lambda(socket_handler *sock) {
    return [sock] (const std::string& telem) -> estts::Status {
        if (telem.empty() || sock == nullptr) {
            return estts::ES_UNINITIALIZED;
        }
        sock->write_socket_s(telem);
        return estts::ES_OK;
    };
}

std::function<Status(std::string)> get_groundstation_session_transmit_func(groundstation_manager * gm) {
    return [gm] (std::string value) -> Status {
        // Route
        if (value.rfind("ES+", 0) == 0) {
            value.append("\r");
        }
        return gm->gs_transmit(value);
    };
}

std::function<std::string()> get_groundstation_session_receive_func(groundstation_manager * gm) {
    return [gm] () -> std::string {
        return gm->receive();
    };
}

std::function<Status()> get_groundstation_session_start_session_func() {
    return [] () -> Status { return ES_OK; };
}

std::function<Status()> get_groundstation_session_end_session_func() {
    return [] () -> Status { return ES_OK; };
}

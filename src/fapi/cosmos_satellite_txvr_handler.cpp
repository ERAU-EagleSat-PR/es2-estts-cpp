/* Copyright © EagleSat II - Embry Riddle Aeronautical University - All rights reserved - 2022 */

#include "groundstation_manager.h"
#include "cosmos_satellite_txvr_handler.h"

using namespace estts;
using namespace estts::endurosat;
using namespace std::this_thread; // sleep_for, sleep_until
using namespace std::chrono; // nanoseconds, system_clock, seconds

std::function<Status(std::string)> get_satellite_txvr_command_callback_lambda(const std::string& command, socket_handler * sock);
std::function<Status(std::string)> get_satellite_txvr_telemetry_callback_lambda(socket_handler * sock);
std::function<Status(std::string)> get_satellite_txvr_session_transmit_func(groundstation_manager * gm);
std::function<std::string()> get_satellite_txvr_session_receive_func(groundstation_manager * gm);
std::function<Status()> get_satellite_txvr_session_start_session_func(groundstation_manager * gm);
std::function<Status()> get_satellite_txvr_session_end_session_func(groundstation_manager * gm);

cosmos_satellite_txvr_handler::cosmos_satellite_txvr_handler() {
    sock = new socket_handler(estts::cosmos::COSMOS_SERVER_ADDR, estts::cosmos::COSMOS_SATELLITE_TXVR_CMD_TELEM_PORT);
}

estts::Status cosmos_satellite_txvr_handler::cosmos_satellite_txvr_init(groundstation_manager * gm) {
    this->gm = gm;
    sock->init_socket_handle();

    cosmos_worker = std::thread(&cosmos_satellite_txvr_handler::satellite_txvr_cosmos_worker, this);
    SPDLOG_TRACE("Created satellite transceiver COSMOS worker thread with ID {}", std::hash<std::thread::id>{}(cosmos_worker.get_id()));

    return estts::ES_OK;
}

void cosmos_satellite_txvr_handler::satellite_txvr_cosmos_worker() {
    auto config = new session_config;
    config->receive_func = get_satellite_txvr_session_receive_func(gm);
    config->end_session_func = get_satellite_txvr_session_end_session_func(gm);
    config->start_session_func = get_satellite_txvr_session_start_session_func(gm);
    config->transmit_func = get_satellite_txvr_session_transmit_func(gm);
    config->priority = 2;
    config->satellite_range_required_for_execution = true;
    config->endpoint = EAGLESAT2_TRANSCEIVER;
    auto command_handle = gm->generate_session_manager(config);
    if (command_handle == nullptr)
        throw std::runtime_error("Primary COSMOS worker failed to request a session manager.");

    std::string command;
    for (;;) {
        command = sock->read_socket_s();
        if (not command.empty()) {
            command_handle->schedule_command(command, get_satellite_txvr_command_callback_lambda(command, sock));
        }
    }
}

/**
 * Function that returns a function pointer that takes argument for a command received by COSMOS and the local socket
 * handler for dealing with any responses. This function is passed as the command callback to the schedule_command function
 * @param command String command passed by COSMOS
 * @param sock Pointer to socket handler used to handle the command response.
 * @return Function pointer with form std::function<estts::Status(std::string)>
 */
std::function<estts::Status(std::string)> get_satellite_txvr_command_callback_lambda(const std::string& command, socket_handler *sock) {
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

std::function<estts::Status(std::string)> get_satellite_txvr_telemetry_callback_lambda(socket_handler * socket) {
    return [socket] (const std::string& telem) -> estts::Status {
        if (telem.empty() || socket == nullptr) {
            return estts::ES_UNINITIALIZED;
        }
        socket->write_socket_s(telem);
        return estts::ES_OK;
    };
}

std::function<Status(std::string)> get_satellite_txvr_session_transmit_func(groundstation_manager * gm) {
    return [gm] (std::string value) -> Status {
        int retries = 0;
        // Route
        if (value.rfind("ES+", 0) == 0) {
            value.append("\r");
        }

        // Try to transmit frame
        while (gm->transmit(value) != ES_OK) {
            spdlog::error("Failed to transmit frame. Waiting {} seconds", ESTTS_RETRY_WAIT_SEC);
            sleep_until(system_clock::now() + seconds(ESTTS_RETRY_WAIT_SEC));
            retries++;
            if (retries > MAX_RETRIES) return ES_UNSUCCESSFUL;
            spdlog::info("Retrying transmit ({}/{})", retries, ESTTS_MAX_RETRIES);
        }

        return ES_OK;
    };
}
std::function<std::string()> get_satellite_txvr_session_receive_func(groundstation_manager * gm) {
    return [gm] () -> std::string {
        return gm->receive();
    };
}
std::function<Status()> get_satellite_txvr_session_start_session_func(groundstation_manager * gm) {
    return [gm] () -> Status {
        // Enable PIPE has built-in retries. Don't cascade retries, if this function failed
        // something is pretty messed up.
        if (ES_OK != gm->enable_pipe()) {
            return ES_UNSUCCESSFUL;
        }

        // Sanity check - make sure PIPE is enabled
        if (PIPE_ON != gm->get_pipe_state()) {
            spdlog::error("enable_pipe() succeeded, but trace variable is not set to PIPE_ON");
            return ES_SERVER_ERROR;
        }

        gm->set_session_status(true);

        return ES_OK;
    };
}
std::function<Status()> get_satellite_txvr_session_end_session_func(groundstation_manager * gm) {
    return [gm] () -> Status {
        // Disable PIPE has built-in retries. Don't cascade retries, if this function failed
        // something is pretty messed up.
        if (ES_OK != gm->disable_pipe()) {
            return ES_UNSUCCESSFUL;
        }
        // Sanity check - make sure PIPE is enabled
        if (PIPE_OFF != gm->get_pipe_state()) {
            spdlog::error("enable_pipe() succeeded, but trace variable is not set to PIPE_ON");
            return ES_SERVER_ERROR;
        }

        gm->set_session_status(false);

        return ES_OK;
    };
}

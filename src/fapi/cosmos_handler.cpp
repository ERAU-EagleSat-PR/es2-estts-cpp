/* Copyright © EagleSat II - Embry Riddle Aeronautical University - All rights reserved - 2022 */
//
// Created by Hayden Roszell on 2/24/22.
//

#include <chrono>
#include <thread>
#include <iostream>
#include <utility>
#include <unistd.h>
#include "cosmos_handler.h"

using namespace std::this_thread; // sleep_for, sleep_until
using namespace std::chrono; // nanoseconds, system_clock, seconds
using namespace estts;
using namespace estts::endurosat;

std::function<Status(std::string)> get_primary_command_callback_lambda(const std::string& command, socket_handler * sock);
std::function<Status(std::string)> get_primary_telemetry_callback_lambda(socket_handler * sock);
std::function<Status(std::string)> get_obc_session_transmit_func(groundstation_manager * gm);
std::function<std::string()> get_obc_session_receive_func(groundstation_manager * gm);
std::function<Status()> get_obc_session_start_session_func(groundstation_manager * gm);
std::function<Status()> get_obc_session_end_session_func(groundstation_manager * gm);

cosmos_handler::cosmos_handler() {
    gm = new groundstation_manager();
    this->sock = new socket_handler(cosmos::COSMOS_SERVER_ADDR, cosmos::COSMOS_PRIMARY_CMD_TELEM_PORT);
    this->telem_sock = new socket_handler(cosmos::COSMOS_SERVER_ADDR, cosmos::COSMOS_PRIMARY_AX25_TELEM_PORT);
}

Status cosmos_handler::cosmos_init() {
    while (sock->init_socket_handle() != ES_OK) {
        SPDLOG_WARN("Socket handler init failed. Retry in 1 second.");
        sleep_until(system_clock::now() + seconds(1));
    }
    while (telem_sock->init_socket_handle() != ES_OK) {
        SPDLOG_WARN("Socket handler init failed. Retry in 1 second.");
        sleep_until(system_clock::now() + seconds(1));
    }

    gm->groundstation_manager_init();

    gm->set_connectionless_telem_callback(get_primary_telemetry_callback_lambda(sock));

    cosmos_worker = std::thread(&cosmos_handler::primary_cosmos_worker, this);
    SPDLOG_TRACE("Created primary COSMOS worker thread with ID {}", std::hash<std::thread::id>{}(cosmos_worker.get_id()));

    this->cosmos_satellite_txvr_init(gm);
    this->cosmos_groundstation_init(gm);

    return ES_OK;
}

[[noreturn]] Status cosmos_handler::primary_cosmos_worker() {
    auto config = new session_config;
    config->receive_func = get_obc_session_receive_func(gm);
    config->end_session_func = get_obc_session_end_session_func(gm);
    config->start_session_func = get_obc_session_start_session_func(gm);
    config->transmit_func = get_obc_session_transmit_func(gm);
    config->priority = 3;
    config->satellite_range_required_for_execution = true;
    config->endpoint = EAGLESAT2_OBC;
    auto command_handle = gm->generate_session_manager(config);
    if (command_handle == nullptr)
        throw std::runtime_error("Primary COSMOS worker failed to request a session manager.");

    std::string temp_string;
    for (;;) {
        temp_string = sock->read_socket_s();
        if (!temp_string.empty()) {
            command_handle->schedule_command(temp_string, get_primary_command_callback_lambda(temp_string, sock));
        }
    }
}

/**
 * Function that returns a function pointer that takes argument for a command received by COSMOS and the local socket
 * handler for dealing with any responses. This function is passed as the command callback to the schedule_command function
 * @param command String command passed by COSMOS
 * @param sock Pointer to socket handler used to handle the command response.
 * @return Function pointer with form std::function<Status(std::string)>
 */
std::function<Status(std::string)> get_primary_command_callback_lambda(const std::string& command, socket_handler * sock) {
    return [command, sock] (const std::string& telem) -> Status {
        if (telem.empty() || sock == nullptr) {
            return ES_UNINITIALIZED;
        }
        std::stringstream temp;
        temp << trim_command_arguments(command) << telem;
        sock->write_socket_s(temp.str());
        return ES_OK;
    };
}

std::string trim_command_arguments(std::string command) {
    // ESTTC protocol uses commands of length ES+R1100
    auto temp = std::move(command);

    // If the address is EPS, we want to preserve the argument.
    // IE ES+R180001 => read battery voltage
    if (temp[4] == '1' && temp[5] == '8')
        temp.resize(10);
    else
        temp.resize(8);
    return temp;
}

/**
 * Function that returns a function pointer that takes argument for the local socket
 * handler for dealing with telemetry received by ESTTS.
 * @param sock Pointer to socket handler used to handle the telemetry response.
 * @return Function pointer with form std::function<Status(std::string)>
 */
std::function<Status(std::string)> get_primary_telemetry_callback_lambda(socket_handler * sock) {
    return [sock] (const std::string& telem) -> Status {
        if (telem.empty() || sock == nullptr) {
            return ES_UNINITIALIZED;
        }
        sock->write_socket_s(telem);
        return ES_OK;
    };
}

std::function<Status(std::string)> get_obc_session_transmit_func(groundstation_manager * gm) {
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

std::function<std::string()> get_obc_session_receive_func(groundstation_manager * gm) {
    return [gm] () -> std::string {
        return gm->receive();
    };
}

std::function<Status()> get_obc_session_start_session_func(groundstation_manager * gm) {
    return [gm] () -> Status {

        std::string pipe_en = "ES+W22003323\r";
        int retries = 0;
        std::stringstream buf;

        // COSMOS write command 02 parameter 1 = CONNECTING
        gm->groundstation_telemetry_callback("ES+W69021");

        // Clear FIFO buffer
        gm->flush_transmission_interface();

        // Enable PIPE has built-in retries. Don't cascade retries, if this function failed
        // something is pretty messed up.
        if (ES_OK != gm->enable_pipe()) {
            // COSMOS write command 02 parameter 0 = DISCONNECTED
            gm->groundstation_telemetry_callback("ES+W69020");
            return ES_UNSUCCESSFUL;
        }

        // Sanity check - make sure PIPE is enabled
        if (PIPE_ON != gm->get_pipe_state()) {
            spdlog::error("enable_pipe() succeeded, but trace variable is not set to PIPE_ON");
            // COSMOS write command 02 parameter 0 = DISCONNECTED
            gm->groundstation_telemetry_callback("ES+W69020");
            return ES_SERVER_ERROR;
        }

        // Clear FIFO buffer
        gm->flush_transmission_interface();

        // Now, try to enable PIPE on the satellite.
        while (true) {
            if (retries > MAX_RETRIES) {
                spdlog::error("Failed to enable PIPE on satellite transceiver. ({}/{} retries)", retries, MAX_RETRIES);
                // COSMOS write command 02 parameter 0 = DISCONNECTED
                gm->groundstation_telemetry_callback("ES+W69020");
                return ES_UNSUCCESSFUL;
            }
            gm->write_serial_s(pipe_en);
            sleep_until(system_clock::now() + milliseconds (50));
            buf << gm->internal_receive();
            if (buf.str().find("OK+3323\r") != std::string::npos) {
                spdlog::trace("PIPE is probably enabled on the satellite");
                break;
            }
            retries++;
            spdlog::error("Failed to enable PIPE on satellite. Waiting {} seconds (retry {}/{})", WAIT_TIME_SEC, retries, MAX_RETRIES);
            sleep_until(system_clock::now() + seconds(WAIT_TIME_SEC));
            // Once again don't clear buf, maybe confirmation got lost in the weeds.
        }

        sleep_until(system_clock::now() + milliseconds (800)); // Give it a sec i guess

        // At this point, there is already a thread maintaining the PIPE state.

        // Clear FIFO buffer
        gm->flush_transmission_interface();

        gm->set_session_status(true);

        // COSMOS write command 02 parameter 3 = CONNECTED
        gm->groundstation_telemetry_callback("ES+W69023");

        return ES_OK;
    };
}

std::function<Status()> get_obc_session_end_session_func(groundstation_manager * gm) {
    return [gm] () -> Status {
        // COSMOS write command 02 parameter 2 = DISCONNECTING
        gm->groundstation_telemetry_callback("ES+W69022");
        auto status = gm->disable_pipe();
        if (status == ES_OK) {
            sleep_until(system_clock::now() + seconds(1));
            gm->set_session_status(false);
        }

        // COSMOS write command 02 parameter 0 = DISCONNECTED
        gm->groundstation_telemetry_callback("ES+W69020");
        return status;
    };
}

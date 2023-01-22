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
#include "helper.h"
#include "obc_filesystem.h"

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

std::function<estts::Status(std::string)> get_download_file_modifier(cosmos_handler * ch);

cosmos_handler::cosmos_handler() {
    gm = new groundstation_manager();
    this->sock = new socket_handler();
    this->telem_sock = new socket_handler();
}

Status cosmos_handler::cosmos_init() {
    if (cosmos_server_address.empty()) {
        return Status::ES_UNINITIALIZED;
    }

    while (sock->init_socket_handle(cosmos_server_address.c_str(), cosmos::COSMOS_OBC_CMD_TELEM_PORT) != ES_OK) {
        SPDLOG_WARN("Socket handler init failed. Retry in 1 second.");
        sleep_until(system_clock::now() + seconds(1));
    }
    while (telem_sock->init_socket_handle(cosmos_server_address.c_str(), cosmos::COSMOS_PRIMARY_AX25_TELEM_PORT) != ES_OK) {
        SPDLOG_WARN("Socket handler init failed. Retry in 1 second.");
        sleep_until(system_clock::now() + seconds(1));
    }

    gm->groundstation_manager_init();

    gm->set_connectionless_telem_callback(get_primary_telemetry_callback_lambda(telem_sock));

    config = new session_config;
    config->receive_func = get_obc_session_receive_func(gm);
    config->end_session_func = get_obc_session_end_session_func(gm);
    config->start_session_func = get_obc_session_start_session_func(gm);
    config->transmit_func = get_obc_session_transmit_func(gm);
    config->priority = 3;
    config->satellite_range_required_for_execution = true;
    config->endpoint = EAGLESAT2_OBC;

    cosmos_worker = std::thread(&cosmos_handler::primary_cosmos_worker, this);
    SPDLOG_TRACE("Created primary COSMOS worker thread with ID {}", std::hash<std::thread::id>{}(cosmos_worker.get_id()));

    this->cosmos_satellite_txvr_init(gm, cosmos_server_address);
    this->cosmos_groundstation_init(gm, cosmos_server_address);

    return ES_OK;
}

[[noreturn]] Status cosmos_handler::primary_cosmos_worker() {

    auto command_handle = gm->generate_session_manager(config);
    if (command_handle == nullptr)
        throw std::runtime_error("Primary COSMOS worker failed to request a session manager.");

    auto modifier = new session_manager_modifier;
    modifier->insert_execution_modifier("ES+D1101", get_download_file_modifier(this));
    command_handle->set_session_modifier(modifier);

    std::string temp_string;
    for (;;) {
        temp_string = sock->read_socket_s();
        if (!temp_string.empty()) {
            command_handle->schedule_command(temp_string, get_primary_command_callback_lambda(temp_string, sock), true);
        }
    }
}

std::function<estts::Status(std::string)> get_download_file_modifier(cosmos_handler * ch) {
    return [ch] (const std::string& command) -> estts::Status {
        std::string original = "ES+D1101";

        std::string filename = command;
        filename.erase(0, 8);

        obc_filesystem obc(ch->get_session_config());
        obc.set_base_git_dir(ch->base_git_dir);
        obc.set_git_ssh_url(ch->telem_git_repo_url);
        std::string data = obc.download_file(filename);

        return get_primary_command_callback_lambda(original, ch->get_sh())(data);
    };
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
        sock->write_socket_s(temp.str()); // TODO append delimiter for COSMOS
        return ES_OK;
    };
}

std::string trim_command_arguments(std::string command) {
    // ESTTC protocol uses commands of length len(ES+R1100)
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
        return gm->receive_from_obc();
    };
}

std::function<Status()> get_obc_session_start_session_func(groundstation_manager * gm) {
    return [gm] () -> Status {

        std::string pipe_en = "ES+W22003323\r";
        std::string obc_version_cmd = "ES+R117F\r";
        std::string obc_get_rst_cmd = "ES+R117F\r";
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

        // Sync ground station PIPE expectations with OBC
        spdlog::debug("Syncing ground station PIPE expectations with satellite");
        gm->validate_pipe_duration(PIPE_DURATION_SEC);

        // Now, try to enable PIPE on the satellite
        bool conn = false;
        while (true) {
            // Try to enable PIPE on the satellite
            gm->write_serial_s(pipe_en);

            // Figure out if transceiver registered the request
            buf << gm->internal_receive();
            if (buf.str().find("OK+3323\r") != std::string::npos) {
                conn = true;
                break;
            }

            retries++;

            if (retries < MAX_RETRIES) {
                spdlog::warn("Failed to enable PIPE on satellite. Waiting {} seconds (retry {}/{})", WAIT_TIME_SEC, retries + 1, MAX_RETRIES);
                sleep_until(system_clock::now() + seconds(WAIT_TIME_SEC));
            }
        }

        if (!conn) {
            spdlog::error("Failed to enable PIPE on satellite transceiver.");
            // COSMOS write command 02 parameter 0 = DISCONNECTED
            gm->groundstation_telemetry_callback("ES+W69020");
            return ES_UNSUCCESSFUL;
        }

        std::stringstream buf1;
        gm->set_delimiter_timeout_ms(1000);

        // Now, wait for OBC to be able to receive requests by pinging it
        // for (int ms_elapsed = 0; ms_elapsed < ESTTS_AWAIT_RESPONSE_PERIOD_SEC * 1000; ms_elapsed++) {
        int ms_elapsed = 0;
        while (true) {
            gm->write_serial_s(obc_version_cmd);
            buf1 << gm->read_to_delimeter(OBC_ESTTC_DELIMETER, OBC_ESTTC_DELIMETER_SIZE);
            if (buf1.str().find("OK+") != std::string::npos) {
                auto version = buf1.str().erase(0, 3);
                spdlog::info("Connection established with OBC. Version/boot string: {}", version);
                gm->set_delimiter_timeout_ms(400);
                break;
            }
            sleep_until(system_clock::now() + milliseconds (100));
            ms_elapsed += 100;
            if (ms_elapsed > ESTTS_AWAIT_RESPONSE_PERIOD_SEC * 1000) {
                spdlog::error("Failed to establish connection with OBC - no response within {} seconds.", ESTTS_AWAIT_RESPONSE_PERIOD_SEC);
                // COSMOS write command 02 parameter 0 = DISCONNECTED
                gm->groundstation_telemetry_callback("ES+W69020");
                return ES_UNSUCCESSFUL;
            }
        }

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
            sleep_until(system_clock::now() + seconds(1)); // TODO reduce delay
            gm->set_session_status(false);
        }

        // COSMOS write command 02 parameter 0 = DISCONNECTED
        gm->groundstation_telemetry_callback("ES+W69020");
        return status;
    };
}

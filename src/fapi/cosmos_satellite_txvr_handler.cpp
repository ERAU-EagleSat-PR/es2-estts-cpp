/* Copyright © EagleSat II - Embry Riddle Aeronautical University - All rights reserved - 2022 */

#include "groundstation_manager.h"
#include "cosmos_satellite_txvr_handler.h"
#include "helper.h"

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

std::function<estts::Status(std::string)> get_read_txvr_scw_modifier(cosmos_satellite_txvr_handler * csth);
std::function<estts::Status(std::string)> get_set_txvr_scw_modifier(cosmos_satellite_txvr_handler * csth);

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

    auto modifier = new session_manager_modifier();
    modifier->insert_execution_modifier("ES+R2200", get_read_txvr_scw_modifier(this));
    modifier->insert_execution_modifier("ES+W2200", get_set_txvr_scw_modifier(this));

    command_handle->set_session_modifier(modifier);

    std::string command;
    for (;;) {
        command = sock->read_socket_s();
        if (not command.empty()) {
            command_handle->schedule_command(command, get_satellite_txvr_command_callback_lambda(command, sock), false);
        }
    }
}

std::function<estts::Status(std::string)> get_set_txvr_scw_modifier(cosmos_satellite_txvr_handler * csth) {
    return [csth] (const std::string& command) -> estts::Status {
        std::string original_command = "ES+W2200";
        spdlog::trace("Modifier found raw packet {}", command);

        std::string scw = command;
        scw.erase(0, original_command.length());
        auto scw_uc = const_cast<unsigned char *>(reinterpret_cast<const unsigned char *>(scw.c_str()));

        auto hex = new char[2];
        sprintf(hex, "%02X%02X", scw_uc[0], scw_uc[1]);

        spdlog::trace("Encoded SCW to {}", hex);
        std::string encoded_scw = original_command + hex;
        spdlog::debug("Complete command modifier: {}", encoded_scw);

        Status status;
        std::string resp;
        for (int i = 0; i < ESTTS_MAX_RETRIES; i++) {

            status = get_satellite_txvr_session_transmit_func(csth->get_groundstation_manager())(encoded_scw);
            if (status != ES_OK) {
                spdlog::error("Failed to transmit command.");
                return status;
            }

            resp = get_satellite_txvr_session_receive_func(csth->get_groundstation_manager())();
            if (!resp.empty()) break;

            spdlog::debug("Retrying set SCW command");
        }

        if (resp.empty()) {
            spdlog::error("Failed to receive response to set SCW command.");
            get_satellite_txvr_command_callback_lambda(original_command, csth->get_socket_handler())("ERR+Failed to set SCW");
            return ES_UNSUCCESSFUL;
        }

        if (resp.find("ERR") != std::string::npos) {
            spdlog::error("Failed to get SCW. Got back: {}", resp);
            get_satellite_txvr_command_callback_lambda(original_command, csth->get_socket_handler())("ERR+Failed to set SCW");
            return ES_UNSUCCESSFUL;
        }

        // Lazy, just call read SCW
        return get_read_txvr_scw_modifier(csth)("ES+R2200");
    };
}

std::function<estts::Status(std::string)> get_read_txvr_scw_modifier(cosmos_satellite_txvr_handler * csth) {
    return [csth] (const std::string& command) -> estts::Status {
        std::string original_command = "ES+R2200";

        Status status;
        std::string resp;
        for (int i = 0; i < ESTTS_MAX_RETRIES; i++) {

            status = get_satellite_txvr_session_transmit_func(csth->get_groundstation_manager())(original_command);
            if (status != ES_OK) {
                spdlog::error("Failed to transmit command.");
                return status;
            }

            resp = get_satellite_txvr_session_receive_func(csth->get_groundstation_manager())();
            if (!resp.empty()) break;

            spdlog::debug("Retrying get SCW command");
        }

        if (resp.empty()) {
            spdlog::error("Failed to receive response to get SCW.");
            get_satellite_txvr_command_callback_lambda(original_command, csth->get_socket_handler())("ERR+Failed to get SCW");
            return ES_UNSUCCESSFUL;
        }

        if (resp.find("ERR") != std::string::npos) {
            spdlog::error("Failed to get SCW. Got back: {}", resp);
            get_satellite_txvr_command_callback_lambda(original_command, csth->get_socket_handler())("ERR+Failed to get SCW");
            return ES_UNSUCCESSFUL;
        }

        std::string scw = resp.substr(9, 4);

        auto scw_uc = const_cast<unsigned char *>(reinterpret_cast<const unsigned char *>(scw.c_str()));

        spdlog::trace("string scw: {} uc scw: {}", scw, scw_uc);

        for (int i = 0, j = 0; i < 4; i += 2, j++)
            scw_uc[j] = HexToBin(scw_uc[i], scw_uc[i + 1]);
        scw_uc[3] = '\0';

        std::string new_scw(reinterpret_cast<char const *>(scw_uc));

        std::stringstream reconstructed_scw;
        reconstructed_scw << "OK+" << resp.substr(3, 6);

        reconstructed_scw << ((scw_uc[0] >> 7) & 0x01); // Res
        reconstructed_scw << ((scw_uc[0] >> 6) & 0x01); // HFXT
        reconstructed_scw << ((scw_uc[0] >> 4) & 0x03); // UartBaud
        reconstructed_scw << ((scw_uc[0] >> 3) & 0x01); // Reset
        reconstructed_scw << ((scw_uc[0] >> 0) & 0x07); // RfMode
        for (int i = 7; i >= 0; i--) {
            reconstructed_scw << ((scw_uc[1] >> i) & 0x01);
        }

        spdlog::debug("Reconstructed SCW: {}", reconstructed_scw.str());

        return get_satellite_txvr_command_callback_lambda(original_command, csth->get_socket_handler())(reconstructed_scw.str());
    };
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

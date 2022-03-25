/* Copyright © EagleSat II - Embry Riddle Aeronautical University - All rights reserved - 2022 */

#include "sstream"
#include "cosmos_groundstation_handler.h"
#include "socket_handler.h"
#include "constants.h"

cosmos_groundstation_handler::cosmos_groundstation_handler() {
   sock = new socket_handler(estts::cosmos::COSMOS_SERVER_ADDR, estts::cosmos::COSMOS_GROUNDSTATION_CMD_TELEM_PORT);
}

void cosmos_groundstation_handler::groundstation_cosmos_worker() {
    std::string command;
    for (;;) {
        command = sock->read_socket_s();
        if (not command.empty()) {
            groundstation_manager->schedule_command(command, get_generic_command_callback_lambda(command, sock));
        }
    }
}

estts::Status cosmos_groundstation_handler::cosmos_groundstation_init(transmission_interface *ti) {
    sock->init_socket_handle();
    groundstation_manager = new groundstation_cmdtelem_manager(ti, get_generic_telemetry_callback_lambda(sock));
    cosmos_worker = std::thread(&cosmos_groundstation_handler::groundstation_cosmos_worker, this);
    SPDLOG_TRACE("Created groundstation COSMOS worker thread with ID {}", std::hash<std::thread::id>{}(cosmos_worker.get_id()));

    return estts::ES_OK;
}

std::function<estts::Status(std::string)>
cosmos_groundstation_handler::get_generic_command_callback_lambda(std::string command, socket_handler *sock) {
    return [command, sock] (const std::string& telem) -> estts::Status {
        if (telem.empty() || sock == nullptr) {
            return estts::ES_UNINITIALIZED;
        }
        sock->write_socket_s(command);
        return estts::ES_OK;
    };
}

std::function<estts::Status(std::string)>
cosmos_groundstation_handler::get_generic_telemetry_callback_lambda(socket_handler *sock) {
    return [sock] (const std::string& telem) -> estts::Status {
        if (telem.empty() || sock == nullptr) {
            return estts::ES_UNINITIALIZED;
        }
        sock->write_socket_s(telem);
        return estts::ES_OK;
    };
}

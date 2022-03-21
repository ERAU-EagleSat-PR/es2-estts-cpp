/* Copyright © EagleSat II - Embry Riddle Aeronautical University - All rights reserved - 2022 */

#include "cosmos_satellite_txvr_handler.h"

cosmos_satellite_txvr_handler::cosmos_satellite_txvr_handler() {
    // todo Create a new socket handler instance with the COSMOS server address and COSMOS_GROUNDSTATION_CMD_TELEM_PORT
}

estts::Status cosmos_satellite_txvr_handler::cosmos_satellite_txvr_init() {
    // todo Initialize the socket created in the constructor with the init_socket_handle() method
    // todo Create new groundstation cmdtelem handler and initialize it with the telemetry callback

    cosmos_worker = std::thread(&cosmos_satellite_txvr_handler::satellite_txvr_cosmos_worker, this);
    SPDLOG_TRACE("Created satellite transceiver COSMOS worker thread with ID {}", std::hash<std::thread::id>{}(cosmos_worker.get_id()));

    return estts::ES_OK;
}

void cosmos_satellite_txvr_handler::satellite_txvr_cosmos_worker() {
    for (;;) {
        // todo Indefinitely read the open socket port, and schedule a new command with the associated cmdtelem manager when a command is received from COSMOS
    }
}

std::function<estts::Status(std::string)>
cosmos_satellite_txvr_handler::get_generic_command_callback_lambda(std::string command, socket_handler *sock) {
    return [command, sock] (const std::string& telem) -> estts::Status {
        if (telem.empty() || sock == nullptr) {
            return estts::ES_UNINITIALIZED;
        }
        // todo handle command response that is passed to this callback as argument (hint use the socket handler)
        return estts::ES_OK;
    };
}
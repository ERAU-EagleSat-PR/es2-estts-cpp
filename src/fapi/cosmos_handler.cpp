/* Copyright © EagleSat II - Embry Riddle Aeronautical University - All rights reserved - 2022 */
//
// Created by Hayden Roszell on 2/24/22.
//

#include <iostream>
#include <unistd.h>
#include "cosmos_handler.h"

cosmos_handler::cosmos_handler() {
    ti = new transmission_interface();
    this->sock = new socket_handler(estts::cosmos::COSMOS_SERVER_ADDR, estts::cosmos::COSMOS_PRIMARY_CMD_TELEM_PORT);
    obc_session = nullptr;
}

[[noreturn]] estts::Status cosmos_handler::primary_cosmos_worker() {
    std::string temp_string;
    for (;;) {
        temp_string = sock->read_socket_s();
        if (!temp_string.empty()) {
            auto sn = obc_session->schedule_command(temp_string, get_generic_command_callback_lambda(temp_string, sock));
        }
    }
}

estts::Status cosmos_handler::cosmos_init() {
    if (sock->init_socket_handle() != estts::ES_OK)
        return estts::ES_UNSUCCESSFUL;
    obc_session = new obc_session_manager(ti, get_generic_telemetry_callback_lambda(sock));

    cosmos_worker = std::thread(&cosmos_handler::primary_cosmos_worker, this);
    SPDLOG_TRACE("Created primary COSMOS worker thread with ID {}", std::hash<std::thread::id>{}(cosmos_worker.get_id()));

    //this->cosmos_satellite_txvr_init();
    this->cosmos_groundstation_init(ti);

    return estts::ES_OK;
}

std::function<estts::Status(std::string)> cosmos_handler::get_generic_command_callback_lambda(const std::string& command, socket_handler * sock) {
    return [command, sock] (const std::string& telem) -> estts::Status {
        if (telem.empty() || sock == nullptr) {
            return estts::ES_UNINITIALIZED;
        }
        std::stringstream temp;
        for (char i : command) {
            if (i != '\r')
                temp << i;
        }
        spdlog::info("COSMOS Command Callback Lambda --> Sent {} and got back: {}", temp.str(), telem);
        sock->write_socket_s(telem);
        return estts::ES_OK;
    };
}

std::function<estts::Status(std::string)> cosmos_handler::get_generic_telemetry_callback_lambda(socket_handler * sock) {
    return [sock] (const std::string& telem) -> estts::Status {
        if (telem.empty() || sock == nullptr) {
            return estts::ES_UNINITIALIZED;
        }
        std::stringstream temp;
        for (char i : telem) {
            if (i != '\r')
                temp << i;
        }
        spdlog::info("COSMOS Telemetry Callback Lambda --> Found: {}", temp.str());
        sock->write_socket_s(telem);
        return estts::ES_OK;
    };
}

/* Copyright © EagleSat II - Embry Riddle Aeronautical University - All rights reserved - 2022 */
//
// Created by Hayden Roszell on 2/24/22.
//

#include <iostream>
#include <unistd.h>
#include "primary_cosmos_handler.h"



primary_cosmos_handler::primary_cosmos_handler() {
    this->sock = new ti_socket_handler(estts::cosmos::COSMOS_SERVER_ADDR, estts::cosmos::COSMOS_PORT);
}

[[noreturn]] estts::Status primary_cosmos_handler::cosmos() {
    std::string temp_string;
    for (;;) {
        temp_string = sock->read_socket_s();
        if (!temp_string.empty()) {
            auto sn = estts_session->schedule_command(temp_string, get_generic_command_callback_lambda(temp_string, sock));
        }
    }
}

estts::Status primary_cosmos_handler::cosmos_init() {
    if (sock->init_socket_handle() != estts::ES_OK)
        return estts::ES_UNSUCCESSFUL;
    estts_session = new session_manager(get_generic_telemetry_callback_lambda(sock));

    cosmos_worker = std::thread(&primary_cosmos_handler::cosmos, this);
    SPDLOG_TRACE("Created COSMOS worker thread with ID {}", std::hash<std::thread::id>{}(cosmos_worker.get_id()));
    return estts::ES_OK;
}

std::function<estts::Status(std::string)> primary_cosmos_handler::get_generic_command_callback_lambda(std::string command, ti_socket_handler * sock) {
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

std::function<estts::Status(std::string)> primary_cosmos_handler::get_generic_telemetry_callback_lambda(ti_socket_handler * sock) {
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

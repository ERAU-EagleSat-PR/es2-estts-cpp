//
// Created by Hayden Roszell on 2/24/22.
//

#include <iostream>
#include <unistd.h>
#include "cosmos_handler.h"



cosmos_handler::cosmos_handler() {
    this->sock = new ti_socket_handler(estts::cosmos::COSMOS_SERVER_ADDR, estts::cosmos::COSMOS_PORT);
}

[[noreturn]] estts::Status cosmos_handler::cosmos() {
    unsigned char * temp_char;
    for (;;) {
        temp_char = sock->read_socket_uc();
        if (temp_char != nullptr) {
            auto sn = estts_session->schedule_command(temp_char, get_generic_command_callback_lambda(temp_char, sock));
        }
        delete temp_char;
    }

    return estts::ES_OK;
}

estts::Status  cosmos_handler::cosmos_init() {
    if (sock->init_socket_handle() != estts::ES_OK)
        return estts::ES_UNSUCCESSFUL;
    estts_session = new session_manager(get_generic_telemetry_callback_lambda(sock));

    cosmos_worker = std::thread(&cosmos_handler::cosmos, this);
    SPDLOG_TRACE("Created COSMOS worker thread with ID {}", std::hash<std::thread::id>{}(cosmos_worker.get_id()));
    return estts::ES_OK;
}

std::function<estts::Status(std::string)> cosmos_handler::get_generic_command_callback_lambda(const unsigned char * command, ti_socket_handler * sock) {
    return [command, sock] (const std::string& telem) -> estts::Status {
        if (telem.empty() || sock == nullptr) {
            return estts::ES_UNINITIALIZED;
        }
        std::stringstream temp;
        for (int i = 0; i < strlen(reinterpret_cast<const char *>(command)); i ++) {
            if (command[i] != '\r')
                temp << command[i];
        }
        spdlog::info("COSMOS Command Callback Lambda --> Sent {} and got back: {}", temp.str(), telem);
        sock->write_socket_s(telem);
        return estts::ES_OK;
    };
}

std::function<estts::Status(std::string)> cosmos_handler::get_generic_telemetry_callback_lambda(ti_socket_handler * sock) {
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

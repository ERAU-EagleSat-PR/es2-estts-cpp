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
        /*
        auto r = read(sock->sock, temp_char, estts::ti_socket::TI_SOCKET_BUF_SZ);
        temp_char[r] = '\0';
        if (r > 0) {
            std::cout << "Read " << r << " bytes - ";
            for (int i = 0; i < r; i++) {
                if (temp_char[i] == '\r')
                    std::cout << "\\r";
                else if (temp_char[i] != '\0')
                    std::cout << temp_char[i];
            }
            std::cout << std::endl;
            */
        if (temp_char != nullptr) {
            auto sn = estts_session->schedule_command(temp_char, get_generic_callback_lambda(temp_char));
        }
        delete temp_char;
    }

    return estts::ES_OK;
}

estts::Status  cosmos_handler::cosmos_init() {
    if (sock->init_socket_handle() != estts::ES_OK)
        return estts::ES_UNSUCCESSFUL;
    estts_session = new session_manager();

    cosmos_worker = std::thread(&cosmos_handler::cosmos, this);
    SPDLOG_TRACE("Created COSMOS worker thread with ID {}", std::hash<std::thread::id>{}(cosmos_worker.get_id()));
    return estts::ES_OK;
}

std::function<estts::Status(std::string)> cosmos_handler::get_generic_callback_lambda(const unsigned char * command) {
    return [command] (const std::string& telem) -> estts::Status {
        if (telem.empty()) {
            return estts::ES_UNINITIALIZED;
        }
        std::stringstream temp;
        spdlog::info("Sent {} and got back: {}", command, telem);
        return estts::ES_OK;
    };
}

//
// Created by Hayden Roszell on 2/24/22.
//

#ifndef ESTTS_COSMOS_COSMOS_HANDLER_H
#define ESTTS_COSMOS_COSMOS_HANDLER_H

#include <thread>
#include "constants.h"
#include "cosmos_handler.h"
#include "ti_socket_handler.h"
#include "session_manager.h"

class cosmos_handler {
private:
    ti_socket_handler * sock;

    std::thread cosmos_worker;

    session_manager * estts_session;

    [[noreturn]] estts::Status cosmos();

    static std::function<estts::Status(std::string)> get_generic_command_callback_lambda(const unsigned char * command, ti_socket_handler * sock);

    static std::function<estts::Status(std::string)> get_generic_telemetry_callback_lambda(ti_socket_handler * sock);
public:
    cosmos_handler();

    estts::Status cosmos_init();

    void initialize_cosmos_daemon() { cosmos_worker.join(); }
};


#endif //ESTTS_COSMOS_COSMOS_HANDLER_H

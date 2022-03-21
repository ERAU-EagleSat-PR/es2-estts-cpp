/* Copyright © EagleSat II - Embry Riddle Aeronautical University - All rights reserved - 2022 */

#ifndef ESTTS_COSMOS_GROUNDSTATION_HANDLER_H
#define ESTTS_COSMOS_GROUNDSTATION_HANDLER_H

#include <thread>
#include "constants.h"
#include "socket_handler.h"
#include "groundstation_cmdtelem_manager.h"

class cosmos_groundstation_handler {
private:
    socket_handler * sock;

    std::thread cosmos_worker;

    groundstation_cmdtelem_manager * groundstation_manager;

    /**
     * Thread worker function that handles the interaction between COSMOS and the groundstation. This function doesn't return
     * @return none
     */
    [[noreturn]] void groundstation_cosmos_worker();

    /**
     * Function that returns a function pointer that takes argument for a command received by COSMOS and the local socket
     * handler for dealing with any responses. This function is passed as the command callback to the schedule_command function
     * @param command String command passed by COSMOS
     * @param sock Pointer to socket handler used to handle the command response.
     * @return Function pointer with form std::function<estts::Status(std::string)>
     */
    static std::function<estts::Status(std::string)> get_generic_command_callback_lambda(std::string command, socket_handler * sock);

    /**
     * Function that returns a function pointer that takes argument for the local socket
     * handler for dealing with telemetry received by ESTTS.
     * @param sock Pointer to socket handler used to handle the telemetry response.
     * @return Function pointer with form std::function<estts::Status(std::string)>
     */
    static std::function<estts::Status(std::string)> get_generic_telemetry_callback_lambda(socket_handler * sock);

public:
    /**
     * Default constructor that initializes socket.
     */
    cosmos_groundstation_handler();

    /**
     * Function that initializes ESTTS to work with COSMOS. This includes defining the telemetry callback and creating
     * the COSMOS worker thread
     * @return ES_OK if successful, anything else if not
     */
    estts::Status cosmos_groundstation_init();
};


#endif //ESTTS_COSMOS_GROUNDSTATION_HANDLER_H

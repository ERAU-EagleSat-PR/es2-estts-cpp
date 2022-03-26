/* Copyright © EagleSat II - Embry Riddle Aeronautical University - All rights reserved - 2022 */
//
// Created by Hayden Roszell on 2/24/22.
//

#ifndef ESTTS_COSMOS_COSMOS_HANDLER_H
#define ESTTS_COSMOS_COSMOS_HANDLER_H

#include <thread>
#include "constants.h"
#include "cosmos_groundstation_handler.h"
#include "cosmos_satellite_txvr_handler.h"
#include "socket_handler.h"
#include "obc_session_manager.h"

/**
 * The primary COSMOS handler is in charge of communication between COSMOS and the OBC.
 * When the primary COSMOS handler receives a command from COSMOS, the logic that it
 * uses inside ESTTS handles bi-directional communication between the ground station and the satellite.
 * The primary COSMOS handler also manages the COSMOS groundstation and satellite transceiver handlers.
 */
class cosmos_handler : virtual public cosmos_groundstation_handler, virtual public cosmos_satellite_txvr_handler {
private:
    transmission_interface *ti;

    socket_handler * sock;

    std::thread cosmos_worker;

    obc_session_manager * obc_session;

    /**
     * Thread worker function that handles the interaction between ESTTS and COSMOS. This function doesn't return
     * @return none
     */
    [[noreturn]] estts::Status primary_cosmos_worker();

    /**
     * Function that returns a function pointer that takes argument for a command received by COSMOS and the local socket
     * handler for dealing with any responses. This function is passed as the command callback to the schedule_command function
     * @param command String command passed by COSMOS
     * @param sock Pointer to socket handler used to handle the command response.
     * @return Function pointer with form std::function<estts::Status(std::string)>
     */
    static std::function<estts::Status(std::string)> get_generic_command_callback_lambda(const std::string& command, socket_handler * sock);

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
    cosmos_handler();

    /**
     * Function that initializes ESTTS to work with COSMOS. This includes defining the telemetry callback and creating
     * the COSMOS worker thread
     * @return ES_OK if successful, anything else if not
     */
    estts::Status cosmos_init();

    /**
     * Joins the primary_cosmos_worker worker thread, which doesn't exit unless something catastrophic occurs.
     * This function is used to initialize the ESTTS runtime, as it never returns.
     */
    void initialize_cosmos_daemon() { cosmos_worker.join(); }
};


#endif //ESTTS_COSMOS_COSMOS_HANDLER_H

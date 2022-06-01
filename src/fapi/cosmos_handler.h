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
#include "groundstation_manager.h"

/**
 * The primary COSMOS handler is in charge of communication between COSMOS and the OBC.
 * When the primary COSMOS handler receives a command from COSMOS, the logic that it
 * uses inside ESTTS handles bi-directional communication between the ground station and the satellite.
 * The primary COSMOS handler also manages the COSMOS groundstation and satellite transceiver handlers.
 */
class cosmos_handler : virtual public cosmos_groundstation_handler, virtual public cosmos_satellite_txvr_handler {
private:
    groundstation_manager * gm;

    socket_handler * sock;

    socket_handler * telem_sock;

    std::thread cosmos_worker;

    /**
     * Thread worker function that handles the interaction between ESTTS and COSMOS. This function doesn't return
     * @return none
     */
    [[noreturn]] estts::Status primary_cosmos_worker();
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

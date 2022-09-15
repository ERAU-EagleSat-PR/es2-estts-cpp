/* Copyright © EagleSat II - Embry Riddle Aeronautical University - All rights reserved - 2022 */

#ifndef ESTTS_COSMOS_SATELLITE_TXVR_HANDLER_H
#define ESTTS_COSMOS_SATELLITE_TXVR_HANDLER_H

#include <thread>
#include "constants.h"
#include "socket_handler.h"

class cosmos_satellite_txvr_handler {
    /**
     * Ground station manager object configured by the primary COSMOS handler.
     */
    groundstation_manager * gm;

    /**
     * Primary socket interface used for communicating with the COSMOS satellite transceiver target.
     */
    socket_handler * sock;

    /**
     * Worker thread that schedules commands coming from COSMOS.
     */
    std::thread cosmos_worker;

    /**
     * Thread worker function that handles the interaction between COSMOS and the groundstation. This function doesn't return
     * @return none
     */
    [[noreturn]] void satellite_txvr_cosmos_worker();

public:
    /**
     * Default constructor that initializes socket.
     */
    cosmos_satellite_txvr_handler();

    groundstation_manager * get_groundstation_manager() { return gm; };

    socket_handler * get_socket_handler() { return sock; }

    /**
     * Function that initializes ESTTS to work with COSMOS. This includes defining the telemetry callback and creating
     * the COSMOS worker thread
     * @return ES_OK if successful, anything else if not
     */
    estts::Status cosmos_satellite_txvr_init(groundstation_manager * gm);
};


#endif //ESTTS_COSMOS_SATELLITE_TXVR_HANDLER_H

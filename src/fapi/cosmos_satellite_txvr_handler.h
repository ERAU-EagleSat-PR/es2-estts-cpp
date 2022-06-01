/* Copyright © EagleSat II - Embry Riddle Aeronautical University - All rights reserved - 2022 */

#ifndef ESTTS_COSMOS_SATELLITE_TXVR_HANDLER_H
#define ESTTS_COSMOS_SATELLITE_TXVR_HANDLER_H

#include <thread>
#include "constants.h"
#include "socket_handler.h"

class cosmos_satellite_txvr_handler {
    groundstation_manager * gm;

    socket_handler * sock;

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

    /**
     * Function that initializes ESTTS to work with COSMOS. This includes defining the telemetry callback and creating
     * the COSMOS worker thread
     * @return ES_OK if successful, anything else if not
     */
    estts::Status cosmos_satellite_txvr_init(groundstation_manager * gm);
};


#endif //ESTTS_COSMOS_SATELLITE_TXVR_HANDLER_H

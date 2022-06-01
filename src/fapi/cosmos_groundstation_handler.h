/* Copyright © EagleSat II - Embry Riddle Aeronautical University - All rights reserved - 2022 */

#ifndef ESTTS_COSMOS_GROUNDSTATION_HANDLER_H
#define ESTTS_COSMOS_GROUNDSTATION_HANDLER_H

#include <thread>
#include "constants.h"
#include "socket_handler.h"
#include "groundstation_manager.h"

class cosmos_groundstation_handler {
private:
    socket_handler * sock;
    std::thread cosmos_worker;

    groundstation_manager * gm;

    /**
     * Thread worker function that handles the interaction between COSMOS and the groundstation. This function doesn't return
     * @return none
     */
    [[noreturn]] void groundstation_cosmos_worker();

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
    estts::Status cosmos_groundstation_init(groundstation_manager * gm);
};


#endif //ESTTS_COSMOS_GROUNDSTATION_HANDLER_H

/* Copyright © EagleSat II - Embry Riddle Aeronautical University - All rights reserved - 2022 */

#ifndef ESTTS_COSMOS_GROUNDSTATION_HANDLER_H
#define ESTTS_COSMOS_GROUNDSTATION_HANDLER_H

#include <thread>
#include "constants.h"
#include "socket_handler.h"
#include "groundstation_manager.h"

class cosmos_groundstation_handler {
private:
    /**
     * Socket interface used for communicating with the COSMOS ground station target.
     */
    socket_handler * sock;

    /**
     * Worker thread that schedules commands coming from COSMOS.
     */
    std::thread cosmos_worker;

    /**
     * Ground station manager object configured by the primary COSMOS handler.
     */
    groundstation_manager * gm;

    /**
     * Session manager
     */
    groundstation_manager::session_manager * sm;

    /**
     * Thread worker function that handles the interaction between COSMOS and the groundstation. This function doesn't return
     * @return none
     */
    [[noreturn]] void groundstation_cosmos_worker();

    double satellite_txvr_nominal_frequency_hz = 435000000;

    bool dynamic_doppler_mode = true;

    session_manager_modifier * build_session_modifier();

    /**
    * Local thread object that executes the doppler shift method
    */
    std::thread ds_worker;

    [[noreturn]] void doppler_cosmos_worker();

public:
    /**
     * Default constructor that initializes socket.
     */
    cosmos_groundstation_handler();

    estts::Status set_transceiver_frequency(double frequency);

    groundstation_manager * get_groundstation_manager() { return gm; };

    socket_handler * get_socket_handler() { return sock; }

    groundstation_manager::session_manager * get_session_manager() { return sm; }

    /**
     * Function that initializes ESTTS to work with COSMOS. This includes defining the telemetry callback and creating
     * the COSMOS worker thread
     * @return ES_OK if successful, anything else if not
     */
    estts::Status cosmos_groundstation_init(groundstation_manager * gm);

    void set_satellite_txvr_nominal_frequency_hz(double freq) { satellite_txvr_nominal_frequency_hz = freq; }

    double get_doppler(void);

};


#endif //ESTTS_COSMOS_GROUNDSTATION_HANDLER_H

/* Copyright © EagleSat II - Embry Riddle Aeronautical University - All rights reserved - 2022 */

#ifndef ESTTS_SATELLITE_TXVR_CMDTELEM_MANAGER_H
#define ESTTS_SATELLITE_TXVR_CMDTELEM_MANAGER_H

#include <functional>
#include <mutex>
#include <thread>
#include <utility>
#include <vector>
#include <queue>
#include "transmission_interface.h"
#include "constants.h"

class satellite_txvr_cmdtelem_manager {
private:
    transmission_interface *ti;
    std::thread cmdtelem_worker;
    std::deque<estts::waiting_command *> waiting;
    std::function<estts::Status(std::string)> telem_callback = nullptr;

    /**
     * @brief Dispatcher that runs in an infinite loop as long as commands are available.
     */
    [[noreturn]] void dispatch();

public:
    /**
     * @brief Default constructor for satellite transceiver command/telemetry manager. Takes argument for a transmission
     * interface and initializes the command handler. Note that in order to run the dispatcher, dispatcher_init must be called.
     */
    satellite_txvr_cmdtelem_manager(transmission_interface * ti, std::function<estts::Status(std::string)> telem_callback);

    /**
     * Function that takes argument for a string command passed by a higher level interface, and a callback function that is
     * called with the response to the command as argument. It's implied that this function should handle whatever telemetry
     * is returned by the dispatch process. This function creates a waiting_command object and stores a serial number, the callback function,
     * and the command string expected to dispatch during the next satellite pass.
     * @param command String command received by higher level interface
     * @param callback Callback with form std::function<estts::Status(std::string)>
     * @return String serial number associated with newly scheduled command
     */
    std::string schedule_command(std::string command, const std::function<estts::Status(std::string)>& callback);
};


#endif //ESTTS_SATELLITE_TXVR_CMDTELEM_MANAGER_H

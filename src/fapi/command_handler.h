/* Copyright © EagleSat II - Embry Riddle Aeronautical University - All rights reserved - 2022 */
//
// Created by Hayden Roszell on 12/21/21.
//

#ifndef ESTTS_FAPI_COMMAND_HANDLER_H
#define ESTTS_FAPI_COMMAND_HANDLER_H

#include <deque>
#include <functional>
#include <vector>
#include "transmission_interface.h"
#include "constants.h"

class command_handler {
private:
    typedef struct {
        std::string serial_number;
        estts::Status response_code;
    } completed;

    transmission_interface *ti;

    /**
     * @brief Maps dispatched command objects to their associatd telemetry response. Matched telemetry responses
     * are collected and then passed to the obj_callback function stored inside the associated dispatch object.
     * @param telem Vector of pointers to telemetry_objects.
     * @return ES_OK if all callbacks were successful, and ES_UNSUCCESSFUL if no match was made.
     */
    estts::Status map_telemetry_to_dispatched(const std::vector<estts::telemetry_object *> &telem);

    /**
     * Primary execution handler that sends a command as a string packed inside a waiting command object. This function
     * completes the full execution including calling the associated callback found inside the waiting command object.
     * @param command Pointer to an estts::waiting_command object.
     * @return ES_OK if command was executed properly.
     */
    estts::Status execute_str(estts::waiting_command *command);
protected:
    std::vector<estts::dispatched_command *> dispatched;

    std::vector<completed *> completed_cache;

    /**
     * @brief Default constructor. Note that the command_handler requires that init_command_handler
     *        be called before use.
     */
    explicit command_handler();

    /**
     * @brief Initializes command handler by allocating local transmission interface instance
     * @param ti Transmission interface object
     * @return
     */
    estts::Status init_command_handler(transmission_interface *ti);

    ~command_handler();

    /**
     * Primary command execution point. Takes argument for a waiting command object and calls associated execution handler.
     * @param command Pointer to an estts::waiting_command object.
     * @return ES_OK if command was executed properly.
     */
    estts::Status execute(estts::waiting_command * command);
};


#endif //ESTTS_FAPI_COMMAND_HANDLER_H

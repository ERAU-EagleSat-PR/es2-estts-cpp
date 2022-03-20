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
     * @brief Waits for a response from the satellite after command has been sent. This function waits the number of seconds
     * specified by ESTTS_AWAIT_RESPONSE_PERIOD_SEC. This function only waits for a response and decodes received frames,
     * and then calls map_telemetry_to_dispatched to handle the response. This is ONLY used by the command stream method,
     * and is deprecated as of 3/19/22.
     * @return ES_OK if response was received, or ES_UNSUCCESSFUL if timeout elapses.
     */
    estts::Status await_response();

    /**
     * @brief Maps dispatched command objects to their associatd telemetry response. Matched telemetry responses
     * are collected and then passed to the obj_callback function stored inside the associated dispatch object.
     * @param telem Vector of pointers to telemetry_objects.
     * @return ES_OK if all callbacks were successful, and ES_UNSUCCESSFUL if no match was made.
     */
    estts::Status map_telemetry_to_dispatched(const std::vector<estts::telemetry_object *> &telem);

    /**
     * Primary execution handler for a waiting command object containing a frame object that requires construction by the
     * AX.25 terminal node controller emulator. This is deprecated as of 3/19/22.
     * @param command Pointer to an estts::waiting_command object.
     * @return ES_OK if command was executed properly.
     */
    estts::Status execute_obj(estts::waiting_command *command);

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
     * @brief Executes commands passed in as a vector of waiting_command structures. One frame, of whatever type,
     * is created PER command object inside EACH waiting_command object. This means that if there are 8 commands each
     * with 2 frames, 16 frames will be generated. Whether they are AX.25 frames or ESTTC commands is
     * implementation defined.
     * @param commands Vector of pointers to waiting_command structures. Note that waiting_command structures hold
     * pointers to obj_callback functions that should handle whatever is returned by the executed command.
     * @return ES_OK if successful, ES_UNINITIALIZED if the transmission interface wasn't initialized.
     */
    estts::Status execute(const std::deque<estts::waiting_command *> &commands);

    /**
     * Primary command execution point. Takes argument for a waiting command object and calls associated execution handler.
     * @param command Pointer to an estts::waiting_command object.
     * @return ES_OK if command was executed properly.
     */
    estts::Status execute(estts::waiting_command * command);
};


#endif //ESTTS_FAPI_COMMAND_HANDLER_H

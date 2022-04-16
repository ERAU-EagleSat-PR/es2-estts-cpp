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

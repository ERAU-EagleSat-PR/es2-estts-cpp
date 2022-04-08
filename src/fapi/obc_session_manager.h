/* Copyright © EagleSat II - Embry Riddle Aeronautical University - All rights reserved - 2022 */
//
// Created by Hayden Roszell on 1/4/22.
//

#ifndef ESTTS_COMMAND_SCHEDULER_H
#define ESTTS_COMMAND_SCHEDULER_H

#include <functional>
#include <mutex>
#include <thread>
#include <utility>
#include <vector>
#include <queue>
#include "constants.h"
#include "command_handler.h"

// Idea: schedule command takes in a function pointer. Then, a new command scheduler
// structure should be created, containing a serial number, the function pointer,
// and an Status containing the completion status. When a command is scheduled to run with
// the schedule_command() function, the unique serial number is returned. When a job is scheduled and
// run, it is popped off the queue, and the status variable associated with the structure is
// updated. Finally, the structure is pushed onto another vector containing completed jobs.
// the number of completed jobs that are stored is limited to a predefined number.
// Finally, there should be a function that takes argument for a serial number associated
// with a command, and returns the completion status.

class obc_session_manager : virtual public command_handler {
private:
    transmission_interface *ti;
    std::thread session_worker;
    std::deque<estts::waiting_command *> waiting;
    std::function<estts::Status(std::string)> telem_callback = nullptr;

    /**
     * @brief Dispatcher that runs in an infinite loop as long as commands are available. When no commands are available,
     * the dispatcher receives telemetry. When commands are available, a session is created with the satellite with the
     * transmission interface.
     */
    void dispatch();

public:
    /**
     * @brief Default constructor for command scheduler. Takes argument for a transmission interface and initializes the
     * command handler. Note that in order to run the dispatcher, dispatcher_init must be called.
     */
    explicit obc_session_manager(transmission_interface * ti);

    /**
     * Secondary constructor for command scheduler that takes argument for a telemetry callback handler that is called when
     * lower layers receive telemetry.
     * @param telem_callback Callback with form std::function<estts::Status(std::string)>
     */
    explicit obc_session_manager(transmission_interface * ti, std::function<estts::Status(std::string)> telem_callback);

    /**
     * @brief Cleans up internal structures
     */
    ~obc_session_manager();

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

    /**
     * @brief Function that takes argument to a vector of command objects and a obj_callback function expecting a pointer to a
     * telemetry object as argument. It's implied that this function should handle whatever telemetry is returned by the
     * dispatch process. This function creates a waiting_command object and stores a serial number, the obj_callback function,
     * and the command frames (as command_objects) expected to dispatch during the next satellite pass.
     * @param command Vector of command_object pointers to schedule for dispatching.
     * @param decomp_callback Callback pointer to a function that returns a Status and takes argument for a vector of
     * telemetry_objects. It's implied that this obj_callback function knows how to decode a vector of telemetry frames.
     * @return Returns a unique string serial number for later retrieval of the command status.
     */
    std::string schedule_command(estts::command_object * command,
                                 std::function<estts::Status(std::vector<estts::telemetry_object *>)> decomp_callback);

    /**
     * @brief Uses std::thread::join() to await thread completion. If commands continue
     * to be added to queue, this function will block indefinitely.
     */
    void await_completion();
};


#endif //ESTTS_COMMAND_SCHEDULER_H

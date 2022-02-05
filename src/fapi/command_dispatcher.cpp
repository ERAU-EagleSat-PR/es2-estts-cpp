//
// Created by Hayden Roszell on 1/4/22.
//

#include <condition_variable>
#include <random>
#include <utility>
#include "command_dispatcher.h"

// https://stackoverflow.com/questions/15752659/thread-pooling-in-c11

// Idea: schedule command takes in a function pointer. Then, a new command scheduler
// structure should be created, containing a serial number, the function pointer,
// and an estts::Status containing the completion status. When a command is scheduled to run with
// the schedule_command() function, the unique serial number is returned. When a job is scheduled and
// run, it is popped off the queue, and the status variable associated with the structure is
// updated. Finally, the structure is pushed onto another vector containing completed jobs.
// the number of completed jobs that are stored is limited to a predefined number.
// Finally, there should be a function that takes argument for a serial number associated
// with a command, and returns the completion status.

/**
 * @brief Creates 16-character serial number using C++ random library
 * @return 16-character serial number
 */
std::string generate_serial_number() {
    auto len = 16;
    static const char alphanum[] =
        "0123456789"
        "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
        "abcdefghijklmnopqrstuvwxyz";
    std::string tmp_s;
    tmp_s.reserve(len);
    std::random_device dev;
    std::mt19937 rng(dev());
    std::uniform_int_distribution<std::mt19937::result_type> dist6(1,sizeof(alphanum));

    for (int i = 0; i < len; ++i) {
        tmp_s += alphanum[dist6(rng) % (sizeof(alphanum) - 1)];
    }

    return tmp_s;
}

/**
 * @brief Function that takes argument to a vector of command objects and a callback function expecting a pointer to a
 * telemetry object as argument. It's implied that this function should handle whatever telemetry is returned by the
 * dispatch process. This function creates a waiting_command object and stores a serial number, the callback function,
 * and the command frames (as command_objects) expected to dispatch during the next satellite pass.
 * @param command Vector of command_object pointers to schedule for dispatching.
 * @param decomp_callback Callback pointer to a function that returns a Status and takes argument for a vector of
 * telemetry_objects. It's implied that this callback function knows how to decode a vector of telemetry frames.
 * @return Returns a unique string serial number for later retrieval of the command status.
 */
std::string command_dispatcher::schedule_command(const std::vector<estts::command_object *>& command, std::function<estts::Status(std::vector<estts::telemetry_object *>)> decomp_callback) {
    // We only need a worker thread if there are commands to dispatch
    if (!dispatch_worker.joinable()) {
        // Create a new thread, pass in schedule() function and current object
        dispatch_worker = std::thread(&command_dispatcher::dispatch, this);
        SPDLOG_TRACE("Created dispatch worker thread with ID {}", std::hash<std::thread::id>{}(dispatch_worker.get_id()));
    }
    auto new_command = new estts::waiting_command;
    new_command->command = command;
    new_command->serial_number = generate_serial_number();
    new_command->callback = std::move(decomp_callback);

    // todo this should add new commands to the *back* not the front
    waiting.push_back(new_command);

    SPDLOG_DEBUG("Scheduled new command with serial number {}", new_command->serial_number);
    return new_command->serial_number;
}

/**
 * @brief Default constructor for command scheduler. Creates new transmission interface and initializes the
 * command handler. Note that in order to run the dispatcher, dispatcher_init must be called.
 */
command_dispatcher::command_dispatcher() {
    command_progress = estts::ES_UNINITIALIZED;
    handshake = false;
    this->ti = new transmission_interface();
    this->init_command_handler(ti);
}

/**
 * @brief Uses std::thread::join() to await thread completion. If commands continue
 * to be added to queue, this function will block indefinitely.
 */
void command_dispatcher::await_completion() {
    // If the thread is joinable (IE it's active), join the thread
    // Join blocks until the thread returns.
    if (dispatch_worker.joinable())
        dispatch_worker.join();
}

/**
 * @brief Takes argument for a serial number, returns status of completed command
 * if found, ES_INPROGRESS if command hasn't run yet, or ES_NOTFOUND if
 * serial number was not found.
 * @param serial_number Serial number returned by schedule_command()
 * @return Status of scheduled job
 */
estts::Status command_dispatcher::get_command_status(const std::string& serial_number) {
    // Search dispatched queue for serial number
    for (auto &i : dispatched)
        if (i->serial_number == serial_number)
            return estts::ES_INPROGRESS;
    // If not found in completed, search waiting.
    for (auto &i : waiting) {
        if (i->serial_number == serial_number)
            return estts::ES_WAITING;
    }
    // Search completed queue for serial number
    for (auto &i : completed_cache)
        if (i->serial_number == serial_number)
            return i->response_code;

    // If we get to this point, the serial number doesn't exist in the dispatcher.
    return estts::ES_NOTFOUND;
}

/**
 * @brief Cleans up internal structures
 */
command_dispatcher::~command_dispatcher() {
    await_completion();
    delete ti;
    if (!dispatched.empty())
        for (auto &i : dispatched)
            delete i;
    if (!waiting.empty())
        for (auto &i : waiting)
            delete i;
}

/**
 * @brief Dispatcher that runs in an infinite loop as long as commands are available. If a command isn't received for
 * ESTTS_AWAIT_RESPONSE_PERIOD_SEC seconds, the dispatcher thread exits and must be re-initialized. When the satellite
 * is deemed in range, the dispatcher uses the command handler to execute the commands loaded into the waiting queue.
 */
void command_dispatcher::dispatch() {
    using namespace std::this_thread; // sleep_for, sleep_until
    using namespace std::chrono; // nanoseconds, system_clock, seconds
    int wait = 0;
    // TODO implement satellite location (IE listen for beacon)
    handshake = true;
    for (;;) {
        if (handshake) {
            // Todo Idea: something else on its own thread (or just inside this thread) should set these variables
            handshake = false;
            if (!waiting.empty()) {
                // After execute is called, the session is in progress. Set this state before, so that abstracted objects
                // stay up to date.
                command_progress = estts::ES_INPROGRESS;
                this->execute(waiting); // Execute the queue of waiting commands.
                waiting.clear();
                command_progress = estts::ES_SUCCESS;
            } else {
                // If no command is added to queue within ESTTS_AWAIT_RESPONSE_PERIOD_SEC seconds, exit.
                sleep_until(system_clock::now() + seconds(1));
                wait++;
                if (wait > estts::ESTTS_AWAIT_RESPONSE_PERIOD_SEC) {
                    SPDLOG_DEBUG("Scheduler inactive; didn't receive a command within {} seconds. Exiting", estts::ESTTS_AWAIT_RESPONSE_PERIOD_SEC);
                    return;
                }
            }
        } else {
            command_progress = estts::ES_WAITING;
            sleep_until(system_clock::now() + seconds(1));
            handshake = true;
        }
    }
}

estts::Status command_dispatcher::get_dispatch_session_progress() {
    return command_progress;
}


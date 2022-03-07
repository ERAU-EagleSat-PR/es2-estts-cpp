//
// Created by Hayden Roszell on 1/4/22.
//

#include <condition_variable>
#include <random>
#include <utility>
#include "session_manager.h"

using namespace estts;

// https://stackoverflow.com/questions/15752659/thread-pooling-in-c11

// Idea: schedule command takes in a function pointer. Then, a new command scheduler
// structure should be created, containing a serial number, the function pointer,
// and an Status containing the completion status. When a command is scheduled to run with
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
 * @brief Function that takes argument to a vector of command objects and a obj_callback function expecting a pointer to a
 * telemetry object as argument. It's implied that this function should handle whatever telemetry is returned by the
 * dispatch process. This function creates a waiting_command object and stores a serial number, the obj_callback function,
 * and the command frames (as command_objects) expected to dispatch during the next satellite pass.
 * @param command Vector of command_object pointers to schedule for dispatching.
 * @param decomp_callback Callback pointer to a function that returns a Status and takes argument for a vector of
 * telemetry_objects. It's implied that this obj_callback function knows how to decode a vector of telemetry frames.
 * @return Returns a unique string serial number for later retrieval of the command status.
 */
std::string session_manager::schedule_command(command_object * command, std::function<Status(std::vector<telemetry_object *>)> decomp_callback) {
    if (ti == nullptr) {
        SPDLOG_ERROR("Transmission interface not initialized. Was init_session_manager() called?");
    }

    auto new_command = new waiting_command;
    new_command->command = command;
    new_command->serial_number = generate_serial_number();
    new_command->obj_callback = std::move(decomp_callback);
    new_command->str_callback = nullptr;
    new_command->frame = nullptr;

    waiting.push_back(new_command);

    SPDLOG_DEBUG("Scheduled new command with serial number {}", new_command->serial_number);
    return new_command->serial_number;
}

std::string session_manager::schedule_command(unsigned char * command,
                                              const std::function<estts::Status(std::string)>& decomp_callback) {
    if (ti == nullptr) {
        SPDLOG_ERROR("Transmission interface not initialized. Was init_session_manager() called?");
    }
    auto new_command = new waiting_command;
    new_command->frame = command;
    new_command->serial_number = generate_serial_number();
    new_command->str_callback = decomp_callback;
    new_command->command = nullptr;
    new_command->obj_callback = nullptr;

    waiting.push_back(new_command);

    SPDLOG_DEBUG("Scheduled new command with serial number {}", new_command->serial_number);
    return new_command->serial_number;
}

/**
 * @brief Default constructor for command scheduler. Creates new transmission interface and initializes the
 * command handler. Note that in order to run the dispatcher, dispatcher_init must be called.
 */
session_manager::session_manager() {
    ti = new transmission_interface();
    session = false;
    this->init_command_handler(ti);
    // Create a new thread, pass in schedule() function and current object
    session_worker = std::thread(&session_manager::dispatch, this);
    SPDLOG_TRACE("Created dispatch worker thread with ID {}", std::hash<std::thread::id>{}(session_worker.get_id()));
}

/**
 * @brief Uses std::thread::join() to await thread completion. If commands continue
 * to be added to queue, this function will block indefinitely.
 */
void session_manager::await_completion() {
    // If the thread is joinable (IE it's active), join the thread
    // Join blocks until the thread returns.
    if (session_worker.joinable())
        session_worker.join();
}

/**
 * @brief Takes argument for a serial number, returns status of completed command
 * if found, ES_INPROGRESS if command hasn't run yet, or ES_NOTFOUND if
 * serial number was not found.
 * @param serial_number Serial number returned by schedule_command()
 * @return Status of scheduled job
 */
Status session_manager::get_command_status(const std::string& serial_number) {
    // Search dispatched queue for serial number
    for (auto &i : dispatched)
        if (i->serial_number == serial_number)
            return ES_INPROGRESS;
    // If not found in completed, search waiting.
    for (auto &i : waiting) {
        if (i->serial_number == serial_number)
            return ES_WAITING;
    }
    // Search completed queue for serial number
    for (auto &i : completed_cache)
        if (i->serial_number == serial_number)
            return i->response_code;

    // If we get to this point, the serial number doesn't exist in the dispatcher.
    return ES_NOTFOUND;
}

/**
 * @brief Cleans up internal structures
 */
session_manager::~session_manager() {
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
[[noreturn]] void session_manager::dispatch() {
    using namespace std::this_thread; // sleep_for, sleep_until
    using namespace std::chrono; // nanoseconds, system_clock, seconds
    int wait = 0;
    for (;;) {
        if (!waiting.empty()) {
            if (!session) {
                // Request a new communication session from EagleSat II
                if (ES_OK == this->ti->request_new_session(ax25::NEW_SESSION_FRAME)) {
                    session = true;
                }
            }

            // After execute is called, the session is in progress. Set this state before, so that abstracted objects
            // stay up to date.
#ifdef __ESTTS_COMMAND_STREAM_MODE__
            this->execute(waiting); // Execute the queue of waiting commands.
            waiting.clear();
            if (ES_OK == this->ti->end_session(ax25::END_SESSION_FRAME)) {
                session = false;
            }
#else
            this->execute(waiting.front());
            waiting.pop_front();

            if (waiting.empty() && session) {
                if (ES_OK == this->ti->end_session(ax25::END_SESSION_FRAME)) {
                    session = false;
                }
            }
#endif
        } else {

            // Handle stream
            handle_stream();
        }
    }
}

Status session_manager::handle_stream() {
    auto stream = ti->receive();
    SPDLOG_DEBUG("Found {}", stream);

    return ES_OK;
}

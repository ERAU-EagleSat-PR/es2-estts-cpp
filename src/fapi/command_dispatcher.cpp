//
// Created by Hayden Roszell on 1/4/22.
//

#include <condition_variable>
#include <random>
#include "command_dispatcher.h"

using Command = std::function<estts::Status()>;
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
 * @brief Function that takes argument for a command to schedule. If scheduling is successful, the
 * command will be carried out in the order that the request was received. Only one
 * command can be processed at a time.
 * @param command Function object as std::function<estts::Status()> to be called
 * @return Returns serial number of scheduled command if scheduling is successful
 */
std::string command_dispatcher::schedule_command(const Command& command) {
    // If the thread is active, add the command.
    if (worker.joinable()) {
        auto new_command = new waiting_command;
        new_command->command = command;
        new_command->serial_number = generate_serial_number();
        waiting.push_back(new_command);

        SPDLOG_DEBUG("Scheduled new command with serial number {}", new_command->serial_number);
        return new_command->serial_number;
    }
    SPDLOG_WARN("Failed to queue command; scheduler inactive.");
    return "";
}

/**
 * @brief Constructor for command scheduler. Creates thread and starts indefinitely looping function running
 */
command_dispatcher::command_dispatcher() {
    // Create a new thread, pass in schedule() function and current object
    worker = std::thread(&command_dispatcher::schedule, this);
    SPDLOG_TRACE("Created scheduler thread with ID {}", std::hash<std::thread::id>{}(worker.get_id()));
}

/**
 * @brief Function that loops while commands are available.
 */
void command_dispatcher::schedule() {
    using namespace std::this_thread; // sleep_for, sleep_until
    using namespace std::chrono; // nanoseconds, system_clock, seconds
    int wait = 0;
    // Loop indefinitely
    for (;;) {
        // If the queue isn't empty, process commands
        if (!waiting.empty()) {
            auto command = waiting.back(); // Get the oldest command in the queue

            auto current_running = new completed_command;
            current_running->serial_number = command->serial_number;

            SPDLOG_DEBUG("Processing command with serial number {}", current_running->serial_number);

            current_running->response_code = command->command(); // This is where the command is run
            completed.push_back(current_running); // Update list of completed commands
            clean_completed_cache(); // Make sure we don't remember all completed commands
            waiting.pop_back(); // Remove completed job from queue
            delete command;

            sleep_until(system_clock::now() + seconds(estts::ESTTS_RETRY_WAIT_SEC));
            wait = 0;
        } else {
            // If no command is added to queue within ESTTS_AWAIT_RESPONSE_PERIOD_SEC seconds, exit.
            sleep_until(system_clock::now() + seconds(1));
            wait++;
            if (wait > estts::ESTTS_AWAIT_RESPONSE_PERIOD_SEC) {
                SPDLOG_WARN("Scheduler inactive; didn't receive a command within {} seconds. Exiting", estts::ESTTS_AWAIT_RESPONSE_PERIOD_SEC);
                return;
            }
        }
    }
}

/**
 * @brief Uses std::thread::join() to await thread completion. If commands continue
 * to be added to queue, this function will block indefinitely.
 */
void command_dispatcher::await_completion() {
    // If the thread is joinable (IE it's active), join the thread
    // Join blocks until the thread returns.
    if (worker.joinable())
        worker.join();
}

/**
 * @brief Removes and cleans up completed command queue when size is
 * greater than dispatcher::MAX_COMPLETED_CACHE
 */
void command_dispatcher::clean_completed_cache() {
    if (completed.size() >= estts::dispatcher::MAX_COMPLETED_CACHE) {
        auto temp = completed.back();
        completed.pop_back();
        delete temp;
    }
}

/**
 * @brief Takes argument for a serial number, returns status of completed command
 * if found, ES_INPROGRESS if command hasn't run yet, or ES_NOTFOUND if
 * serial number was not found.
 * @param serial_number Serial number returned by schedule_command()
 * @return Status of scheduled job
 */
estts::Status command_dispatcher::get_command_status(std::string serial_number) {
    // Search completed queue for serial number
    for (auto &i : completed)
        if (i->serial_number == serial_number)
            return i->response_code;

    // If not found in completed, search waiting. Maybe something is broken
    for (auto &i : waiting) {
        if (i->serial_number == serial_number)
            return estts::ES_INPROGRESS;
    }
    // If we get to this point, the serial number doesn't exist in the dispatcher.
    return estts::ES_NOTFOUND;
}

/**
 * @brief Cleans up internal structures
 */
command_dispatcher::~command_dispatcher() {
    for (auto &i : completed)
        delete i;
    for (auto &i : waiting)
        delete i;
}

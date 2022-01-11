//
// Created by Hayden Roszell on 1/4/22.
//

#include <condition_variable>
#include "command_scheduler.h"

using Command = std::function<void()>;
// https://stackoverflow.com/questions/15752659/thread-pooling-in-c11

/**
 * @brief Function that takes argument for a command to schedule. If scheduling is successful, the
 * command will be carried out in the order that the request was received. Only one
 * command can be processed at a time.
 * @param command Function object as std::function<void()> to be called
 * @return Returns ES_OK if scheduling is successful
 */
estts::Status command_scheduler::schedule_command(Command command) {
    // If the thread is active, add the command.
    if (worker.joinable()) {
        command_queue.push_back(command);
    }
    SPDLOG_WARN("Failed to queue command; scheduler inactive.");
    return estts::ES_UNINITIALIZED;
}

/**
 * @brief Constructor for command scheduler. Creates thread and starts indefinitely looping function running
 */
command_scheduler::command_scheduler() {
    // Create a new thread, pass in schedule() function and current object
    worker = std::thread(&command_scheduler::schedule, this);
    SPDLOG_TRACE("Created scheduler thread with ID {}", std::hash<std::thread::id>{}(worker.get_id()));
}

/**
 * @brief Function that loops while commands are available.
 */
void command_scheduler::schedule() {
    using namespace std::this_thread; // sleep_for, sleep_until
    using namespace std::chrono; // nanoseconds, system_clock, seconds
    int wait = 0;
    // Loop indefinitely
    for (;;) {
        // If the queue isn't empty, process commands
        if (!command_queue.empty()) {
            auto Job = command_queue.front();
            Job(); // This is where the command is run
            command_queue.pop_front();
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
void command_scheduler::await_completion() {
    // If the thread is joinable (IE it's active), join the thread
    // Join blocks until the thread returns.
    if (worker.joinable())
        worker.join();
}

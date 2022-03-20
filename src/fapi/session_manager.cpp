/* Copyright © EagleSat II - Embry Riddle Aeronautical University - All rights reserved - 2022 */
//
// Created by Hayden Roszell on 1/4/22.
//

#include <condition_variable>
#include <random>
#include <utility>
#include "session_manager.h"

using namespace estts;

// https://stackoverflow.com/questions/15752659/thread-pooling-in-c11

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

std::string session_manager::schedule_command(std::string command,
                                              const std::function<estts::Status(std::string)>& decomp_callback) {
    if (ti == nullptr) {
        SPDLOG_ERROR("Transmission interface not initialized. Was init_session_manager() called?");
    }
    auto new_command = new waiting_command;
    new_command->frame = std::move(command);
    new_command->serial_number = generate_serial_number();
    new_command->str_callback = decomp_callback;
    new_command->command = nullptr;
    new_command->obj_callback = nullptr;

    waiting.push_back(new_command);

    SPDLOG_DEBUG("Scheduled new command with serial number {}", new_command->serial_number);
    return new_command->serial_number;
}

session_manager::session_manager() {
    ti = new transmission_interface();
    this->init_command_handler(ti);
    // Create a new thread, pass in schedule() function and current object
    session_worker = std::thread(&session_manager::dispatch, this);
    SPDLOG_TRACE("Created dispatch worker thread with ID {}", std::hash<std::thread::id>{}(session_worker.get_id()));
}

session_manager::session_manager(std::function<estts::Status(std::string)> telem_callback) {
    this->telem_callback = std::move(telem_callback);

    ti = new transmission_interface();
    this->init_command_handler(ti);
    ti->set_telem_callback(this->telem_callback);
    // Create a new thread, pass in schedule() function and current object
    session_worker = std::thread(&session_manager::dispatch, this);
    SPDLOG_TRACE("Created dispatch worker thread with ID {}", std::hash<std::thread::id>{}(session_worker.get_id()));
}

void session_manager::await_completion() {
    // If the thread is joinable (IE it's active), join the thread
    // Join blocks until the thread returns.
    if (session_worker.joinable())
        session_worker.join();
}

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

[[noreturn]] void session_manager::dispatch() {
    using namespace std::this_thread; // sleep_for, sleep_until
    using namespace std::chrono; // nanoseconds, system_clock, seconds
    for (;;) {
start:
        if (!waiting.empty()) {
            SPDLOG_TRACE("{} commands in queue", waiting.size());
            if (!ti->session_active) {
                // Request a new communication session from EagleSat II
                if (ES_OK != this->ti->request_new_session()) {
                    SPDLOG_ERROR("Failed to request new session.");
                    goto start; // todo This should probably have a more elegant solution..
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
            SPDLOG_TRACE("Session status: {}", ti->session_active);
            if (ES_OK != this->execute(waiting.front())) {
                SPDLOG_WARN("Failed to execute command with serial number {}", waiting.front()->serial_number);
            } else {
                SPDLOG_INFO("Command executed successfully");
            }
            waiting.pop_front();

            if (waiting.empty() && ti->session_active) {
                if (ES_OK != this->ti->end_session(ax25::END_SESSION_FRAME)) {
                    SPDLOG_WARN("Failed to end session rip");
                }
                SPDLOG_INFO("Waiting for more commands");
            }

#endif
        } else {

            // Handle stream
            handle_stream();
        }
    }
}

Status session_manager::handle_stream() {

    auto stream = ti->nonblock_receive();
    if (!stream.empty() & this->telem_callback != nullptr)
        telem_callback(stream);

    return ES_OK;
}
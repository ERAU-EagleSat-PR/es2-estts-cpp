/* Copyright © EagleSat II - Embry Riddle Aeronautical University - All rights reserved - 2022 */
//
// Created by Hayden Roszell on 1/4/22.
//

#include "helper.h"
#include "obc_session_manager.h"

using namespace estts;

// https://stackoverflow.com/questions/15752659/thread-pooling-in-c11

std::string obc_session_manager::schedule_command(std::string command,
                                              const std::function<estts::Status(std::string)>& callback) {
    if (ti == nullptr) {
        SPDLOG_ERROR("Transmission interface not initialized. Was init_session_manager() called?");
    }
    auto new_command = new waiting_command;
    new_command->frame = std::move(command);
    new_command->serial_number = generate_serial_number();
    new_command->str_callback = callback;

    waiting.push_back(new_command);

    SPDLOG_DEBUG("Scheduled new command with serial number {}", new_command->serial_number);
    return new_command->serial_number;
}

obc_session_manager::obc_session_manager(transmission_interface * ti) {
    this->ti = ti;
    this->init_command_handler(ti);
    ti->register_dispatch_function( [this] () {this->dispatch();} );
}

obc_session_manager::obc_session_manager(transmission_interface * ti, std::function<estts::Status(std::string)> telem_callback) {
    this->telem_callback = std::move(telem_callback);

    this->ti = ti;
    this->init_command_handler(this->ti);
    ti->set_telem_callback(this->telem_callback);
    // Create a new thread, pass in dispatch() function and current object
    ti->register_dispatch_function( [this] () {this->dispatch();} );
}

void obc_session_manager::await_completion() {
    // If the thread is joinable (IE it's active), join the thread
    // Join blocks until the thread returns.
    if (session_worker.joinable())
        session_worker.join();
}

obc_session_manager::~obc_session_manager() {
    await_completion();
    delete ti;
    if (!dispatched.empty())
        for (auto &i : dispatched)
            delete i;
    if (!waiting.empty())
        for (auto &i : waiting)
            delete i;
}

void obc_session_manager::dispatch() {
    using namespace std::this_thread; // sleep_for, sleep_until
    using namespace std::chrono; // nanoseconds, system_clock, seconds
    for (;;) {
start:
        if (!waiting.empty()) {
            SPDLOG_TRACE("{} commands in queue", waiting.size());
            if (!ti->obc_session_active) {
                // Request a new communication session from EagleSat II
                if (ES_OK != this->ti->request_obc_session()) {
                    SPDLOG_ERROR("Failed to request new session.");
                    goto start; // todo This should probably have a more elegant solution..
                }
            }

            // After execute is called, the session is in progress. Set this state before, so that abstracted objects
            // stay up to date.
            SPDLOG_TRACE("Session status: {}", ti->obc_session_active);
            if (ES_OK != this->execute(waiting.front())) {
                SPDLOG_WARN("Failed to execute command with serial number {}", waiting.front()->serial_number);
            } else {
                SPDLOG_INFO("Command executed successfully");
            }
            waiting.pop_front();

            if (waiting.empty() && ti->obc_session_active) {
                if (ES_OK != this->ti->end_obc_session(ax25::END_SESSION_FRAME)) {
                    SPDLOG_WARN("Failed to end session rip");
                }
                SPDLOG_INFO("Waiting for more commands");
            }

        } else {

            // Handle stream
            auto stream = ti->nonblock_receive();
            if (!stream.empty() && this->telem_callback != nullptr)
                telem_callback(stream);
        }
        if (!ti->satellite_in_range) {
            SPDLOG_DEBUG("OBC Dispatch worker - Detected satellite is outside range, exiting.");
            return;
        }
    }
}
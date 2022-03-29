/* Copyright © EagleSat II - Embry Riddle Aeronautical University - All rights reserved - 2022 */

#include "helper.h"
#include "groundstation_cmdtelem_manager.h"

using namespace estts;

std::string groundstation_cmdtelem_manager::schedule_command(std::string command, const std::function<Status(
        std::string)> &callback) {
    if (ti == nullptr) {
        SPDLOG_ERROR("Transmission interface not initialized. Was init_session_manager() called?");
    }
    auto new_command = new waiting_command;
    new_command->frame = std::move(command);
    new_command->serial_number = generate_serial_number();
    new_command->str_callback = callback;
    new_command->command = nullptr;
    new_command->obj_callback = nullptr;

    waiting.push_back(new_command);

    SPDLOG_DEBUG("Scheduled new command with serial number {}", new_command->serial_number);
    return new_command->serial_number;
}

groundstation_cmdtelem_manager::groundstation_cmdtelem_manager(transmission_interface *ti, std::function<estts::Status(
        std::string)> telem_callback) {
    this->telem_callback = std::move(telem_callback);
    this->ti = ti;

    // Create a new thread, pass in dispatch() function and current object
    cmdtelem_worker = std::thread(&groundstation_cmdtelem_manager::dispatch, this);
    SPDLOG_TRACE("Created dispatch worker thread with ID {}", std::hash<std::thread::id>{}(cmdtelem_worker.get_id()));
}

void groundstation_cmdtelem_manager::dispatch() {
    std::string message;
    for (;;) {
        if (!waiting.empty()) {
            SPDLOG_TRACE("{} commands in queue", waiting.size());
            if(this->ti->gs_transmit(waiting.front()->frame) != estts::ES_OK){
                SPDLOG_TRACE("Failed to transmit.");
                }
            else {
                auto telem = this->ti->receive();
                if(waiting.front()->str_callback != nullptr){
                    waiting.front()->str_callback(telem);
                }
                SPDLOG_TRACE("Transmission Successful");
            }
            waiting.pop_front();
        }
    }
}

/* Copyright © EagleSat II - Embry Riddle Aeronautical University - All rights reserved - 2022 */
//
// Created by Hayden Roszell on 12/21/21.
//

#include <chrono>
#include <thread>
#include <iostream>
#include <vector>
#include <sstream>

#include "command_handler.h"

using namespace std::this_thread; // sleep_for, sleep_until
using namespace std::chrono; // nanoseconds, system_clock, seconds

/**
 * @brief Validates response code returned by telemetry object.
 * @param code integer response code.
 * @return Status translation
 */
estts::Status validate_response_code(int code) {
    if (code == estts::estts_response_code::SUCCESS)
        return estts::ES_SUCCESS;
    else if (code == estts::estts_response_code::UNRECOGNIZED_REQUEST)
        return estts::ES_BAD_OPTION;
    else if (code == estts::estts_response_code::OBC_FAILURE)
        return estts::ES_SERVER_ERROR;
    else return estts::ES_UNINITIALIZED;
}

command_handler::command_handler() {
    this->ti = nullptr;
}

estts::Status command_handler::execute(estts::waiting_command *command) {
    if (ti == nullptr) {
        SPDLOG_ERROR("Transmission interface not initialized. Was init_command_handler() called?");
        return estts::ES_UNINITIALIZED;
    }
    /*
    if (command->command != nullptr && command->frame.empty())
        return execute_obj(command);
    else if (command->command == nullptr && !command->frame.empty())

    else {
        SPDLOG_ERROR("Command object not initialized properly. Please see documentation.");
        return estts::ES_UNINITIALIZED;
    }
     */
    return execute_str(command);
}

estts::Status command_handler::execute_str(estts::waiting_command *command) {
    try {
        SPDLOG_INFO("Sending command");
        bool retry = true;
        int retries = 0;
        // Try to transmit frame
        while (ti->transmit(command->frame) != estts::ES_OK) {
            spdlog::error("Failed to transmit frame. Waiting {} seconds", estts::ESTTS_RETRY_WAIT_SEC);
            sleep_until(system_clock::now() + seconds(estts::ESTTS_RETRY_WAIT_SEC));
            retries++;
            if (retries > estts::endurosat::MAX_RETRIES) return estts::ES_UNSUCCESSFUL;
            SPDLOG_INFO("Retrying transmit ({}/{})", retries, estts::ESTTS_MAX_RETRIES);
        }
        // If we got this far,
        SPDLOG_DEBUG("Successfully transmitted command");
    }
    catch (const std::exception &e) {
        // TODO catch exceptions & do something smart with them
        spdlog::error("We failed somewhere");
        return estts::ES_UNSUCCESSFUL;
    }

    SPDLOG_INFO("Waiting for a response from EagleSat II");
    auto telem = ti->receive();
    if (telem.empty())
        return estts::ES_UNSUCCESSFUL;
    SPDLOG_DEBUG("Got response from EagleSat II");

    // todo there are likely error cases here that aren't accounted for. Find & fix them

    if (command->str_callback != nullptr)
        if (estts::ES_OK != command->str_callback(telem))
            return estts::ES_UNSUCCESSFUL;

    auto temp_completed = new completed;
    temp_completed->serial_number = command->serial_number;
    temp_completed->response_code = estts::ES_OK;
    completed_cache.push_back(temp_completed);

    return estts::ES_OK;
}

command_handler::~command_handler() = default;

estts::Status command_handler::init_command_handler(transmission_interface *ti) {
    this->ti = ti;
    return estts::ES_OK;
}

estts::Status command_handler::map_telemetry_to_dispatched(const std::vector<estts::telemetry_object *> &telem) {
    // This function requires some intelligence. The basic premise is that the decoded telemetry objects
    // need to be mapped back to their associated dispatched command objects. There are a couple of ways we
    // can handle this. For this version, we're just going to handle each response individually and in real-time.

    while (!dispatched.empty()) {
        auto current = dispatched.back();
        auto command_id = current->command->commandID;
        auto command_address = current->command->address;
        std::vector<estts::telemetry_object *> temp_telem;
        bool associated_frame_found;
        for (auto j : telem) {
            if (command_id == j->commandID && command_address == j->address) {
                // We have a match!
                associated_frame_found = true;
                temp_telem.push_back(j);
            }
        }
        if (associated_frame_found) {
            current->telem_obj = temp_telem;
            current->response_code = validate_response_code(temp_telem[0]->response_code);
            if (current->obj_callback != nullptr)
                if (estts::ES_OK != current->obj_callback(current->telem_obj)) // Call the obj_callback with the telemetry object
                    return estts::ES_UNSUCCESSFUL;
            // Todo this shoudn't return, it should schedule a new retry, or at least notify something that can retry

            auto temp_completed = new completed;
            temp_completed->serial_number = current->serial_number;
            temp_completed->response_code = current->response_code;
            completed_cache.push_back(temp_completed);
        } else {
            SPDLOG_WARN("Didn't receive a telemetry response for command with SN {}", current->serial_number);
            current->response_code = estts::ES_UNSUCCESSFUL;
        }
        delete current;
        dispatched.pop_back();
    }

    return estts::ES_OK;
}

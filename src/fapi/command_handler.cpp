//
// Created by Hayden Roszell on 12/21/21.
//

#include <chrono>
#include <thread>
#include <iostream>
#include <vector>
#include <sstream>

#include "command_handler.h"
#include "ax25_ui_frame_constructor.h"
#include "ax25_ui_frame_destructor.h"

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

/**
 * @brief Default constructor. Note that the command_handler requires that init_command_handler
 *        be called before use.
 */
command_handler::command_handler() {
    this->ti = nullptr;
}

estts::Status command_handler::execute(estts::waiting_command *command) {
    if (ti == nullptr) {
        SPDLOG_ERROR("Transmission interface not initialized. Was init_command_handler() called?");
        return estts::ES_UNINITIALIZED;
    }

    try {
        using namespace std::this_thread; // sleep_for, sleep_until
        using namespace std::chrono; // nanoseconds, system_clock, seconds
        // Create transmission interface object
        SPDLOG_INFO("Sending command");
        bool retry = true;
        int retries = 0;
        std::string frame;
        // Try to build AX.25 frame from command object
        while (retry) {
            auto sapi_tx = new ax25_ui_frame_constructor(command->command);
            frame = sapi_tx->construct_ax25();
            if (frame.empty()) {
                spdlog::error("Frame construction failed. Waiting {} seconds", estts::ESTTS_RETRY_WAIT_SEC);
                sleep_until(system_clock::now() + seconds(estts::ESTTS_RETRY_WAIT_SEC));
                retries++;
                if (retries > estts::endurosat::MAX_RETRIES) return estts::ES_UNSUCCESSFUL;
                SPDLOG_INFO("Retrying construction (retry {}/{})", retries, estts::ESTTS_MAX_RETRIES);
            }
            retry = false;
            delete sapi_tx;
        }
        retries = 0;
        // Try to transmit constructed AX.25 frame
        while (ti->transmit(frame) != estts::ES_OK) {
            spdlog::error("Failed to transmit frame. Waiting {} seconds", estts::ESTTS_RETRY_WAIT_SEC);
            sleep_until(system_clock::now() + seconds(estts::ESTTS_RETRY_WAIT_SEC));
            retries++;
            if (retries > estts::endurosat::MAX_RETRIES) return estts::ES_UNSUCCESSFUL;
            SPDLOG_INFO("Retrying transmit ({}/{})", retries, estts::ESTTS_MAX_RETRIES);
        }
        // If we got this far,
        SPDLOG_DEBUG("Successfully transmitted command");

        // De-allocate memory for command object
        delete command->command;
    }
    catch (const std::exception &e) {
        // TODO catch exceptions & do something smart with them
        spdlog::error("We failed somewhere");
        return estts::ES_UNSUCCESSFUL;
    }

    SPDLOG_INFO("Waiting for a response from EagleSat II");
    int seconds_elapsed;
    std::stringstream resp;
    for (seconds_elapsed = 0; seconds_elapsed < estts::ESTTS_AWAIT_RESPONSE_PERIOD_SEC; seconds_elapsed++) {
        auto temp = ti->receive();
        if (!temp.empty()) {
            resp << temp;
            break;
        }
        sleep_until(system_clock::now() + milliseconds (100));
    }
    if (resp.str().empty())
        return estts::ES_UNSUCCESSFUL;
    SPDLOG_DEBUG("Got response from EagleSat II");
    SPDLOG_TRACE("Starting frame deconstruction on {}", resp.str());
    auto destructor = new ax25_ui_frame_destructor(resp.str());
    auto telem = destructor->destruct_ax25();

    // The feature API command handler doesn't care if the response was successful. Update telem attribute & exit
    // todo there are likely error cases here that aren't accounted for. Find & fix them

    if (command->callback != nullptr)
        if (estts::ES_OK != command->callback(telem))
            // todo probably retry
            return estts::ES_UNSUCCESSFUL;

    auto temp_completed = new completed;
    temp_completed->serial_number = command->serial_number;
    temp_completed->response_code = validate_response_code(telem[0]->response_code);
    completed_cache.push_back(temp_completed);
    return estts::ES_OK;
}

/**
 * @brief Executes commands passed in as a vector of waiting_command structures. One frame, of whatever type,
 * is created PER command object inside EACH waiting_command object. This means that if there are 8 commands each
 * with 2 frames, 16 frames will be generated. Whether they are AX.25 frames or ESTTC commands is
 * implementation defined.
 * @param commands Vector of pointers to waiting_command structures. Note that waiting_command structures hold
 * pointers to callback functions that should handle whatever is returned by the executed command.
 * @return ES_OK if successful, ES_UNINITIALIZED if the transmission interface wasn't initialized.
 */
estts::Status command_handler::execute(const std::deque<estts::waiting_command *> &waiting) {
    if (ti == nullptr) {
        SPDLOG_ERROR("Transmission interface not initialized. Was init_command_handler() called?");
        return estts::ES_UNINITIALIZED;
    }

    try {
        std::vector<estts::command_object *> to_run;
        int total_frames = 0;

        int command_iterator = 0;
        for (auto command : waiting) {
            command_iterator++;
            SPDLOG_INFO("Sending command {}/{}", command_iterator, waiting.size());
            int frame_iterator = 0;
            frame_iterator++;
            SPDLOG_INFO("Sending command {}", command_iterator);
            bool retry = true;
            int retries = 0;
            std::string frame;
            // Try to build AX.25 frame from command object
            while (retry) {
                auto sapi_tx = new ax25_ui_frame_constructor(command->command);
                frame = sapi_tx->construct_ax25();
                // Todo this probably doesn't need retries lol
                if (frame.empty()) {
                    spdlog::error("Frame construction failed. Waiting {} seconds", estts::ESTTS_RETRY_WAIT_SEC);
                    sleep_until(system_clock::now() + seconds(estts::ESTTS_RETRY_WAIT_SEC));
                    retries++;
                    if (retries > estts::endurosat::MAX_RETRIES) return estts::ES_UNSUCCESSFUL;
                    SPDLOG_INFO("Retrying construction (retry {}/{})", retries, estts::ESTTS_MAX_RETRIES);
                }
                retry = false;
                delete sapi_tx;
            }
            retries = 0;
            // Try to transmit constructed AX.25 frame
            while (ti->transmit(frame) != estts::ES_OK) {
                spdlog::error("Failed to transmit frame. Waiting {} seconds", estts::ESTTS_RETRY_WAIT_SEC);
                sleep_until(system_clock::now() + seconds(estts::ESTTS_RETRY_WAIT_SEC));
                retries++;
                if (retries > estts::endurosat::MAX_RETRIES) return estts::ES_UNSUCCESSFUL;
                SPDLOG_INFO("Retrying transmit ({}/{})", retries, estts::ESTTS_MAX_RETRIES);
            }
            // If we got this far,
            SPDLOG_DEBUG("Successfully sent command {}", command_iterator);
            // Now we need to move the command state from waiting to dispatched.
            auto newly_dispatched = new estts::dispatched_command;
            // todo make sure these things aren't empty before blindly assigning
            newly_dispatched->callback = command->callback;
            newly_dispatched->serial_number = command->serial_number;
            newly_dispatched->command = command->command;
            newly_dispatched->response_code = estts::ES_INPROGRESS;
            // Update the dispatched queue with the newly dispatched command.
            dispatched.push_back(newly_dispatched);

            // At this point, we don't care about the waiting object anymore, it's no longer waiting.
            delete command;
        }
        SPDLOG_DEBUG("Successfully sent {} frames from {} commands", total_frames, waiting.size());
    }
    catch (const std::exception &e) {
        // TODO catch exceptions & do something smart with them
        spdlog::error("We failed somewhere");
        return estts::ES_UNSUCCESSFUL;
    }

    // Pass handler to await response
    if (await_response() != estts::ES_OK)
        return estts::ES_UNSUCCESSFUL;
    return estts::ES_OK;
}

/**
 * @brief Waits for a response from the satellite after command has been sent. This function waits the number of seconds
 * specified by ESTTS_AWAIT_RESPONSE_PERIOD_SEC. This function only waits for a response and decodes received frames,
 * and then calls map_telemetry_to_dispatched to handle the response.
 * @return ES_OK if response was received, or ES_UNSUCCESSFUL if timeout elapses.
 */
estts::Status command_handler::await_response() {
    using namespace std::this_thread; // sleep_for, sleep_until
    using namespace std::chrono; // nanoseconds, system_clock, seconds
    int seconds_elapsed;
    SPDLOG_INFO("Waiting for a response from EagleSat II");
    std::stringstream resp;
    for (seconds_elapsed = 0; seconds_elapsed < estts::ESTTS_AWAIT_RESPONSE_PERIOD_SEC; seconds_elapsed++) {
        auto temp = ti->receive();
        if (!temp.empty()) {
            resp << temp;
            break;
        }
        sleep_until(system_clock::now() + milliseconds (100));
    }
    if (resp.str().empty())
        return estts::ES_UNSUCCESSFUL;
    SPDLOG_DEBUG("Got response from EagleSat II");
    SPDLOG_TRACE("Starting frame deconstruction on {}", resp.str());
    auto destructor = new ax25_ui_frame_destructor(resp.str());
    auto telem = destructor->destruct_ax25();

    return map_telemetry_to_dispatched(telem);

    // The feature API command handler doesn't care if the response was successful. Update telem attribute & exit
    // todo there are likely error cases here that aren't accounted for. Find & fix them
}

command_handler::~command_handler() = default;

/**
 * @brief Initializes command handler by allocating local transmission interface instance
 * @param ti Transmission interface object
 * @return
 */
estts::Status command_handler::init_command_handler(transmission_interface *ti) {
    this->ti = ti;
    return estts::ES_OK;
}

/**
 * @brief Maps dispatched command objects to their associatd telemetry response. Matched telemetry responses
 * are collected and then passed to the callback function stored inside the associated dispatch object.
 * @param telem Vector of pointers to telemetry_objects.
 * @return ES_OK if all callbacks were successful, and ES_UNSUCCESSFUL if no match was made.
 */
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
            current->telem = temp_telem;
            current->response_code = validate_response_code(temp_telem[0]->response_code);
            if (current->callback != nullptr)
                if (estts::ES_OK != current->callback(current->telem)) // Call the callback with the telemetry object
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

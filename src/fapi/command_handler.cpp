/* Copyright © EagleSat II - Embry Riddle Aeronautical University - All rights reserved - 2022 */
//
// Created by Hayden Roszell on 12/21/21.
//

#include <chrono>
#include <thread>
#include <iostream>
#include <vector>
#include <sstream>

#include "groundstation_manager.h"
#include "command_handler.h"

using namespace std::this_thread; // sleep_for, sleep_until
using namespace std::chrono; // nanoseconds, system_clock, seconds

command_handler::command_handler() {
    this->gm = nullptr;
}

estts::Status command_handler::execute(estts::waiting_command *command) {
    if (gm == nullptr) {
        SPDLOG_ERROR("Transmission interface not initialized. Was init_command_handler() called?");
        return estts::ES_UNINITIALIZED;
    }

    try {
        SPDLOG_INFO("Sending command");
        int retries = 0;
        auto value = command->frame;

        // Route
        if (value.rfind("ES+", 0) == 0) {
            value.append("\r");
        }

        // Try to transmit frame
        while (gm->transmit(value) != estts::ES_OK) {
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
    sleep_until(system_clock::now() + milliseconds (100));
    auto telem = gm->receive();
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

estts::Status command_handler::init_command_handler(groundstation_manager * gm) {
    this->gm = gm;
    return estts::ES_OK;
}

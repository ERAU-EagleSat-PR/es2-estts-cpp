//
// Created by Hayden Roszell on 12/21/21.
//

#include <chrono>
#include <thread>
#include <iostream>
#include <vector>
#include <sstream>

#include "fapi_command_handler.h"
#include "frame_constructor.h"
#include "frame_destructor.h"

fapi_command_handler::fapi_command_handler(transmission_interface *ti) {
    this->ti = ti;
}

/**
 * @brief get_timestamp calculates an integer value representing the number of seconds since the start of the day UTC
 * @return 16-bit (2-byte) timestamp as seconds
 */
int fapi_command_handler::get_timestamp() {
    using namespace std::chrono;
    using namespace std;
    using days = duration<int, ratio<86400>>;
    nanoseconds last_midnight =
            time_point_cast<days>(system_clock::now()).time_since_epoch();
    cout << last_midnight.count() << '\n';
    return 0;
}

/**
 * @brief Handles the transmission of a command to EagleSat and the reception of telemetry.
 * @param command Contains an array of command_object objects
 * @return Returns an array of telemetry objects
 */
estts::Status fapi_command_handler::send_command(const std::vector<estts::command_object *> &command) {
    try {
        using namespace std::this_thread; // sleep_for, sleep_until
        using namespace std::chrono; // nanoseconds, system_clock, seconds
        // Create transmission interface object
        for (int i = 0; i < command.size(); i++) {
            SPDLOG_INFO("Sending command (frame {}/{})", i + 1, command.size());
            bool retry = true;
            int retries = 0;
            std::string frame;
            // Try to build AX.25 frame from command object
            while (retry) {
                auto sapi_tx = new frame_constructor(command[i]);
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
            SPDLOG_DEBUG("Successfully transmitted command (frame {}/{})", i + 1, command.size());

            // De-allocate memory for command object
            delete command[i];
        }
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
 * specified by ESTTS_AWAIT_RESPONSE_PERIOD_SEC.
 * @return ES_OK if response was received, or ES_UNSUCCESSFUL if timeout elapses.
 */
estts::Status fapi_command_handler::await_response() {
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
        sleep_until(system_clock::now() + seconds(1));
    }
    if (resp.str().empty())
        return estts::ES_UNSUCCESSFUL;
    SPDLOG_DEBUG("Got response from EagleSat II");
    SPDLOG_TRACE("Starting frame deconstruction on {}", resp.str());
    auto destructor = new frame_destructor(resp.str());
    auto telem = destructor->destruct_ax25();

    // The feature API command handler doesn't care if the response was successful. Update telem attribute & exit
    // todo there are likely error cases here that aren't accounted for. Find & fix them
    telemetry = telem;
    return estts::ES_OK;
}

estts::Status fapi_command_handler::validate_response_code(int code) {
    if (code == estts::estts_response_code::SUCCESS)
        return estts::ES_SUCCESS;
    else if (code == estts::estts_response_code::UNRECOGNIZED_REQUEST)
        return estts::ES_BAD_OPTION;
    else if (code == estts::estts_response_code::OBC_FAILURE)
        return estts::ES_SERVER_ERROR;
    else return estts::ES_UNINITIALIZED;
}

estts::Status fapi_command_handler::validate_telemetry_response() {
    return estts::ES_UNINITIALIZED;
}

fapi_command_handler::~fapi_command_handler() = default;

//
// Created by Hayden Roszell on 12/21/21.
//

#include <chrono>
#include <thread>
#include <iostream>
#include <vector>
#include <sstream>
#include "spdlog/spdlog.h"
#include "fapi_command_handler.h"
#include "frame_constructor.h"
#include "frame_destructor.h"

fapi_command_handler::fapi_command_handler(transmission_interface * ti) {
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
estts::Status fapi_command_handler::send_command(const std::vector<estts::command_object *>& command) {
    try {
        using namespace std::this_thread; // sleep_for, sleep_until
        using namespace std::chrono; // nanoseconds, system_clock, seconds
        // Create transmission interface object
        for (int i = 0; i < command.size(); i++) {
            spdlog::info("Sending command (frame {}/{})", i+1, command.size());
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
                    spdlog::info("Retrying construction (retry {}/{})", retries, estts::ESTTS_MAX_RETRIES);
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
                spdlog::info("Retrying transmit ({}/{})", retries, estts::ESTTS_MAX_RETRIES);
            }
            // If we got this far,
            spdlog::debug("Successfully transmitted command (frame {}/{})", i+1, command.size());

            // Pass handler to await response
            if (await_response() != estts::ES_OK)
                return estts::ES_UNSUCCESSFUL;
        }
    }
    catch (const std::exception& e) {
        // TODO catch exceptions
        spdlog::error("We failed somewhere");
        return estts::ES_UNSUCCESSFUL;
    }
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
    spdlog::info("Waiting for a response from EagleSat II");
    std::stringstream resp;
    for (seconds_elapsed = 0; seconds_elapsed < estts::ESTTS_AWAIT_RESPONSE_PERIOD_SEC; seconds_elapsed++) {
        auto temp = ti->receive();
        if (!temp.empty()) {
            resp << temp;
            break;
        }
        sleep_until(system_clock::now() + seconds(1));
    }
    spdlog::trace("Starting frame deconstruction on {}", resp.str());
    auto destructor = new frame_destructor(resp.str());
    auto telem = destructor->destruct_ax25();
    if (telem[0]->response_code == 0)
        spdlog::info("Received successful telemetry response with response code {}", telem[0]->response_code);
    return estts::ES_OK;
}

fapi_command_handler::~fapi_command_handler() = default;

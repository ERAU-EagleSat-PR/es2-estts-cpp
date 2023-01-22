/* Copyright © EagleSat II - Embry Riddle Aeronautical University - All rights reserved - 2022 */
//
// Created by Hayden Roszell on 12/21/21.
//

#include <chrono>
#include <thread>
#include "helper.h"

#include "transmission_interface.h"

using namespace std::this_thread; // sleep_for, sleep_until
using namespace std::chrono; // nanoseconds, system_clock, seconds
using namespace estts;
using namespace estts::endurosat;

transmission_interface::transmission_interface() {
    connectionless_telem_cb = nullptr;
    pipe_mode = PIPE_OFF;
    pipe_duration_sec = 0;
    session_active = false;

    refresh_constants();
    mtx.unlock();
}

Status transmission_interface::transmit(const std::string &value) {
    using namespace std::this_thread; // sleep_for, sleep_until
    using namespace std::chrono; // nanoseconds, system_clock, seconds
    if (value.empty())
        return ES_MEMORY_ERROR;
    if (!session_active)
        SPDLOG_WARN("Communication session not active, message may not get to satellite.");
    mtx.lock();
    if (connectionless_telem_cb)
        clear_serial_fifo(connectionless_telem_cb);
    else
        clear_serial_fifo();
#ifndef __TI_DEV_MODE__
    clear_serial_fifo();

    // EnduroSat manual says that at 115200 baud, around 1ms should be waited per byte transmitted
    // TODO fuck em don't wait that long
    if (pipe_mode == estts::endurosat::PIPE_ON && duration_cast<milliseconds>(high_resolution_clock::now() - tx_trace_timestamp).count() < last_transmission_byte_count )
        sleep_until(system_clock::now() + milliseconds (last_transmission_byte_count));

    if (this->write_serial_s(value) != ES_OK) {
        SPDLOG_ERROR("Failed to transmit.");
        mtx.unlock();
        return ES_UNSUCCESSFUL;
    }
#endif
    mtx.unlock();
    return ES_OK;
}



std::string transmission_interface::receive() {
    mtx.lock();
    reset_sync_buf();
#ifndef __TI_DEV_MODE__
    auto buf = internal_receive();
    mtx.unlock();
    return buf;
#else
    std::string received;
    do {

    }
    while ((received = this->read_socket_s()).empty());
    mtx.unlock();
    return received;
#endif
}

std::string transmission_interface::internal_receive() {
    int bytes_avail;
    for (int ms_elapsed = 0; ms_elapsed < ESTTS_AWAIT_RESPONSE_PERIOD_SEC * 20; ms_elapsed++) {
        bytes_avail = check_serial_bytes_avail();
        if (bytes_avail > 0) {
            break;
        }
        sleep_until(system_clock::now() + milliseconds (50));
    }
    if (bytes_avail <= 0) {
        SPDLOG_WARN("Receive timeout - {} seconds elapsed with no response", ESTTS_AWAIT_RESPONSE_PERIOD_SEC);
        return "";
    }
    auto buf = this->read_to_delimeter('\r');
    return buf;
}

std::string transmission_interface::receive_from_obc() {
    mtx.lock();
    int bytes_avail;
    for (int ms_elapsed = 0; ms_elapsed < ESTTS_AWAIT_RESPONSE_PERIOD_SEC * 20; ms_elapsed++) {
        bytes_avail = check_serial_bytes_avail();
        if (bytes_avail > 0) {
            break;
        }
        sleep_until(system_clock::now() + milliseconds (50));
    }
    if (bytes_avail <= 0) {
        SPDLOG_WARN("Receive timeout - {} seconds elapsed with no response", ESTTS_AWAIT_RESPONSE_PERIOD_SEC);
        mtx.unlock();
        return "";
    }
    auto buf = this->read_to_delimeter(endurosat::OBC_ESTTC_DELIMETER, endurosat::OBC_ESTTC_DELIMETER_SIZE);
    mtx.unlock();
    return buf;
}

transmission_interface::~transmission_interface() {
    mtx.lock();

#ifdef __TI_DEV_MODE__
    this->write_socket_s("close");
#endif

    mtx.unlock();
}

bool transmission_interface::data_available() {
#ifdef __TI_DEV_MODE__
    if (check_sock_bytes_avail() > 0)
        return true;
    else
        return false;
#endif
    if (check_serial_bytes_avail() > 0)
        return true;
    else
        return false;
}

Status transmission_interface::transmit(const unsigned char *value, int length) {
    using namespace std::this_thread; // sleep_for, sleep_until
    using namespace std::chrono; // nanoseconds, system_clock, seconds
    if (length <= 0)
        return ES_MEMORY_ERROR;
    int retries = 0;
    if (!session_active)
        SPDLOG_WARN("Communication session not active, message may not get to satellite.");
    mtx.lock();
#ifndef __TI_DEV_MODE__
    retries = 0;
    clear_serial_fifo();

    // EnduroSat manual says that at 115200 baud, around 1ms should be waited per byte transmitted
    if (pipe_mode == estts::endurosat::PIPE_ON && duration_cast<milliseconds>(high_resolution_clock::now() - tx_trace_timestamp).count() < last_transmission_byte_count )
        sleep_until(system_clock::now() + milliseconds (last_transmission_byte_count));

    while (this->write_serial_uc((unsigned char *)value, length) < length) {
        spdlog::error("Failed to transmit. Waiting {} seconds", endurosat::WAIT_TIME_SEC);
        sleep_until(system_clock::now() + seconds(endurosat::WAIT_TIME_SEC));
        retries++;
        if (retries > endurosat::MAX_RETRIES) {
            mtx.unlock();
            return ES_UNSUCCESSFUL;
        }
        SPDLOG_INFO("Retrying transmit (retry {}/{})", retries, endurosat::MAX_RETRIES);
    }
#else
    SPDLOG_DEBUG("Transmitting {}", value);
    while (this->write_socket_uc((unsigned char *)value, length) < length) {
        spdlog::error("Failed to transmit. Waiting {} seconds", ti_socket::WAIT_TIME_SEC);
        sleep_until(system_clock::now() + seconds(ti_socket::WAIT_TIME_SEC));
        retries++;
        if (retries > ti_socket::MAX_RETRIES) {
            mtx.unlock();
            return ES_UNSUCCESSFUL;
        }
        SPDLOG_INFO("Retrying transmit (retry {}/{})", retries, ti_socket::MAX_RETRIES);
    }
#endif
    mtx.unlock();
    return ES_OK;
}

std::string transmission_interface::nonblock_receive() {
    if (!mtx.try_lock())
        return "";
    int bytes;
    sleep_until(system_clock::now() + milliseconds (20)); // TODO reduce delay
    if ((bytes = check_serial_bytes_avail()) > 0) {
        mtx.unlock();
        return this->read_serial_s(bytes);
    }
    mtx.unlock();
    return "";
}

Status transmission_interface::gs_transmit(const std::string &value) {
    if (value.empty())
        return ES_MEMORY_ERROR;
    mtx.lock();
#ifndef __TI_DEV_MODE__
    clear_serial_fifo();
    if (this->write_serial_s(value) != ES_OK) {
        SPDLOG_ERROR("Failed to transmit.");
        mtx.unlock();
        return ES_UNSUCCESSFUL;
    }
#endif
    mtx.unlock();
    return ES_OK;
}

/**
 * @brief Enables transparent pipe mode on Endurosat UHF Transceiver module
 * @return estts::Status indication success/failure of ESTTC command transmission
 */
Status transmission_interface::enable_pipe() {
    std::string pipe_en = "ES+W22003323\r";
    std::stringstream buf;
    if (pipe_mode != PIPE_OFF) {
        return estts::ES_UNSUCCESSFUL;
    }
    mtx.lock();
    pipe_mode = PIPE_OFF;

    for (int retries = 0; retries < endurosat::MAX_RETRIES; retries++) {

        SPDLOG_TRACE("Attempting to enable PIPE on ground station");
        write_serial_s(pipe_en);
        sleep_until(system_clock::now() + milliseconds (50));

        buf << read_to_delimeter('\r') << read_to_delimeter('\r'); // This is looking for two delimiters, so call read twice
        if (buf.str().find("OK+3323\r") != std::string::npos && buf.str().find("+PIPE\r") != std::string::npos) {
            pipe_mode = PIPE_ON;
            break;
        }

        if (pipe_mode == PIPE_ON) {
            SPDLOG_INFO("PIPE is enabled on ground station transceiver");
            break;
        }

        SPDLOG_WARN("PIPE enable unsuccessful. Retrying ({}/{} retries)", retries + 1, endurosat::MAX_RETRIES);
    }

    if (pipe_mode != PIPE_ON) {
        SPDLOG_ERROR("Failed to enable PIPE on ground station transceiver.");
        mtx.unlock();
        return ES_UNSUCCESSFUL;
    }

    pipe_keeper = std::thread(&transmission_interface::maintain_pipe, this);
    SPDLOG_TRACE("Created maintain PIPE thread with ID {}", std::hash<std::thread::id>{}(pipe_keeper.get_id()));
    SPDLOG_DEBUG("PIPE is active.");
    mtx.unlock();
    return ES_OK;
}

Status transmission_interface::disable_pipe() {
    SPDLOG_DEBUG("Exiting PIPE mode");
    auto max_time_to_wait_ms = pipe_duration_sec * 1000 * 2; // Wait up to two times the max pipe duration

    if (pipe_mode != PIPE_ON)
        return ES_OK;

    pipe_mode = PIPE_WAITING;
    pipe_keeper.join();
    std::string resp;
    std::stringstream buf;

    auto start_time = high_resolution_clock::now();
    while (true) {
        if (check_serial_bytes_avail() > 0)
            buf << read_to_delimeter('\r');

        if (buf.str().find("+ESTTC") != std::string::npos)
            break;

        sleep_until(system_clock::now() + milliseconds (50)); // TODO reduce delay

        if (duration_cast<milliseconds>(high_resolution_clock::now() - start_time).count() > max_time_to_wait_ms) {
            SPDLOG_ERROR("Oof PIPE didn't exit properly...");
            pipe_mode = PIPE_OFF;
            return ES_UNSUCCESSFUL;
        }
    }
    SPDLOG_TRACE("PIPE successfully exited.");
    pipe_mode = PIPE_OFF;
    return ES_OK;
}

void transmission_interface::maintain_pipe() {
    auto check_interval_ms = (int)((1000 * pipe_duration_sec) * .7); // Check 70% of the way through the pipe duration
    while (pipe_mode == PIPE_ON) {
        // Only send something across interface if the time since the last packet is greater than check_interval_ms
        if (duration_cast<milliseconds>(high_resolution_clock::now() - tx_trace_timestamp).count() > check_interval_ms) {
            this->write_serial_uc((unsigned char *) " ", 1);
        }

        sleep_until(system_clock::now() + milliseconds (1));
        // Serial handler tracks the mode implicitly. Verify that desired state is still active.
    }
}

void transmission_interface::set_session_status(bool status) {
    this->session_active = status;
}

void transmission_interface::flush_transmission_interface() {
    if (connectionless_telem_cb)
        clear_serial_fifo(connectionless_telem_cb);
    else
        clear_serial_fifo();
}

void transmission_interface::refresh_constants() {
    SPDLOG_INFO("Syncing constants with transceiver");
    validate_pipe_duration(PIPE_DURATION_SEC);
}

estts::Status transmission_interface::validate_pipe_duration(int duration) {
    // First, get current PIPE duration
    get_pipe_duration();

    // If the current duration doesn't equal the desired duration, set it
    if (pipe_duration_sec != duration) {
        SPDLOG_INFO("Setting pipe duration to {} seconds", duration);
        char hex_buf[10];
        sprintf(hex_buf, "%08X", duration);
        std::string cmd = "ES+W2206" + std::string(hex_buf) + "\r";
        for (int i = 0; i < ESTTS_MAX_RETRIES; i++) {
            if (ES_OK == write_serial_s(cmd)) {
                auto resp = read_to_delimeter('\r');
                if (resp.find("OK+")) {
                    SPDLOG_INFO("Pipe duration set to {} seconds", duration);
                    pipe_duration_sec = duration;
                    return ES_OK;
                }
            }
        }
    }
}

estts::Status transmission_interface::get_pipe_duration() {
    unsigned int pipe_duration = 0;
    for (int i = 0; i < ESTTS_MAX_RETRIES; i++) {
        if (ES_OK == write_serial_s("ES+R2206\r")) {
            auto resp = read_to_delimeter('\r');
            if (resp.find("ES+")) {
                try {
                    resp.erase(0, 5);
                    // Remove the last character of resp
                    resp.pop_back();
                    pipe_duration = hex_string_to_int(resp);
                } catch (std::exception &e) {
                    SPDLOG_ERROR("Failed to parse pipe duration: {}", e.what());
                    SPDLOG_DEBUG("Retry");
                    continue;
                }

                break;
            }
        }
    }

    if (pipe_duration == 0) {
        SPDLOG_WARN("Failed to get pipe duration. Setting to default value of 10 seconds.");
        pipe_duration_sec = 10;
        return estts::ES_UNSUCCESSFUL;
    } else {
        SPDLOG_DEBUG("Pipe duration is {} seconds", pipe_duration);
        pipe_duration_sec = pipe_duration;
    }

    return ES_OK;
}

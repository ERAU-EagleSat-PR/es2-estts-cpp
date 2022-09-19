/* Copyright © EagleSat II - Embry Riddle Aeronautical University - All rights reserved - 2022 */
//
// Created by Hayden Roszell on 12/21/21.
//

#include <chrono>
#include <thread>

#include "transmission_interface.h"

using namespace std::this_thread; // sleep_for, sleep_until
using namespace std::chrono; // nanoseconds, system_clock, seconds
using namespace estts;
using namespace estts::endurosat;

transmission_interface::transmission_interface() : socket_handler(ti_socket::TI_SOCKET_ADDRESS,
                                                                                        ti_socket::TI_SOCKET_PORT) {
    connectionless_telem_cb = nullptr;
    pipe_mode = PIPE_OFF;
    pipe_duration_sec = 5;
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
    sleep_until(system_clock::now() + milliseconds (20));
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

        // This for loop is redundant in like 99% of cases, but it vastly reduces the risk of double enabling
        // PIPE on whatever device is listening.
        for (int i = 0; i < 3; i++) {
            buf << read_to_delimeter('\r') << read_to_delimeter('\r'); // This is looking for two delimiters, so call read twice
            if (buf.str().find("OK+3323\r") != std::string::npos && buf.str().find("+PIPE\r") != std::string::npos) {
                pipe_mode = PIPE_ON;
                break;
            }
            sleep_until(system_clock::now() + milliseconds (50));
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
    int retries = pipe_duration_sec * 2;

    if (pipe_mode != PIPE_ON)
        return ES_OK;

    pipe_mode = PIPE_WAITING;
    pipe_keeper.join();
    std::string resp;
    std::stringstream buf;

    while (true) {
        if (check_serial_bytes_avail() > 0)
            buf << read_to_delimeter('\r');

        if (buf.str().find("+ESTTC") != std::string::npos)
            break;

        sleep_until(system_clock::now() + seconds(1));
        retries--;
        if (retries <= 0) {
            SPDLOG_WARN("Oof PIPE didn't exit properly..");
            pipe_mode = PIPE_OFF;
            return ES_UNSUCCESSFUL;
        }
    }
    SPDLOG_TRACE("PIPE successfully exited.");
    pipe_mode = PIPE_OFF;
    return ES_OK;
}

void transmission_interface::maintain_pipe() {
    auto duration_ms = 1000 * pipe_duration_sec;
    while (pipe_mode == PIPE_ON) {
        // Only send something across interface if the time since the last packet is greater than PIPE_DURATION_SEC * 0.8
        if (duration_cast<milliseconds>(high_resolution_clock::now() - tx_trace_timestamp).count() > duration_ms - (int)(duration_ms * 0.2)) {
            this->write_serial_uc((unsigned char *) " ", 1);
            sleep_until(system_clock::now() + milliseconds (50));
        }

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
    if (ES_OK == write_serial_s("ES+R2206\r")) {
        auto resp = read_to_delimeter('\r');
        if (resp.find("ES+")) {
            resp.erase(0, 11);
            try {
                pipe_duration_sec = stoi(resp);
            } catch (...) { SPDLOG_WARN("{} is not an integer.", resp); }

            SPDLOG_INFO("Setting PIPE duration to {} seconds", pipe_duration_sec);
        }
    }
}
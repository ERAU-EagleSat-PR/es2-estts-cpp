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
    for (int seconds_elapsed = 0; seconds_elapsed < ESTTS_AWAIT_RESPONSE_PERIOD_SEC * 50; seconds_elapsed++) {
        bytes_avail = check_serial_bytes_avail();
        if (bytes_avail > 0) {
            break;
        }
        sleep_until(system_clock::now() + milliseconds (100));
    }
    if (bytes_avail <= 0) {
        SPDLOG_WARN("Receive timeout - {} seconds elapsed with no response", ESTTS_AWAIT_RESPONSE_PERIOD_SEC);
        return "";
    }
    auto buf = this->read_serial_s(bytes_avail);
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

unsigned char *transmission_interface::receive_uc() {
    mtx.lock();
#ifndef __TI_DEV_MODE__
    auto buf = this->read_serial_uc();
    mtx.unlock();
    return buf;
#else
    unsigned char * received;
    do {
    }
    while (!(received = this->read_socket_uc()));
    mtx.unlock();
    return received;
#endif
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
        if (retries > endurosat::MAX_RETRIES) {
            mtx.unlock();
            return ES_UNSUCCESSFUL;
        }
        SPDLOG_TRACE("Attempting to enable PIPE on ground station");
        write_serial_s(pipe_en);
        sleep_until(system_clock::now() + milliseconds (50));

        // Try to get data 3 times
        for (int i = 0; i < 3; i++) {
            if (data_available()) {
                buf << read_serial_s();
                if (buf.str().find("OK+3323\r") != std::string::npos && buf.str().find("+PIPE\r") != std::string::npos) {
                    pipe_mode = PIPE_ON;
                    break;
                }
            }
        }
        if (pipe_mode == PIPE_ON)
            break;
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
    int retries = endurosat::PIPE_DURATION_SEC * 2;

    if (pipe_mode != PIPE_ON)
        return ES_OK;

    pipe_mode = PIPE_WAITING;
    pipe_keeper.join();
    std::string resp;
    std::stringstream buf;
    while (true) {
        auto avail = this->check_serial_bytes_avail();
        if (avail > 0) {
            buf << read_serial_s();
        }
        if (buf.str().find("+ESTTC") != std::string::npos) {
            break;
        }
        sleep_until(system_clock::now() + seconds(1));
        retries--;
        if (retries <= 0) {
            SPDLOG_ERROR("Oof PIPE didn't exit properly..");
            pipe_mode = PIPE_OFF;
            return ES_UNSUCCESSFUL;
        }
    }
    SPDLOG_TRACE("PIPE successfully exited.");
    pipe_mode = PIPE_OFF;
    return ES_OK;
}

void transmission_interface::maintain_pipe() {
    int counter = 0;
    while (pipe_mode == PIPE_ON) {
        counter++;
        if ((counter / 10) > (endurosat::PIPE_DURATION_SEC - 4)) {
            this->write_serial_uc((unsigned char *) " ", 1);
            counter = 0;
        }
        sleep_until(system_clock::now() + milliseconds (100));
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


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

transmission_interface::transmission_interface() : socket_handler(estts::ti_socket::TI_SOCKET_ADDRESS,
                                                                                        estts::ti_socket::TI_SOCKET_PORT) {
    primary_telem_cb = nullptr;
    obc_session_active = false;
    mtx.unlock();
}

estts::Status transmission_interface::transmit(const std::string &value) {
    using namespace std::this_thread; // sleep_for, sleep_until
    using namespace std::chrono; // nanoseconds, system_clock, seconds
    if (value.empty())
        return ES_MEMORY_ERROR;
    if (!obc_session_active)
        SPDLOG_WARN("Communication session not active, message may not get to satellite.");
    mtx.lock();
    if (primary_telem_cb)
        clear_serial_fifo(primary_telem_cb);
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
    return estts::ES_OK;
}

std::string transmission_interface::receive() {
    mtx.lock();
#ifndef __TI_DEV_MODE__
    int bytes_avail;
    for (int seconds_elapsed = 0; seconds_elapsed < estts::ESTTS_AWAIT_RESPONSE_PERIOD_SEC * 50; seconds_elapsed++) {
        bytes_avail = check_serial_bytes_avail();
        if (bytes_avail > 0) {
            break;
        }
        sleep_until(system_clock::now() + milliseconds (100));
    }
    if (bytes_avail <= 0) {
        mtx.unlock();
        return "";
    }
    auto buf = this->read_serial_s(bytes_avail);
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

transmission_interface::~transmission_interface() {
    mtx.lock();

#ifdef __TI_DEV_MODE__
    this->write_socket_s("close");
#endif

    mtx.unlock();
}

estts::Status transmission_interface::request_obc_session() {
    mtx.lock();

    SPDLOG_INFO("Requesting new session");
    std::string pipe_en = "ES+W22003323\r";
    int retries = 0;
    std::string resp;
    while (true) {
        if (primary_telem_cb)
            clear_serial_fifo(primary_telem_cb);
        else
            clear_serial_fifo();

        write_serial_s(pipe_en);
        sleep_until(system_clock::now() + milliseconds (50));
        write_serial_s(pipe_en);
        resp = read_serial_s(8); // length of "OK+3323\r"
        if (resp.find("OK+3323\r") == std::string::npos) {
            goto tryagain;
        }
        sleep_until(system_clock::now() + milliseconds (50));
        resp = read_serial_s(6); // length of "+PIPE\r"
        if (resp.find("+PIPE\r") == std::string::npos) {
            goto tryagain;
        }
        SPDLOG_TRACE("PIPE is enabled on the ground station transceiver");
        sleep_until(system_clock::now() + milliseconds (100));
        resp = read_serial_s(8); // length of "OK+3323\r"
        if (resp.find("OK+3323") != std::string::npos) {
            SPDLOG_TRACE("PIPE is probably enabled on the satellite");
            break;
        }

tryagain:
        SPDLOG_ERROR("Failed to enable PIPE. Waiting {} seconds", estts::endurosat::WAIT_TIME_SEC);
        sleep_until(system_clock::now() + seconds(estts::endurosat::WAIT_TIME_SEC));
        retries++;
        if (retries > estts::endurosat::MAX_RETRIES) {
            mtx.unlock();
            return estts::ES_UNSUCCESSFUL;
        }
    }

    obc_session_active = true;
    session_keeper = std::thread(&transmission_interface::maintain_pipe, this);
    SPDLOG_TRACE("Created session keeper thread with ID {}", std::hash<std::thread::id>{}(session_keeper.get_id()));
    sleep_until(system_clock::now() + seconds(2));

    SPDLOG_INFO("Session active");

    mtx.unlock();
    return ES_OK;
}

Status transmission_interface::end_obc_session(const std::string &end_frame) {
    SPDLOG_INFO("Ending session");

    int retries = estts::endurosat::PIPE_DURATION_SEC * 2;
#ifdef __TI_DEV_MODE__
    if (write_socket_s(ax25::NEW_SESSION_FRAME) != ES_OK) {
        SPDLOG_ERROR("Failed to end session");
        return ES_UNSUCCESSFUL;
    }

    std::string received;
    do {}
    while ((received = this->read_socket_s()).empty());
    if (std::string::npos == received.find(end_frame)) {
        SPDLOG_ERROR("Failed to end session");
        return ES_UNSUCCESSFUL;
    }
    obc_session_active = false;
#else
    obc_session_active = false;
    session_keeper.join();
    std::string resp;

    do {
        sleep_until(system_clock::now() + seconds(1));
        retries--;
        if (retries <= 0) {
            mtx.unlock();
            SPDLOG_ERROR("Oof PIPE didn't exit properly..");
            return estts::ES_UNSUCCESSFUL;
        }
    } while ((resp = read_serial_s()).empty() || resp.find("+ESTTC") == std::string::npos);
    // todo apparently this doesn't work very well
    // ohhh its because readserials blocks, this needs to be rethought
#endif
    sleep_until(system_clock::now() + seconds(1));
    SPDLOG_INFO("Successfully ended session");
    return ES_OK;
}

bool transmission_interface::check_data_available() {
#ifdef __TI_DEV_MODE__
    if (check_sock_bytes_avail() > 0)
        return true;
    else
        return false;
#endif
}

estts::Status transmission_interface::transmit(const unsigned char *value, int length) {
    using namespace std::this_thread; // sleep_for, sleep_until
    using namespace std::chrono; // nanoseconds, system_clock, seconds
    if (length <= 0)
        return ES_MEMORY_ERROR;
    int retries = 0;
    if (!obc_session_active)
        SPDLOG_WARN("Communication session not active, message may not get to satellite.");
    mtx.lock();
#ifndef __TI_DEV_MODE__
    retries = 0;
    clear_serial_fifo();
    while (this->write_serial_uc((unsigned char *)value, length) < length) {
        spdlog::error("Failed to transmit. Waiting {} seconds", estts::endurosat::WAIT_TIME_SEC);
        sleep_until(system_clock::now() + seconds(estts::endurosat::WAIT_TIME_SEC));
        retries++;
        if (retries > estts::endurosat::MAX_RETRIES) {
            mtx.unlock();
            return estts::ES_UNSUCCESSFUL;
        }
        SPDLOG_INFO("Retrying transmit (retry {}/{})", retries, estts::endurosat::MAX_RETRIES);
    }
#else
    SPDLOG_DEBUG("Transmitting {}", value);
    while (this->write_socket_uc((unsigned char *)value, length) < length) {
        spdlog::error("Failed to transmit. Waiting {} seconds", estts::ti_socket::WAIT_TIME_SEC);
        sleep_until(system_clock::now() + seconds(estts::ti_socket::WAIT_TIME_SEC));
        retries++;
        if (retries > estts::ti_socket::MAX_RETRIES) {
            mtx.unlock();
            return estts::ES_UNSUCCESSFUL;
        }
        SPDLOG_INFO("Retrying transmit (retry {}/{})", retries, estts::ti_socket::MAX_RETRIES);
    }
#endif
    mtx.unlock();
    return estts::ES_OK;
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

void transmission_interface::maintain_pipe() {
    int counter = 0;
    while (obc_session_active) {
        counter++;
        if ((counter / 10) > (estts::endurosat::PIPE_DURATION_SEC - 4)) {
            this->write_serial_uc((unsigned char *) " ", 1);
            counter = 0;
        }
        sleep_until(system_clock::now() + milliseconds (100));
    }
}

std::string transmission_interface::nonblock_receive() {
    int bytes;
    sleep_until(system_clock::now() + milliseconds (20));
    if ((bytes = check_serial_bytes_avail()) > 0) {
        return this->read_serial_s(bytes);
    }
    return "";
}

estts::Status transmission_interface::gs_transmit(const std::string &value) {
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
    return estts::ES_OK;
}
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
    primary_telem_cb = nullptr;
    obc_session_active = false;
    pipe_mode = PIPE_OFF;
    satellite_in_range = false;
    dispatch_threadpool_active = false;

    inrange_checker = std::thread(&transmission_interface::detect_satellite_in_range, this);
    SPDLOG_TRACE("Created thread to detect if the satellite is in range with ID {}", std::hash<std::thread::id>{}(pipe_keeper.get_id()));

    mtx.unlock();
}

Status transmission_interface::transmit(const std::string &value) {
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
        mtx.unlock();
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

bool transmission_interface::check_data_available() {
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
    if (!obc_session_active)
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
    pipe_mode = PIPE_OFF;

    for (int retries = 0; retries < endurosat::MAX_RETRIES; retries++) {
        if (retries > endurosat::MAX_RETRIES) {
            return ES_UNSUCCESSFUL;
        }
        SPDLOG_TRACE("Attempting to enable PIPE on ground station");
        write_serial_s(pipe_en);
        sleep_until(system_clock::now() + milliseconds (50));

        // Try to get data 3 times
        for (int i = 0; i < 3; i++) {
            if (check_data_available()) {
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
        return ES_UNSUCCESSFUL;
    }

    pipe_keeper = std::thread(&transmission_interface::maintain_pipe, this);
    SPDLOG_TRACE("Created maintain PIPE thread with ID {}", std::hash<std::thread::id>{}(pipe_keeper.get_id()));
    SPDLOG_DEBUG("PIPE is active.");

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

Status transmission_interface::request_obc_session() {
    mtx.lock();

    // Wait up to ESTTS_SATELLITE_CONNECTION_TIMEOUT_MIN minutes for the satellite to come in range.
    // Check every 1 minute
    auto wait = 0;
    while (!satellite_in_range) {
        if (wait > ESTTS_SATELLITE_CONNECTION_TIMEOUT_MIN) {
            SPDLOG_INFO("Satellite not detected within {} minutes.");
            mtx.unlock();
            return estts::ES_UNSUCCESSFUL;
        }
        sleep_until(system_clock::now() + minutes(1));
        wait++;
    }

    SPDLOG_INFO("Requesting new session");
    std::string pipe_en = "ES+W22003323\r";
    int retries = 0;
    std::stringstream buf;

    if (primary_telem_cb)
        clear_serial_fifo(primary_telem_cb);
    else
        clear_serial_fifo();

    // Enable PIPE has built-in retries. Don't cascade retries, if this function failed
    // something is pretty messed up.
    if (ES_OK != enable_pipe()) {
        mtx.unlock();
        return ES_UNSUCCESSFUL;
    }

    // Sanity check - make sure PIPE is enabled
    if (PIPE_ON != pipe_mode) {
        SPDLOG_ERROR("enable_pipe() succeeded, but trace variable is not set to PIPE_ON");
        mtx.unlock();
        return ES_SERVER_ERROR;
    }

    // Clear FIFO buffer
    if (primary_telem_cb)
        clear_serial_fifo(primary_telem_cb);
    else
        clear_serial_fifo();

    // Now, try to enable PIPE on the satellite.
    while (true) {
        if (retries > endurosat::MAX_RETRIES) {
            SPDLOG_ERROR("Failed to enable PIPE on satellite transceiver. ({}/{} retries)", retries, endurosat::MAX_RETRIES);
            mtx.unlock();
            return ES_UNSUCCESSFUL;
        }
        write_serial_s(pipe_en);
        sleep_until(system_clock::now() + milliseconds (100));
        buf << internal_receive();
        if (buf.str().find("OK+3323\r") != std::string::npos) {
            SPDLOG_TRACE("PIPE is probably enabled on the satellite");
            obc_session_active = true;
            break;
        }
        retries++;
        SPDLOG_ERROR("Failed to enable PIPE on satellite. Waiting {} seconds (retry {}/{})", endurosat::WAIT_TIME_SEC, retries, endurosat::MAX_RETRIES);
        sleep_until(system_clock::now() + seconds(endurosat::WAIT_TIME_SEC));
        // Once again don't clear buf, maybe confirmation got lost in the weeds.
    }

    sleep_until(system_clock::now() + seconds(2));

    // At this point, there is already a thread maintaining the PIPE state.
    // Exit at this point.

    SPDLOG_INFO("Session active");

    if (primary_telem_cb)
        clear_serial_fifo(primary_telem_cb);
    else
        clear_serial_fifo();

    mtx.unlock();
    return ES_OK;
}

Status transmission_interface::end_obc_session(const std::string &end_frame) {
    mtx.lock();
    SPDLOG_INFO("Ending session");

    int retries = endurosat::PIPE_DURATION_SEC * 2;
    disable_pipe();
    sleep_until(system_clock::now() + seconds(1));

    SPDLOG_INFO("Successfully ended session");
    obc_session_active = false;
    mtx.unlock();
    return ES_OK;
}

void transmission_interface::detect_satellite_in_range() {
    std::string get_scw = "ES+R2200\r";
    std::string enable_bcn = "ES+W22003340\r";

    for (;;) {
        if (!obc_session_active && pipe_mode == PIPE_OFF && !gstxvr_session_active) {
            SPDLOG_TRACE("detect_satellite_in_range - locking mutex");
            mtx.lock();
            if (ES_OK == enable_pipe()) {
                // Try to read some data from the satellite transceiver 3 times
                std::stringstream buf;
                for (int i = 0; i < 3; i++) {
                    write_serial_s(get_scw);
                    sleep_until(system_clock::now() + milliseconds (500));
                    if (check_data_available())
                        buf << read_serial_s();
                    if (buf.str().find("OK+") != std::string::npos) {
                        satellite_in_range = true;
                        start_dispatch_threads();
                        break;
                    } else {
                        satellite_in_range = false;
                        end_dispatch_threads();
                    }
                    sleep_until(system_clock::now() + seconds (ESTTS_RETRY_WAIT_SEC));
                }
                if (satellite_in_range)
                    SPDLOG_INFO("Satellite in range.");
                else
                    SPDLOG_INFO("Satellite not in range");
            }
            disable_pipe();
            SPDLOG_TRACE("detect_satellite_in_range - unlocking mutex");
            mtx.unlock();
            sleep_until(system_clock::now() + seconds (ESTTS_CHECK_SATELLITE_INRANGE_INTERVAL_SEC));
        } else
            sleep_until(system_clock::now() + seconds (ESTTS_RETRY_WAIT_SEC));
    }
}

void transmission_interface::register_dispatch_function(const std::function<void()>& fct) {
    dispatch_functions.push_back(fct);
}

void transmission_interface::start_dispatch_threads() {

    if (!dispatch_threadpool_active) {
        for (auto& i : dispatch_functions) {
            std::thread t(i);
            SPDLOG_TRACE("Created dispatch worker thread with ID {}", std::hash<std::thread::id>{}(t.get_id()));
            dispatch_threadpool.push_back(std::move(t));
        }
        dispatch_threadpool_active = true;
    }
}

void transmission_interface::end_dispatch_threads() {
    if (dispatch_threadpool_active) {
        for (auto &i : dispatch_threadpool) {
            if (i.joinable())
                i.join();
        }
        dispatch_threadpool_active = false;
    }
}

estts::Status transmission_interface::request_gstxvr_session() {
    mtx.lock();
    int retries = 0;
    while (obc_session_active || pipe_mode != PIPE_OFF) {
        sleep_until(system_clock::now() + milliseconds (500));
        if (retries > ESTTS_REQUEST_SESSION_TIMEOUT_SECONDS * 2) {
            SPDLOG_WARN("Failed to request ground station transceiver session - timed out");
            return ES_UNSUCCESSFUL;
        }
    }
    gstxvr_session_active = true;
    mtx.unlock();
    return ES_OK;
}

estts::Status transmission_interface::end_gstxvr_session() {
    mtx.lock();
    gstxvr_session_active = false;
    mtx.unlock();
    return ES_OK;
}

//
// Created by Hayden Roszell on 12/21/21.
//

#include <chrono>
#include <thread>
#include "transmission_interface.h"

using namespace std::this_thread; // sleep_for, sleep_until
using namespace std::chrono; // nanoseconds, system_clock, seconds
using namespace estts;

transmission_interface::transmission_interface() : ti_socket_handler(ti_socket::TI_SOCKET_ADDRESS,
                                                                                        ti_socket::TI_SOCKET_PORT),
                                                                      ti_esttc(ti_serial::TI_SERIAL_ADDRESS,
                                                                               endurosat::ES_BAUD),
                                                                      ti_serial_handler(ti_serial::TI_SERIAL_ADDRESS,
                                                                                        endurosat::ES_BAUD) {
    mtx.lock();
    if (initialize_ti() != ES_OK) {
        spdlog::error("Failed initialize transmission interface.");
        throw std::runtime_error("Failed to open serial port.");
    }

    session_active = false;
    stream_active = false;
    mtx.unlock();
}

/**
 * @brief Uses EnduroSat transceiver to transmit string value. String is not sent if transceiver is deemed to be
 * unhealthy.
 * @param value String value to transmit.
 * @return ES_OK if transmission was successful
 */
Status transmission_interface::transmit(const std::string &value) {
    int retries = 0;
    mtx.lock();

    if (!session_active) {
        SPDLOG_ERROR("No session is active; can't transmit. Use request_new_session() to request new session.");
        return ES_UNINITIALIZED;
    }
#ifndef __TI_DEV_MODE__
    if (check_ti_health() != ES_OK) return ES_UNSUCCESSFUL;
    SPDLOG_TRACE("Transceiver passed checks.");
    retries = 0;
    while (this->enable_pipe() != ES_OK) {
        spdlog::error("Failed to enable pipe. Waiting {} seconds", endurosat::WAIT_TIME_SEC);
        sleep_until(system_clock::now() + seconds(endurosat::WAIT_TIME_SEC));
        retries++;
        if (retries > endurosat::MAX_RETRIES) {
            mtx.unlock();
            return ES_UNSUCCESSFUL;
        }
        SPDLOG_INFO("Retrying transmit (retry {}/{})", retries, endurosat::MAX_RETRIES);
    }
    SPDLOG_DEBUG("Transmitting frame with value:\n{}", value);
    retries = 0;
    while (this->write_serial_s(value) != ES_OK) {
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
    while (this->write_socket_s(value) != ES_OK) {
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

/**
 * @brief Initializes transmission interface. Uses esttc_handler to set transceiver constants as specified by constants
 * file.
 * @return ES_OK if transmission interface was initialized successfully
 */
Status transmission_interface::initialize_ti() {
    // TODO create initializers for EnduroSat transceiver
    return ES_OK;
}

/**
 * @brief Checks the health of the transmission interface.
 * @return ES_OK if all health checks pass
 */
Status transmission_interface::check_ti_health() {
    // TODO create list of health checks for transmission interface and EnduroSat transceiver
    int retries = 0;
    while (this->get_temp() > endurosat::MAX_ES_TXVR_TEMP) {
        spdlog::error("EnduroSat Transceiver over temp. Waiting {} seconds", endurosat::WAIT_TIME_SEC);
        sleep_until(system_clock::now() + seconds(endurosat::WAIT_TIME_SEC));
        retries++;
        if (retries > endurosat::MAX_RETRIES) return ES_UNSUCCESSFUL;
        SPDLOG_INFO("Retrying transmit (retry {}/{})", retries, endurosat::MAX_RETRIES);
    }
    return ES_OK;
}

std::string transmission_interface::receive() {
    mtx.lock();
#ifndef __TI_DEV_MODE__
    auto buf = this->read_serial_s();
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

    session_active = false;

#ifdef __TI_DEV_MODE__
    // this->write_socket_s("close");
#endif

    mtx.unlock();
}

Status transmission_interface::request_new_session(const std::string& handshake) {
#ifdef __TI_DEV_MODE__
    SPDLOG_INFO("Requesting new session");

    SPDLOG_TRACE("Waiting for gap in stream");
    auto tstart  = std::chrono::high_resolution_clock::now();
    bool gap_potential = false;
    while (true) {
        if (!check_data_available()) {
            if (!gap_potential) {
                tstart = std::chrono::high_resolution_clock::now();
                gap_potential = true;
            }
            else {
                auto tstop = std::chrono::high_resolution_clock::now();
                if (std::chrono::duration_cast<std::chrono::seconds>(tstop - tstart).count() >= 2) {
                    SPDLOG_TRACE("Found gap, attempting handshake");
                    break;
                }
            }
        } else {
            receive();
            gap_potential = false;
        }
        sleep_until(system_clock::now() + milliseconds (10));
    }

    if (write_socket_s(handshake) != ES_OK) {
        SPDLOG_ERROR("Failed to request session");
        return ES_UNSUCCESSFUL;
    }

    std::string received;
    do {}
    while ((received = this->read_socket_s()).empty());
    if (handshake != received) {
        SPDLOG_ERROR("Failed to request session");
        return ES_UNSUCCESSFUL;
    }
    SPDLOG_TRACE("Handshake succeeded, session active");
    session_active = true;
    return ES_OK;
#endif
}

Status transmission_interface::end_session(const string &end_frame) {
#ifdef __TI_DEV_MODE__
    SPDLOG_INFO("Ending session");
    if (write_socket_s(end_frame) != ES_OK) {
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
    session_active = false;
    SPDLOG_TRACE("Successfully ended session");
#endif

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


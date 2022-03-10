//
// Created by Hayden Roszell on 12/21/21.
//

#include <chrono>
#include <thread>

#include "transmission_interface.h"

using namespace std::this_thread; // sleep_for, sleep_until
using namespace std::chrono; // nanoseconds, system_clock, seconds
using namespace estts;

transmission_interface::transmission_interface() : ti_socket_handler(estts::ti_socket::TI_SOCKET_ADDRESS,
                                                                                        estts::ti_socket::TI_SOCKET_PORT),
                                                                      ti_esttc(estts::ti_serial::TI_SERIAL_ADDRESS,
                                                                               estts::endurosat::ES_BAUD),
                                                                      ti_serial_handler(estts::ti_serial::TI_SERIAL_ADDRESS,
                                                                                        estts::endurosat::ES_BAUD) {
    mtx.lock();
    if (initialize_ti() != estts::ES_OK) {
        spdlog::error("Failed initialize transmission interface.");
        throw std::runtime_error("Failed to open serial port.");
    }
    mtx.unlock();
}

/**
 * @brief Uses EnduroSat transceiver to transmit string value. String is not sent if transceiver is deemed to be
 * unhealthy.
 * @param value String value to transmit.
 * @return ES_OK if transmission was successful
 */
estts::Status transmission_interface::transmit(const std::string &value) {
    using namespace std::this_thread; // sleep_for, sleep_until
    using namespace std::chrono; // nanoseconds, system_clock, seconds
    int retries = 0;
    if (!session_active)
        SPDLOG_WARN("Communication session not active, message may not get to satellite.");
    mtx.lock();
#ifndef __TI_DEV_MODE__
    if (check_ti_health() != estts::ES_OK) return estts::ES_UNSUCCESSFUL;
    SPDLOG_TRACE("Transceiver passed checks.");
    SPDLOG_DEBUG("Transmitting packet with value:\n{}", value);
    auto resp = this->write_serial_s(value);

#else

    SPDLOG_DEBUG("Transmitting packet with value {}", value);
    auto resp = this->write_socket_s(value);

#endif
    mtx.unlock();
    return resp;
}

/**
 * @brief Initializes transmission interface. Uses esttc_handler to set transceiver constants as specified by constants
 * file.
 * @return ES_OK if transmission interface was initialized successfully
 */
estts::Status transmission_interface::initialize_ti() {
    // TODO create initializers for EnduroSat transceiver
#if __TI_DEV_MODE__
    if (ES_OK != this->init_socket_handle()) return estts::ES_UNINITIALIZED;
#endif
    return estts::ES_OK;
}

/**
 * @brief Checks the health of the transmission interface.
 * @return ES_OK if all health checks pass
 */
estts::Status transmission_interface::check_ti_health() {
    // TODO create list of health checks for transmission interface and EnduroSat transceiver
    using namespace std::this_thread; // sleep_for, sleep_until
    using namespace std::chrono; // nanoseconds, system_clock, seconds
    int retries = 0;
//    while (this->get_temp() > estts::endurosat::MAX_ES_TXVR_TEMP) {
//        spdlog::error("EnduroSat Transceiver over temp. Waiting {} seconds", estts::endurosat::WAIT_TIME_SEC);
//        sleep_until(system_clock::now() + seconds(estts::endurosat::WAIT_TIME_SEC));
//        retries++;
//        if (retries > estts::endurosat::MAX_RETRIES) return estts::ES_UNSUCCESSFUL;
//        SPDLOG_INFO("Retrying transmit (retry {}/{})", retries, estts::endurosat::MAX_RETRIES);
//    }
    return estts::ES_OK;
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

#ifdef __TI_DEV_MODE__
    this->write_socket_s("close");
#endif

    mtx.unlock();
}

Status transmission_interface::request_new_session() {
    mtx.lock();
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
    session_active = true;
    SPDLOG_TRACE("Handshake succeeded, session active");
#else
    SPDLOG_INFO("Requesting new session");
    SPDLOG_TRACE("Enabling PIPE on ground station transceiver");
    int retries = 0;
    while (this->enable_pipe() != estts::ES_OK) {
        clear_serial_fifo();
        spdlog::error("Failed to enable pipe. Waiting {} seconds", estts::endurosat::WAIT_TIME_SEC);
        sleep_until(system_clock::now() + seconds(estts::endurosat::WAIT_TIME_SEC));
        retries++;
        if (retries > estts::endurosat::MAX_RETRIES) {
            mtx.unlock();
            return estts::ES_UNSUCCESSFUL;
        }
        SPDLOG_INFO("Retrying transmit (retry {}/{})", retries, estts::endurosat::MAX_RETRIES);
    }

    SPDLOG_TRACE("Enabling PIPE on satellite transceiver");
    std::string pipe_en = "ES+W22003323\r";
    this->write_serial_s(pipe_en);
    std::string resp;
    do { //todo timeout
    } while ((resp = read_serial_s()).empty());
    if (resp.find("OK+") == std::string::npos) {
        SPDLOG_ERROR("Failed to enable PIPE on satellite.");
        // todo try again
    }
    SPDLOG_TRACE("PIPE is probably enabled on the satellite");
    session_active = true;
    session_keeper = std::thread(&transmission_interface::maintain_pipe, this);
    SPDLOG_TRACE("Created session keeper thread with ID {}", std::hash<std::thread::id>{}(session_keeper.get_id()));
    sleep_until(system_clock::now() + seconds(2));
    SPDLOG_INFO("Session active");
#endif
    mtx.unlock();
    return ES_OK;
}

Status transmission_interface::end_session(const std::string &end_frame) {
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
    session_active = false;
#else
    session_active = false;
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
    if (!session_active)
        SPDLOG_WARN("Communication session not active, message may not get to satellite.");
    mtx.lock();
#ifndef __TI_DEV_MODE__
    if (check_ti_health() != estts::ES_OK) return estts::ES_UNSUCCESSFUL;
    SPDLOG_TRACE("Transceiver passed checks.");
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
    while (session_active) {
        counter++;
        if ((counter / 10) > (estts::endurosat::PIPE_DURATION_SEC - 4)) {
            this->write_serial_uc((unsigned char *) " ", 1);
            counter = 0;
        }
        sleep_until(system_clock::now() + milliseconds (100));
    }
}

std::string transmission_interface::nonblock_receive() {
    if (check_serial_bytes_avail() > 0) {
        return this->receive();
    }
    return "";
}

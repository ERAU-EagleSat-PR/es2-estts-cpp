//
// Created by Hayden Roszell on 12/21/21.
//

#include <chrono>
#include <thread>

#include "transmission_interface.h"

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
    mtx.lock();
#ifndef __TI_DEV_MODE__
    if (check_ti_health() != estts::ES_OK) return estts::ES_UNSUCCESSFUL;
    SPDLOG_TRACE("Transceiver passed checks.");
    retries = 0;
    while (this->enable_pipe() != estts::ES_OK) {
        spdlog::error("Failed to enable pipe. Waiting {} seconds", estts::endurosat::WAIT_TIME_SEC);
        sleep_until(system_clock::now() + seconds(estts::endurosat::WAIT_TIME_SEC));
        retries++;
        if (retries > estts::endurosat::MAX_RETRIES) {
            mtx.unlock();
            return estts::ES_UNSUCCESSFUL;
        }
        SPDLOG_INFO("Retrying transmit (retry {}/{})", retries, estts::endurosat::MAX_RETRIES);
    }
    SPDLOG_DEBUG("Transmitting frame with value:\n{}", value);
    retries = 0;
    while (this->write_serial_s(value) != estts::ES_OK) {
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
    while (this->write_socket_s(value) != estts::ES_OK) {
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

/**
 * @brief Initializes transmission interface. Uses esttc_handler to set transceiver constants as specified by constants
 * file.
 * @return ES_OK if transmission interface was initialized successfully
 */
estts::Status transmission_interface::initialize_ti() {
    // TODO create initializers for EnduroSat transceiver
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
    while (this->get_temp() > estts::endurosat::MAX_ES_TXVR_TEMP) {
        spdlog::error("EnduroSat Transceiver over temp. Waiting {} seconds", estts::endurosat::WAIT_TIME_SEC);
        sleep_until(system_clock::now() + seconds(estts::endurosat::WAIT_TIME_SEC));
        retries++;
        if (retries > estts::endurosat::MAX_RETRIES) return estts::ES_UNSUCCESSFUL;
        SPDLOG_INFO("Retrying transmit (retry {}/{})", retries, estts::endurosat::MAX_RETRIES);
    }
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

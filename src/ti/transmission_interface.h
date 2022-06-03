/* Copyright © EagleSat II - Embry Riddle Aeronautical University - All rights reserved - 2022 */
//
// Created by Hayden Roszell on 12/21/21.
//

#ifndef ESTTS_TRANSMISSION_INTERFACE_H
#define ESTTS_TRANSMISSION_INTERFACE_H

#include <mutex>
#include <thread>
#include "esttc.h"
#include "socket_handler.h"

class transmission_interface : virtual public esttc, virtual public socket_handler {
private:
    std::function<estts::Status(std::string)> connectionless_telem_cb;

    std::thread pipe_keeper;

    /**
     * Function designed to run on its own thread that sends a single character through the UART to the transceiver
     * to keep PIPE mode active, thereby maintaining a session. This function returns when it notices that obc_session_active
     * is false, and checks every 100 milliseconds to reduce latency.
     */
    void maintain_pipe();

    estts::endurosat::PIPE_State pipe_mode;

    bool session_active;

    // This mutex is a blunt force method of ensuring that only one thing is using
    // the transmission interface at a time. Ideally, higher layer code should be written
    // to prevent the mutex from preventing any communication.
    std::mutex mtx;

public:

    /**
     * Internal receive that should ONLY be used if you know exactly what is calling this function.
     * Operates the same as normal receive but doesn't lock the mutex. Be HIGHLY cautious using this function.
     * @return string
     */
    std::string internal_receive();

    /**
     * Function that sets a telemetry callback used by the request new session function, which
     * clears the serial FIFO register before requesting a session. This ensures that whatever is receiving telemetry
     * gets all available telemetry.
     * @param cb Telemetry callback with form std::function<estts::Status(std::string)>
     */
    void set_connectionless_telem_callback(const std::function<estts::Status(std::string)>& cb) { connectionless_telem_cb = cb;}

    estts::endurosat::PIPE_State get_pipe_state() { return pipe_mode; }

    void set_session_status(bool status);

    /**
     * Default constructor that initializes each class inherited by transmission_interface, and ensures that the mutex
     * is unlocked.
     */
    explicit transmission_interface();

    /**
     * Default destructor that ensures a safe exit of transmission_interface
     */
    ~transmission_interface();

    /**
     * Uses EnduroSat transceiver to transmit const unsigned char * value. Function warns if a session is not currently active.
     * Note that this is designed to be used to transmit data to the satellite, not to other peripherals. Other methods
     * should be created if another communication medium is required.
     * @param value String value to transmit.
     * @return ES_OK if transmission was successful
     */
    estts::Status transmit(const std::string &value);

    /**
     * Uses EnduroSat transceiver to transmit string value. Function warns if a session is not currently active.
     * Note that this is designed to be used to transmit data to the satellite, not to other peripherals. Other methods
     * should be created if another communication medium is required.
     * @param value
     * @param length
     * @return
     */
    estts::Status transmit(const unsigned char *value, int length);

    /**
     * Function that uses underlying communication interface to receive data. This function blocks for ESTTS_AWAIT_RESPONSE_PERIOD_SEC
     * seconds while data is not available on the interface.
     * @return "" if function times out, or a string if data is received.
     */
    std::string receive();

    /**
     * Nonblocking receive that returns immediately if no data is available.
     * @return "" if no data is available, or a string if data is received.
     */
    std::string nonblock_receive();

    /**
     * Function that receives data in the unsigned char * form
     * @return nullptr if no data is found, or an unsigned char *
     */
    unsigned char * receive_uc();

    /**
     * Function that returns the data available on the underlying interface.
     * @return bool
     */
    bool data_available();

    /**
     * Uses EnduroSat transceiver to transmit string Value.
     * Note that this is designed to be used to transmit data to the groundstation transceiver, not to other peripherals. Other methods
     * should be created if another communication medium is required.
     * @param value String value to transmit.
     * @return ES_OK if transmission was successful
     */
    estts::Status gs_transmit(const std::string &value);

    /*
     * CRITICAL NOTE - The function calling enable_pipe OR disable_pipe MUST take the mutex.
     * These functions execute REGARDLESS of the mutex state.
     */

    estts::Status enable_pipe();

    estts::Status disable_pipe();

    void flush_transmission_interface();
};


#endif //ESTTS_TRANSMISSION_INTERFACE_H

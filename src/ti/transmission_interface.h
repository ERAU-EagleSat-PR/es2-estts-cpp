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

    /**
     * Local thread object that the transmission interfaces uses to maintain an active PIPE on the configured transceiver.
     */
    std::thread pipe_keeper;

    /**
     * Local variable that tracks the PIPE state of the transceiver. Value has three possible states configured by PIPE_State.
     */
    estts::endurosat::PIPE_State pipe_mode;

    /**
     * Local variable that tracks if a higher class has an active session using the transmission interface.
     */
    bool session_active;

    /**
     * This mutex is a blunt force method of ensuring that only one thing is using
     * the transmission interface at a time. Ideally, higher layer code should be written
     * to prevent the mutex from preventing any communication.
     */
    std::mutex mtx;

    unsigned int pipe_duration_sec;

    /**
     * Function designed to run on its own thread that sends a single character through the UART to the transceiver
     * to keep PIPE mode active, thereby maintaining a session. This function returns when it notices that obc_session_active
     * is false, and checks every 100 milliseconds to reduce latency.
     */
    void maintain_pipe();

    void refresh_constants();

    estts::Status get_pipe_duration();

protected:

    /**
     * Function pointer used by flush_transmission_interface() if configured to save data that comes through the serial
     * port with no destination.
     */
    std::function<estts::Status(std::string)> connectionless_telem_cb;

public:

    /**
     * Function that validates the PIPE duration value with the transceiver. If the PIPE durations don't match the value
     * passed, the function will attempt to set the PIPE duration to the value passed.
     * The context that this function is run is important. If the communication context is not with a transceiver, whether
     * on the satellite or on the ground, behavior is undefined.
     * @param duration The expected PIPE duration in seconds.
     * @return ES_OK if the PIPE duration is set to the value passed, ES_UNSUCCESSFUL if the PIPE duration is not set to the value passed.
     */
    estts::Status validate_pipe_duration(int duration);

    /**
     * Internal receive that should ONLY be used if you know exactly what is calling this function.
     * Operates the same as normal receive but doesn't lock the mutex. Be HIGHLY cautious using this function.
     * Reads up to \r as delimiter.
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

    /**
     * Getter function that returns the current pipe mode.
     * @return estts::endurosat::PIPE_State pipe_mode
     */
    estts::endurosat::PIPE_State get_pipe_state() { return pipe_mode; }

    /**
     * Setter used to inform the transmission interface of an active session in a higher level.
     * @param ES_OK if successful.
     */
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
     * Reads up to \r as delimiter.
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
     * @return Status
     */
    estts::Status transmit(const unsigned char *value, int length);

    /**
     * Function that uses underlying communication interface to receive data. This function blocks for ESTTS_AWAIT_RESPONSE_PERIOD_SEC
     * seconds while data is not available on the interface.
     * @return "" if function times out, or a string if data is received.
     */
    std::string receive();

    std::string receive_from_obc();

    /**
     * Nonblocking receive that returns immediately if no data is available.
     * @return "" if no data is available, or a string if data is received.
     */
    std::string nonblock_receive();

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

    /**
     * Function that enables PIPE mode on the ground station transceiver, and creates a PIPE maintainer thread that
     * forces it to stay on until disable_pipe() is called. CRITICAL NOTE: disable_pipe() MUST be called to close PIPE mode.
     * Otherwise, it will stay on forever.
     * @return ES_OK if successful.
     */
    estts::Status enable_pipe();

    /**
     * Function that disables PIPE mode on the ground station transceiver by informing the maintainer thread to stop
     * maintaining, and waits for it to exit before returning.
     * @return ES_OK if successful.
     */
    estts::Status disable_pipe();

    /**
     * Function that uses the underlying interface to flush the serial FIFO queue. If connectionless_telem_cb is configured,
     * flushed data will be passed by invoking this function.
     */
    void flush_transmission_interface();
};


#endif //ESTTS_TRANSMISSION_INTERFACE_H

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
    std::vector<std::function<void()>> dispatch_functions;

    std::vector<std::thread> dispatch_threadpool;

    std::function<estts::Status(std::string)> primary_telem_cb;

    std::mutex mtx;

    std::thread pipe_keeper;

    std::thread inrange_checker;

    estts::endurosat::PIPE_State pipe_mode;

    bool dispatch_threadpool_active;

    /**
     * Function designed to run on its own thread that sends a single character through the UART to the transceiver
     * to keep PIPE mode active, thereby maintaining a session. This function returns when it notices that obc_session_active
     * is false, and checks every 100 milliseconds to reduce latency.
     */
    void maintain_pipe();

    /**
     * Waits up to ESTTS_SATELLITE_CONNECTION_TIMEOUT_MIN seconds for satellite
     * to come within range of ground station. Detects in range by asking transceiver
     * @return
     */
    [[noreturn]] void detect_satellite_in_range();

    void start_dispatch_threads();

    void end_dispatch_threads();

    /**
     * Internal receive that should ONLY be used if you know exactly what is calling this funciton.
     * Operates the same as normal receive but doesn't lock the mutex. Be HIGHLY cautious using this function.
     * @return
     */
    std::string internal_receive();

public:
    bool obc_session_active;

    bool satellite_in_range;

    void register_dispatch_function(const std::function<void()>& fct);

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
     * Function that sets a telemetry callback used by the request new session function, which
     * clears the serial FIFO register before requesting a session. This ensures that whatever is receiving telemetry
     * gets all available telemetry.
     * @param cb Telemetry callback with form std::function<estts::Status(std::string)>
     */
    void set_telem_callback(const std::function<estts::Status(std::string)>& cb) { primary_telem_cb = cb;}

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
     * Function that requests a new session with the OBC on the satellite. This function enables PIPE on the ground txvr
     * and on the satellite txvr, and creates a new thread to keep the session as long as obc_session_active is true.
     * @return ES_OK if a session is active
     */
    estts::Status request_obc_session();

    /**
     * Function that returns the status of obc_session_active
     * @return bool
     */
    bool check_session_active() const { return obc_session_active; };

    /**
     * Function that waits for PIPE to exit, thereby ending the communication session with the satellite.
     * @param end_frame Deprecated
     * @return ES_OK if session ends successfully
     */
    estts::Status end_obc_session(const std::string& end_frame);

    /**
     * Function that returns the data available on the underlying interface.
     * @return bool
     */
    bool check_data_available();

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
};


#endif //ESTTS_TRANSMISSION_INTERFACE_H

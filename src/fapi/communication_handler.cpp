//
// Created by Hayden Roszell on 1/12/22.
//

#include <sys/stat.h>
#include <string>
#include <fstream>
#include "communication_handler.h"

using nlohmann::json;

json telem_template = {
    {"latest_downlink_timestamp", ""},
    {"eps", {
        {"latest_downlink_timestamp", ""},
        {"battery_voltage", 0.0},
        {"brownouts", 0.0},
        {"charge_time_min", 0.0},
    }},
    {"acs", {
        {"latest_downlink_timestamp", ""},
        {"", ""}
    }},
    {"obc", {
        {"latest_downlink_timestamp", ""},
        {"safe_mode_time", 0.0},
        {"current_mode", "nominal"},
        {"schedule_faults", 0}
    }},
    {"crp", {
        {"latest_downlink_timestamp", ""},
        {"running", false},
        {"camera1_brightness", 0},
        {"camera2_brightness", 0},
        {"camera3_brightness", 0},
        {"camera4_brightness", 0},
        {"camera5_brightness", 0},
        {"camera6_brightness", 0},
        {"camera7_brightness", 0},
        {"camera8_brightness", 0}
    }},
    {"mde", {
        {"latest_downlink_timestamp", ""},
        {"running", false}
    }}
};

/**
 * @brief Uses some proprties of Unix filesystems to test if a file exists.
 * @param name String filename to test
 * @return True if it exists, false if not.
 */
inline bool test_file_exists(const std::string& name) {
    struct stat buffer{};
    return (stat (name.c_str(), &buffer) == 0);
}

/**
 * @brief Default constructor that initializes the communication handler by creating a new dispatcher,
 * creating a new state file (if one doesn't already exist), and setting the local state to the state of
 * the state file.
 */
communication_handler::communication_handler() {
    dispatch = new session_manager();
    filename = estts::telem_handler::TELEM_HANDLER_STATE_FILE;
    if (!test_file_exists(filename)) {
        std::ofstream o(filename);
        o << std::setw(4) << telem_template << std::endl;
    }
    read_current_state();
}

/**
 * @brief Stores collected EPS Vitals structure to current satellite state, and eventually
 * will update the telemetry database.
 * @param vitals
 * @return
 */
estts::Status communication_handler::store_eps_vitals(estts::es2_telemetry::eps::vitals * vitals) {
    read_current_state();
    if (vitals == nullptr)
        return estts::ES_UNSUCCESSFUL;
    SPDLOG_TRACE("Storing retrieved EPS vitals");
    state["eps"]["battery_voltage"] = vitals->battery_voltage;
    state["eps"]["brownouts"] = vitals->brownouts;
    state["eps"]["charge_time_min"] = vitals->charge_time_mins;
    write_current_state();
    return estts::ES_OK;
}

/**
 * @brief Doesn't do much lol
 * @return Battery voltage
 */
double communication_handler::get_recent_battery_voltage() {

    return state["eps"]["battery_voltage"].get<double>();
}

/**
 * @brief Reads the local state file, and updates the object's satellite state
 * @return ES_OK if successful
 */
estts::Status communication_handler::read_current_state() {
    std::ifstream i(filename);
    i >> state;
    return estts::ES_OK;
}

/**
 * @brief Writes local state to file
 * @return ES_OK if successful
 */
estts::Status communication_handler::write_current_state() {
    std::ofstream o(filename);
    o << std::setw(4) << state << std::endl;
    return estts::ES_OK;
}

/**
 * @brief Default destructor. Deallocates allocated memory.
 */
communication_handler::~communication_handler() {
    write_current_state();
    delete dispatch;
}

/**
 * @brief Communication initializer that creates a new communication worker thread
 * @return ES_OK if successful
 */
estts::Status communication_handler::autonomous_communication_init() {
    // communication_worker = std::thread(&communication_handler::start_communication_automation, this);
    // SPDLOG_TRACE("Created communication worker thread with ID {}", std::hash<std::thread::id>{}(communication_worker.get_id()));
    start_communication_automation();
    return estts::ES_OK;

}

/**
 * @brief Not implemented.
 */
[[noreturn]] void communication_handler::start_communication_automation() {
    using namespace std::this_thread; // sleep_for, sleep_until
    using namespace std::chrono; // nanoseconds, system_clock, seconds
    for (;;) {
        // First of all, we HAVE to make sure that our dispatcher is active or this program will shit the bed.
        if (!dispatch) {
            SPDLOG_ERROR("Dispatcher is not active. Something catastrophic went wrong.");
            throw std::runtime_error("Dispatcher is not active.");
        }
        // Schedule an EPS get vitals command
        command_serial_number_cache.push_back(get_eps_vitals(dispatch_lambda(), [this](estts::es2_telemetry::eps::vitals *vitals) -> estts::Status {
            return store_eps_vitals(vitals);
        }));

        // All threads should be joined at some point....
        await_dispatcher();

        // Wait until the session has completed
        // todo this is super broken and idk why. something to do with the session progress variable in the dispatcher.
        /*
        do {
            sleep_until(system_clock::now() + seconds(1));
        } while (estts::ES_SUCCESS != dispatch->get_dispatch_session_progress());
        SPDLOG_INFO("Telemetry responses came back successfully.");
        */

        // When we know that the commands have all come back,
        for (const auto& i : command_serial_number_cache) {
            if (estts::ES_OK == dispatch->get_command_status(i)) {
                SPDLOG_DEBUG("Command with serial number {} succeeded", i);
            } else {
                SPDLOG_WARN("Command with serial number {} failed", i);
                // todo eventually notify somebody that commands are failing. if they failed even at this stage, there are issues deeper than packet loss
            }
        }
        command_serial_number_cache.clear();
        sleep_until(system_clock::now() + seconds(estts::ESTTS_AWAIT_RESPONSE_PERIOD_SEC));
    }
}

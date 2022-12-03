//
// Created by Hayden Roszell on 5/31/22.
//

#include <chrono>
#include <thread>
#include <utility>

#include "constants.h"
#include "helper.h"
#include "groundstation_manager.h"

using namespace estts;
using namespace std::this_thread; // sleep_for, sleep_until
using namespace std::chrono; // nanoseconds, system_clock, seconds
using namespace estts::endurosat;

groundstation_manager::groundstation_manager() : socket_handler(ti_socket::TI_SOCKET_ADDRESS,
                                                                ti_socket::TI_SOCKET_PORT) {
    satellite_in_range = false;
    highest_session_priority = 0;
    currently_executing = nullptr;

    session_mutex.unlock();
}

void groundstation_manager::detect_satellite_in_range() {
    std::string get_scw = "ES+R2200\r";
    std::string enable_bcn = "ES+W22003340\r";
    session_mutex.lock();
    if (ES_OK == enable_pipe()) {
        // Try to read some data from the satellite transceiver 3 times
        std::stringstream buf;
        for (int i = 0; i < 3; i++) {
            write_serial_s(get_scw);
            sleep_until(system_clock::now() + milliseconds (500));
            if (data_available())
                buf << read_to_delimeter('\r');
            if (buf.str().find("OK+") != std::string::npos) {
                satellite_in_range = true;
                break;
            } else {
                satellite_in_range = false;
            }
            sleep_until(system_clock::now() + seconds (ESTTS_RETRY_WAIT_SEC));
        }
        if (satellite_in_range) {
            SPDLOG_DEBUG("Satellite in range.");
            if (groundstation_telemetry_callback)
                groundstation_telemetry_callback("ES+W69011");
        }
        else {
            SPDLOG_DEBUG("Satellite not in range");
            if (groundstation_telemetry_callback)
                groundstation_telemetry_callback("ES+W69010");
        }
    }
    disable_pipe();
    session_mutex.unlock();
}

estts::Status groundstation_manager::groundstation_manager_init() {
    gm_thread = std::thread(&groundstation_manager::manage, this);
    SPDLOG_TRACE("Created ground station manager thread with ID {}", std::hash<std::thread::id>{}(gm_thread.get_id()));
    return ES_OK;
}

groundstation_manager::internal_session *groundstation_manager::get_highest_priority_session() {
    if (session_list.empty())
        return nullptr;
    internal_session * highest = nullptr;
    for (auto i : session_list)
        if (highest == nullptr || i->priority > highest->priority)
            highest = i;
    return highest;
}

estts::Status groundstation_manager::validate_session_approval(estts::SessionEndpoint endpoint) {
    bool approved = true;
    if (get_highest_priority_session()->endpoint != endpoint) {
        SPDLOG_INFO("Session with endpoint {} is not the highest priority. Validation failed.", get_endpoint_enum_string(endpoint));
        approved = false;
    }
    for (auto i : session_list)
        if (i->endpoint == endpoint) {
            if (i->satellite_range_required_for_execution && !is_satellite_in_range()) {
                SPDLOG_INFO("Session with endpoint {} requires that the satellite is in range. Satellite is not in range; Validation failed.", get_endpoint_enum_string(endpoint));
                approved = false;
            }
            break;
        }

    if (approved)
        return ES_OK;
    else
        return ES_UNAUTHORIZED;
}

estts::Status groundstation_manager::adjust_session_priorities() {
    // For each session in session_list, calculate a new priority represented by the average
    // of the waiting time, base priority assigned at configuration, and the number of commands
    // in queue.

    // If there are no commands ready to execute, the priority is zero.
    for (auto i : session_list) {
        double priority = 0.0;
        if (i->session->get_command_queue_count() <= 0)
            priority = 0.0;
        else if (i->satellite_range_required_for_execution && !is_satellite_in_range())
            priority = 0.0;
        else {
            priority += calculate_waiting_time_weight(i);
            priority += calculate_base_priority_weight(i);
            priority += calculate_queue_count_weight(i);
            priority /= 3;
        }

        i->priority = priority;
    }
    return ES_OK;
}

double groundstation_manager::calculate_waiting_time_weight(internal_session * session) {
    auto duration = duration_cast<seconds>(high_resolution_clock::now() - session->last_active).count();

    // Quadratic function that sets priority to 100 after 16.66 minutes, or 1000 seconds.
    double a = 0.0001;
    double b = 0.0;
    auto weight = a*(double)(duration*duration)+b*(double)(duration);
    if (weight > 100) {
        weight = 100;
    }
    return weight;
}

double groundstation_manager::calculate_base_priority_weight(internal_session * session) const {
    double initial = session->init_priority + 0.000001;
    double highest = highest_session_priority - 0.000001;
    return (initial / highest) * 100;
}

double groundstation_manager::calculate_queue_count_weight(internal_session * session) {
    auto qc = session->session->get_command_queue_count();

    // x^4 function that sets priority to 100 after 9.56 commands are in queue with a slow ramp
    // up between 1 and 2.4 commands.
    double a = 0.0001;
    double b = 0.09;
    double c = 0.122;
    auto weight = a*(qc*qc*qc*qc)+b*(qc*qc*qc)+c*(qc*qc);
    if (weight > 100) {
        weight = 100;
    }
    return weight;
}

void groundstation_manager::manage() {
    auto last_satellite_range_check_timestamp = high_resolution_clock::now();
    detect_satellite_in_range();
    for (;;) {
        // First, adjust all registered session priorities. Priorities are calculated as a mathematical function of
        // the duration since last executing, initial priority, and commands in queue.
        adjust_session_priorities();

        // Then, find the highest priority.
        auto highest = get_highest_priority_session();

        // If the highest priority session has priority of 0.0, there are no commands to execute. Detect if the satellite is in range.
        if (highest != nullptr && highest->priority > 0.0) {
            // If the highest priority session is currently executing, let it execute.
            // If the highest priority session is not currently executing, replace it with the highest session.
            if (currently_executing == nullptr) {
                SPDLOG_DEBUG("There is no session currently executing. Starting session with endpoint {}", get_endpoint_enum_string(highest->endpoint));
                start_session_executor(highest);
            }
            if (currently_executing->endpoint != highest->endpoint) {
                SPDLOG_DEBUG("{} is no longer the highest priority. Switching to {}.", get_endpoint_enum_string(highest->endpoint),
                             get_endpoint_enum_string(highest->endpoint));
                SPDLOG_TRACE("{} has priority {}", get_endpoint_enum_string(highest->endpoint), highest->priority);
                replace_current_session(highest);
            }
        } else {
            auto duration = duration_cast<seconds>(high_resolution_clock::now() - last_satellite_range_check_timestamp).count();
            if (duration > ESTTS_MIN_SATELLITE_RANGE_CHECK_INTERVAL_SEC) {
                SPDLOG_DEBUG("Satellite range hasn't been checked in {} seconds. Checking range.", floor(duration));
                detect_satellite_in_range();
                last_satellite_range_check_timestamp = high_resolution_clock::now();
            }
        }

        if (!currently_executing && connectionless_telem_cb) {
            auto telem = nonblock_receive();
            if (!telem.empty())
                connectionless_telem_cb(telem);
        }

        // If the time since the satellite's range was last checked is greater than ESTTS_MAX_SATELLITE_RANGE_CHECK_INTERVAL_SEC,
        // force the current session to end and check if the satellite is in range.
        auto duration = duration_cast<seconds>(high_resolution_clock::now() - last_satellite_range_check_timestamp).count();
        if (duration > ESTTS_MAX_SATELLITE_RANGE_CHECK_INTERVAL_SEC) {
            SPDLOG_DEBUG("Satellite range hasn't been checked in {} seconds. Stopping {} session to check range.", floor(duration),
                         get_endpoint_enum_string(currently_executing->endpoint));
            stop_current_session();
            detect_satellite_in_range();
            last_satellite_range_check_timestamp = high_resolution_clock::now();
            start_session_executor(currently_executing);
        }

        // Wait a sec so we don't starve the CPU
        sleep_until(system_clock::now() + milliseconds (30));
    }
}

estts::Status groundstation_manager::replace_current_session(internal_session * new_session) {
    if (new_session == nullptr) {
        SPDLOG_WARN("No session provided for replacement.");
        return ES_NOTFOUND;
    }
    // Cancel current session
    auto status = stop_current_session();
    if (ES_OK != status) {
        SPDLOG_ERROR("Failed to stop currently executing session with endpoint %s", get_endpoint_enum_string(currently_executing->endpoint));
        return status;
    }

    // Start new session
    status = start_session_executor(new_session);
    if (ES_OK != status) {
        SPDLOG_ERROR("Failed to start new session executor for endpoint %s", get_endpoint_enum_string(new_session->endpoint));
        return status;
    }
    return ES_OK;
}

estts::Status groundstation_manager::start_session_executor(internal_session * session) {
    if (session == nullptr) {
        SPDLOG_WARN("No session to start.");
        return ES_NOTFOUND;
    }
    session_mutex.lock();
    SPDLOG_TRACE("Starting session executor for endpoint {}", get_endpoint_enum_string(session->endpoint));
    currently_executing = session;
    try {
        currently_executing->executor = new std::thread(&groundstation_manager::session_manager::dispatch, currently_executing->session);
    } catch (const std::exception &e) {
        SPDLOG_ERROR("Failed to create session executor: {}", e.what());
        currently_executing->executor->join();
        currently_executing = nullptr;
        return ES_UNSUCCESSFUL;
    }

    return ES_OK;
}

estts::Status groundstation_manager::stop_current_session() {
    if (currently_executing == nullptr) {
        SPDLOG_WARN("No session is currently executing.");
        return ES_NOTFOUND;
    }
    SPDLOG_TRACE("Stopping current session with endpoint {}", get_endpoint_enum_string(currently_executing->endpoint));
    currently_executing->session->force_session_end();
    if (currently_executing->executor->joinable())
        currently_executing->executor->join();
    return ES_OK;
}

groundstation_manager::session_manager * groundstation_manager::register_session(session_manager * new_session, int priority, bool satellite_range_required_for_execution) {
    if (new_session == nullptr) {
        SPDLOG_WARN("No session provided for registration.");
        return nullptr;
    }
    for (auto &i : session_list) {
        if (i->endpoint == new_session->get_session_endpoint()) {
            SPDLOG_WARN("Session of type {} already exists! Cannot request new session before session with serial number {} exits.",
                        get_endpoint_enum_string(i->endpoint), i->sn);
            return nullptr;
        }
    }

    new_session->set_groundstation_manager(this);

    auto session = new internal_session;
    session->session = new_session;
    session->endpoint = new_session->get_session_endpoint();
    session->init_priority = priority;
    session->priority = session->init_priority;
    session->sn = generate_serial_number();
    session->satellite_range_required_for_execution = satellite_range_required_for_execution;

    if (session->init_priority > highest_session_priority)
        highest_session_priority = session->init_priority;

    session_list.push_back(session);
    return session->session;
}

groundstation_manager::session_manager * groundstation_manager::generate_session_manager(session_config * config) {
    if (config == nullptr) {
        SPDLOG_WARN("No configuration provided.");
        return nullptr;
    }
    auto error = false;
    if (!config->endpoint) {
        SPDLOG_ERROR("Endpoint is required to generate a new session manager.");
        error = true;
    }
    if (config->priority <= 0) {
        SPDLOG_ERROR("Priority greater than 0 is required to generate a new session manager.");
        error = true;
    }
    if (config->receive_func == nullptr) {
        SPDLOG_ERROR("receive_func is required to generate a new session manager.");
        error = true;
    }
    if (config->transmit_func == nullptr) {
        SPDLOG_ERROR("transmit_func is required to generate a new session manager.");
        error = true;
    }
    if (config->end_session_func == nullptr) {
        SPDLOG_ERROR("end_session_func is required to generate a new session manager.");
        error = true;
    }
    if (config->start_session_func == nullptr) {
        SPDLOG_ERROR("start_session_func is required to generate a new session manager.");
        error = true;
    }
    if (error)
        return nullptr;
    return register_session(new groundstation_manager::session_manager (this, config), config->priority, config->satellite_range_required_for_execution);
}

std::string groundstation_manager::get_endpoint_enum_string(estts::SessionEndpoint s) {
    if (s == EAGLESAT2_OBC) return "EAGLESAT2_OBC";
    else if (s == EAGLESAT2_TRANSCEIVER) return "EAGLESAT2_TRANSCEIVER";
    else if (s == GROUND_STATION) return "GROUND_STATION";
    return "";
}

estts::Status groundstation_manager::notify_session_closing(estts::SessionEndpoint endpoint) {
    int highest_priority = 0;
    bool found = false;
    for (auto i : session_list) {
        if (i->init_priority > highest_priority)
            highest_priority = i->init_priority;
        if (i->endpoint == endpoint) {
            delete i;
            found = true;
        }
    }
    if (!found)
        return ES_NOTFOUND;
    highest_session_priority = highest_priority;
    return ES_OK;
}

void groundstation_manager::notify_session_active(estts::SessionEndpoint endpoint) {
    for (auto i : session_list)
        if (i->endpoint == endpoint) {
            i->last_active = high_resolution_clock::now();
            break;
        }
}

void groundstation_manager::notify_session_executor_exiting(estts::SessionEndpoint endpoint) {
    if (currently_executing->endpoint == endpoint) {
        SPDLOG_TRACE("Ground Station Manager was notified that {} executor is exiting.", get_endpoint_enum_string(endpoint));
        session_mutex.unlock();
        currently_executing = nullptr;
    }
}

groundstation_manager::session_manager::session_manager(groundstation_manager * gm, session_config * config) {
    this->gm = gm;
    this->endpoint = config->endpoint;
    this->session_opener = config->start_session_func;
    this->session_closer = config->end_session_func;
    this->transmit_func = config->transmit_func;
    this->receive_func = config->receive_func;
    session_active = false;
}

estts::Status groundstation_manager::session_manager::request_session() {
    auto resp = gm->validate_session_approval(this->endpoint);
    if (resp != ES_OK)
        return resp;
    resp = session_opener();
    if (resp != ES_OK)
        return resp;
    session_active = true;
    return resp;
}

void groundstation_manager::session_manager::dispatch() {
    Status status;
    auto timestamp = high_resolution_clock::now();
    for (;;) {
        if (ES_OK != gm->validate_session_approval(this->endpoint)) {
            SPDLOG_DEBUG("Communication session not approved. Exiting.");
            gm->notify_session_executor_exiting(endpoint);
            return;
        }
        if (!waiting.empty()) {
            SPDLOG_TRACE("{} commands in queue", waiting.size());

            if (!session_active) {
                SPDLOG_INFO("Requesting new session with endpoint {}", gm->get_endpoint_enum_string(endpoint));
                status = request_session();
                if (status != ES_OK) {
                    SPDLOG_ERROR("Failed to request session from session handler with endpoint {}",
                                 gm->get_endpoint_enum_string(this->endpoint));
                    gm->notify_session_executor_exiting(endpoint);
                    sleep_until(system_clock::now() + seconds (1)); // TODO reduce wait
                    return;
                }
            }
            gm->notify_session_active(endpoint);
            SPDLOG_INFO("Session active.");

            auto command = waiting.front();
            bool executed = false;

            // If session object has declared modifiers, call execute_command_modifier with frame.
            if (this->modifier) {
                // execute_command_modifier returns ES_NOTFOUND if command doesn't have a modifier.
                if (ES_NOTFOUND != modifier->execute_command_modifier(command->frame)) {
                    // If command did have a modifier, getting to this point means the command was executed.
                    waiting.pop_front();
                    gm->notify_session_active(endpoint);
                    executed = true;
                }
            }

            if (!executed) {
                auto resp = this->default_command_executor(command->frame, command->serial_number, command->crc_expected_with_response);
                if (resp.empty()) {
                    SPDLOG_WARN("Command with serial number {} failed to execute. Possible session issue, voluntarily exiting.", command->serial_number);
                    session_active = false;
                }

                waiting.pop_front();
                gm->notify_session_active(endpoint);

                if (command->str_callback != nullptr)
                    if (estts::ES_OK != command->str_callback(resp)) {
                        SPDLOG_WARN("Command callback failed. Continuing anyway.");
                    }
                gm->notify_session_active(endpoint);
            }

            timestamp = high_resolution_clock::now();
        } else {
            auto duration = duration_cast<seconds>(high_resolution_clock::now() - timestamp).count();
            if (duration > 5)
                session_active = false;
            else
                sleep_until(system_clock::now() + milliseconds (100)); // TODO reduce wait
        }

        if (!session_active) {
            SPDLOG_INFO("Ending session for for endpoint {}", gm->get_endpoint_enum_string(endpoint));
            status = session_closer();
            if (status != ES_OK)
                SPDLOG_WARN("Session from session handler with endpoint {} didn't exit properly. Exiting anyway.", gm->get_endpoint_enum_string(this->endpoint));
            else
                SPDLOG_INFO("Successfully ended session");
            gm->notify_session_executor_exiting(endpoint);
            return;
        }
    }
}

std::string groundstation_manager::session_manager::default_command_executor(const std::string& command, std::string sn, bool verify_crc) {
    std::string resp;
    estts::Status status;
    std::string sn_string;
    if (!sn.empty())
        sn_string = "with serial number " + sn;

    for (int i = 0; i < ESTTS_MAX_RETRIES; i++) {
        // If the command fails to transmit, the lower levels really couldn't figure the problem out. There's basically
        // nothing we can do from this level to fix it, so exit and hope that the next initialization fixes the problem
        status = transmit_func(command);
        if (status != ES_OK) {
            SPDLOG_WARN("Failed to transmit command {}", sn_string);
            return "";
        }

        gm->notify_session_active(endpoint);
        SPDLOG_INFO("Successfully transmitted command with serial number {}", sn);

        SPDLOG_DEBUG("Waiting for a response");
        sleep_until(system_clock::now() + milliseconds(100)); // TODO reduce wait

        resp = receive_func();
        if (verify_crc && resp.size() >= 8 + 1 && estts::ES_OK == validate_crc(resp.substr(0, resp.size() - 9), resp.substr(resp.size() - 8, 8)))
            break;
        else if (!resp.empty())
            break;

        SPDLOG_DEBUG("Retrying command with serial number {}", sn);
    }

    return resp;
}

std::string groundstation_manager::session_manager::schedule_command(const std::string& command, const std::function<estts::Status(std::string)>& callback, bool crc_expected) {
    if (gm == nullptr) {
        SPDLOG_ERROR("Ground station manager not initialized.");
        return "";
    }
    // Notify that at least one command is in the queue. This resets the waiting period and therefore prevents skewed
    // priority readings if the session hasn't executed in a while.
    if (this->get_command_queue_count() == 0)
        gm->notify_session_active(this->endpoint);

    auto new_command = new waiting_command;
    new_command->frame = command;
    new_command->serial_number = generate_serial_number();
    new_command->str_callback = callback;
    new_command->crc_expected_with_response = crc_expected;

    waiting.push_back(new_command);

    SPDLOG_DEBUG("Scheduled new command with serial number {}", new_command->serial_number);
    return new_command->serial_number;
}

groundstation_manager::session_manager::~session_manager() {
    force_session_end();
    gm->notify_session_closing(endpoint);
}

void groundstation_manager::session_manager::force_session_end() {
    SPDLOG_TRACE("Force session end called for {}", get_endpoint_enum_string(endpoint));
    session_active = false;
}

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

}

void groundstation_manager::detect_satellite_in_range() {
    std::string get_scw = "ES+R2200\r";
    std::string enable_bcn = "ES+W22003340\r";

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
                break;
            } else {
                satellite_in_range = false;
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

estts::Status groundstation_manager::request_session_approval(estts::SessionEndpoint endpoint) {
    return ES_OK;
}

estts::Status groundstation_manager::adjust_session_priorities() {

    return ES_NOTFOUND;
}

void groundstation_manager::manage() {
    auto timestamp = high_resolution_clock::now();
    for (;;) {
        // todo satellite range must be checked at least every 1 minute.
        // todo this means that even if there are more commands to execute of a given type, pause to check range

        adjust_session_priorities();
        auto highest = get_highest_priority_session();
        if (currently_executing != highest) {
            currently_executing->session->force_session_end();
            if (currently_executing->executor->joinable())
                currently_executing->executor->join();

            currently_executing = highest;
            currently_executing->executor = new std::thread(&groundstation_manager::session_manager::dispatch, currently_executing->session);
        }
        // if (duration_cast<seconds>(high_resolution_clock::now() - timestamp).count() > ESTTS_SATELLITE_RANGE_CHECK_PERIOD_SEC)
    }
}

groundstation_manager::session_manager * groundstation_manager::register_session(session_manager * new_session, int priority) {
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

    session_list.push_back(session);
    return session->session;
}

groundstation_manager::session_manager * groundstation_manager::generate_session_manager(session_config * config) {
    return register_session(new groundstation_manager::session_manager (this, config), config->priority);
}

std::string groundstation_manager::get_endpoint_enum_string(estts::SessionEndpoint s) {
    if (s == EAGLESAT2_OBC) return "EAGLESAT2_OBC";
    else if (s == EAGLESAT2_TRANSCEIVER) return "EAGLESAT2_TRANSCEIVER";
    else if (s == GROUND_STATION) return "GROUND_STATION";
    return "";
}

estts::Status groundstation_manager::close_session_handler(estts::SessionEndpoint endpoint) {
    for (auto i : session_list)
        if (i->endpoint == endpoint) {
            delete i;
            return ES_OK;
        }

    return ES_NOTFOUND;
}

estts::Status groundstation_manager::notify_session_active(estts::SessionEndpoint endpoint) {
    for (auto i : session_list)
        if (i->endpoint == endpoint) {
            i->last_active = high_resolution_clock::now();
            return ES_OK;
        }
    return ES_OK;
}

groundstation_manager::session_manager::session_manager(groundstation_manager * gm, session_config * config) {
    this->gm = gm;
    this->endpoint = config->endpoint;
    this->session_opener = config->start_session_func;
    this->session_closer = config->end_session_func;
    this->transmit_func = config->transmit_func;
    this->receive_func = config->receive_func;
    this->communication_in_progress = false;
    session_active = false;
}

estts::Status groundstation_manager::session_manager::request_session() {
    auto resp = gm->request_session_approval(this->endpoint);
    if (resp != ES_OK)
        return resp;
    return session_opener();
}

void groundstation_manager::session_manager::dispatch() {
    Status status;
    for (;;) {
        if (!waiting.empty()) {
            SPDLOG_TRACE("{} commands in queue", waiting.size());
        }

        SPDLOG_INFO("Requesting new session with endpoint {}", gm->get_endpoint_enum_string(endpoint));

        if (!session_active) {
            status = request_session();
            if (status != ES_OK) {
                SPDLOG_ERROR("Failed to request session from session handler with endpoint {}", gm->get_endpoint_enum_string(this->endpoint));
                return;
            }
        }
        gm->notify_session_active(endpoint);
        SPDLOG_INFO("Session active");

        auto command = waiting.front();
        status = transmit_func(command->frame);
        if (status != ES_OK) {
            SPDLOG_ERROR("Failed to transmit command with serial number {}", command->serial_number);
            return;
        }

        gm->notify_session_active(endpoint);
        SPDLOG_DEBUG("Successfully transmitted command with serial number {}", command->serial_number);

        SPDLOG_INFO("Waiting for a response");
        sleep_until(system_clock::now() + milliseconds (100));

        auto resp = receive_func();
        if (resp.empty()) {
            SPDLOG_ERROR("Response to command with serial number {} was empty.", command->serial_number);
            return;
        }

        waiting.pop_front();

        gm->notify_session_active(endpoint);

        if (command->str_callback != nullptr)
            if (estts::ES_OK != command->str_callback(resp)) {
                SPDLOG_WARN("Command callback failed. Continuing.");
            }
        gm->notify_session_active(endpoint);

        if (!session_active || waiting.empty()) {
            SPDLOG_INFO("Ending session with {}", gm->get_endpoint_enum_string(endpoint));
            status = session_closer();
            if (status != ES_OK)
                SPDLOG_WARN("Session from session handler with endpoint {} didn't exit properly", gm->get_endpoint_enum_string(this->endpoint));
            else
                SPDLOG_INFO("Successfully ended session");
            return;
        }
    }
}

std::string groundstation_manager::session_manager::schedule_command(const std::string& command, const std::function<estts::Status(std::string)>& callback) {
    if (gm == nullptr) {
        SPDLOG_ERROR("Ground station manager not initialized.");
        return "";
    }
    auto new_command = new waiting_command;
    new_command->frame = command;
    new_command->serial_number = generate_serial_number();
    new_command->str_callback = callback;

    waiting.push_back(new_command);

    SPDLOG_DEBUG("Scheduled new command with serial number {}", new_command->serial_number);
    return new_command->serial_number;
}


groundstation_manager::session_manager::~session_manager() {
    session_closer();
    gm->close_session_handler(endpoint);
}

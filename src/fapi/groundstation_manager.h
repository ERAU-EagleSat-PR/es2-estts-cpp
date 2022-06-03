//
// Created by Hayden Roszell on 5/31/22.
//

#ifndef ESTTS_GROUNDSTATION_MANAGER_H
#define ESTTS_GROUNDSTATION_MANAGER_H

#include <deque>
#include <utility>
#include "transmission_interface.h"

class groundstation_manager : virtual public transmission_interface {
public:
    class session_manager {
    private:
        groundstation_manager * gm;
        estts::SessionEndpoint endpoint;
        bool session_active;
        std::deque<estts::waiting_command *> waiting;

        std::function<estts::Status()> session_opener;

        std::function<estts::Status()> session_closer;

        std::function<estts::Status(std::string)> transmit_func;

        std::function<std::string()> receive_func;

        estts::Status request_session();

    public:
        /**
         * Internal use only.
         * @param manager
         */
        void set_groundstation_manager(groundstation_manager * manager) { this->gm = manager; }

        explicit session_manager(groundstation_manager * gm, estts::session_config * config);

        estts::SessionEndpoint get_session_endpoint() { return endpoint; }

        int get_command_queue_count() { return (int)waiting.size(); }

        std::string schedule_command(const std::string& command, const std::function<estts::Status(std::string)>& callback);

        void force_session_end();

        ~session_manager();

        void dispatch();
    };
private:
    typedef struct {
        estts::SessionEndpoint endpoint;
        std::chrono::time_point<std::chrono::high_resolution_clock> last_active;
        int init_priority;
        double priority;
        std::string sn;
        groundstation_manager::session_manager * session;
        std::thread * executor;
        bool satellite_range_required_for_execution;
    } internal_session;

    bool satellite_in_range;

    int highest_session_priority;

    std::vector<internal_session*> session_list;

    // Blunt force object used as a last ditch effort to prevent more than one session from executing at a time.
    std::mutex session_mutex;

    internal_session * currently_executing;

    std::function<estts::Status(std::string)> groundstation_telemetry_callback;

    estts::Status start_session_executor(internal_session * session);

    estts::Status stop_current_session();

    estts::Status replace_current_session(internal_session * new_session);

    internal_session * get_highest_priority_session();

    estts::Status adjust_session_priorities();

    static double calculate_waiting_time_weight(internal_session * session);

    double calculate_base_priority_weight(internal_session * session) const;

    static double calculate_queue_count_weight(internal_session * session);

    std::function<estts::Status(std::string)> primary_telem_cb;

    void detect_satellite_in_range();

    [[noreturn]] void manage();

    std::thread gm_thread;

    groundstation_manager::session_manager * register_session(groundstation_manager::session_manager * new_session, int priority, bool satellite_range_required_for_execution);
public:
    void set_groundstation_telemetry_callback(const std::function<estts::Status(std::string)>& cb) { groundstation_telemetry_callback = cb; }

    static std::string get_endpoint_enum_string(estts::SessionEndpoint s);

    bool is_satellite_in_range() const { return satellite_in_range; }

    estts::Status groundstation_manager_init();

    groundstation_manager();

    groundstation_manager::session_manager * generate_session_manager(estts::session_config * config);

    estts::Status notify_session_closing(estts::SessionEndpoint endpoint);

    estts::Status validate_session_approval(estts::SessionEndpoint endpoint);

    void notify_session_active(estts::SessionEndpoint endpoint);

    void notify_session_executor_exiting(estts::SessionEndpoint endpoint);
};


#endif //ESTTS_GROUNDSTATION_MANAGER_H

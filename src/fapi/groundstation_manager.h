//
// Created by Hayden Roszell on 5/31/22.
//

#ifndef ESTTS_GROUNDSTATION_MANAGER_H
#define ESTTS_GROUNDSTATION_MANAGER_H

#include <deque>
#include "transmission_interface.h"

class groundstation_manager : virtual public transmission_interface {
public:
    class session_manager {
    private:
        groundstation_manager * gm;
        estts::SessionEndpoint endpoint;
        bool communication_in_progress;
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

        void force_session_end() { session_active = false; }

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
        bool allow_session;
    } internal_session;

    std::vector<internal_session*> session_list;

    internal_session * get_highest_priority_session();

    internal_session * currently_executing;

    estts::Status adjust_session_priorities();

    bool satellite_in_range;

    std::function<estts::Status(std::string)> primary_telem_cb;

    void detect_satellite_in_range();

    [[noreturn]] void manage();

    std::thread gm_thread;

    groundstation_manager::session_manager * register_session(groundstation_manager::session_manager * new_session, int priority);
public:
    static std::string get_endpoint_enum_string(estts::SessionEndpoint s);

    bool is_satellite_in_range() const { return satellite_in_range; }

    estts::Status groundstation_manager_init();

    groundstation_manager();

    groundstation_manager::session_manager * generate_session_manager(estts::session_config * config);

    estts::Status close_session_handler(estts::SessionEndpoint endpoint);

    estts::Status request_session_approval(estts::SessionEndpoint endpoint);

    estts::Status notify_session_active(estts::SessionEndpoint endpoint);
};


#endif //ESTTS_GROUNDSTATION_MANAGER_H

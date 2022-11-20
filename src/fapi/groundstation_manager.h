//
// Created by Hayden Roszell on 5/31/22.
//

#ifndef ESTTS_GROUNDSTATION_MANAGER_H
#define ESTTS_GROUNDSTATION_MANAGER_H

#include <deque>
#include <utility>
#include "transmission_interface.h"
#include "session_manager_modifier.h"

class groundstation_manager : virtual public transmission_interface {
public:
    class session_manager {
    private:
        /**
         * Ground station manager object used by session_manager to inform the scheduler of current states
         * for easier session coordination.
         */
        groundstation_manager * gm;

        /**
         * Endpoint of session object configured at creation.
         */
        estts::SessionEndpoint endpoint;

        /**
         * Variable used by the dispatcher to coordinate the beginning cecession of session operation.
         */
        bool session_active;

        /**
         * Waiting command queue populated by schedule_command and managed by dispatch().
         */
        std::deque<estts::waiting_command *> waiting;

        /**
         * Function pointer that represents a method used to create a new session with the specific endpoint
         * represented by this session object.
         */
        std::function<estts::Status()> session_opener;

        /**
         * Function pointer that represents a method used to close the current session with the specific endpoint
         * represented by this session object.
         */
        std::function<estts::Status()> session_closer;

        /**
         * Function pointer that represents a method used to transmit string data to the endpoint, given
         * that session_opener() has been called.
         */
        std::function<estts::Status(std::string)> transmit_func;

        /**
         * Function pointer that represents a method used to receive string data from the endpoint, given
         * that session_opener() has been called.
         */
        std::function<std::string()> receive_func;

        /**
         * Internal method that wraps session_opener() by first requesting session approval from the
         * ground station manager.
         * @return
         */
        estts::Status request_session();

        session_manager_modifier * modifier;

    public:
        /**
         * Internal use only. Used by the ground station manager at the time of session registration.
         * @param manager
         */
        void set_groundstation_manager(groundstation_manager * manager) { this->gm = manager; }

        void set_session_modifier(session_manager_modifier * modifier) { this->modifier = modifier; }

        /**
         * Default constructor method used by the ground station manager to create a new session object.
         * CRITICAL NOTE: Forward facing clients should NEVER use this constructor. The proper way of creating
         * a new session object is to use the generate_session_manager() method. If this is not done,
         * the ground station manager will have no knowledge of the session, and the dispatcher will never execute.
         * @param gm Ground station manager object
         * @param config Configuration object used to configure the session
         */
        explicit session_manager(groundstation_manager * gm, estts::session_config * config);

        /**
         * Getter that retrieves the session's endpoint.
         * @return endpoint
         */
        estts::SessionEndpoint get_session_endpoint() { return endpoint; }

        /**
         * Getter that retrieves the current number of commands waiting for execution.
         * @return
         */
        int get_command_queue_count() { return (int)waiting.size(); }

        /**
         * Primary interfacing method used by higher layers to schedule new commands. This method notifies the ground
         * station manager of new waiting commands, and configures a callback that is called after the command
         * has executed.
         * @param command Command represented as a string that is scheduled by the ground station manager for execution.
         *                The command is executed by the interface represented by the endpoint of this object.
         * @param callback Callback that is called upon successful execution of the command.
         * @return Unique serial number used to track the current status of the command after scheduling.
         */
        std::string schedule_command(const std::string& command, const std::function<estts::Status(std::string)>& callback, bool crc_expected);

        /**
         * Internal use only. Method used to force the end of a currently executing session.
         */
        void force_session_end();

        /**
         * Default destructor that forces any current session to end and notifies the ground station manger
         * to cease management of the session.
         */
        ~session_manager();

        /**
         * Internal use only. Primary dispatcher that executes commands while the session is active.
         */
        void dispatch();

        std::string default_command_executor(const std::string& command, std::string sn, bool verify_crc);
    };
private:
    /**
     * Internal session object used by the ground station manager to facilitate the management of
     * sessions.
     */
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

    /**
     * Variable used to track if the satellite tracked by the ground station manager is in range.
     */
    bool satellite_in_range;

    /**
     * Variable used to track the highest session priority for accurate weight assignment of individual session priority.
     */
    int highest_session_priority;

    /**
     * List of currently registered sessions that are actively managed by the ground station manager.
     */
    std::vector<internal_session*> session_list;

    /**
     * Blunt force object used as a last ditch effort to prevent more than one session from executing at a time.
     */
    std::mutex session_mutex;

    /**
     * Variable used to track the currently executing session.
     */
    internal_session * currently_executing;

    /**
     * Local thread object that executes the ground station manager method
     */
    std::thread gm_thread;

    /**
     * Method used to start a new session, given that no session is currently active. Successful invocation
     * of this method requires locking the session_mutex.
     * @param session New session to start executing.
     * @return ES_OK if successful
     */
    estts::Status start_session_executor(internal_session * session);

    /**
     * Method used to stop the currently executing session. Operates by notifying the currently executing session
     * to return.
     * @return ES_OK if successful
     */
    estts::Status stop_current_session();

    /**
     * Method used to replace the currently executing session with a new session.
     * @param new_session New session to start executing after the current one stops
     * @return ES_OK if successful
     */
    estts::Status replace_current_session(internal_session * new_session);

    /**
     * Method that finds and returns the highest priority session, if one is executing.
     * @return Highest priority session
     */
    internal_session * get_highest_priority_session();

    /**
     * Method that adjusts the session priorities of each session managed by the ground station manager.
     * @return
     */
    estts::Status adjust_session_priorities();

    /**
     * Method that calculates a priority weight based on the time the session has been waiting to execute.
     * @param session Session to calculate weight on
     * @return Weight between 0 and 100
     */
    static double calculate_waiting_time_weight(internal_session * session);

    /**
     * Method that calculates a priority weight based on the original priority given to the session at the time
     * of configuration.
     * @param session Session to calculate weight on
     * @return Weight between 0 and 100
     */
    double calculate_base_priority_weight(internal_session * session) const;

    /**
     * Method that calculates a priority weight based on the number of commands currently in the queue.
     * @param session Session to calculate weight on
     * @return Weight between 0 and 100
     */
    static double calculate_queue_count_weight(internal_session * session);

    /**
     * Function that detect if the satellite is currently in range of the ground station.
     */
    void detect_satellite_in_range();

    /**
     * Primary executor method that manages all ground station operation.
     */
    [[noreturn]] void manage();

    groundstation_manager::session_manager * register_session(groundstation_manager::session_manager * new_session, int priority, bool satellite_range_required_for_execution);

public:
    /**
     * Primary ground station telemetry callback that acts to save data found during connectionless operation.
     */
    std::function<estts::Status(std::string)> groundstation_telemetry_callback;

    /**
     * Setter to set the ground station telemetry callback function.
     * @param cb Function pointer
     */
    void set_groundstation_telemetry_callback(const std::function<estts::Status(std::string)>& cb) { groundstation_telemetry_callback = cb; }

    /**
     * Method that takes argument for a session endpoint enumeration and returns a string
     * @param s SessionEndpoint enumeration
     * @return string
     */
    static std::string get_endpoint_enum_string(estts::SessionEndpoint s);

    /**
     * Getter that returns the local satellite_in_range variable.
     * @return true if satellite is in range, false if not
     */
    bool is_satellite_in_range() const { return satellite_in_range; }

    /**
     * Method that must be called to initialize the ground station manager.
     */
    estts::Status groundstation_manager_init();

    /**
     * Default constructor. groundstation_manager_init() must be called after default constructor.
     */
    groundstation_manager();

    /**
     * Method that generates and registers a new session according to configuration data passed as argument.
     * @param config Pointer to an estts::session_config object
     * @return Pointer to a session_manager object if registration is successful
     */
    groundstation_manager::session_manager * generate_session_manager(estts::session_config * config);

    /**
     * Internal method used by session_manager objects to notify the ground station manager that the session is closing,
     * and that it should no longer be managed.
     * @param endpoint
     * @return
     */
    estts::Status notify_session_closing(estts::SessionEndpoint endpoint);

    /**
     * Internal method used by session_manager objects to validate that the session is allowed to execute.
     * Validation is based on the configuration at registration time and ensuring that the requesting session
     * is indeed the highest priority session.
     * @param endpoint
     * @return
     */
    estts::Status validate_session_approval(estts::SessionEndpoint endpoint);

    /**
     * Internal method used by session_manager objects to notify the ground station manager that execution
     * is occurring. This is used by the calculate_waiting_time_weight() method to assign a weight appropriate for
     * the amount of time waiting since the last time this function was called.
     * @param endpoint
     */
    void notify_session_active(estts::SessionEndpoint endpoint);

    /**
     * Internal method used to notify the ground station manager that the session associated with the
     * endpoint passed as argument is fi
     * @param endpoint
     */
    void notify_session_executor_exiting(estts::SessionEndpoint endpoint);
};


#endif //ESTTS_GROUNDSTATION_MANAGER_H

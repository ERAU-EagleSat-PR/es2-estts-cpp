//
// Created by Hayden Roszell on 1/4/22.
//

#ifndef ESTTS_COMMAND_SCHEDULER_H
#define ESTTS_COMMAND_SCHEDULER_H

#include <functional>
#include <mutex>
#include <thread>
#include <utility>
#include <vector>
#include <queue>
#include "constants.h"
#include "command_handler.h"

class session_manager : virtual public command_handler {
private:
    transmission_interface *ti;
    std::thread session_worker;
    std::deque<estts::waiting_command *> waiting;

    [[noreturn]] void dispatch();

    estts::Status handle_stream();

    std::function<estts::Status(std::string)> telem_callback = nullptr;

public:
    explicit session_manager();

    explicit session_manager(std::function<estts::Status(std::string)> telem_callback);

    ~session_manager();

    std::string schedule_command(unsigned char * command, const std::function<estts::Status(std::string)>& decomp_callback);

    std::string schedule_command(estts::command_object * command,
                                 std::function<estts::Status(std::vector<estts::telemetry_object *>)> decomp_callback);

    estts::Status get_command_status(const std::string &serial_number);

    void await_completion();
};


#endif //ESTTS_COMMAND_SCHEDULER_H

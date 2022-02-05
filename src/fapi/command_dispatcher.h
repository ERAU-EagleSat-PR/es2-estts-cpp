//
// Created by Hayden Roszell on 1/4/22.
//

#ifndef ESTTS_COMMAND_SCHEDULER_H
#define ESTTS_COMMAND_SCHEDULER_H

#include <functional>
#include <mutex>
#include <thread>
#include <vector>
#include <queue>
#include "constants.h"
#include "command_handler.h"

class command_dispatcher : virtual public command_handler {
private:
    transmission_interface *ti;
    std::thread dispatch_worker;
    std::vector<estts::waiting_command *> waiting;
    estts::Status command_progress;

    void dispatch();

    bool handshake;
public:
    explicit command_dispatcher();

    ~command_dispatcher();

    std::string schedule_command(const std::vector<estts::command_object *> &command,
                                 std::function<estts::Status(std::vector<estts::telemetry_object *>)> decomp_callback);

    estts::Status get_command_status(const std::string &serial_number);

    void await_completion();

    estts::Status get_dispatch_session_progress();
};


#endif //ESTTS_COMMAND_SCHEDULER_H

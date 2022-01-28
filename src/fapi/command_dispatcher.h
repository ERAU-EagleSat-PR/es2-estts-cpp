//
// Created by Hayden Roszell on 1/4/22.
//

#ifndef ESTTS_COMMAND_SCHEDULER_H
#define ESTTS_COMMAND_SCHEDULER_H

#include <thread>
#include <vector>
#include <queue>
#include "constants.h"
#include "telemetry_handler.h"

class command_dispatcher {
private:
    typedef struct {
        estts::Status response_code;
        std::string serial_number;
    } completed_command;

    typedef struct {
        std::function<estts::Status()> command;
        std::string serial_number;
    } waiting_command;

    std::thread worker;

    std::vector<completed_command *> completed;
    std::deque<waiting_command *> waiting;

    void clean_completed_cache();
public:
    command_dispatcher();
    ~command_dispatcher();
    std::string schedule_command(const std::function<estts::Status()>& command);
    estts::Status get_command_status(std::string serial_number);
    void await_completion();

    void schedule();
};


#endif //ESTTS_COMMAND_SCHEDULER_H

//
// Created by Hayden Roszell on 1/4/22.
//

#ifndef ESTTS_COMMAND_SCHEDULER_H
#define ESTTS_COMMAND_SCHEDULER_H

#include <thread>
#include <vector>
#include <queue>
#include "constants.h"

class command_scheduler {
private:
    std::thread worker;
    std::condition_variable cv;
    std::deque<std::function<void()>> command_queue;
public:
    command_scheduler();
    estts::Status schedule_command(std::function<void()> command);
    void await_completion();

    void schedule();
};


#endif //ESTTS_COMMAND_SCHEDULER_H

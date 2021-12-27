//
// Created by Hayden Roszell on 12/23/21.
//

#include "spdlog/spdlog.h"
#include "frame_constructor.h"
#include "constants.h"

int main() {
    spdlog::set_level(spdlog::level::trace);
    auto command_object = new estts::command_object;

    command_object->address = estts::es2_endpoint::ES_EPS;
    command_object->commandID = estts::es2_commands::eps::EPS_GET_HEALTH;
    command_object->method = estts::es2_commands::method::ES_READ;
    command_object->sequence = 1;
    command_object->timeStamp = 112;

    auto command = new frame_constructor(command_object);
    spdlog::info("AX.25 frame: {}", command->construct_ax25());
}
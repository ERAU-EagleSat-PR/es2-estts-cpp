//
// Created by Hayden Roszell on 12/23/21.
//

#include <sstream>
#include "spdlog/spdlog.h"
#include "frame_constructor.h"
#include "frame_destructor.h"
#include "constants.h"
#include "info_field.h"
#include "esttc.h"
#include "esttc_constructor.h"
#include "esttc_deconstructor.h"

int main() {
    //ax25 test
    spdlog::set_level(spdlog::level::trace);
    auto command_object = new estts::command_object;

    command_object->address = estts::es2_endpoint::ES_EPS;
    command_object->commandID = estts::es2_commands::eps::EPS_GET_HEALTH;
    command_object->method = estts::es2_commands::method::ES_READ;
    command_object->sequence = 01;
    command_object->timeStamp = 1121;

    auto command = new frame_constructor(command_object);
    std::stringstream framestream;

    framestream << command->construct_ax25() << command->construct_ax25();

    delete command;

    std::string frame = "7E7E7E7E7E7E7E7E7E4e4544434241E04e4142434445E103F0021121101727E7E7E7E7E7E7E7E7E7E7E7E7E4e4544434241E04e4142434445E103F0021121101727E7E7E7E7E7E7E7E7E7E7E7E7E4e4544434241E04e4142434445E103F0021121101727E7E7E7E";

    auto command_destructor = new frame_destructor(frame);
    auto telem = command_destructor->destruct_ax25();
    delete command_destructor;
}
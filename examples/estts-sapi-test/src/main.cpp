//
// Created by Hayden Roszell on 12/23/21.
//

#include <sstream>
#include "spdlog/spdlog.h"
#include "frame_constructor.h"
#include "frame_destructor.h"
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
    std::stringstream framestream;

    framestream << command->construct_ax25() << command->construct_ax25();

    delete command;

    std::string frame = "7E7E7E7E7E7E7E7E7E4e4544434241E04e4142434445E103F002112101Ar7E7E7E7E7E7E7E7E7E7E7E7E7E7E7E7E7E7E7E7E7E4e4544434241E04e4142434445E103F002112101Ar7E7E7E7E";

    spdlog::info("AX.25 frame: {}", frame);

    auto command_destructor = new frame_destructor(framestream.str());
    auto telem = command_destructor->get_telemetry();
    delete command_destructor;
}
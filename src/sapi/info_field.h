//
// Created by Cody on 12/17/2021.
//

#ifndef ES2_ESTTS_CPP_INFOFIELD_H
#define ES2_ESTTS_CPP_INFOFIELD_H

#include <string>
#include <unordered_map>
#include <utility>
#include "bin_converter.h"
#include "constants.h"

class info_field {
private:
    /* Getters */
    std::string getData();

    std::string getAddress();

    std::string getTimeStamp();

    std::string getSequence();

    std::string getCommandID();

    std::string getMethod();

protected:
    estts::command_object * command;

    /* Encoded Information Field Getter */
    std::string build_info_field();

    estts::telemetry_object * build_telemetry_object(std::string info_field);

    explicit info_field() : command(nullptr){}

    explicit info_field(estts::command_object *esttsCommand) : command(esttsCommand) {}
};

#endif
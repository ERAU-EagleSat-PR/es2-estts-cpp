//
// Created by Cody on 12/17/2021.
//

#include <iostream>
#include <string>
#include <sstream>
#include "info_field.h"
#include "spdlog/spdlog.h"

using std::cerr;
using std::endl;
using std::string;

/**
 * @description Encodes the Address section of the AX.25 Information Field
 * @return string of bits (e.g. "10010101")
 */
string info_field::getAddress() {
    spdlog::trace("Setting info field address to {}", this->command->address);
    return reinterpret_cast<char const *>(this->command->address);
}

/**
 * @description Encodes the Timestamp section of the AX.25 Information Field
 * @return string of bits (e.g. "10010101")
 */
string info_field::getTimeStamp() {
    spdlog::trace("Setting info field timestamp to {}", this->command->timeStamp);
    return std::to_string(this->command->timeStamp);
}

/**
 * @description Encodes the Sequence section of the AX.25 Information Field
 * @return string of bits (e.g. "10010101")
 */
string info_field::getSequence() {
    spdlog::trace("Setting info field frame sequence to {}", this->command->sequence);
    return std::to_string(this->command->sequence);
}

/**
 * @description Encodes the Command ID section of the AX.25 Information Field
 * @return string of bits (e.g. "10010101")
 */
string info_field::getCommandID() {
    spdlog::trace("Setting info field command ID to {}", this->command->commandID);
    return reinterpret_cast<char const *>(this->command->commandID);
}

/**
 * @description Encodes the Method section of the AX.25 Information Field
 * @return One bit as a char (e.g. "1")
 */
string info_field::getMethod() {
    spdlog::trace("Setting info field method to {}", this->command->method);
    return reinterpret_cast<char const *>(this->command->method);
}

/**
 * @description Encodes the Data section of the AX.25 Information Field
 * @return string of bits (e.g. "10010101...")
 */
string info_field::getData() {
    if (this->command->data != nullptr) {
        spdlog::trace("Setting info field data to {}", this->command->data);
        return reinterpret_cast<char const *>(this->command->data);
    } else return "";
}

/**
 * @description Retrieves the encodings of the entire AX.25 Information Field
 * @return string of bits (e.g. "10010101...")
 */
string info_field::build_info_field() {
    std::stringstream infoFieldStream;
    infoFieldStream << getAddress() << getTimeStamp() << getSequence() << getCommandID() << getMethod() << getData();
    spdlog::debug("SAPI Info Field encoded to {}", infoFieldStream.str());

    return infoFieldStream.str();
}

estts::telemetry_object * info_field::build_telemetry_object(std::string info_field) {
    auto resp = new estts::telemetry_object;

    // TODO put info field stuff into a new info field constructor

    resp->address = info_field.substr(0, 2).c_str();

    int timestamp;
    std::istringstream(info_field.substr(2, 4)) >> timestamp;
    resp->timeStamp = timestamp;

    int sequence;
    std::istringstream(info_field.substr(6, 2)) >> sequence;
    resp->sequence = sequence;

    resp->commandID = info_field.substr(8, 2).c_str();

    resp->response_code = info_field.substr(10, 2).c_str();

    // resp->data = info_field.substr(12).c_str(); todo figure this shit out

    return resp;
}

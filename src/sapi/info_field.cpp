//
// Created by Cody on 12/17/2021.
//

#include <iostream>
#include <string>
#include <sstream>
#include "info_field.h"

using std::cerr;
using std::endl;
using std::string;

/**
 * @description Encodes the Address section of the AX.25 Information Field
 * @return string of bits (e.g. "10010101")
 */
string info_field::getAddress() {
    SPDLOG_TRACE("Setting info field address to {}", this->command->address);
    auto address = std::to_string(this->command->address);
    if (this->command->sequence < 10)
        address.insert(0, "0");
    return address;
}

/**
 * @description Encodes the Timestamp section of the AX.25 Information Field
 * @return string of bits (e.g. "10010101")
 */
string info_field::getTimeStamp() {
    SPDLOG_TRACE("Setting info field timestamp to {}", this->command->timeStamp);
    return std::to_string(this->command->timeStamp);
}

/**
 * @description Encodes the Sequence section of the AX.25 Information Field
 * @return string of bits (e.g. "10010101")
 */
string info_field::getSequence() {
    SPDLOG_TRACE("Setting info field frame sequence to {}", this->command->sequence);
    auto sequence = std::to_string(this->command->sequence);
    if (this->command->sequence < 10)
        sequence.insert(0, "0");
    return sequence;
}

/**
 * @description Encodes the Command ID section of the AX.25 Information Field
 * @return string of bits (e.g. "10010101")
 */
string info_field::getCommandID() {
    SPDLOG_TRACE("Setting info field command ID to {}", this->command->commandID);
    auto command_id = std::to_string(this->command->commandID);
    if (this->command->commandID < 10)
        command_id.insert(0, "0");
    return command_id;
}

/**
 * @description Encodes the Method section of the AX.25 Information Field
 * @return One bit as a char (e.g. "1")
 */
string info_field::getMethod() {
    SPDLOG_TRACE("Setting info field method to {}", this->command->method);
    auto method = std::to_string(this->command->method);
    return method;
}

/**
 * @description Encodes the Data section of the AX.25 Information Field
 * @return string of bits (e.g. "10010101...")
 */
string info_field::getData() {
    if (this->command->data != nullptr) {
        SPDLOG_TRACE("Setting info field data to {}", this->command->data);
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
    SPDLOG_DEBUG("SAPI Info Field encoded to {}", infoFieldStream.str());

    return infoFieldStream.str();
}

estts::telemetry_object *info_field::build_telemetry_object(std::string info_field) {
    auto resp = new estts::telemetry_object;

    int address;
    std::istringstream(info_field.substr(0, 2)) >> address;
    resp->address = address;

    int timestamp;
    std::istringstream(info_field.substr(2, 4)) >> timestamp;
    resp->timeStamp = timestamp;

    int sequence;
    std::istringstream(info_field.substr(6, 2)) >> sequence;
    resp->sequence = sequence;

    int command_id;
    std::istringstream(info_field.substr(8, 2)) >> command_id;
    resp->commandID = command_id;

    int response_code;
    std::istringstream(info_field.substr(10, 1)) >> response_code;
    resp->response_code = response_code;

    // resp->data = info_field.substr(12).c_str(); todo figure this shit out

    SPDLOG_TRACE("Frame info field decoded successfully. Telemetry object stored at {}", static_cast<const void*>(resp));

    return resp;
}

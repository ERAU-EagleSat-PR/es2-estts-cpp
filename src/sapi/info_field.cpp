//
<<<<<<< HEAD
// Created by Cody on 12/17/2021.
=======
// Created by Cody Park on 12/17/2021.
>>>>>>> main
//

#include <iostream>
#include <string>
<<<<<<< HEAD
#include <sstream>
#include "info_field.h"
#include "spdlog/spdlog.h"
=======
#include "info_field.h"
>>>>>>> main

using std::cerr;
using std::endl;
using std::string;

/**
 * @description Encodes the Address section of the AX.25 Information Field
 * @return string of bits (e.g. "10010101")
 */
<<<<<<< HEAD
string info_field::getAddress() {
    spdlog::trace("Setting info field address to {}", this->command->address);
    return reinterpret_cast<char const *>(this->command->address);
=======
string info_field::getAddressBits() {
    return binConverter.toBinary(ADDRESS_SIZE, address);
>>>>>>> main
}

/**
 * @description Encodes the Timestamp section of the AX.25 Information Field
 * @return string of bits (e.g. "10010101")
 */
<<<<<<< HEAD
string info_field::getTimeStamp() {
    spdlog::trace("Setting info field timestamp to {}", this->command->timeStamp);
    return reinterpret_cast<char const *>(this->command->timeStamp);
=======
string info_field::getTimeStampBits() {
    return binConverter.toBinary(TIMESTAMP_SIZE, timeStamp);
>>>>>>> main
}

/**
 * @description Encodes the Sequence section of the AX.25 Information Field
 * @return string of bits (e.g. "10010101")
 */
<<<<<<< HEAD
string info_field::getSequence() {
    spdlog::trace("Setting info field frame sequence to {}", this->command->sequence);
    return std::to_string(this->command->sequence);
=======
string info_field::getSequenceBits() {
    return binConverter.toBinary(SEQUENCE_SIZE, sequence);
>>>>>>> main
}

/**
 * @description Encodes the Command ID section of the AX.25 Information Field
 * @return string of bits (e.g. "10010101")
 */
<<<<<<< HEAD
string info_field::getCommandID() {
    spdlog::trace("Setting info field command ID to {}", this->command->commandID);
    return reinterpret_cast<char const *>(this->command->commandID);
=======
string info_field::getCommandIDBits() {
    return binConverter.toBinary(COMMANDID_SIZE, commandID);
>>>>>>> main
}

/**
 * @description Encodes the Method section of the AX.25 Information Field
 * @return One bit as a char (e.g. "1")
 */
<<<<<<< HEAD
string info_field::getMethod() {
    spdlog::trace("Setting info field method to {}", this->command->method);
    return {1, this->command->method};
=======
char info_field::getMethodBits() {
    /* If input is valid, return associated bit string
     * Else, send error then return */
    if (method == "0x1" || method == "0x0") {
        return method.back();
    } else {
        cerr << "Error [info_field] - Invalid hex value: "<< method
                << "\nExpected size: " << METHOD_SIZE << endl;

        return '0';
    }
>>>>>>> main
}

/**
 * @description Encodes the Data section of the AX.25 Information Field
 * @return string of bits (e.g. "10010101...")
 */
<<<<<<< HEAD
string info_field::getData() {
    if (this->command->data != nullptr) {
        spdlog::trace("Setting info field data to {}", this->command->data);
        return reinterpret_cast<char const *>(this->command->data);
    }
    else return "";
=======
string info_field::getDataBits() {
    return binConverter.toBinary(DATA_SIZE, data);
>>>>>>> main
}

/**
 * @description Retrieves the encodings of the entire AX.25 Information Field
 * @return string of bits (e.g. "10010101...")
 */
<<<<<<< HEAD
string info_field::build_info_field() {
    std::stringstream infoFieldStream;
    infoFieldStream << getAddress() << getTimeStamp() << getSequence() << getCommandID() << getMethod() << getData();
    spdlog::debug("SAPI Info Field encoded to {}", infoFieldStream.str());

    return infoFieldStream.str();
=======
string info_field::encode() {
    string infoFieldStream;

    infoFieldStream += getAddressBits() + getTimeStampBits()
        + getSequenceBits() + getCommandIDBits()
        + getMethodBits() + getDataBits(); 

    return infoFieldStream;
>>>>>>> main
}
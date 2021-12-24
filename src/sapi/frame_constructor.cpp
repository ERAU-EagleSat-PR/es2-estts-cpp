//
// Created by Cody on 12/17/2021.
//

#include <iostream>
#include <cstring>
#include <spdlog/spdlog.h>
#include "frame_constructor.h"
#include "constants.h"

using std::cerr;
using std::cout;
using std::endl;
using std::string;
using std::stringstream;
using std::strlen;
using std::hex;

// ~~~~~~~~~~~ FRAME HEADER GETTERS  ~~~~~~~~~~~

/**
 * @description Encodes the Flag field of the AX.25 Frame Header
 * @return string of bits (e.g. "10010101")
 */
string frame_constructor::getFlag() {
    spdlog::trace("Setting AX.25 frame flag to {}", estts::ax25::AX25_FLAG);
    return reinterpret_cast<char const *>(estts::ax25::AX25_FLAG);
}

/**
 * @description Encodes the Destination Address field of the AX.25 Frame Header
 * @return string of bits (e.g. "10010101")
 */
string frame_constructor::getDestAddr() {
    spdlog::trace("Setting AX.25 frame destination address to {}", estts::ax25::AX25_DESTINATION_ADDRESS);
    return reinterpret_cast<char const *>(estts::ax25::AX25_DESTINATION_ADDRESS);
}

/**
 * @description Encodes the first SSID field of the AX.25 Frame Header
 * @return @return string of bits (e.g. "10010101")
 */
string frame_constructor::getSSID0() {
    spdlog::trace("Setting AX.25 frame SSID0 to {}", estts::ax25::AX25_SSID0);
    return reinterpret_cast<char const *>(estts::ax25::AX25_SSID0);
}

/**
 * @description Encodes the Source Address field of the AX.25 Frame Header
 * @return string of bits (e.g. "10010101")
 */
string frame_constructor::getSrcAddr() {
    spdlog::trace("Setting AX.25 frame source address to {}", estts::ax25::AX25_SOURCE_ADDRESS);
    return reinterpret_cast<char const *>(estts::ax25::AX25_SOURCE_ADDRESS);
}

/**
 * @description Encodes the second SSID field of the AX.25 Frame Header
 * @return @return string of bits (e.g. "10010101")
 */
string frame_constructor::getSSID1() {
    spdlog::trace("Setting AX.25 frame SSID1 to {}", estts::ax25::AX25_SSID1);
    return reinterpret_cast<char const *>(estts::ax25::AX25_SSID1);
}

/**
 * @description Encodes the Control field of the AX.25 Frame Header
 * @return string of bits (e.g. "10010101")
 */
string frame_constructor::getControl() {
    spdlog::trace("Setting AX.25 frame control to {}", estts::ax25::AX25_CONTROL);
    return reinterpret_cast<char const *>(estts::ax25::AX25_CONTROL);
}

/**
 * @description Encodes the PID field of the AX.25 Frame Header
 * @return string of bits (e.g. "10010101")
 */
string frame_constructor::getPID() {
    spdlog::trace("Setting AX.25 frame PID to {}", estts::ax25::AX25_PID);
    return reinterpret_cast<char const *>(estts::ax25::AX25_PID);
}

/**
 * @description Retrieves the encoded AX.25 Information Field
 * @return string of bits (e.g. "10010101")
 */
string frame_constructor::getInfoField() {
    auto info_field = this->build_info_field();
    spdlog::trace("Setting AX.25 frame information field to {}", info_field);
    return info_field;
}

/**
 * @description Encodes the Frame Check Sequence field of the AX.25 Frame Header
 * @return string of bits (e.g. "10010101")
 */
string frame_constructor::getFCSBits() {
    spdlog::trace("Setting AX.25 frame sequence number to {}", this->command->sequence);
    return reinterpret_cast<char const *>(this->command->sequence);
}

// ~~~~~~~~~~~ ENCODING ~~~~~~~~~~~

/**
 * @description Retrieves the encodings of the entire AX.25 Frame
 * @return string of bits (e.g. "10010101...")
 */
string frame_constructor::construct_ax25() {
    std::stringstream encodedFrame;

    encodedFrame << getFlag();
    encodedFrame << getDestAddr();
    encodedFrame << getSSID0();
    encodedFrame << getSrcAddr();
    encodedFrame << getSSID1();
    encodedFrame << getControl();
    encodedFrame << getPID();
    encodedFrame << getInfoField();
    encodedFrame << getFlag();

    return encodedFrame.str();
}
//
// Created by Cody on 12/17/2021.
//

#include <iostream>
#include <cstring>
#include <utility>
#include <spdlog/spdlog.h>
#include "ax25_ui_frame_constructor.h"
#include "constants.h"
#include "helper.h"

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
string ax25_ui_frame_constructor::getFlag() {
    return reinterpret_cast<char const *>(estts::ax25::AX25_FLAG);
}

/**
 * @description Encodes the Destination Address field of the AX.25 Frame Header
 * @return string of bits (e.g. "10010101")
 */
string ax25_ui_frame_constructor::getDestAddr() {
    // Convert const char to HEX
    return ascii_to_hex(estts::ax25::AX25_DESTINATION_ADDRESS);
}

/**
 * @description Encodes the first SSID field of the AX.25 Frame Header
 * @return @return string of bits (e.g. "10010101")
 */
string ax25_ui_frame_constructor::getSSID0() {
    return reinterpret_cast<char const *>(estts::ax25::AX25_SSID0);
}

/**
 * @description Encodes the Source Address field of the AX.25 Frame Header
 * @return string of bits (e.g. "10010101")
 */
string ax25_ui_frame_constructor::getSrcAddr() {
    return ascii_to_hex(estts::ax25::AX25_SOURCE_ADDRESS);
}

/**
 * @description Encodes the second SSID field of the AX.25 Frame Header
 * @return @return string of bits (e.g. "10010101")
 */
string ax25_ui_frame_constructor::getSSID1() {
    return reinterpret_cast<char const *>(estts::ax25::AX25_SSID1);
}

/**
 * @description Encodes the Control field of the AX.25 Frame Header
 * @return string of bits (e.g. "10010101")
 */
string ax25_ui_frame_constructor::getControl() {
    return reinterpret_cast<char const *>(estts::ax25::AX25_CONTROL);
}

/**
 * @description Encodes the PID field of the AX.25 Frame Header
 * @return string of bits (e.g. "10010101")
 */
string ax25_ui_frame_constructor::getPID() {
    return reinterpret_cast<char const *>(estts::ax25::AX25_PID);
}

/**
 * @description Retrieves the encoded AX.25 Information Field
 * @return string of bits (e.g. "10010101")
 */
string ax25_ui_frame_constructor::getInfoField() {
    auto info_field = this->build_info_field();
    return info_field;
}

// ~~~~~~~~~~~ ENCODING ~~~~~~~~~~~

/**
 * @brief Constructs and encodes AX.25 frame using AX.25 constant values
 * @return Returns constructed and encoded AX.25 frame according to EnduroSat specification
 */
string ax25_ui_frame_constructor::construct_ax25() {
    std::stringstream frameStream;

    // Preamble
    for (int i = 0; i < 8; i++)
        frameStream << getFlag();

    frameStream << getFlag();
    // Destination address (hex encoded)
    frameStream << getDestAddr();
    frameStream << getSSID0();
    // Source address (hex encoded)
    frameStream << getSrcAddr();
    frameStream << getSSID1();
    frameStream << getControl();
    frameStream << getPID();
    std::string info_field = getInfoField();
    frameStream << info_field;
    frameStream << calculate_crc16_ccit(info_field);
    frameStream << getFlag();

    // Postamble
    for (int i = 0; i < 3; i++)
        frameStream << getFlag();

    SPDLOG_TRACE("Built AX.25 frame with value {}", frameStream.str());

    std::string encoded_frame = this->encode_ax25_frame(frameStream.str());

    SPDLOG_TRACE("Encoded AX.25 frame to {}", encoded_frame);

    return encoded_frame;
}

/**
 * @brief Performs NRZI encoding on an inputted raw string. Input should all be hexadecimal.
 * @param raw Raw hexadecimal string to perform encoding on.
 * @return Returns an NRZI encoded string of hexidecimal values, or the same string if encoding fails
 */
std::string ax25_ui_frame_constructor::perform_nrzi_encoding(std::string raw) {
    // TODO ESTTS-145 Integrate NRZI encoding - Place NRZI encoding in this function
    return raw;
}

/**
 * @brief Calculates the CRC16-CCIT as specified by FCS of AX.25 ISO standard.
 * @param value Raw hexadecimal value to calculate CRC16 on
 * @return Returns calculated CRC16 of inputted string
 */
std::string ax25_ui_frame_constructor::calculate_crc16_ccit(std::string value) {
    // TODO ESTTS-144 Integrate CRC16-CCIT calculation - Place code in this function.
    return "";
}

/**
 * @brief Scrambles inputted AX.25 frame according to EnduroSat scrambling specification
 * @param raw Raw hexadecimal AX.25 frame to be scrambled
 * @return Returns scrambled AX.25 frame
 */
std::string ax25_ui_frame_constructor::scramble_frame(std::string raw) {
    // TODO ESTTS-146 Integrate EnduroSat scrambler - Place scrambling code in this function
    return raw;
}

/**
 * @brief Encodes AX.25 frame for transmission. Encoding includes scrambling and NRZI.
 * @param raw Raw hexadecimal AX.25 frame
 * @return Returns encoded frame for transmission
 */
std::string ax25_ui_frame_constructor::encode_ax25_frame(std::string raw) {
    std::string encoded = this->scramble_frame(this->perform_nrzi_encoding(std::move(raw)));
    // TODO Add logic to perform bit stuffing in accordance with ISO AX.25 Frame Specification
    return encoded;
}

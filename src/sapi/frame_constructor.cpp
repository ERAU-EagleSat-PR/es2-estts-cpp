//
<<<<<<< HEAD
// Created by Cody on 12/17/2021.
=======
// Created by Cody Park on 12/17/2021.
>>>>>>> main
//

#include <iostream>
#include <cstring>
<<<<<<< HEAD
#include <spdlog/spdlog.h>
#include "frame_constructor.h"
#include "constants.h"
=======
#include "frame_constructor.h"
>>>>>>> main

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
<<<<<<< HEAD
    spdlog::trace("Setting AX.25 frame flag to {}", estts::ax25::AX25_FLAG);
    return reinterpret_cast<char const *>(estts::ax25::AX25_FLAG);
=======
    return binConverter.toBinary(headerConsts.AX25_FLAG);
>>>>>>> main
}

/**
 * @description Encodes the Destination Address field of the AX.25 Frame Header
 * @return string of bits (e.g. "10010101")
 */
string frame_constructor::getDestAddr() {
<<<<<<< HEAD
    spdlog::trace("Setting AX.25 frame destination address to {}", estts::ax25::AX25_DESTINATION_ADDRESS);
    return reinterpret_cast<char const *>(estts::ax25::AX25_DESTINATION_ADDRESS);
=======
    return binConverter.toBinary(headerConsts.AX25_DESTINATION_ADDRESS);
>>>>>>> main
}

/**
 * @description Encodes the first SSID field of the AX.25 Frame Header
 * @return @return string of bits (e.g. "10010101")
 */
string frame_constructor::getSSID0() {
<<<<<<< HEAD
    spdlog::trace("Setting AX.25 frame SSID0 to {}", estts::ax25::AX25_SSID0);
    return reinterpret_cast<char const *>(estts::ax25::AX25_SSID0);
=======
    return binConverter.toBinary(headerConsts.AX25_SSID0);
>>>>>>> main
}

/**
 * @description Encodes the Source Address field of the AX.25 Frame Header
 * @return string of bits (e.g. "10010101")
 */
string frame_constructor::getSrcAddr() {
<<<<<<< HEAD
    spdlog::trace("Setting AX.25 frame source address to {}", estts::ax25::AX25_SOURCE_ADDRESS);
    return reinterpret_cast<char const *>(estts::ax25::AX25_SOURCE_ADDRESS);
=======
    return binConverter.toBinary(headerConsts.AX25_SOURCE_ADDRESS);
>>>>>>> main
}

/**
 * @description Encodes the second SSID field of the AX.25 Frame Header
 * @return @return string of bits (e.g. "10010101")
 */
string frame_constructor::getSSID1() {
<<<<<<< HEAD
    spdlog::trace("Setting AX.25 frame SSID1 to {}", estts::ax25::AX25_SSID1);
    return reinterpret_cast<char const *>(estts::ax25::AX25_SSID1);
=======
    return binConverter.toBinary(headerConsts.AX25_SSID1);
>>>>>>> main
}

/**
 * @description Encodes the Control field of the AX.25 Frame Header
 * @return string of bits (e.g. "10010101")
 */
string frame_constructor::getControl() {
<<<<<<< HEAD
    spdlog::trace("Setting AX.25 frame control to {}", estts::ax25::AX25_CONTROL);
    return reinterpret_cast<char const *>(estts::ax25::AX25_CONTROL);
=======
    // By default, the leading zero for 0x03 will get deleted, so prepending "0000" will compensate
    return string(4, '0') + binConverter.toBinary(headerConsts.AX25_CONTROL);
>>>>>>> main
}

/**
 * @description Encodes the PID field of the AX.25 Frame Header
 * @return string of bits (e.g. "10010101")
 */
string frame_constructor::getPID() {
<<<<<<< HEAD
    spdlog::trace("Setting AX.25 frame PID to {}", estts::ax25::AX25_PID);
    return reinterpret_cast<char const *>(estts::ax25::AX25_PID);
=======
    return binConverter.toBinary(headerConsts.AX25_PID);
>>>>>>> main
}

/**
 * @description Retrieves the encoded AX.25 Information Field
 * @return string of bits (e.g. "10010101")
 */
string frame_constructor::getInfoField() {
<<<<<<< HEAD
    auto info_field = this->build_info_field();
    spdlog::trace("Setting AX.25 frame information field to {}", info_field);
    return info_field;
=======
    if (informationField == nullptr) {
        cerr << "Error [frame_constructor] - Invalid information field: informationField = nullptr";

        return string(INFO_FIELD_SIZE, '0');
    } else {
        return informationField->encode();
    }
>>>>>>> main
}

/**
 * @description Encodes the Frame Check Sequence field of the AX.25 Frame Header
<<<<<<< HEAD
 * @return string of bits (e.g. "10010101")
 */
string frame_constructor::getFCSBits() {
    spdlog::trace("Setting AX.25 frame sequence number to {}", this->command->sequence);
    return reinterpret_cast<char const *>(this->command->sequence);
=======
 * @return @return string of bits (e.g. "10010101")
 */
string frame_constructor::getFCSBits() {
    /* Throw an error if FCS doesn't start with "0x" or if it is the incorrect size */
    if (FCS.substr(0, 2) != "0x" || ((FCS.length() - 2) * 4) != FCS_SIZE) {
        cerr << "Error [frame_constructor] - Invalid hex value: " << FCS << " Expected size: " << FCS_SIZE << endl;

        return string(FCS_SIZE, '0');
    } else {
        return binConverter.toBinary(FCS);
    }
>>>>>>> main
}

// ~~~~~~~~~~~ ENCODING ~~~~~~~~~~~

/**
 * @description Retrieves the encodings of the entire AX.25 Frame
 * @return string of bits (e.g. "10010101...")
 */
<<<<<<< HEAD
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
=======
 string frame_constructor::encode() {
    string encodedFrame;

    encodedFrame += getFlag() + getDestAddr() + getSSID0() 
                                + getSrcAddr() + getSSID1() + getControl() + getPID();
    encodedFrame += getInfoField();
    encodedFrame += getFCSBits() + getFlag(); 

    return encodedFrame;
 }
>>>>>>> main

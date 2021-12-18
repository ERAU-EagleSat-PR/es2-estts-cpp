//
// Created by Cody on 12/17/2021.
//

#include <iostream>
#include <cstring>
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
    return binConverter.toBinary(estts::AX25_FLAG);
}

/**
 * @description Encodes the Destination Address field of the AX.25 Frame Header
 * @return string of bits (e.g. "10010101")
 */
string frame_constructor::getDestAddr() {
    return binConverter.toBinary(estts::AX25_DESTINATION_ADDRESS);
}

/**
 * @description Encodes the first SSID field of the AX.25 Frame Header
 * @return @return string of bits (e.g. "10010101")
 */
string frame_constructor::getSSID0() {
    return binConverter.toBinary(estts::AX25_SSID0);
}

/**
 * @description Encodes the Source Address field of the AX.25 Frame Header
 * @return string of bits (e.g. "10010101")
 */
string frame_constructor::getSrcAddr() {
    return binConverter.toBinary(estts::AX25_SOURCE_ADDRESS);
}

/**
 * @description Encodes the second SSID field of the AX.25 Frame Header
 * @return @return string of bits (e.g. "10010101")
 */
string frame_constructor::getSSID1() {
    return binConverter.toBinary(estts::AX25_SSID1);
}

/**
 * @description Encodes the Control field of the AX.25 Frame Header
 * @return string of bits (e.g. "10010101")
 */
string frame_constructor::getControl() {
    // By default, the leading zero for 0x03 will get deleted, so prepending "0000" will compensate
    return string(4, '0') + binConverter.toBinary(estts::AX25_CONTROL);
}

/**
 * @description Encodes the PID field of the AX.25 Frame Header
 * @return string of bits (e.g. "10010101")
 */
string frame_constructor::getPID() {
    return binConverter.toBinary(estts::AX25_PID);
}

/**
 * @description Retrieves the encoded AX.25 Information Field
 * @return string of bits (e.g. "10010101")
 */
string frame_constructor::getInfoField() {
    if (informationField == nullptr) {
        cerr << "Error [frame_constructor] - Invalid information field: informationField = nullptr";

        return string(INFO_FIELD_SIZE, '0');
    } else {
        return informationField->encode();
    }
}

/**
 * @description Encodes the Frame Check Sequence field of the AX.25 Frame Header
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
}

// ~~~~~~~~~~~ ENCODING ~~~~~~~~~~~

/**
 * @description Retrieves the encodings of the entire AX.25 Frame
 * @return string of bits (e.g. "10010101...")
 */
 string frame_constructor::encode() {
    string encodedFrame;

    encodedFrame += getFlag() + getDestAddr() + getSSID0() 
                                + getSrcAddr() + getSSID1() + getControl() + getPID();
    encodedFrame += getInfoField();
    encodedFrame += getFCSBits() + getFlag(); 

    return encodedFrame;
 }
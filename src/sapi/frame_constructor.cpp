#include <iostream>
#include <string>
#include <sstream>
#include "frame_constructor.h"
#include "constants.h"

using std::cerr;
using std::cout;
using std::endl;
using std::string;
using std::stringstream;
using std::strlen;
using std::hex;

// ~~~~~~~~~~~ CONVERSION FUNCTIONS ~~~~~~~~~~~

/* Convert hex string to binary string */ 
string frame_constructor::convertHexToBin(const string hexField) {
    string bitStream;
        
    /* Convert each hex character to binary */
    for (unsigned int i = 2; i < hexField.length(); ++i) 
        bitStream += hexToBin.find(hexField[i])->second;

    return bitStream;
}

/* Convert a ASCII character array to hex */
string frame_constructor::convertToBin(const unsigned char field[]) {
    stringstream hexStream;

    /* Convert each character in the array into a hex string and append them together */
    hexStream << "0x" << hex;
    for (int i = 0; i < strlen((char*)field); ++i)
        hexStream << (int)field[i];
    
    return convertHexToBin(hexStream.str());
}

/* Convert an unsigned character hex value to binary */
string frame_constructor::convertToBin(const unsigned char field) {
    stringstream hexStream;

    hexStream << "0x" << hex << (int)field;

    return convertHexToBin(hexStream.str());
}

// ~~~~~~~~~~~ FRAME HEADER GETTERS  ~~~~~~~~~~~

/* Get flag as a bit string */
string frame_constructor::getFlag() {
    return convertToBin(estts::AX25_FLAG);
}

/* Get destination address section as a bit string */
string frame_constructor::getDestAddr() {
    return convertToBin(estts::AX25_DESTINATION_ADDRESS);
}

/* Get address SSID0 as a bit string */
string frame_constructor::getSSID0() {
    return convertToBin(estts::AX25_SSID0);
}

/* Get source address section as a bit string */
string frame_constructor::getSrcAddr() {
    return convertToBin(estts::AX25_SOURCE_ADDRESS);
}

/* Get SSID1 section as a bit string */
string frame_constructor::getSSID1() {
    return convertToBin(estts::AX25_SSID1);
}

/* Get control section as a bit string */
string frame_constructor::getControl() {
    // By default, the leading zero for 0x03 will get deleted, so prepending "0000" will compensate
    return string(4, '0') + convertToBin(estts::AX25_CONTROL);
}

/* Get PID section as a bit string */
string frame_constructor::getPID() {
    return convertToBin(estts::AX25_PID);
}

/* Get information field as a bit string */
string frame_constructor::getInfoField() {
    if (informationField == nullptr) {
        cerr << "Error [frame_constructor] - Invalid information field: informationField = nullptr";

        return string(INFO_FIELD_SIZE, '0');
    } else {
        return informationField->encode();
    }
}

/* Get FCS section as a bit string */
string frame_constructor::getFCSBits() {
    /* Throw an error if FCS doesn't start with "0x" or if it is the incorrect size */
    if (FCS.substr(0, 2) != "0x" || ((FCS.length() - 2) * 4) != FCS_SIZE) {
        cerr << "Error [frame_constructor] - Invalid hex value: " << FCS << " Expected size: " << FCS_SIZE << endl;

        return string(FCS_SIZE, '0');
    } else {
        return convertHexToBin(FCS);
    }
}

// ~~~~~~~~~~~ ENCODING ~~~~~~~~~~~

/*  Encode and get the entire AX.25 frame as a bit string */
 string frame_constructor::encode() {
    string encodedFrame = "";

    encodedFrame += getFlag() + getDestAddr() + getSSID0() 
                                + getSrcAddr() + getSSID1() + getControl() + getPID();
    encodedFrame += getInfoField();
    encodedFrame += getFCSBits() + getFlag(); 

    return encodedFrame;
    
 }
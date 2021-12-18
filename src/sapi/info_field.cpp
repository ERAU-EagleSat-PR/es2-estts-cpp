//
// Created by Cody on 12/17/2021.
//

#include <iostream>
#include <string>
#include "info_field.h"

using std::cerr;
using std::endl;
using std::string;

/**
 * @description Encodes the Address section of the AX.25 Information Field
 * @return string of bits (e.g. "10010101")
 */
string info_field::getAddressBits() {
    return binConverter.toBinary(ADDRESS_SIZE, address);
}

/**
 * @description Encodes the Timestamp section of the AX.25 Information Field
 * @return string of bits (e.g. "10010101")
 */
string info_field::getTimeStampBits() {
    return binConverter.toBinary(TIMESTAMP_SIZE, timeStamp);
}

/**
 * @description Encodes the Sequence section of the AX.25 Information Field
 * @return string of bits (e.g. "10010101")
 */
string info_field::getSequenceBits() {
    return binConverter.toBinary(SEQUENCE_SIZE, sequence);
}

/**
 * @description Encodes the Command ID section of the AX.25 Information Field
 * @return string of bits (e.g. "10010101")
 */
string info_field::getCommandIDBits() {
    return binConverter.toBinary(COMMANDID_SIZE, commandID);
}

/**
 * @description Encodes the Method section of the AX.25 Information Field
 * @return One bit as a char (e.g. "1")
 */
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
}

/**
 * @description Encodes the Data section of the AX.25 Information Field
 * @return string of bits (e.g. "10010101...")
 */
string info_field::getDataBits() {
    return binConverter.toBinary(DATA_SIZE, data);
}

/**
 * @description Retrieves the encodings of the entire AX.25 Information Field
 * @return string of bits (e.g. "10010101...")
 */
string info_field::encode() {
    string infoFieldStream;

    infoFieldStream += getAddressBits() + getTimeStampBits()
        + getSequenceBits() + getCommandIDBits()
        + getMethodBits() + getDataBits(); 

    return infoFieldStream;
}
//
// Created by Cody on 12/17/2021.
//

#include <iostream>
#include <cstring>
#include <sstream>
#include "bin_converter.h"

using std::cerr;
using std::endl;
using std::stringstream;
using std::string;
using std::hex;

/**
 * @description Converts hex string to binary string
 * @param const short int size, const string& hexField
 * @return string of bits (e.g. "10010101")
 */
string bin_converter::toBinary(const short int size, const string &hexField) {
    string bitStream;
    int bitPadding = size - ((hexField.length() - 2) * 4);

    /* If padding zeros are needed, prepend them to the string.
     * Else if there is an invalid number of hex values,
     * send an error and return zero's. */
    if (hexField.substr(0, 2) != "0x" || bitPadding < 0) {
        cerr << "Error [info_field] - Invalid hex value: " << hexField << "\nExpected size: " << size << endl;

        return string(size, '0');
    } else if (bitPadding > 0) {
        bitStream += string(bitPadding, '0');
    }

    /* Convert each hex character to binary */
    for (int i = 2; i < hexField.length(); ++i)
        bitStream += hexToBinMap.find(hexField[i])->second;

    return bitStream;
}

/**
 * @description Converts hex string to binary string
 * @param const string hexField
 * @return string of bits (e.g. "10010101")
 */
string bin_converter::toBinary(const string &hexField) {
    string bitStream;

    /* Convert each hex character to binary */
    for (unsigned int i = 2; i < hexField.length(); ++i)
        bitStream += hexToBinMap.find(hexField[i])->second;

    return bitStream;
}

/**
 * @description Converts a ASCII character array to hex
 * @param const unsigned char field[]
 * @return string of bits (e.g. "10010101")
 */
string bin_converter::toBinary(const unsigned char field[]) {
    stringstream hexStream;

    /* Convert each character in the array into a hex string and append them together */
    hexStream << "0x" << hex;
    for (int i = 0; i < strlen((char *) field); ++i)
        hexStream << (int) field[i];

    return toBinary(hexStream.str());
}

/**
 * @description Converts an unsigned character hex value to binary
 * @param const unsigned char field
 * @return string of bits (e.g. "10010101")
 */
string bin_converter::toBinary(const unsigned char field) {
    stringstream hexStream;

    hexStream << "0x" << hex << (int) field;

    return toBinary(hexStream.str());
}
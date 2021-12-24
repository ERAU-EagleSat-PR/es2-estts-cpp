//
<<<<<<< HEAD
// Created by Cody on 12/17/2021.
=======
// Created by Cody Park on 12/17/2021.
>>>>>>> main
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
<<<<<<< HEAD
string bin_converter::toBinary(const short int size, const string &hexField) {
    string bitStream;
    int bitPadding = size - ((hexField.length() - 2) * 4);
=======
string bin_converter::toBinary(const short int size, const string& hexField) {
    string bitStream;
    int bitPadding = size - ((hexField.length()-2) * 4);
>>>>>>> main

    /* If padding zeros are needed, prepend them to the string.
     * Else if there is an invalid number of hex values,
     * send an error and return zero's. */
    if (hexField.substr(0, 2) != "0x" || bitPadding < 0) {
<<<<<<< HEAD
        cerr << "Error [info_field] - Invalid hex value: " << hexField << "\nExpected size: " << size << endl;
=======
        cerr << "Error - Invalid hex value: " << hexField << "\nExpected size: " << size << endl;
>>>>>>> main

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
<<<<<<< HEAD
string bin_converter::toBinary(const string &hexField) {
=======
string bin_converter::toBinary(const string& hexField) {
>>>>>>> main
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
<<<<<<< HEAD
    for (int i = 0; i < strlen((char *) field); ++i)
        hexStream << (int) field[i];
=======
    for (int i = 0; i < strlen((char*)field); ++i)
        hexStream << (int)field[i];
>>>>>>> main

    return toBinary(hexStream.str());
}

/**
 * @description Converts an unsigned character hex value to binary
 * @param const unsigned char field
 * @return string of bits (e.g. "10010101")
 */
string bin_converter::toBinary(const unsigned char field) {
    stringstream hexStream;

<<<<<<< HEAD
    hexStream << "0x" << hex << (int) field;

    return toBinary(hexStream.str());
=======
    hexStream << "0x" << hex << (int)field;

    return toBinary(hexStream.str());
}

/**
 * @description Converts binary string to hex string
 * @param const short int size, const string& hexField
 * @return string of hex digits (e.g. "0x12fd7a")
 */
string bin_converter::toHex(const string binField) {
    std::string hexString = "0x";

    /* Every four bits gets converted to a hexadecimal number,
    * So make sure the bit string is divisible by 4 */
    if (binField.length() % 4 != 0) {
        cerr << "Error - Invalid binary value: Should be a multiple of 4" << endl;

        return "";
    } else {
        /* Convert each hex character to binary */
        for (int i = 0; i < binField.length(); i += 4)
            hexString += BinToHexMap.find(binField.substr(i, 4))->second;

        return hexString;
    }
>>>>>>> main
}
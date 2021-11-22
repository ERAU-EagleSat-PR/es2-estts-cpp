#include <iostream>
#include <string>
#include <sstream>
#include "info_field.h"
using std::cerr;
using std::endl;
using std::string;

/* Convert hex string to binary string */ 
string info_field::convertHexToBin(const short int size, const string& hexField) {
    string bitStream;
    int bitPadding = size - ((hexField.length()-2) * 4);

    /* If padding zero's are needed, prepend them to the string. 
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
        bitStream += hexToBin.find(hexField[i])->second;

    return bitStream;
}

/* Get address section as a bit string */
string info_field::getAddressBits() {
    return convertHexToBin(ADDRESS_SIZE, address);
}

/* Get time stamp section as a bit string */
string info_field::getTimeStampBits() {
    return convertHexToBin(TIMESTAMP_SIZE, timeStamp);
}

/* Get sequence section as a bit string */
string info_field::getSequenceBits() {
    return convertHexToBin(SEQUENCE_SIZE, sequence);
}

/* Get command ID section as a bit string */
string info_field::getCommandIDBits() {
    return convertHexToBin(COMMANDID_SIZE, commandID);
}

/* Get method section as a bit string */
char info_field::getMethodBits() {
    /* If input is valid, return associated bit string
     * Else, send error then return */
    if (method == "0x1" || method == "0x0") {
        return method.back();
    } else {
        cerr << "Error [info_field] - Invalid hex value: " << method << "\nExpected size: " << METHOD_SIZE << endl;
        return '0';
    }
}

/* Get data section as a bit string */
string info_field::getDataBits() {
    return convertHexToBin(DATA_SIZE, data);
}

/* Get entire Information Field in bits */
string info_field::encode() {
    string infoFieldStream;

    infoFieldStream += getAddressBits() + getTimeStampBits()
        + getSequenceBits() + getCommandIDBits()
        + getMethodBits() + getDataBits(); 

    return infoFieldStream;
}
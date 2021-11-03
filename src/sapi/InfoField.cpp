#include <iostream>
#include <string>
#include <sstream>
#include "InfoField.h"
using std::cerr;
using std::endl;
using std::stringstream;
using std::string;

/* Convert hex string to binary string */ 
string InfoField::convertHexToBin(static const short int size, const string hexField) {
    stringstream bitStream;
    int bitPadding = size - ((hexField.length()-2) * 4);

    /* If padding zero's are needed, prepend them to the string. 
     * Else if there is an invalid number of hex values, 
     * send an error and return zero's. */
    if (bitPadding > 0) {
        bitStream << string(bitPadding, '0');
    } else if (bitPadding < 0) {
        cerr << "Invalid hex value" << endl;
        return string(size, '0');
    }
        
    /* Convert each hex character to binary */
    for (int i = 2; i < hexField.length(); ++i) 
        bitStream << hexToBin.find(hexField[i])->second;

    return bitStream.str();
}

/* Get address section as a bit string */
string InfoField::getAddressBits() {
    return convertHexToBin(ADDRESS_SIZE, address);
}

/* Get time stamp section as a bit string */
string InfoField::getTimeStampBits() {
    return convertHexToBin(TIMESTAMP_SIZE, timeStamp);
}

/* Get sequence section as a bit string */
string InfoField::getSequenceBits() {
    return convertHexToBin(SEQUENCE_SIZE, sequence);
}

/* Get command ID section as a bit string */
string InfoField::getCommandIDBits() {
    return convertHexToBin(COMMANDID_SIZE, commandID);
}

/* Get method section as a bit string */
char InfoField::getMethodBits() {
    /* If input is valid, return associated bit string
     * Else, send error then return */
    if (method == "0x1" || method == "0x0") {
        return method.back();
    } else {
        cerr << "Invalid hex value" << endl;
        return '0';
    }
}

/* Get data section as a bit string */
string InfoField::getDataBits() {
    return convertHexToBin(DATA_SIZE, data);
}

/* Get entire Information Field in bits */
string InfoField::getInfoField() {
    stringstream infoFieldStream;

    infoFieldStream << getAddressBits() << getTimeStampBits()
        << getSequenceBits() << getCommandIDBits()
        << getMethodBits() << getDataBits(); 

    return infoFieldStream.str();
}
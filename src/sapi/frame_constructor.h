#ifndef ES2_ESTTS_CPP_FRAME_CONSTRUCTOR_H
#define ES2_ESTTS_CPP_FRAME_CONSTRUCTOR_H

#include <string>
#include <unordered_map>
#include "info_field.h"

class frame_constructor {
private:
    /* Container used to convert Hex characters(Key) to Binary strings(Value) */
    const std::unordered_map<char, std::string> hexToBin = {
        {'0', "0000"},
        {'1', "0001"},
        {'2', "0010"},
        {'3', "0011"},
        {'4', "0100"},
        {'5', "0101"},
        {'6', "0110"},
        {'7', "0111"},
        {'8', "1000"},
        {'9', "1001"},
        {'a', "1010"},
        {'b', "1011"},
        {'c', "1100"},
        {'d', "1101"},
        {'e', "1110"},
        {'f', "1111"}
    };

    /* Constant sizes */
    static const unsigned int INFO_FIELD_SIZE = 616;
    static const unsigned char FCS_SIZE = 16;

    /* Information Field */
    info_field* informationField;
    /* Frame Check Sequence */
    std::string FCS;

    /* Converts hex string to binary */
    std::string convertHexToBin(const std::string hexField);
    /* Converts an unsigned char array to binary */
    std::string convertToBin(const unsigned char field[]);
    /* Converts an unsigned char to binary */
    std::string convertToBin(const unsigned char field);

    /* Getters for Header Field */
    std::string getFlag();
    std::string getDestAddr();
    std::string getSSID0();
    std::string getSrcAddr();
    std::string getSSID1();
    std::string getControl();
    std::string getPID();
    std::string getInfoField();
    std::string getFCSBits();
public:
    /* Constructors */
    frame_constructor() : frame_constructor(nullptr) {}
    
    frame_constructor(info_field* informationField) : 
        informationField(informationField), FCS("") {}

    /* Setters */
    inline void setInfoField(info_field* informationField) { this->informationField = informationField; }
    inline void setFCS(const std::string FCS) { this->FCS = FCS; }

    /* Encoded AX.25 Frame Getter */
    std::string encode();
};

#endif
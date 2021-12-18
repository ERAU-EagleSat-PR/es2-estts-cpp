//
// Created by Cody Park on 12/17/2021.
//

#ifndef ESTTS_BIN_CONVERTER_H
#define ESTTS_BIN_CONVERTER_H

#include <string>
#include <unordered_map>

class bin_converter {
private:
    /* Container used to convert Hex characters (Key) to Binary strings (Value) */
    const std::unordered_map<char, std::string> hexToBinMap = {
            {'0', "0000"}, {'1', "0001"},
            {'2', "0010"}, {'3', "0011"},
            {'4', "0100"}, {'5', "0101"},
            {'6', "0110"}, {'7', "0111"},
            {'8', "1000"}, {'9', "1001"},
            {'a', "1010"}, {'b', "1011"},
            {'c', "1100"}, {'d', "1101"},
            {'e', "1110"}, {'f', "1111"}
    };

    /* Container used to convert Binary strings (Key) to Hex characters (Value) */
    const std::unordered_map<std::string, char> BinToHexMap = {
            {"0000", '0'}, {"0001", '1'},
            {"0010", '2'}, {"0011", '3'},
            {"0100", '4'}, {"0101", '5'},
            {"0110", '6'}, {"0111", '7'},
            {"1000", '8'}, {"1001", '9'},
            {"1010", 'a'}, {"1011", 'b'},
            {"1100", 'c'}, {"1101", 'd'},
            {"1110", 'e'}, {"1111", 'f'}
    };
public:
    // ~~~~~~~~~~~ Convert To Binary  ~~~~~~~~~~~

    /* Converts hex string to binary and also checks size */
    std::string toBinary(short int size, const std::string& hexField);
    /* Converts hex string to binary */
    std::string toBinary(const std::string& hexField);
    /* Converts an unsigned char array to binary */
    std::string toBinary(const unsigned char field[]);
    /* Converts an unsigned char to binary */
    std::string toBinary(unsigned char field);

    // ~~~~~~~~~~~ Convert To Hex  ~~~~~~~~~~~

    /* Converts binary string to hex string */
    std::string toHex(const std::string binField);
};

#endif //ESTTS_BIN_CONVERTER_H

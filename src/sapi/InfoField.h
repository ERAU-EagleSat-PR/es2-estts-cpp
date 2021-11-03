#pragma once
#include <string>
#include <unordered_map>
#include <sstream>

class InfoField {
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

    /* Bit sizes of each section in AX.25 Packet Information Field */
    const unsigned char ADDRESS_SIZE = 8;
    const unsigned char TIMESTAMP_SIZE = 16;
    const unsigned char SEQUENCE_SIZE = 8;
    const unsigned char COMMANDID_SIZE = 8;
    const unsigned char METHOD_SIZE = 1;
    static constexpr short int DATA_SIZE = 575;

    /* Each section in the AX.25 Packet Information Field */
    std::string address;
    std::string timeStamp;
    std::string sequence;
    std::string commandID;
    std::string method;
    std::string data;

public:
    /* Constructors */
    InfoField() : address(""), 
                       timeStamp(""),
                       sequence(""), 
                       commandID(""),
                       method(""),
                       data("") {}

    InfoField(std::string address, 
                   std::string timeStamp,
                   std::string sequence, 
                   std::string commandID,
                   std::string method, 
                   std::string data) :
                   address(address), 
                   timeStamp(timeStamp),
                   sequence(sequence), 
                   commandID(commandID),
                   method(method), 
                   data(data) {}
    
    /* Converts hex string to binary */
    std::string convertHexToBin(static const short int size, const std::string hexField);

    /* Setters (Accepts hex strings) */
    inline void setAddress(const std::string address) { this->address = address;  }
    inline void setTimeStamp(const std::string timeStamp) { this->timeStamp = timeStamp; }
    inline void setSequence(const std::string sequence) { this->sequence = sequence; }
    inline void setCommandID(const std::string commandID) { this->commandID = commandID; }
    inline void setMethod(const std::string method) { this->method = method; }
    inline void setData(const std::string data) { this->data = data; }

    /* Getters */
    std::string getAddressBits();
    std::string getTimeStampBits();
    std::string getSequenceBits();
    std::string getCommandIDBits();
    char getMethodBits();
    std::string getDataBits();

    std::string getInfoField();
};


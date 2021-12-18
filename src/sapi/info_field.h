//
// Created by Cody on 12/17/2021.
//

#ifndef ES2_ESTTS_CPP_INFOFIELD_H
#define ES2_ESTTS_CPP_INFOFIELD_H

#include <string>
#include <unordered_map>
#include "bin_converter.h"

class info_field {
private:
    /* Binary converter */
    bin_converter binConverter;

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

    /* Getters */
    std::string getAddressBits();
    std::string getTimeStampBits();
    std::string getSequenceBits();
    std::string getCommandIDBits();
    char getMethodBits();
    std::string getDataBits();
public:
    /* Constructors */
    info_field() : info_field("", "", "", "", "", "") {}

    info_field(std::string address, 
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
    
    /* Setters (Accepts hex strings) */
    inline void setAddress(const std::string address) { this->address = address;  }
    inline void setTimeStamp(const std::string timeStamp) { this->timeStamp = timeStamp; }
    inline void setSequence(const std::string sequence) { this->sequence = sequence; }
    inline void setCommandID(const std::string commandID) { this->commandID = commandID; }
    inline void setMethod(const std::string method) { this->method = method; }
    inline void setData(const std::string data) { this->data = data; }

    /* Encoded Information Field Getter */
    std::string encode();
};

#endif
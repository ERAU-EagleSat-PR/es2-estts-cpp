//
// Created by Hayden Roszell on 10/28/21.
//

#ifndef ES2_ESTTS_CPP_SYSTEM_API_H
#define ES2_ESTTS_CPP_SYSTEM_API_H

#include <iostream>
#include "constants.h"
#include "InfoField.h"
using namespace std;

namespace sapi {
    typedef struct SAPICommandStructure {
        unsigned char address;
        unsigned char command;
        unsigned char method;
        unsigned char * body;
    } SAPICommandStructure;
}

class System_API {
private:
    const unsigned char flag; // Unsigned char -> [0,255]

    /* AX.25 frame header */
    // These values are constant, see constants.h
    const unsigned char * destAddr;
    const unsigned char ssid0;
    const unsigned char * srcAddr;
    const unsigned char ssid1;
    const unsigned char control;
    const unsigned char pid;

    /* AX.25 frame body */
    unsigned char * crc16;
    unsigned char * information_field;
    sapi::SAPICommandStructure * command;
public:
    explicit System_API() : flag(estts::AX25_FLAG), destAddr(estts::AX25_DESTINATION_ADDRESS), ssid0(estts::AX25_SSID0),
            srcAddr(estts::AX25_SOURCE_ADDRESS), ssid1(estts::AX25_SSID1), control(estts::AX25_CONTROL),
            pid(estts::AX25_PID), crc16(nullptr), information_field(nullptr) {}

    explicit System_API(sapi::SAPICommandStructure * command) : System_API() {
        this->information_field = (unsigned char *)0x55555; // TODO - populate information field inside constructor
        this->command = command;
    };
    static estts::Status buildAX25Frame();
    std::string buildFrameHeader();
};


#endif //ES2_ESTTS_CPP_SYSTEM_API_H

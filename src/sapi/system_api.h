//
// Created by Hayden Roszell on 10/28/21.
//

#ifndef ES2_ESTTS_CPP_SYSTEM_API_H
#define ES2_ESTTS_CPP_SYSTEM_API_H

#include <iostream>
#include "constants.h"
#include "info_field.h"
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

};


#endif //ES2_ESTTS_CPP_SYSTEM_API_H

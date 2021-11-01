//
// Created by Hayden Roszell on 10/28/21.
//

#ifndef ES2_ESTTS_CPP_SYSTEM_API_H
#define ES2_ESTTS_CPP_SYSTEM_API_H

#include "constants.h"

typedef struct AX25FrameHeader {
    const unsigned char * destAddr; // Unsigned char -> [0,255]
    const unsigned char * ssid0; // These values are constant, see constants.h
    const unsigned char * srcAddr;
    const unsigned char * ssid1;
    const unsigned char * control;
    const unsigned char * pid;
} AX25FrameHeader;

typedef struct AX25Frame {
    const unsigned char * flag; // Unsigned char -> [0,255]
    AX25FrameHeader header;
    const unsigned char crc16[16];
    const unsigned char * information_field;

} AX25Frame;

class system_api {
private:

public:

};


#endif //ES2_ESTTS_CPP_SYSTEM_API_H

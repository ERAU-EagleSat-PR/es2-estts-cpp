//
// Created by bstri on 2/18/2022.
//

#ifndef ESTTS_AX25_FRAME_H
#define ESTTS_AX25_FRAME_H

#include <string>
#include "constants.h"
#include "bitset"
#include <iostream>
#include <string>
#include <array>
#include <sstream>

#define POLY 0x8408

class ax25_data {

private:
    //Header
    std::string flag = estts::ax25::AX25_FLAG;
    std::string dest_addr = estts::ax25::AX25_DESTINATION_ADDRESS;
    std::string ssid0 = estts::ax25::AX25_SSID0;
    std::string src_addr = estts::ax25::AX25_SOURCE_ADDRESS;
    std::string ssid1 = estts::ax25::AX25_SSID1;
    std::string control = estts::ax25::AX25_CONTROL;
    std::string pid = estts::ax25::AX25_PID;
    std::string info = "";
    std::string fcs = "";

public:
    //Header Getters
    std::string getFlag();
    std::string getDestAddr();
    std::string getSSID0();
    std::string getSrcAddr();
    std::string getSSID1();
    std::string getControl();
    std::string getPID();
    std::string getInfoField();
    std::string getFcs();

    //Header Setters
    void setInfo(std::string info);
    void setFCS(std::string fcs);

    //Worker Methods
    std::string build_data();
    std::string build_preamble();
    std::string build_postamble();
    void crc16_ccitt();
    //todo move to util?
    std::string nrzi_encode(std::string info);
    std::string convertString(std::string input, int arrIndex);
    std::string scramble(std::string info);

    //constructor
    ax25_data();

};


#endif //ESTTS_AX25_FRAME_H

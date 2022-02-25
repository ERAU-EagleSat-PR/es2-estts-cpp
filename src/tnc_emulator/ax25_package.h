//
// Created by Home on 2/24/2022.
//

#ifndef ES2_ESTTS_CPP_AX25_PACKAGE_H
#define ES2_ESTTS_CPP_AX25_PACKAGE_H
#include "string"

class ax25_package {
private:
    std::string preamble = "0xAAAAAAAAAA";
    std::string sync = "0x7E";
    std::string data1= "";
    std::string data2= "";
    std::string crc= "";
public:
    //getters
    std::string getPreamble();
    std::string getSync();
    std::string getData1();
    std::string getData2();
    std::string getCrc();
    //setters
    void setData1(std::string length);
    void setData2(std::string data);
    void setCrc(std::string cyclic);
    //worker methods
    //calc crc todo
    std::string buildFrame();
};


#endif //ES2_ESTTS_CPP_AX25_PACKAGE_H

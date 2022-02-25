//
// Created by Home on 2/24/2022.
//

#include "ax25_package.h"
//getters
std::string ax25_package::getPreamble(){
    return preamble;
}
std::string ax25_package::getSync(){
    return sync;
}
std::string ax25_package::getData1(){
    return data1;
}
std::string ax25_package::getData2(){
    return data2;
}
std::string ax25_package::getCrc(){
    return crc;
}
//setters
void  ax25_package::setData1(std::string length){
    this->data1 = length;
}
void  ax25_package::setData2(std::string data){
    this->data2 = data;
}
void  ax25_package::setCrc(std::string cyclic){
    this->crc = cyclic;
}
//worker methods
std::string ax25_package::buildFrame(){
    std::string frame = getPreamble();
    frame += getSync();
    frame += getData1();
    frame += getData2();
    //todo crc
    //this->setCrc(calc_crc(data1 + data2));
    frame += getCrc();
    return frame;
}
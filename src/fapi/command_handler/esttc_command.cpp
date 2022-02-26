//
// Created by bstri on 2/20/2022.
//

#include "esttc_command.h"
#include <string>
#include "crc/FastCRC.h"

//Header Getters
std::string esttc_command::getHeader(){
    return header;
}
std::string esttc_command::getCmdType(){
    return cmd_type;
}
std::string esttc_command::getAddr(){
    return addr;
}
std::string esttc_command::getCmdCode(){
    return cmd_code;
}
std::string esttc_command::getVariableData(){
    return variable_data;
}
std::string esttc_command::getBlank(){
    return blank;
}
std::string esttc_command::getCRC32(){
    return crc32;
}
std::string esttc_command::getCr(){
    return cr;
}

//Header Setters
void esttc_command::setCmdType(std::string cmd_t){
    this->cmd_type = cmd_t;
}
void esttc_command::setAddr(std::string a){
    this->addr = a;
}
void esttc_command::setCmdCode(std::string cmd_c){
    this->cmd_code = cmd_c;
}
void esttc_command::setVariableData(std::string vd){
    this->variable_data = vd;
}
void esttc_command::setBlank(std::string b){
    this->blank = b;
}
void esttc_command::setCrc32(std::string crc){
    this->crc32 = crc;
}

std::string esttc_command::buildCommand(){
    std::string cmdStr = this->header;
    cmdStr += this->cmd_type;
    cmdStr += this->addr;
    cmdStr += this->cmd_code;
    cmdStr += this->variable_data;
    cmdStr += this->blank;
    cmdStr += this->crc32;
    cmdStr += this->cr;
    return cmdStr;
}

void esttc_command::calcCrc32(){
    std::string crc = "";
    FastCRC32 calculator;
    crc = calculator.crc32(reinterpret_cast<const uint8_t*>(&this->variable_data[0]), sizeof(this->variable_data));
    this->crc32 = crc;
}
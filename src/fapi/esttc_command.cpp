//
// Created by bstri on 2/20/2022.
//

#include "esttc_command.h"
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
void esttc_command::setCmdType(std::string cmd_type){
    this->cmd_type = cmd_type;
}
void esttc_command::setAddr(std::string addr){
    this->addr = addr;
}
void esttc_command::setCmdCode(std::string cmd_code){
    this->cmd_code = cmd_code;
}
void esttc_command::setVariableData(std::string variable_data){
    this->variable_data = variable_data;
}
void esttc_command::setBlank(std::string blank){
    this->blank = blank;
}
void esttc_command::setCrc32(std::string crc32){
    this->crc32 = crc32;
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
    this->blank = " ";
    this->crc32 = crc;
}
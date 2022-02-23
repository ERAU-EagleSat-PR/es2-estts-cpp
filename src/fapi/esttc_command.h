//
// Created by bstri on 2/20/2022.
//

#ifndef ESTTS_ESTTC_COMMAND_H
#define ESTTS_ESTTC_COMMAND_H
#include "constants.h"

class esttc_command {
private:
    //Header
    std::string header = "ES+";
    std::string cmd_type = "";
    std::string addr = "";
    std::string cmd_code = "";
    std::string variable_data = "";
    std::string blank = "";
    std::string crc32 = "";
    std::string cr = "<CR>";


public:
    //Header Getters
    std::string getHeader();
    std::string getCmdType();
    std::string getAddr();
    std::string getCmdCode();
    std::string getVariableData();
    std::string getBlank();
    std::string getCRC32();
    std::string getCr();

    //Header Setters
    void setCmdType(std::string cmd_type);
    void setAddr(std::string addr);
    void setCmdCode(std::string cmd_code);
    void setVariableData(std::string variable_data);
    void setBlank(std::string blank);
    void setCrc32(std::string crc32);

    std::string buildCommand();
    void calcCrc32();
};


#endif //ESTTS_ESTTC_COMMAND_H

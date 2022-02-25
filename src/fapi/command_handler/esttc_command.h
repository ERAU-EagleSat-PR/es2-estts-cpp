//
// Created by bstri on 2/20/2022.
//

#ifndef ESTTS_ESTTC_COMMAND_H
#define ESTTS_ESTTC_COMMAND_H
#include "string"

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
    void setCmdType(std::string cmd_t);
    void setAddr(std::string a);
    void setCmdCode(std::string cmd_c);
    void setVariableData(std::string vd);
    void setBlank(std::string b);
    void setCrc32(std::string crc);

    std::string buildCommand();
    void calcCrc32();
};


#endif //ESTTS_ESTTC_COMMAND_H

//
// Created by bstri on 2/7/2022.
//

#ifndef ESTTS_ESTTC_H
#define ESTTS_ESTTC_H

#include <string>


class esttc {
private:
    std::string header = "";
    std::string cmd = "";
    std::string addr = "";
    std::string code = "";
    std::string variable = "";
    std::string blank = "";
    std::string crc= "";
    std::string cr = "";
public:

    esttc(std::string header = "", std::string cmd = "", std::string addr = "",
          std::string code = "", std::string variable = "", std::string blank = "",
          std::string crc = "", std::string cr = "") :
          header(header), cmd(cmd), addr(addr),code(code),variable(variable),
          blank(blank),crc(crc),cr(cr){}

    std::string get_header();
    std::string get_cmd();
    std::string get_addr();
    std::string get_code();
    std::string get_variable();
    std::string get_blank();
    std::string get_crc();
    std::string get_cr();

    void set_header(std::string header);
    void set_cmd(std::string cmd);
    void set_addr(std::string addr);
    void set_code(std::string code);
    void set_variable(std::string variable);
    void set_blank(std::string blank);
    void set_crc(std::string crc);
    void set_cr(std::string cr);
};


#endif //ESTTS_ESTTC_H

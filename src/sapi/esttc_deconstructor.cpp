//
// Created by bstri on 2/7/2022.
//

#include "esttc_deconstructor.h"
using std::string;
/**
 * @brief deconstruct_esttc deconstructs a command string into esttc object
 * @param string packet to be deconstructed
 * @return esttc object with command data
 */
esttc deconstruct_esttc(std::string command_string){

    esttc command;

    const string header = command_string.substr(0, 3);
    const string cmd = command_string.substr(3, 1);
    const string addr = command_string.substr(4, 2);
    const string code = command_string.substr(6, 2);
    string::size_type variable_end = command_string.substr(8).find(' ');
    //  NOTE If string.find() doesn't find it's character at all, it returns string::npos which is just -1 of the size_type type
    const string variable = command_string.substr(8, variable_end - 1);
    const string blank = command_string.substr(variable_end, 1);
    string::size_type end_tag = command_string.substr(variable_end + 1).find('<');
    const string crc = command_string.substr(variable_end + 1, end_tag);
    const string cr = command_string.substr(end_tag);

    command.set_header(header);
    command.set_cmd(cmd);
    command.set_addr(addr);
    command.set_code(code);
    command.set_variable(variable);
    command.set_blank(blank);
    command.set_crc(crc);
    command.set_cr(cr);

    return command;
}
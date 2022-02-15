//
// Created by bstri on 2/7/2022.
//

#include "esttc.h"
using std::string;

/**
 * @brief get esttc header
 * @param None
 * @return esttc header
 */
string esttc::get_header(){
    return header;
}
/**
 * @brief get esttc command type
 * @param None
 * @return esttc command type
 */
string esttc::get_cmd(){
    return cmd;
}
/**
 * @brief get esttc address
 * @param None
 * @return esttc address
 */
string esttc::get_addr(){
    return addr;
}
/**
 * @brief get esttc command code
 * @param None
 * @return esttc command code
 */
string esttc::get_code(){
    return code;
}
/**
 * @brief get esttc variable data
 * @param None
 * @return esttc variable data
 */
string esttc::get_variable(){
    return variable;
}
/**
 * @brief get esttc blank
 * @param None
 * @return esttc blank
 */
string esttc::get_blank(){
    return blank;
}
/**
 * @brief get esttc crc32
 * @param None
 * @return esttc crc32
 */
string esttc::get_crc(){
    return crc;
}
/**
 * @brief get esttc cr
 * @param None
 * @return esttc cr
 */
string esttc::get_cr(){
    return cr;
}
/**
 * @brief set esttc header
 * @param string header
 * @return None
 */
void esttc::set_header(string header) {
    this->header = header;
}
/**
 * @brief set esttc command type
 * @param string command type
 * @return None
 */
void esttc::set_cmd(string cmd) {
    this->cmd = cmd;
}
/**
 * @brief set esttc address
 * @param string address
 * @return None
 */
void  esttc::set_addr(string addr) {
    this-> addr = addr;
}
/**
 * @brief set esttc command code
 * @param string command code
 * @return None
 */
void  esttc::set_code(string code) {
    this->code = code;
}
/**
 * @brief set esttc variable data
 * @param string variable data
 * @return None
 */
void  esttc::set_variable(string variable) {
    this->variable = variable;
}
/**
 * @brief set esttc blank
 * @param string blank
 * @return None
 */
void  esttc::set_blank(string blank) {
    this->blank = blank;
}
/**
 * @brief set esttc crc32
 * @param string crc32
 * @return None
 */
void  esttc::set_crc(string crc) {
    this->crc = crc;
}
/**
 * @brief set esttc cr
 * @param string cr
 * @return None
 */
void  esttc::set_cr(string cr) {
    this->cr = cr;
}

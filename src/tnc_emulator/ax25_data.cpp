//
// Created by bstri on 2/18/2022.
//

#include "ax25_data.h"
#include "crc/FastCRC.h"
#include "queue.h"

//Header Getters
std::string ax25_data::getFlag(){
    return flag;
}
std::string ax25_data::getDestAddr(){
    return dest_addr;
}
std::string ax25_data::getSSID0(){
    return ssid0;
}
std::string ax25_data::getSrcAddr(){
    return src_addr;
}
std::string ax25_data::getSSID1(){
    return ssid1;
}
std::string ax25_data::getControl(){
    return control;
}
std::string ax25_data::getPID(){
    return pid;
}
std::string ax25_data::getInfoField(){
    return info;
}
std::string ax25_data::getFcs(){
    return fcs;
}

//Header Setters
void ax25_data::setInfo(std::string info){
    this->info = info;
}
void ax25_data::setFCS(std::string fcs){
    this->fcs = fcs;
}

//Worker Methods
std::string ax25_data::build_data(){
    std::string data = this->build_preamble();
    data += this->flag;
    data += this->dest_addr;
    data += this->ssid0;
    data += this->src_addr;
    data += this->ssid1;
    data += this->control;
    data += this->pid;
    data += this->info;
    data += this->fcs;
    data += this->flag;
    data += this->build_postamble();
    return data;
}

std::string ax25_data::build_preamble(){
    std::string preamble = "";
    for(int i = 0; i < 8; i++){
        preamble += this->flag;
    }
    return preamble;
}

std::string ax25_data::build_postamble(){
    std::string postamble = "";
    for(int i = 0; i < 3; i++){
        postamble += this->flag;
    }
    return postamble;
}
//https://github.com/FrankBoesing/FastCRC use this instead
void ax25_data::crc16_ccitt(){
    std::string crc = "";
    FastCRC16 calculator;
    crc = calculator.ccitt(reinterpret_cast<const uint8_t*>(&this->info[0]), sizeof(this->info));
    this->fcs = crc;
}

std::string stuffed(std::string info){
    std::string stuffed_str = "";
    for (std::size_t i = 0; i < info.size(); ++i)
    {
        stuffed_str += std::bitset<8>(info.c_str()[i]).to_string();
    }
    while(stuffed_str.size() != 744){
        stuffed_str += '0';
    }
    return stuffed_str;
}

std::string nrzi_encode(std::string info){
    std::string nrzi = "";
    std:: string binary;
    char low = '0';
    char high = '1';
    char current = high;
    for (std::size_t i = 0; i < info.size(); ++i)
    {
        binary += std::bitset<8>(info.c_str()[i]).to_string();
    }
    for(int i = 0; i < binary.size(); i++){
        if(binary[i] == ' '){
            binary.erase(i, 1);
        }
    }

    for (int i = 0; i < binary.size(); i++){
        if (binary[i] == '0'){
            nrzi += current;
        }
        else if (binary[i] == '1'){
            if (current == low){
                current = high;
            }
            else if (current == high){
                current = low;
                nrzi += current;
            }
        }
    }
    return nrzi;
}
//todo move to util?
std::string convertString(std::string input, int arrIndex) {
    std:: stringstream ret;
    ret << input[arrIndex] << input[arrIndex + 1] << input[arrIndex + 2] << input[arrIndex + 3];
    return ret.str();
}
//https://www.amsat.org/amsat/articles/g3ruh/109.html
// The detected data, still randomised is then passed through an unscrambler, where the original data is recovered,
// and this goes off to the TNC. A scrambler is very simple, consisting of a 17 bit shift register and 3 Exor gates. See for example https://www.amsat.org/amsat/articles/g3ruh/109/fig03.gif
//
//The scrambling polynomial is 1 + X^12 + X^17. This means the currently transmitted bit is the EXOR of the current data bit, plus the bits that were transmitted 12 and 17 bits earlier.
// Likewise the unscrambling operation simply EXORs the bit received now with those sent 12 and 17 bits earlier. The unscrambler perforce requires 17 bits to synchronise
std::string scramble(std::string info){
    bool temp, reg[17];
    std::string scrambledStr;
    //populate the bit register with 0's
    for(int regSize = 0; regSize < 17; regSize++){
        reg[regSize] = 0;
    }
    //loop through the bit string
    for(int i = 0; i<sizeof(info);i++){
        //xor 17 bit with 12 bit of register then xor the previous xor with current string bit
        temp = (reg[17] ^ reg[12]) ^ (info[i] - 48);
        //shift contents of reg array up and populate the first pos with exor'd output
        for(int j = 1; j < sizeof(reg);j++){
            reg[j] = reg[j-1];
        }
        reg[0] = temp;
        //add the output to the scrambled string
        scrambledStr += (char) temp;
    }
    return scrambledStr;
}

std::string descramble(std::string info) {
    bool temp, reg[17];
    std::string descrambledStr;
    //populate the bit register with 0's
    for(int regSize = 0; regSize < 17; regSize++){
        reg[regSize] = 0;
    }
    //loop through the bit string
    for(int i = 0; i<sizeof(info);i++){
        //xor 17 bit with 12 bit of register then xor the previous xor with current string bit
        temp = (reg[17] ^ reg[12]) ^ (info[i] - 48);
        //shift contents of reg array up and populate the first pos with the current string bit
        for(int j = 1; j < sizeof(reg);j++){
            reg[j] = reg[j-1];
        }
        reg[0] = (bool)(info[i] - 48);
        //add the output to the scrambled string
        descrambledStr += (char) temp;
    }
    return descrambledStr;
}
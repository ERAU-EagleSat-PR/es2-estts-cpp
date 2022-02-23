//
// Created by bstri on 2/18/2022.
//

#include "ax25_data.h"


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

void ax25_data::crc16_ccitt(){
    std::string crc = "";
    int length = this->info.size();
    unsigned char i;
    unsigned int data;
    unsigned int fcs = 0xffff;
    do
    {
        for (i=0, data=(unsigned int)0xff & this->info[i];
             i < 8;
             i++, data >>= 1)
        {
            if ((fcs & 0x0001) ^ (data & 0x0001))
                fcs = (fcs >> 1) ^ POLY;
            else  fcs >>= 1;
        }
    } while (--length);

    fcs = ~fcs;
    data = fcs;
    fcs = (fcs << 8) | (data >> 8 & 0xff);
    crc = std::to_string(fcs);

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

std::string scramble(std::string info){
    std::string newHexArray;
    if (info.length() <= 77) {
        //int* binaryArray = static_cast<int *>(malloc(sizeof(int) * 4 * info.length()));
        int binaryArray[info.length()*4];
        try {
            for (int i = 0; i < info.length() * 4; i += 4) {
                switch (toupper(info[i / 4]))
                {
                    case '0':
                        binaryArray[i] = 0;
                        binaryArray[i + 1] = 0;
                        binaryArray[i + 2] = 0;
                        binaryArray[i + 3] = 0;
                        break;
                    case '1':
                        binaryArray[i] = 0;
                        binaryArray[i + 1] = 0;
                        binaryArray[i + 2] = 0;
                        binaryArray[i + 3] = 1;
                        break;
                    case '2':
                        binaryArray[i] = 0;
                        binaryArray[i + 1] = 0;
                        binaryArray[i + 2] = 1;
                        binaryArray[i + 3] = 0;
                        break;
                    case '3':
                        binaryArray[i] = 0;
                        binaryArray[i + 1] = 0;
                        binaryArray[i + 2] = 1;
                        binaryArray[i + 3] = 1;
                        break;
                    case '4':
                        binaryArray[i] = 0;
                        binaryArray[i + 1] = 1;
                        binaryArray[i + 2] = 0;
                        binaryArray[i + 3] = 0;
                        break;
                    case '5':
                        binaryArray[i] = 0;
                        binaryArray[i + 1] = 1;
                        binaryArray[i + 2] = 0;
                        binaryArray[i + 3] = 1;
                        break;
                    case '6':
                        binaryArray[i] = 0;
                        binaryArray[i + 1] = 1;
                        binaryArray[i + 2] = 1;
                        binaryArray[i + 3] = 0;
                        break;
                    case '7':
                        binaryArray[i] = 0;
                        binaryArray[i + 1] = 1;
                        binaryArray[i + 2] = 1;
                        binaryArray[i + 3] = 1;
                        break;
                    case '8':
                        binaryArray[i] = 1;
                        binaryArray[i + 1] = 0;
                        binaryArray[i + 2] = 0;
                        binaryArray[i + 3] = 0;
                        i += 4;
                        break;
                    case '9':
                        binaryArray[i] = 1;
                        binaryArray[i + 1] = 0;
                        binaryArray[i + 2] = 0;
                        binaryArray[i + 3] = 1;
                        break;
                    case 'A':
                        binaryArray[i] = 1;
                        binaryArray[i + 1] = 0;
                        binaryArray[i + 2] = 1;
                        binaryArray[i + 3] = 0;
                        break;
                    case 'B':
                        binaryArray[i] = 1;
                        binaryArray[i + 1] = 0;
                        binaryArray[i + 2] = 1;
                        binaryArray[i + 3] = 1;
                        break;
                    case 'C':
                        binaryArray[i] = 1;
                        binaryArray[i + 1] = 1;
                        binaryArray[i + 2] = 0;
                        binaryArray[i + 3] = 0;
                        break;
                    case 'D':
                        binaryArray[i] = 1;
                        binaryArray[i + 1] = 1;
                        binaryArray[i + 2] = 0;
                        binaryArray[i + 3] = 1;
                        break;
                    case 'E':
                        binaryArray[i] = 1;
                        binaryArray[i + 1] = 1;
                        binaryArray[i + 2] = 1;
                        binaryArray[i + 3] = 0;
                        break;
                    case 'F':
                        binaryArray[i] = 1;
                        binaryArray[i + 1] = 1;
                        binaryArray[i + 2] = 1;
                        binaryArray[i + 3] = 1;
                        break;
                    default:
                        throw("Error: Not a hex array");
                        break;
                }

                //Scramble by the polynomial 1 + x^12 + x^17

                if (i + 1 >= 12) {
                    if (i + 1 >= 17) {
                        binaryArray[i] = binaryArray[i] ^ (binaryArray[i - 12] ^ binaryArray[i - 17]);
                    }
                    else if (i + 1 < 17) {
                        binaryArray[i] = binaryArray[i] ^ (binaryArray[i - 12] ^ 0);
                    }
                }
                else if (i + 1 < 12) {
                    binaryArray[i] = binaryArray[i] ^ 0;
                }

                // End of the for
            }
        }
        catch (std::string errorMessage) {
            //i = sizeof(info) * 8;
            std::cout << errorMessage;
        }

        // Convert to a hex array

        info = "";


        // Converts to string
        for (int i = 0; i < *(&binaryArray + 1) - binaryArray; i++) {
            info = info.append(std::to_string(binaryArray[i]));
            // End of the for
        }
        //Converts to hex
        for (int i = 0; i < info.length(); i += 4) {
            if (i + 3 <= info.length()) {
                if (convertString(info, i) == "0000") {
                    newHexArray.append("0");
                }
                else if (convertString(info, i) == "0001") {
                    newHexArray.append("1");
                }
                else if (convertString(info, i) == "0010") {
                    newHexArray.append("2");
                }
                else if (convertString(info, i) == "0011") {
                    newHexArray.append("3");
                }
                else if (convertString(info, i) == "0100") {
                    newHexArray.append("4");
                }
                else if (convertString(info, i) == "0101") {
                    newHexArray.append("5");
                }
                else if (convertString(info, i) == "0110") {
                    newHexArray.append("6");
                }
                else if (convertString(info, i) == "0111") {
                    newHexArray.append("7");
                }
                else if (convertString(info, i) == "1000") {
                    newHexArray.append("8");
                }
                else if (convertString(info, i) == "1001") {
                    newHexArray.append("9");
                }
                else if (convertString(info, i) == "1010") {
                    newHexArray.append("A");
                }
                else if (convertString(info, i) == "1011") {
                    newHexArray.append("B");
                }
                else if (convertString(info, i) == "1100") {
                    newHexArray.append("C");
                }
                else if (convertString(info, i) == "1101") {
                    newHexArray.append("D");
                }
                else if (convertString(info, i) == "1110") {
                    newHexArray.append("E");
                }
                else if (convertString(info, i) == "1111") {
                    newHexArray.append("F");
                }
            }
            // End of the for
        }
        // End of the if
    }
    return newHexArray;
}
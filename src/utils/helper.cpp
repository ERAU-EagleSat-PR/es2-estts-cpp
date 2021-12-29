//
// Created by Hayden Roszell on 12/28/21.
//

#include <sstream>
#include "helper.h"

std::string ascii_to_hex(const std::string& in) {
    std::stringstream ret;
    for (auto i : in) {
        ret << std::hex << (unsigned)i;
    }
    return ret.str();
}

std::string hex_to_ascii(const std::string& hex) {
    // initialize the ASCII code string as empty.
    std::string ascii = "";
    for (size_t i = 0; i < hex.length(); i += 2)
    {
        // extract two characters from hex string
        std::string part = hex.substr(i, 2);

        // change it into base 16 and
        // typecast as the character
        char ch = stoul(part, nullptr, 16);

        // add this char to final ASCII string
        ascii += ch;
    }
    return ascii;
}
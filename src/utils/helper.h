//
// Created by Hayden Roszell on 12/28/21.
//

#ifndef ESTTS_HELPER_H
#define ESTTS_HELPER_H

#include <string>

/**
 * Convert ASCII string to HEX string. Be careful when using this function..
 * @param in ASCII string for conversion
 * @return HEX string characters
 */
std::string ascii_to_hex(const std::string& in);

/**
 * Convert HEX string to ASCII string. Borrowed from
 * https://www.geeksforgeeks.org/convert-hexadecimal-value-string-ascii-value-string/
 * @param hex HEX string characters for conversion
 * @return ASCII string
 */
std::string hex_to_ascii(const std::string& hex);

std::string generate_serial_number();

#endif //ESTTS_HELPER_H

//
// Created by Hayden Roszell on 12/28/21.
//

#ifndef ESTTS_HELPER_H
#define ESTTS_HELPER_H

#include <string>
#include "constants.h"

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

std::string find_removable_storage();

void print_write_trace_msg(unsigned char *message_uc, size_t bytes, const std::string& endpoint);

void print_read_trace_msg(unsigned char *message_uc, size_t bytes, const std::string& endpoint);

#endif //ESTTS_HELPER_H

//
// Created by Hayden Roszell on 12/28/21.
//

#include <algorithm>
#include <dirent.h>
#include <sstream>
#include <condition_variable>
#include <random>
#include <utility>
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

/**
 * @brief Creates 16-character serial number using C++ random library
 * @return 16-character serial number
 */
std::string generate_serial_number() {
    auto len = 16;
    static const char alphanum[] =
            "0123456789"
            "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
            "abcdefghijklmnopqrstuvwxyz";
    std::string tmp_s;
    tmp_s.reserve(len);
    std::random_device dev;
    std::mt19937 rng(dev());
    std::uniform_int_distribution<std::mt19937::result_type> dist6(1,sizeof(alphanum));

    for (int i = 0; i < len; ++i) {
        tmp_s += alphanum[dist6(rng) % (sizeof(alphanum) - 1)];
    }

    return tmp_s;
}

std::string find_removable_storage() {
    std::stringstream ssd_dir;
#ifdef __ESTTS_OS_LINUX__
    ssd_dir << "/media/";
    auto path_found = false;
    DIR * d = opendir(ssd_dir.str().c_str());
    if (d == nullptr) return "";
    struct dirent * dir;
    while ((dir = readdir(d)) != nullptr) {
        ssd_dir.clear();
        if (strcmp(dir->d_name, ".")!=0 && strcmp(dir->d_name, "..")!=0 && !path_found) {
            std::stringstream temp_path;
            temp_path << ssd_dir.str() << dir->d_name;
            DIR * d1 = opendir(temp_path.str().c_str());
            if (d1 != nullptr) {
                struct dirent * dir1;
                while ((dir1 = readdir(d1)) != nullptr) {
                    if (dir->d_type == DT_DIR && strcmp(dir1->d_name, estts::REMOVABLE_STORAGE_NAME)==0) {
                        ssd_dir << dir->d_name << "/" << dir1->d_name;
                        SPDLOG_INFO("Constructed path to removable storage device - {}", ssd_dir.str());
                        path_found = true;
                    }
                }
            }
            closedir(d1);
        }
    }
    closedir(d);
    if (path_found)
        return ssd_dir.str();
    else
        return "";
#else
    return "";
#endif
}

std::string get_write_trace_msg(unsigned char *message_uc, size_t bytes, const std::string& endpoint) {
    std::string message(reinterpret_cast<char*>(message_uc));
    std::replace( message.begin(), message.end(), '\r', ' ');
    message.append("\0");
    std::stringstream temp;
    temp << "Wrote '" << message << "' (size=" << bytes << " bytes) to " << endpoint;
    return temp.str();
}

std::string get_read_trace_msg(unsigned char *message_uc, size_t bytes, const std::string& endpoint) {
    std::string message(reinterpret_cast<char*>(message_uc));
    std::replace( message.begin(), message.end(), '\r', ' ');
    message.append("\0");
    std::stringstream temp;
    temp << "Read '" << message << "' (size=" << bytes << " bytes) from " << endpoint;
    return temp.str();
}

// stolen from endurosat
unsigned char HexToBin(unsigned char hb, unsigned char lb) {
    if (hb > '9')
        hb += 9;

    if (lb > '9')
        lb += 9;

    return (hb << 4) + (lb & 0x0f);
}

estts::Status validate_crc(std::string buf, std::string crc) {
    SPDLOG_TRACE("Found CRC: {}", crc);
    return estts::ES_OK;
}
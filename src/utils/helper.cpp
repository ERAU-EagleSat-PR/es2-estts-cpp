//
// Created by Hayden Roszell on 12/28/21.
//

#include <algorithm>
#include <iomanip>
#include <dirent.h>
#include <sstream>
#include <condition_variable>
#include <random>
#include <utility>
#include "helper.h"
#include "crc32.h"

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
    std::stringstream message_ss;
    std::string message(reinterpret_cast<char*>(message_uc));

    if (message.find("OK+ADCS ") != std::string::npos) {
        for (int i = 0; i < 8; i++)
            message_ss << message_uc[i];
        message_ss << "0x";
        for (int i = 8; i < bytes - 10; i++) {
            message_ss << std::setw(2) << std::setfill('0') << std::hex << (int)message_uc[i] << std::dec;
            printf("0x%02X ", message_uc[i]);
        }

        message_ss << " ";
        for (int i = bytes - 10; i < bytes - 1; i++)
            message_ss << message_uc[i];
    } else {
        std::replace(message.begin(), message.end(), '\r', ' ');
        message.append("\0");
        message_ss << message;
    }

    std::stringstream temp;
    temp << "Read '" << message_ss.str() << "' (size=" << bytes << " bytes) from " << endpoint;
    return temp.str();
}

uint32_t hex_string_to_int(std::string hex_val) {
    std::transform(hex_val.begin(), hex_val.end(),hex_val.begin(), ::toupper);
    if (hex_val.size() % 2 != 0) {
        hex_val.insert(0, "0");
    }

    std::string copy = hex_val;
    unsigned char *uc_val;
    uc_val = const_cast<unsigned char *>(reinterpret_cast<const unsigned char *>(copy.c_str()));

    for (int i = 0, j = 0; i < hex_val.size(); i += 2, j++)
        uc_val[j] = HexToBin(uc_val[i], uc_val[i + 1]);
    uc_val[hex_val.size() / 2] = '\0';

    uint32_t ret = 0;
    for (int i = 0, j = (int)hex_val.size() / 2 - 1; i < hex_val.size() / 2; i++, j--)
        ret += (uint32_t)(uc_val[i] << (j * 8));

    return ret;
}

// stolen from endurosat
unsigned char HexToBin(unsigned char hb, unsigned char lb) {
    if (hb > '9')
        hb += 9;

    if (lb > '9')
        lb += 9;

    unsigned char ret = (hb << 4) + (lb & 0x0f);
    return ret;
}

estts::Status validate_crc(const std::string& buf, const std::string& crc) {
    return validate_crc(buf, hex_string_to_int(crc));
}

estts::Status validate_crc(const std::string& buf, uint32_t crc) {
    unsigned int calc_crc = crc32b((unsigned char *)buf.c_str());

    if (crc == calc_crc) {
        SPDLOG_TRACE("CRC matches.");
    } else {
        SPDLOG_WARN("CRC doesn't match. Expected {} but got {}", crc, calc_crc);
        return estts::ES_UNSUCCESSFUL;
    }

    return estts::ES_OK;
}

estts::Status execute_shell(const std::string& cmd, std::string result) {
    SPDLOG_TRACE("Sending {}", cmd);
    char buffer[128];
    FILE* pipe = popen(cmd.c_str(), "r");
    if (!pipe) {
        SPDLOG_WARN("execute_shell: popen() failed!");
        return estts::ES_UNSUCCESSFUL;
    }
    try {
        while (fgets(buffer, sizeof buffer, pipe) != nullptr) {
            result += buffer;
        }
    } catch (...) {
        pclose(pipe);
        SPDLOG_WARN("execute_shell: popen() failed!");
        return estts::ES_UNSUCCESSFUL;
    }
    pclose(pipe);

    SPDLOG_TRACE("Got back: {}", result);
    return estts::ES_OK;
}
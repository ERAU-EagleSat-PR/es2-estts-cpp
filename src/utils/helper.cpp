//
// Created by Hayden Roszell on 12/28/21.
//

#include <fstream>
#include <algorithm>
#include <iomanip>
#include <dirent.h>
#include <sstream>
#include <random>
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

estts::Status execute_shell(const std::string& cmd) {
    char buffer[128];
    std::string result;
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
    return estts::ES_OK;
}

estts::Status publish_file_to_git(const std::string& base_git_dir, const std::string& repo, const std::string& filename, const std::string& data) {
    std::ofstream file;
    struct dirent * dir;
    std::stringstream command;

    SPDLOG_INFO("Publishing {} to {}", filename, base_git_dir);

    // Create base git directory
    command << "mkdir " << base_git_dir;
    execute_shell(command.str());

    // Determine if git repo is cloned -> .git will exist
    DIR * d = opendir(base_git_dir.c_str());
    bool git_repo_is_local = false;
    while ((dir = readdir(d)) != nullptr) {
        if (strcmp(dir->d_name, ".git") == 0) {
            SPDLOG_DEBUG("Git directory found in {}.", base_git_dir);
            git_repo_is_local = true;

            command.str("");
            command << "git -C " << base_git_dir << " pull";
            execute_shell(command.str());
            break;
        }
    }
    closedir(d);

    // If it doesn't exist, clone it
    if (!git_repo_is_local) {
        command.str("");
        command << "git clone " << repo << " " << base_git_dir;
        execute_shell(command.str());
    }

    // Then, create a directory with the date in Y-m-d format if it doesn't exist
    std::time_t rawtime;
    std::tm* timeinfo;

    std::time(&rawtime);
    timeinfo = std::localtime(&rawtime);

    std::stringstream datebuf;
    datebuf << timeinfo->tm_year + 1900 << "-" << timeinfo->tm_mon + 1 << "-" << timeinfo->tm_mday;

    std::stringstream timebuf;
    timebuf << timeinfo->tm_hour << "-" << timeinfo->tm_min << "-" << timeinfo->tm_sec;

    auto folder_exists_for_date = false;

    d = opendir(base_git_dir.c_str());
    while ((dir = readdir(d)) != nullptr) {
        if (strcmp(dir->d_name, datebuf.str().c_str()) == 0) {
            SPDLOG_DEBUG("Date directory already exists.");
            folder_exists_for_date = true;
            break;
        }
    }

    if (!folder_exists_for_date) {
        SPDLOG_DEBUG("Date directory does not exist. Creating {}", datebuf.str());
        command.str("");
        command << "mkdir " << base_git_dir << "/" << datebuf.str();
        execute_shell(command.str());
    }

    // Modify the filename by appending date before type
    std::stringstream mod_filename;

    auto filename_without_type_len = filename.find('.');
    if (filename_without_type_len != std::string::npos) {
        mod_filename << filename.substr(0, filename_without_type_len) << "_" << timebuf.str() << filename.substr(filename_without_type_len);
    } else {
        mod_filename << filename << "_" << timebuf.str();
    }

    SPDLOG_TRACE("Modifying filename from {} to {}", filename, mod_filename.str());

    SPDLOG_DEBUG("Opening {}/{}/{}", base_git_dir, datebuf.str(), mod_filename.str());
    std::stringstream path;
    path << base_git_dir << "/" << datebuf.str() << "/" << mod_filename.str();
    file.open(path.str(), std::ios::in | std::ios::out | std::ios::app);
    if (file.is_open()) {
        SPDLOG_TRACE("File is open");
        SPDLOG_TRACE("Writing {}", data);
        file << data;
    }
    file.close();

    SPDLOG_TRACE("Staging changes");
    command.str("");
    command << "git -C " << base_git_dir << " add .";
    execute_shell(command.str());

    std::stringstream commit_msg;
    commit_msg << "\"estts git automation: added " << mod_filename.str() << " to directory " << datebuf.str() << "\"";
    SPDLOG_DEBUG("Creating new commit with message {}", commit_msg.str());
    command.str("");
    command << "git -C " << base_git_dir << " commit -m " << commit_msg.str();
    execute_shell(command.str());

    SPDLOG_TRACE("Pushing to origin");
    command.str("");
    command << "git -C " << base_git_dir << " push";
    execute_shell(command.str());

    return estts::ES_OK;
}
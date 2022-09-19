//
// Created by Hayden Roszell on 9/14/22.
//

#include <sstream>
#include <iostream>
#include <fstream>
#include "obc_filesystem.h"
#include "helper.h"

estts::Status obc_filesystem::open_file(const std::string& filename) {
    std::string open_file_cmd = "ES+D11FI";

    SPDLOG_DEBUG("Opening {} on the OBC", filename);

    auto resp = execute_command(open_file_cmd + filename);
    estts::Status status;

    if (resp.empty() || resp.find("ERR") != std::string::npos) {
        SPDLOG_WARN("Failed to open {}: {}", filename, resp);
        return estts::ES_UNSUCCESSFUL;
    }
    else {
        SPDLOG_INFO("Opened {} on the OBC", filename);
        status = estts::ES_OK;
    }

    resp.erase(0, 3);
    resp.resize(8);

    union conv { char c[4]; size_t l; };

    union conv conv{};

    for (int i = 0, j = 3; i < 8; i += 2, j--)
        conv.c[j] = HexToBin(resp[i], resp[i + 1]);

    file_size = conv.l;

    SPDLOG_TRACE("String file size (hex): {} Unsigned long file size: {}", resp, file_size);

    return status;
}

estts::Status obc_filesystem::close_file() {
    std::string close_file_cmd = "ES+D11FC";

    SPDLOG_DEBUG("Closing currently opened file on OBC");

    auto resp = execute_command(close_file_cmd);
    estts::Status status;

    if (resp.empty() || resp.find("ERR") != std::string::npos) {
        SPDLOG_WARN("Failed to close currently opened file: {}", resp);
        status = estts::ES_UNSUCCESSFUL;
    }
    else {
        SPDLOG_INFO("File is closed.");
        status = estts::ES_OK;
    }

    return status;
}

std::string obc_filesystem::read_file(unsigned int position, unsigned int size) {
    std::string read_file_cmd = "ES+D11FR";

    char buf[20];
    sprintf(buf, "%s%08X%04X", read_file_cmd.c_str(), position, size);

    SPDLOG_TRACE("Reading {} bytes at position {} => {}", size, position, buf);

    return execute_command(buf);
}

estts::Status obc_filesystem::delete_file(const std::string& filename) {
    std::string delete_file_cmd = "ES+D11FD";

    SPDLOG_DEBUG("Deleting {} on the OBC", filename);

    auto resp = execute_command(delete_file_cmd + filename);
    estts::Status status;

    if (resp.empty() || resp.find("ERR") != std::string::npos) {
        SPDLOG_WARN("Failed to close currently opened file: {}", resp);
        status = estts::ES_UNSUCCESSFUL;
    }
    else {
        SPDLOG_INFO("File is closed.");
        status = estts::ES_OK;
    }

    SPDLOG_INFO("{} is deleted.", filename);

    return status;
}

std::string obc_filesystem::execute_command(const std::string& command) {
    estts::Status status;
    std::string resp;
    for (int i = 0; i < estts::ESTTS_MAX_RETRIES; i++) {

        status = session->transmit_func(command);
        if (status != estts::ES_OK) {
            SPDLOG_ERROR("Failed to transmit command.");
            return resp;
        }
        resp = session->receive_func();
        if (!resp.empty()) break;

        SPDLOG_DEBUG("Retrying");
    }

    return resp;
}

std::string obc_filesystem::download_file(const std::string& filename) {
    SPDLOG_DEBUG("Downloading {}", filename);

    if (open_file(filename) != estts::ES_OK)
        return "";

    std::string data;
    unsigned int round_iterations = file_size / estts::endurosat::MAX_ESTTC_PACKET_SIZE;
    unsigned int remainder_size = file_size - (round_iterations * estts::endurosat::MAX_ESTTC_PACKET_SIZE) - 1;
    unsigned int pos = 0;

    SPDLOG_TRACE("{} requires {} collections of {} bytes and 1 collection of {} bytes", filename, round_iterations, estts::endurosat::MAX_ESTTC_PACKET_SIZE, remainder_size);

    std::string read_buf;
    for (int i = 0; i < round_iterations; i++) {
        for (int j = 0; j < estts::endurosat::MAX_RETRIES; j++) {
            read_buf = read_file(pos, estts::endurosat::MAX_ESTTC_PACKET_SIZE);
            // TODO validate error code:
            /*
             * Answer: ERR+FIH<CR> - No opened file.
             * Answer: ERR+FIP=fpos-fsize,size-num<CR> - invalid position/size parameters
             * Answer: ERR+FIS(code)=fpos<CR> - invalid file position seek operation and error code
             * Answer: ERR+FIR(code)=num<CR> - fail to read from file error code
             * Answer: ERR+FRS(size)=num<CR> – invalid number of read bytes
             * Answer: ERR – Wrong CRC <CR>
             */

            // Grab CRC
            std::string crc;
            if (read_buf.size() >= 9)
                crc = read_buf.substr(read_buf.size() - 9, 8);

            if (validate_crc(read_buf, crc) == estts::ES_OK) {
                // Delete space, CRC, and carriage return
                if (read_buf.size() >= 10)
                    read_buf.resize(read_buf.size() - 10);

                pos += read_buf.size();
                data += read_buf;
                break;
            }
            SPDLOG_WARN("File CRC verification failed. Retrying {}/{}", j + 1, estts::endurosat::MAX_RETRIES);
        }
    }

    if (remainder_size > 0) {
        for (int i = 0; i < estts::endurosat::MAX_RETRIES; i++) {
            read_buf = read_file(pos, remainder_size);
            // TODO validate error code:

            // Grab CRC
            auto crc = read_buf.substr(read_buf.size() - 9, 8);

            if (validate_crc(read_buf, crc) == estts::ES_OK) {
                // Delete space, CRC, and carriage return
                if (read_buf.size() >= 10)
                    read_buf.resize(read_buf.size() - 10);

                pos += read_buf.size();
                data += read_buf;
                break;
            }
        }
    }

    close_file();

    std::ofstream file;
    SPDLOG_DEBUG("Opening /opt/estts/{}", filename);
    file.open("/opt/estts/" + filename, std::ios::in | std::ios::out | std::ios::app);
    if (file.is_open()) {
        SPDLOG_TRACE("File is open");
        file << data;
    } else
        SPDLOG_TRACE("File is not open");

    file.close();

    return data;
}

obc_filesystem::obc_filesystem(estts::session_config *config) {
    session = config;
    file_size = 0;
}

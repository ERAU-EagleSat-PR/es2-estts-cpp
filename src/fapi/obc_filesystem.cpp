//
// Created by Hayden Roszell on 9/14/22.
//

#include <sstream>
#include <iostream>
#include <fstream>
#include <dirent.h>
#include "obc_filesystem.h"
#include "helper.h"

estts::Status validate_obc_file_data(const std::string& data);
int get_read_bytes_from_frs(const std::string& msg);

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

    char buf[21];
    sprintf(buf, "ES+D11FR%08X%04X", position, size);

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
    } else {
        SPDLOG_INFO("File is closed.");
        status = estts::ES_OK;
    }

    SPDLOG_INFO("{} is deleted.", filename);

    return status;
}

std::string obc_filesystem::execute_command(const std::string& command) {
    estts::Status status;
    std::string resp;
    auto begin_ts = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < estts::ESTTS_MAX_RETRIES; i++) {
        status = session->transmit_func(command);
        if (status != estts::ES_OK) {
            SPDLOG_ERROR("Failed to transmit command.");
            return "";
        }
        resp = session->receive_func(); // TODO this model breaks down if a \0 is found
        SPDLOG_TRACE("resp.size(): {}", resp.size());
        // Verify CRC.
        if (!resp.empty() && resp.size() >= 8 + 1 && estts::ES_OK == validate_crc(resp.substr(0, resp.size() - 9), resp.substr(resp.size() - 8, 8))) {
            break;
        }

        SPDLOG_DEBUG("Retrying");
    }
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - begin_ts).count();
    SPDLOG_TRACE("Effective data rate: {0:.3f}kB/s", (resp.size() * 0.001) / (duration * 0.001));

    return resp;
}

std::string obc_filesystem::download_file(const std::string& filename) {
    SPDLOG_DEBUG("Downloading {}", filename);

    if (open_file(filename) != estts::ES_OK)
        return "";

    auto begin_ts = std::chrono::high_resolution_clock::now();
    std::string data;
    estts::Status status;
    unsigned int round_iterations = file_size / estts::endurosat::MAX_ESTTC_PACKET_SIZE;
    unsigned int remainder_size = (file_size - (round_iterations * estts::endurosat::MAX_ESTTC_PACKET_SIZE)); // TODO this number seems to change frequently. Something isn't right
    unsigned int pos = 0;

    SPDLOG_TRACE("{} requires {} collections of {} bytes and 1 collection of {} bytes", filename, round_iterations, estts::endurosat::MAX_ESTTC_PACKET_SIZE, remainder_size);

    std::string read_buf;
    for (int i = 0; i < round_iterations; i++) {
        read_buf = read_file(pos, estts::endurosat::MAX_ESTTC_PACKET_SIZE);
        status = validate_obc_file_data(read_buf);
        if (status != estts::ES_OK) {
            if (status != estts::ES_READ_BYTE_MISMATCH) {
                break;
            }

            auto correct_size = get_read_bytes_from_frs(read_buf);
            if (correct_size > 0) {
                read_buf = read_file(pos, correct_size);
            }
        }

        // Delete space, CRC, and delimiter
        if (read_buf.size() >= (1 + 8))
            read_buf.resize(read_buf.size() - (1 + 8));

        pos += read_buf.size();
        if (read_buf.substr(0, 3) != "ERR") {
            data += read_buf;
        }
    }

    if (remainder_size > 0) {
        read_buf = read_file(pos, remainder_size);
        status = validate_obc_file_data(read_buf);
        if (status == estts::ES_READ_BYTE_MISMATCH) {
            auto correct_size = get_read_bytes_from_frs(read_buf);
            if (correct_size > 0) {
                SPDLOG_TRACE("OBC reported correct size as {} bytes. Retrying", correct_size);
                read_buf = read_file(pos, correct_size);
                status = validate_obc_file_data(read_buf);
            }
        }
        if (estts::ES_OK == status) {
            // Delete space and CRC
            if (read_buf.size() >= (1 + 8))
                read_buf.resize(read_buf.size() - (1 + 8));

            pos += read_buf.size();
            data += read_buf;
        }
    }

    close_file();

    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - begin_ts).count();

    SPDLOG_INFO("{} bytes downloaded in {} seconds (average {}kB/s)", (int)data.size(), duration * 0.001, (data.size() * 0.001) / (duration * 0.001));

    publish_file_to_git(filename, data);
    return data;
}

int get_read_bytes_from_frs(const std::string& msg) {
    // Example: ERR+FRS(149)=18
    int actual_len = 0;

    auto start_pos = msg.find('=') + 1;
    auto end_pos = msg.find(' ');

    auto temp = msg.substr(start_pos, end_pos - start_pos);

    try {
        actual_len = stoi(temp);
    } catch (std::invalid_argument &e) {
        SPDLOG_WARN("{} is not a number: {}", actual_len, e.what());
    }

    return actual_len;
}

estts::Status validate_obc_file_data(const std::string& data) {
    if (data.empty()) {
        return estts::ES_MEMORY_ERROR;
    }
    /*
     * Answer: ERR+FIH<CR> - No opened file.
     * Answer: ERR+FIP=fpos-fsize,size-num<CR> - invalid position/size parameters
     * Answer: ERR+FIS(code)=fpos<CR> - invalid file position seek operation and error code
     * Answer: ERR+FIR(code)=num<CR> - fail to read from file error code
     * Answer: ERR+FRS(size)=num<CR> – invalid number of read bytes
     * Answer: ERR – Wrong CRC <CR>
     */

    if (data.find("ERR+FIH") != std::string::npos) {
        SPDLOG_WARN("No file is open on OBC");
        return estts::ES_NOTFOUND;
    } else if (data.find("ERR+FIH") != std::string::npos) {
        SPDLOG_WARN("Invalid position/size parameters when reading from file on OBC.");
        return estts::ES_BAD_OPTION;
    } else if (data.find("ERR+FIH") != std::string::npos) {
        SPDLOG_WARN("Invalid file position seek operation. Raw string: {}", data);
        return estts::ES_BAD_OPTION;
    } else if (data.find("ERR+FIR") != std::string::npos) {
        SPDLOG_WARN("Failed to read from file. Raw string: {}", data);
        return estts::ES_UNSUCCESSFUL;
    } else if (data.find("ERR+FRS") != std::string::npos) {
        SPDLOG_WARN("Invalid number of read bytes. Raw string: {}", data);
        return estts::ES_READ_BYTE_MISMATCH;
    }
    
    return estts::ES_OK;
}

obc_filesystem::obc_filesystem(estts::session_config *config) {
    session = config;
    file_size = 0;
}

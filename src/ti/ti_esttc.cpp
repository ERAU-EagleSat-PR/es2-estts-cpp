//
// Created by Hayden Roszell on 12/10/21.
//

#include <string>
#include <sstream>
#include "ti_esttc.h"

using std::stringstream;
using std::string;

/**
 * @brief ti_esttc default constructor that initializes ti_serial_handler
 * @param port Serial port (EX "/dev/tty.usbmodem")
 * @param baud Serial baud rate (EX 115200)
 * @return None
 */
ti_esttc::ti_esttc(const char *es_transmitter_port, int baud) : ti_serial_handler(es_transmitter_port, baud) {
    esttc_symbols = new estts::endurosat::esttc;
}

// 10.1 - STATUS CONTROL WORD ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

/**
 * @brief Enables transparent pipe mode on Endurosat UHF Transceiver module.
 * @return estts::Status indication success/failure of ESTTC command transmission
 */
estts::Status ti_esttc::enable_pipe() {
    return write_scw(esttc_symbols->enable_pipe);
};

/**
 *  @brief Sends a command with a Status Control Word (SCW) that changes the Endurosat UHF Transceiver's settings
 *  @param scw_command A command to change the SCW to a different configuration (e.g. enable_pipe)
 * @return estts::Status indication success/failure of ESTTC command transmission
 */
estts::Status ti_esttc::write_scw(uint16_t scw_command) {
    estts::Status return_status = estts::ES_UNSUCCESSFUL;
    string response;

    // TODO - Handle when the scw_command > stopper
    string command_body = esttc_symbols->scw_body[scw_command];

    return_status = build_esttc_command(
            esttc_symbols->METHOD_WRITE,
            esttc_symbols->CMD_SCW,
            response,
            command_body);

    if (return_status == estts::ES_SUCCESS && response.length() >= 7) {
        if (response.substr(3, 4) == "C3C3") {
            SPDLOG_INFO("Successfully commanded UHF to enter Bootloader mode from from Application mode");
        } else if (response.substr(3, 4) == "8787") {
            SPDLOG_INFO("Successfully commanded UHF to enter Application mode from from Bootloader mode");
        }
    }
    return return_status;
}

/**
 * @brief Send a command to read the current status of the Transceiver given its current Status Control Word (SCW)
 * @param RSSI Received Signal Strength Indicator
 * @param dvc_addr The device address in HEX format
 * @param rst_ctr  Reset Counter - Counts number of times the device has been reset
 * @param scw The current Status Control Word
 * @return estts::Status indication success/failure of ESTTC command transmission
 */
estts::Status ti_esttc::read_scw(std::string &RSSI, std::string &dvc_addr, std::string &rst_ctr, std::string &scw) {
    estts::Status return_status = estts::ES_UNSUCCESSFUL;
    string response;

    return_status = build_esttc_command(
            esttc_symbols->METHOD_READ,
            esttc_symbols->CMD_SCW,
            response);

    if (return_status == estts::ES_SUCCESS) {
        RSSI = response.substr(3, 2); // [RR]
        dvc_addr = response.substr(5, 2); // [AA]
        rst_ctr = response.substr(7, 2); // [BB]
        scw = response.substr(9, 4); // [WWWW]
    }

    return return_status;
}

// 10.2 - RADIO FREQUENCY CONFIGURATION ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

/**
 * @brief Writes (configures) the radio frequency of the Endurosat UHF Transceiver module.
 * @param frac Fractional part of the radio PLL synthesizer in HEX format (default = "")
 * @param div Integer divider of the radio PLL synthesizer in HEX format (default = "")
 * @return estts::Status indication success/failure of ESTTC command transmission
 */
estts::Status ti_esttc:: ti_esttc::write_radio_freq_config(const string& frac, const string& div) {
    estts::Status return_status = estts::ES_UNSUCCESSFUL;
    string response;
    string command_body;

    command_body += frac;
    command_body += div;

    return_status = build_esttc_command(
            esttc_symbols->METHOD_WRITE,
            esttc_symbols->CMD_RADIO_FREQ_CONFIG,
            response,
            command_body);

    return return_status;
}

/**
 * @brief Get the radio frequency and the last RSSI (Received signal strength indication) of the UHF Transceiver module
 * @param RSSI  Received Signal Strength Indicator
 * @param frac Fractional part of the radio PLL synthesizer in HEX format
 * @param div Integer divider of the radio PLL synthesizer in HEX format
 * @return estts::Status indication success/failure of ESTTC command transmission
 */
estts::Status ti_esttc::read_radio_freq(string &RSSI, string &frac, string &div) {
    estts::Status return_status = estts::ES_UNSUCCESSFUL;
    string response;

    return_status = build_esttc_command(
            esttc_symbols->METHOD_READ,
            esttc_symbols->CMD_RADIO_FREQ_CONFIG,
            response);

    if (return_status == estts::ES_SUCCESS) {
        RSSI = response.substr(3, 2);
        frac = response.substr(5, 6);
        div = response.substr(11, 2);
    }

    return return_status;
}

// 10.3 - READ UPTIME ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

estts::Status ti_esttc::read_uptime(std::string &RSSI, std::string &uptime) {
    estts::Status return_status = estts::ES_UNSUCCESSFUL;
    string response;

    return_status = build_esttc_command(
            esttc_symbols->METHOD_READ,
            esttc_symbols->CMD_READ_UPTIME,
            response);

    if (return_status == estts::ES_SUCCESS) {
        RSSI = response.substr(3, 2);
        uptime = response.substr(5, 8);
    }

    return return_status;
}

// TODO - Code the Read uptime command builder

/**
 * @brief Gets internal IMU temperature of EnduroSat UHF Transceiver module
 * @return Double representing internal IMU temperature in Celsius
 */
double ti_esttc::read_temp() {
//#ifdef __TI_DEV_MODE__
//    return 26;
//#else
//    auto command = build_esttc_command(esttc_symbols->METHOD_READ, esttc_symbols->COMMAND_TEMP_SENSOR, nullptr);
//    if (this->write_serial_s(command) < 0) {
//        spdlog::error("Failed to transmit command");
//        return -1;
//    }
//    auto ret = this->read_serial_s();
//    ret.replace(ret.find("OK +"), 4, "");
//    SPDLOG_INFO("Transceiver internal temperature is {}°C", ret);
//    // TODO make this actually return the temperature
//    return 0;
//#endif

    return 0;
}

/**
 * @brief Builds ESTTC command in format specified by EnduroSat ESTTC Specification
 * @param method Command method ('r' or 'w')
 * @param command_code EnduroSat ESTTC command code
 * @param response String to be filled with ESTTC command answer
 * @param body Request body for command if writing
 * @return estts::Status indication success/failure of ESTTC command transmission
 */
estts::Status ti_esttc::build_esttc_command(const char method, const char *command_code, string &response, const string& body) {
    estts::Status return_status = estts::ES_UNINITIALIZED;
    estts::Status serial_status = estts::ES_UNSUCCESSFUL;
    stringstream command;

    /* Build ESTTC command*/
    command << esttc_symbols->HEADER;
    command << method;
    command << esttc_symbols->ADDRESS;
    command << command_code;

    if (method == esttc_symbols->METHOD_WRITE) {
        command << body;
    }
    //command << ' ' << calculate_crc32(command.str());
    command << esttc_symbols->END;

    /*Attempt to transmit ESTTC command */
    for (uint8_t i = 0; i < esttc_symbols->NUM_OF_RETRIES; ++i ) {
        serial_status = this->write_serial_s(command.str());

        if (serial_status == estts::ES_SUCCESS) {
            response = this->read_serial_s();

            if (response.length() >= 2 && response.substr(0, 2) == "OK") {
                return_status = estts::ES_SUCCESS;
                break;
            } else if (response.length() >= 3 && response.substr(0, 3) == "ERR") {
                return_status = estts::ES_UNSUCCESSFUL;
            } else if (response.length() >= 7 && response.substr(0, 7) == "ERR+VAL") {
                return_status = estts::ES_BAD_OPTION;
            }
        } else {
            return_status = serial_status;
        }
    }

    /* Check for transmission failure and send message indicating status */
    if (return_status == estts::ES_SUCCESS) {
        SPDLOG_INFO("Successfully transmitted ESTTC command: {}", command.str());
    } else if (return_status == estts::ES_BAD_OPTION) {
        spdlog::error("Invalid  ESTTC command input data");
    } else {
        spdlog::error("Failed to transmit  ESTTC command");
    }

    return return_status;
}

/**
 * @brief Calculates CRC32 of command string specified by EnduroSat
 * @param string String input to calculate CRC32
 * @return Calculated CRC32 of inputted string
 */
std::string ti_esttc::calculate_crc32(string string) {
    // TODO Taylor's task for sprint #3
    return "";
}

ti_esttc::~ti_esttc() {
    delete esttc_symbols;
}

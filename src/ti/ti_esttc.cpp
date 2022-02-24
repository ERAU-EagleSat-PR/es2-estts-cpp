//
// Created by Hayden Roszell on 12/10/21.
//

#include <string>
#include <sstream>
#include <algorithm>
#include <vector>
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
 * @brief Enables the default value of every SCW field on Endurosat UHF Transceiver module
 * @return estts::Status indication success/failure of ESTTC command transmission
 */
estts::Status ti_esttc::default_mode() {
    return write_scw(esttc_symbols->default_mode);
}

/**
 * @brief Enables transparent pipe mode on Endurosat UHF Transceiver module
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

    // Temp Solution
    if (scw_command > stopper)
        return estts::ES_BAD_OPTION;

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
estts::Status ti_esttc::read_scw(string &RSSI, string &dvc_addr, string &rst_ctr, string &scw) {
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
 * @param frac Fractional part of the radio PLL synthesizer in HEX format LEAST SIGNIFICANT BYTE FIRST
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

/**
 * @brief Get the uptime of the UHF Transceiver
 * @param RSSI Received Signal Strength Indicator
 * @param uptime Uptime value in seconds in HEX format
 * @return estts::Status indication success/failure of ESTTC command transmission
 */
estts::Status ti_esttc::read_uptime(string &RSSI, string &uptime) {
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

// 10.4 - READ NUMBER OF TRANSMITTED PACKETS ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

/**
 * @brief Get the number of transmitted packets from the UHF Transceiver
 * @param RSSI Received Signal Strength Indicator
 * @param pckt_num The number of transmitted packets in HEX format
 * @return estts::Status indication success/failure of ESTTC command transmission
 */
estts::Status ti_esttc::read_trans_pckts(string &RSSI, string &pckt_num) {
    estts::Status return_status = estts::ES_UNSUCCESSFUL;
    string response;

    return_status = build_esttc_command(
            esttc_symbols->METHOD_READ,
            esttc_symbols->CMD_READ_TRANS_PCKTS,
            response);

    if (return_status == estts::ES_SUCCESS) {
        RSSI = response.substr(3, 2);
        pckt_num = response.substr(5, 8);
    }

    return return_status;
}

// 10.6 - READ NUMBER OF TRANSMITTED PACKETS WITH A CRC ERROR ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

/**
 * @brief Get the number of transmitted packets that contained CRC errors from the UHF Transceiver
 * @param RSSI Received Signal Strength Indicator
 * @param pckt_num The number of transmitted packets with CRC errors in HEX format
 * @return estts::Status indication success/failure of ESTTC command transmission
 */
estts::Status ti_esttc::read_trans_pckts_crc(string &RSSI, string &pckt_num) {
    estts::Status return_status = estts::ES_UNSUCCESSFUL;
    string response;

    return_status = build_esttc_command(
            esttc_symbols->METHOD_READ,
            esttc_symbols->CMD_READ_TRANS_PCKTS_CRC,
            response);

    if (return_status == estts::ES_SUCCESS) {
        RSSI = response.substr(3, 2);
        pckt_num = response.substr(5, 8);
    }

    return return_status;
}

// 10.8 - BEACON MESSAGE TRANSMISSION PERIOD CONFIGURATION ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

/**
 * @brief Configure the beacon message transmission period of the UHF Transceiver
 * @param period The desired period (in seconds in HEX format) to set
 * @return estts::Status indication success/failure of ESTTC command transmission
 */
estts::Status ti_esttc::write_bcn_trans_period(const string &period) {
    estts::Status return_status = estts::ES_UNSUCCESSFUL;
    string response;
    string command_body;

    command_body += "0000";
    command_body += period;

    return_status = build_esttc_command(
            esttc_symbols->METHOD_WRITE,
            esttc_symbols->CMD_BCN_MSG_TRANS_CONFIG,
            response,
            command_body);

    return return_status;
}

/**
 * Read the beacon message transmission period from the UHF Transceiver
 * @param RSSI Received Signal Strength Indicator
 * @param period The current beacon message transmission period
 * @return estts::Status indication success/failure of ESTTC command transmission
 */
estts::Status ti_esttc::read_bcn_trans_period(string &RSSI, string &period) {
    estts::Status return_status = estts::ES_UNSUCCESSFUL;
    string response;

    return_status = build_esttc_command(
            esttc_symbols->METHOD_READ,
            esttc_symbols->CMD_BCN_MSG_TRANS_CONFIG,
            response);

    if (return_status == estts::ES_SUCCESS) {
        RSSI = response.substr(3, 2);
        period = response.substr(9, 4);
    }

    return return_status;
}

// 10.10 - RESTORE DEFAULT VALUES ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

/**
 * @brief restore default values to UHF Transceiver for
 *  Destination/Source/Morse code call sign,
 *  Audio beacon period and message,
 * Text beadon period,
 * and Pipe timeout period
 * @return estts::Status indication success/failure of ESTTC command transmission
 */
estts::Status ti_esttc::write_res_default_vals() {
        estts::Status return_status = estts::ES_UNSUCCESSFUL;
        string response;
        string command_body;

        command_body += "";

        return_status = build_esttc_command(
                esttc_symbols->METHOD_WRITE,
                esttc_symbols->CMD_RESTORE,
                response,
                command_body);

        return return_status;
}

// 10.31 - I2C PULL-UP RESISTORS CONFIGURATION READ/WRITE ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

/**
 * @brief Configure the I2C Pull-Up resistor configuration of the UHF Transceiver
 * @param resistor_config The desired I2C pull-up resistor configuration in HEX
 * @return estts::Status indication success/failure of ESTTC command transmission
 */
estts::Status ti_esttc::write_i2c_resist_config(const string &resistor_config) {
    estts::Status return_status = estts::ES_UNSUCCESSFUL;
    string response;
    string command_body;

    command_body += resistor_config;

    return_status = build_esttc_command(
            esttc_symbols->METHOD_WRITE,
            esttc_symbols->CMD_I2C_RESIST_CONFIG,
            response,
            command_body);

    return return_status;
}

/**
 * @brief Read the current I2C Pull-Up resistor configuration of the UHF Transceiver
 * @param selected_resistor The currently selected I2C pull-up resistor configuration in HEX
 * @return estts::Status indication success/failure of ESTTC command transmission
 */
estts::Status ti_esttc::read_i2c_resist_config(string &selected_resistor) {
    estts::Status return_status = estts::ES_UNSUCCESSFUL;
    string response;

    return_status = build_esttc_command(
            esttc_symbols->METHOD_READ,
            esttc_symbols->CMD_I2C_RESIST_CONFIG,
            response);

    if (return_status == estts::ES_SUCCESS) {
        selected_resistor = response.substr(3, 2);
    }

    return return_status;
}

// 10.12 - ENABLING/DISABLING RADIO PACKET CRC16 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

/**
 * @brief Enable or disable the UHF Transceiver's radio packet CRC16
 * @param mode A bit ('0', or '1') that determines if radio packet CRC16 is enabled or disabled
 * @return estts::Status indication success/failure of ESTTC command transmission
 */
estts::Status ti_esttc::write_radio_crc16(const string &mode) {
    estts::Status return_status = estts::ES_UNSUCCESSFUL;
    string response;
    string command_body;

    command_body += mode;

    return_status = build_esttc_command(
            esttc_symbols->METHOD_WRITE,
            esttc_symbols->CMD_ENABLE_DISABLE_RADIO_CRC,
            response,
            command_body);

    return return_status;
}

/**
 * @brief Read whether radio packet CRC16 is enabled or disabled for the UHF Transceiver
 * @param mode A bit ('0', or '1') that determines if radio packet CRC16 is enabled or disabled
 * @return estts::Status indication success/failure of ESTTC command transmission
 */
estts::Status ti_esttc::read_radio_crc16(string &mode) {
    estts::Status return_status = estts::ES_UNSUCCESSFUL;
    string response;

    return_status = build_esttc_command(
            esttc_symbols->METHOD_READ,
            esttc_symbols->CMD_ENABLE_DISABLE_RADIO_CRC,
            response);

    if (return_status == estts::ES_SUCCESS) {
        mode = response.substr(4, 1);
    }

    return return_status;
}

// 10.14 - ENABLING/DISABLING AUTOMATIC AX.25 DECODING ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

/**
 * @brief Write the configuration bit that determines whether the UHF Transceiver's Auto AX.25 Decode is Enabled/Disabled
 * @param config_bit The bit ('1' or '0') that determines if automatic ax.25 decoding is enabled
 * @return estts::Status indication success/failure of ESTTC command transmission
 */
estts::Status ti_esttc::write_config_ax25_decode(const string &config_bit) {
    estts::Status return_status = estts::ES_UNSUCCESSFUL;
    string response;
    string command_body;

    command_body += '0';
    command_body += config_bit;

    return_status = build_esttc_command(
            esttc_symbols->METHOD_WRITE,
            esttc_symbols->CMD_AUTO_AX25_DECODE,
            response,
            command_body);

    return return_status;
}

/**
 * @brief Get the configuration bit that determines whether the UHF Transceiver's Auto AX.25 Decode is Enabled/Disabled
 * @param config_bit The bit ('1' or '0') that determines if automatic ax.25 decoding is enabled
 * @return estts::Status indication success/failure of ESTTC command transmission
 */
estts::Status ti_esttc::read_config_ax25_decode(string &config_bit) {
    estts::Status return_status = estts::ES_UNSUCCESSFUL;
    string response;

    return_status = build_esttc_command(
            esttc_symbols->METHOD_READ,
            esttc_symbols->CMD_AUTO_AX25_DECODE,
            response);

    if (return_status == estts::ES_SUCCESS) {
        config_bit = response.substr(4, 1);
    }

    return return_status;
}

// 10.16 - UHF ANTENNA RELEASE CONFIGURATION ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

/**
 * @brief Configure the UHF Transceiver antenna release configuration
 * @param ant_config UHF Antenna Release Configuration (4 chars)
 * @return estts::Status indication success/failure of ESTTC command transmission
 */
estts::Status ti_esttc::write_ant_release_config(const string &ant_config) {
    estts::Status return_status = estts::ES_UNSUCCESSFUL;
    string response;
    string command_body;

    command_body += ant_config;

    return_status = build_esttc_command(
            esttc_symbols->METHOD_WRITE,
            esttc_symbols->CMD_ANT_RELEASE_CONFIG,
            response,
            command_body);

    return return_status;
}

/**
 * @brief Read the UHF Transceiver's current antenna release configuration
 * @param ant_config The current UHF Antenna Release Configuration in HEX
 * @return estts::Status indication success/failure of ESTTC command transmission
 */
estts::Status ti_esttc::read_ant_release_config(string &ant_config) {
    estts::Status return_status = estts::ES_UNSUCCESSFUL;
    string response;

    return_status = build_esttc_command(
            esttc_symbols->METHOD_READ,
            esttc_symbols->CMD_ANT_RELEASE_CONFIG,
            response);

    if (return_status == estts::ES_SUCCESS) {
        ant_config = response.substr(3, 4);
    }

    return return_status;
}

// 10.18 - LOW POWER MODE ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

/**
 * @brief Toggle the low power mode of the UHF Transceiver
 * @return estts::Status indication success/failure of ESTTC command transmission
 */
estts::Status ti_esttc::write_low_pwr_mode() {
    estts::Status return_status = estts::ES_UNSUCCESSFUL;
    string response;
    string command_body;

    command_body = "";

    return_status = build_esttc_command(
            esttc_symbols->METHOD_WRITE,
            esttc_symbols->CMD_LOW_PWR_MODE,
            response,
            command_body);

    return return_status;
}

/**
 * @brief Read the current power mode of the UHF Transceiver
 * @param power_mode The current power mode of the UHF Transceiver ("00" [Normal Mode] or "01" [Low Power Mode])
 * @return estts::Status indication success/failure of ESTTC command transmission
 */
estts::Status ti_esttc::read_low_pwr_mode(string &power_mode) {
    estts::Status return_status = estts::ES_UNSUCCESSFUL;
    string response;

    return_status = build_esttc_command(
            esttc_symbols->METHOD_READ,
            esttc_symbols->CMD_LOW_PWR_MODE,
            response);

    if (return_status == estts::ES_SUCCESS) {
        power_mode = response.substr(3, 2);
    }

    return return_status;
}

// 10.20 - SOURCE CALL SIGN ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

/**
 * @brief Write a new source call sign for the UHF Transceiver
 * @param call_sign  The new source call sign you want to replace the old one with (ex. "XX0UHF")
 * @return estts::Status indication success/failure of ESTTC command transmission
 */
estts::Status ti_esttc::write_src_call_sign(const string &call_sign) {
    estts::Status return_status = estts::ES_UNSUCCESSFUL;
    string response;
    string command_body;

    command_body += call_sign;

    return_status = build_esttc_command(
            esttc_symbols->METHOD_WRITE,
            esttc_symbols->CMD_SRC_CALL_SIGN,
            response,
            command_body);

    return return_status;
}

/**
 * @brief Read the current source call sign of the UHF Transceiver
 * @param call_sign  The current source call sign (ex. "XX0UHF")
 * @return estts::Status indication success/failure of ESTTC command transmission
 */
estts::Status ti_esttc::read_src_call_sign(string &call_sign) {
    estts::Status return_status = estts::ES_UNSUCCESSFUL;
    string response;

    return_status = build_esttc_command(
            esttc_symbols->METHOD_READ,
            esttc_symbols->CMD_SRC_CALL_SIGN,
            response);

    if (return_status == estts::ES_SUCCESS) {
        call_sign = response.substr(3, 6);
    }

    return return_status;
}

// 10.24 - READ DEVICE PAYLOAD SIZE ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

/**
 * @brief Read the current size of the device payload of the UHF Transceiver
 * @param payload_size The size of the payload in HEX
 * @return estts::Status indication success/failure of ESTTC command transmission
 */
estts::Status ti_esttc::read_dvc_payload_size(string &payload_size) {
    estts::Status return_status = estts::ES_UNSUCCESSFUL;
    string response;

    return_status = build_esttc_command(
            esttc_symbols->METHOD_READ,
            esttc_symbols->CMD_READ_DVC_PAYLOAD,
            response);

    if (return_status == estts::ES_SUCCESS) {
        payload_size = response.substr(3, 6);
    }

    return return_status;
}

// 10.26 - DEVICE ADDRESS CONFIGURATION ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

/**
 * @brief Write a new device address for the UHF Transceiver
 * @param new_addr The new device address for the UHF Transceiver in HEX ("22" or "23")
 * @return estts::Status indication success/failure of ESTTC command transmission
 */
estts::Status ti_esttc::write_dvc_addr_config(const string &new_addr) {
    estts::Status return_status = estts::ES_UNSUCCESSFUL;
    string response;
    string command_body;

    command_body += new_addr;

    return_status = build_esttc_command(
            esttc_symbols->METHOD_WRITE,
            esttc_symbols->CMD_DVC_ADDR_CONFIG,
            response,
            command_body);

    return return_status;
}

// 10.28 - RADIO TRANSCEIVER PROPERTY CONFIGURATION ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

/**
 * @brief Write a new configuration for the Radio Transceiver Property of the UHF Transceiver
 * @param prop_group The property group in HEX
 * @param bytes The number of bytes in HEX
 * @param offset  The start offset of the property content in HEX
 * @param data  The variable size data in HEX
 * @return estts::Status indication success/failure of ESTTC command transmission
 */
estts::Status ti_esttc::write_radio_trans_prop_config(const string &prop_group, const string &bytes, const string &offset, const string &data) {
    estts::Status return_status = estts::ES_UNSUCCESSFUL;
    string response;
    string command_body;
    uint16_t data_size;
    stringstream hexToDec;

    // Convert hex string to decimal integer
    hexToDec << std::hex << bytes;
    hexToDec >> data_size;

    // If the data exceeds the number of bytes given in the "bytes" parameter, return
    if ((data_size * 2) < data.length()) {
        return estts::ES_BAD_OPTION;
    }

    command_body += prop_group;
    command_body += bytes;
    command_body += offset;
    command_body += data;

    return_status = build_esttc_command(
            esttc_symbols->METHOD_WRITE,
            esttc_symbols->CMD_RADIO_TRANS_PROP_CONFIG,
            response,
            command_body);

    return return_status;
}

/**
 * @brief Read the current configuration of the Radio Transceiver Property of the UHF Transceiver
 * @param prop_group The current property group in HEX
 * @param bytes The current number of bytes in HEX
 * @param offset  The current start offset of the property content in HEX
 * @param data  The current variable size data in HEX
 * @return estts::Status indication success/failure of ESTTC command transmission
 */
estts::Status ti_esttc::read_radio_trans_prop_config(const string &prop_group, const string &bytes, const string &offset, string &data) {
    estts::Status return_status = estts::ES_UNSUCCESSFUL;
    string response;
    string command_body;
    uint16_t data_size;
    stringstream hexToDec;

    command_body += prop_group;
    command_body += bytes;
    command_body += offset;

    return_status = build_esttc_command(
            esttc_symbols->METHOD_READ,
            esttc_symbols->CMD_RADIO_TRANS_PROP_CONFIG,
            response,
            command_body);

    // Convert hex string to decimal integer
    hexToDec << std::hex << bytes;
    hexToDec >> data_size;

    if (return_status == estts::ES_SUCCESS) {
        // Check if the number of given size of data is in range of response
        if (response.length() >= (3 + (data_size * 2))) {
            data = response.substr(3, (data_size * 2));
        } else {
            return_status == estts::ES_BAD_OPTION;
        }
    }

    return return_status;
}

// 10.30 - FIRMWARE UPDATE ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

/**
 * @brief Fully update the firmware of the UHF Transceiver
 * @param all_lines The variable length data which comprises of all lines of the .SCRM file separated by new line characters '\\n'
 * @return estts::Status indication success/failure of ESTTC command transmission
 */
estts::Status ti_esttc::update_firmware(const string &all_lines) {
    estts::Status update_status = estts::ES_UNSUCCESSFUL;

    // TODO - Write the first two steps of Firmware Update
    //Step 1
    // Read Secure Mode

    //Step 2
    // seedkey = Down_seedkey XOR DOWNLINK_XOR
    // Up_seedkey = seedkey XOR UPLINK_XOR
    // Write secure mode (Up_seedkey)
    // if ok, continue, else stop

    // prelim setup
    const size_t num_of_lines = std::count(all_lines.begin(), all_lines.end(), '\n');

    if (num_of_lines <= 0) {
        return estts::ES_BAD_OPTION;
    }

    const std::vector<string> lines = split_lines(all_lines, num_of_lines);

    // Step 3
    update_status = update_firmware_sequence(lines.front());

    if (update_status == estts::ES_MEMORY_ERROR) {
        return estts::ES_MEMORY_ERROR;
    }
    // Step 4
    if (num_of_lines > 2) {
        for (const string& line : lines) {
            if (line != lines.front() && line != lines.back()) {
                update_status = update_firmware_sequence(line);
            }
        }
    }

    //Step 5
    update_status = update_firmware_sequence(lines.back());

    if (update_status == estts::ES_SUCCESS) {
        SPDLOG_INFO("Firmware Update Successful");
    } else {
        spdlog::error("Firmware Update Unsuccessful");
    }

    // If "ERR" or "ERR+FA" was received throughout the process of the update, DANGER, contact endurosat support
    return update_status;
}

/**
 * @brief Update the firmware of the UHF Transceiver
 * @param one_line The variable length data which comprises of a single line of the .SCRM file
 * @return estts::Status indication success/failure of ESTTC command transmission
 */
estts::Status ti_esttc::update_firmware_sequence(const string &one_line) {
    // TODO - Double check this whole function. Too tired to check now

    estts::Status return_status = estts::ES_UNSUCCESSFUL;
    string response;
    string command_body;

    command_body = one_line;

    return_status = build_esttc_command(
            esttc_symbols->METHOD_FIRMWARE_UPDATE,
            esttc_symbols->CMD_FRMWR_UPDATE,
            response,
            command_body);

    if (return_status == estts::ES_UNSUCCESSFUL && response.substr(0, 6) == "ERR+FB") {
        return_status = estts::ES_MEMORY_ERROR;
    }

    return return_status;
}

/**
 * @brief Designed to be used by update_firmware(), Accept a whole file seperated line by line with '\\n' and split into a vector of lines
 * @param all_lines The entire .SCRM file with each line separated by a new line character
 * @param num_of_lines  The number of expected lines in the .SCRM file
 * @return A vector containing each line as an element
 */
std::vector<string> ti_esttc::split_lines(const string &all_lines, size_t num_of_lines) {
    string::size_type prev_line = 0;
    string::size_type new_line = -1;
    std::vector<string> lines(num_of_lines);

    for (int i = 0; i < num_of_lines; ++i) {
        prev_line += new_line + 1;
        new_line = all_lines.substr(prev_line).find('\n');
        lines.push_back(all_lines.substr(prev_line, new_line));
    }

    return lines;
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

    if (method == esttc_symbols->METHOD_WRITE ||
        esttc_symbols->METHOD_FIRMWARE_UPDATE ||
        command_code == esttc_symbols->CMD_RADIO_TRANS_PROP_CONFIG) {
        command << body;
    }
    //command << ' ' << calculate_crc32(command.str());
    command << esttc_symbols->END;

    /*Attempt to transmit ESTTC command */
    serial_status = this->write_serial_s(command.str());

    if (serial_status == estts::ES_SUCCESS) {
        response = this->read_serial_s();

        // TODO - Make sure this covers ALL cases
        if (response.length() >= 7 && response.substr(0, 7) == "ERR+VAL") {
            return_status = estts::ES_BAD_OPTION;
            spdlog::error("Invalid  ESTTC command input data: {}", command.str());
        } else if (response.length() >= 3 && response.substr(0, 3) == "ERR") {
            return_status = estts::ES_UNSUCCESSFUL;
            spdlog::error("Failed to transmit  ESTTC command: {}", command.str());
        } else if (response.length() >= 2 && response.substr(0, 2) == "OK") {
            return_status = estts::ES_SUCCESS;
            SPDLOG_INFO("Successfully transmitted ESTTC command: {}", command.str());
        } else {
            return_status = estts::ES_UNINITIALIZED;
            SPDLOG_INFO("Unhandled ESTTC command response ...\ncommand: {}\nanswer: {}", command.str(), response);
        }
    } else {
        return_status = serial_status;
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
    return string;
}

ti_esttc::~ti_esttc() {
    delete esttc_symbols;
}

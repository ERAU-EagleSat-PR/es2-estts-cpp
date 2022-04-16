//
// Created by Hayden Roszell on 12/10/21.
//

#ifndef ESTTS_TI_ESTTC_H
#define ESTTS_TI_ESTTC_H

#include "ti_serial_handler.h"
#include "constants.h"

class ti_esttc : virtual public ti_serial_handler {
private:
    estts::endurosat::esttc *esttc_symbols;

    estts::Status build_esttc_command(char method, const char *command_code,  string &response, const string& body = "");

    static std::vector<std::string> split_lines(const std::string &all_lines, size_t num_of_lines);
    static int hexToInt(const string &s);
    static string intToHex(int n);

protected:
public:

    // 10.1 - STATUS CONTROL WORD (SCW)
    estts::Status default_mode();
    estts::Status enable_pipe();
    estts::Status write_scw(uint16_t scw_command);
    estts::Status read_scw(std::string &RSSI, std::string &dvc_addr, std::string &rst_ctr, std::string &scw);
    // 10.2 - RADIO FREQUENCY CONFIGURATION
    estts::Status write_radio_freq_config(const std::string& frac = "76620F41", const std::string& div = "41");
    estts::Status read_radio_freq(std::string &RSSI, std::string &frac, std::string &div);
    // 10.3 - READ UPTIME
    estts::Status read_uptime(std::string &RSSI, std::string &uptime);
    // 10.4 - READ NUMBER OF TRANSMITTED PACKETS
    estts::Status read_trans_pckts(std::string &RSSI, std::string &pckt_num);
    // 10.5 - READ NUMBER OF RX'D PACKETS
    estts::Status read_num_rx_packets(std::string &RSSI, std::string &num_rxd);
    // 10.6 - READ NUMBER OF TRANSMITTED PACKETS WITH A CRC ERROR
    estts::Status read_trans_pckts_crc(std::string &RSSI, std::string &pckt_num);
    // 10.7 - TRANSPARENT PIPE MODE TMOUT CFG ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    estts::Status read_pipe_mode_timeout(std::string &RSSI, std::string &time_period);
    estts::Status write_pipe_mode_timeout(const std::string& time_period);
    // 10.8 - BEACON MESSAGE TRANSMISSION PERIOD CONFIGURATION
    estts::Status write_bcn_trans_period(const std::string &period = "003C");
    estts::Status read_bcn_trans_period(std::string &RSSI, std::string &period);
    // 10.9 - AUDIO BEACON PERIOD BETWEEN TRANS
    estts::Status read_beacon_time_period(std::string &RSSI, std::string &time_period);
    estts::Status write_beacon_time_period(const std::string& time_period);
    // 10.10 - RESTORE DEFAULT VALUES
    estts::Status write_res_default_vals();
    // 10.11 - READ INTERNAL TEMP ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    estts::Status read_beacon_time_period(std::string &internal_temp);
    // 10.31 - I2C PULL-UP RESISTORS CONFIGURATION READ/WRITE
    estts::Status write_i2c_resist_config(const std::string &resistor_config);
    estts::Status read_i2c_resist_config(std::string &selected_resistor);
    // 10.12 - ENABLING/DISABLING RADIO PACKET CRC16
    estts::Status write_radio_crc16(const std::string &mode);
    estts::Status read_radio_crc16(std::string &mode);
    // 10.13 - FORCE BEACON COMMAND
    estts::Status force_beacon();
    estts::Status read_config_ax25_decode(std::string &config_bit);
    // 10.15 - AUDIO BEACON PERIOD BETWEEN TRANS
    estts::Status read_write_generic_I2C(std::string& send_mode, std::string &slave_address,
                                                   std::string& bytes_to_read, std::string& bytes_to_write,
                                                   std::string& data);
    // 10.16 - UHF ANTENNA RELEASE CONFIGURATION
    estts::Status write_ant_release_config(const std::string &ant_config);
    estts::Status read_ant_release_config(std::string &ant_config);
    // 10.14 - ENABLING/DISABLING AUTOMATIC AX.25 DECODING
    estts::Status write_config_ax25_decode(const std::string &config_bit);
    // 10.17 - UHF ANTENNA READ/WRITE ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    estts::Status read_uhf_antenna(std::string &antenna_status);
    estts::Status write_uhf_antenna(const std::string& antenna_command);
    // 10.18 - LOW POWER MODE
    estts::Status write_low_pwr_mode();
    estts::Status read_low_pwr_mode(std::string &mode);
    // 10.19 - DESTINATION CALL SIGN
    estts::Status read_dest_call_sign(std::string &call_sign);
    estts::Status write_dest_call_sign(const std::string& call_sign);
    // 10.20 - SOURCE CALL SIGN
    estts::Status write_src_call_sign(const std::string &call_sign = "XX0UHF");
    estts::Status read_src_call_sign(std::string &call_sign);
    // 10.23 - READ SOFTWARE VERSION
    estts::Status read_software_version(std::string &version);
    // 10.24 - READ DEVICE PAYLOAD SIZE
    estts::Status read_dvc_payload_size(std::string &payload_size);
    // 10.25 - BEACON MESSAGE CONTENT CONFIG ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    estts::Status read_beacon_content(std::string &content);
    estts::Status write_beacon_content(const std::string content);
    // 10.26 - DEVICE ADDRESS CONFIGURATION
    estts::Status write_dvc_addr_config(const std::string &new_addr = "22");
    // 10.27 - FRAM MEMORY READ/WRITE
    estts::Status read_fram(std::string &data, std::string &mem_add);
    estts::Status write_fram(const string &mem_add, std::string &data);
    // 10.28 - RADIO TRANSCEIVER PROPERTY CONFIGURATION
    estts::Status write_radio_trans_prop_config(const std::string &prop_group, const std::string &bytes,
                                                const std::string &offset, const std::string &data);
    estts::Status read_radio_trans_prop_config(const std::string &prop_group, const std::string &bytes, const std::string &offset, std::string &data);

    // 10.29 - FRAM MEMORY READ/WRITE
    estts::Status read_secure_mode(std::string &key);
    estts::Status write_secure_mode(const std::string &key);
    // 10.30 - FIRMWARE UPDATE
    estts::Status update_firmware(const std::string &all_lines);
    estts::Status update_firmware_sequence(const std::string &one_line);

    static std::string calculate_crc32(std::string string);

    ti_esttc(const char *es_transmitter_port, int baud);

    ~ti_esttc();
};


#endif //ESTTS_TI_ESTTC_H

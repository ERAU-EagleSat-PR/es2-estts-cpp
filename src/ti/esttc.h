//
// Created by Hayden Roszell on 12/10/21.
//

#ifndef ESTTS_ESTTC_H
#define ESTTS_ESTTC_H

#include "serial_handler.h"
#include "constants.h"

class esttc : virtual public serial_handler {
private:
    estts::endurosat::esttc *esttc_symbols;

    estts::Status build_esttc_command(char method, const char *command_code,  std::string &response, const std::string& body = "");

    static std::vector<std::string> split_lines(const std::string &all_lines, size_t num_of_lines);
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
    // 10.6 - READ NUMBER OF TRANSMITTED PACKETS WITH A CRC ERROR
    estts::Status read_trans_pckts_crc(std::string &RSSI, std::string &pckt_num);
    // 10.8 - BEACON MESSAGE TRANSMISSION PERIOD CONFIGURATION
    estts::Status write_bcn_trans_period(const std::string &period = "003C");
    estts::Status read_bcn_trans_period(std::string &RSSI, std::string &period);
    // 10.10 - RESTORE DEFAULT VALUES
    estts::Status write_res_default_vals();
    // 10.31 - I2C PULL-UP RESISTORS CONFIGURATION READ/WRITE
    estts::Status write_i2c_resist_config(const std::string &resistor_config);
    estts::Status read_i2c_resist_config(std::string &selected_resistor);
    // 10.12 - ENABLING/DISABLING RADIO PACKET CRC16
    estts::Status write_radio_crc16(const std::string &mode);
    estts::Status read_radio_crc16(std::string &mode);
    // 10.14 - ENABLING/DISABLING AUTOMATIC AX.25 DECODING
    estts::Status write_config_ax25_decode(const std::string &config_bit); // Consider replacing the param with a char type
    estts::Status read_config_ax25_decode(std::string &config_bit);
    // 10.16 - UHF ANTENNA RELEASE CONFIGURATION
    estts::Status write_ant_release_config(const std::string &ant_config);
    estts::Status read_ant_release_config(std::string &ant_config);
    // 10.18 - LOW POWER MODE
    estts::Status write_low_pwr_mode();
    estts::Status read_low_pwr_mode(std::string &mode);
    // 10.20 - SOURCE CALL SIGN
    estts::Status write_src_call_sign(const std::string &call_sign = "XX0UHF");
    estts::Status read_src_call_sign(std::string &call_sign);
    // 10.24 - READ DEVICE PAYLOAD SIZE
    estts::Status read_dvc_payload_size(std::string &payload_size);
    // 10.26 - DEVICE ADDRESS CONFIGURATION
    estts::Status write_dvc_addr_config(const std::string &new_addr = "22");
    // 10.28 - RADIO TRANSCEIVER PROPERTY CONFIGURATION
    estts::Status write_radio_trans_prop_config(const std::string &prop_group, const std::string &bytes,
                                                                                                    const std::string &offset, const std::string &data);
    estts::Status read_radio_trans_prop_config(const std::string &prop_group, const std::string &bytes,
                                                                                                    const std::string &offset, std::string &data);
    // 10.30 - FIRMWARE UPDATE
    estts::Status update_firmware(const std::string &all_lines);
    estts::Status update_firmware_sequence(const std::string &one_line);

    static std::string calculate_crc32(std::string string);

    // Misc commands
    estts::Status enable_satellite_bcn();

    esttc();

    ~esttc();
};


#endif //ESTTS_ESTTC_H

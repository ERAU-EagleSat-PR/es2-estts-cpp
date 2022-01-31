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

protected:
    estts::Status enable_pipe();
    estts::Status write_scw(uint16_t scw_command);
    estts::Status read_swc(std::string &RSSI, std::string &dvc_addr, std::string &rst_ctr, std::string &scw);

    estts::Status config_radio_freq(const std::string& frac, const std::string& div);
    estts::Status get_radio_freq(std::string &RSSI, std::string &frac, std::string &div);

    static std::string calculate_crc32(std::string string);

public:
    ti_esttc(const char *es_transmitter_port, int baud);

    ~ti_esttc();
};


#endif //ESTTS_TI_ESTTC_H

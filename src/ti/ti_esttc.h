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

    std::string build_esttc_command(char method, const char *command_code, const char *body);

protected:

    estts::Status enable_pipe();
    char* get_radio_freq();
    estts::Status config_radio_freq(const char* fractional, const char* divider);

    double get_temp();

    static std::string calculate_crc32(std::string string);

public:
    ti_esttc(const char *es_transmitter_port, int baud);

    ~ti_esttc();
};


#endif //ESTTS_TI_ESTTC_H

//
// Created by Hayden Roszell on 12/10/21.
//

#ifndef ESTTS_TI_SERIAL_HANDLER_H
#define ESTTS_TI_SERIAL_HANDLER_H

#include <sstream>

#include "constants.h"

class ti_serial_handler {
private:
    // Private variables
    int serial_port;

    // Private functions
    estts::Status open_port();

    estts::Status initialize_serial_port() const;

    const char *port;
    int baud;
protected:
    // Check here first, maybe what you're waiting for is already received..
    // Note - cleared every time read is called
    std::stringstream cache;

    ti_serial_handler(const char *port, int baud);

    ~ti_serial_handler();

    ssize_t write_serial_uc(unsigned char *data, int size) const;

    unsigned char *read_serial_uc();

    estts::Status write_serial_s(const std::string &data) const;

    std::string read_serial_s();

    void clear_serial_fifo();

    estts::Status search_read_buf(const std::string& query);

};


#endif //ESTTS_TI_SERIAL_HANDLER_H

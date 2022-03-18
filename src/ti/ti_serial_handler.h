//
// Created by Hayden Roszell on 12/10/21.
//

#ifndef ESTTS_TI_SERIAL_HANDLER_H
#define ESTTS_TI_SERIAL_HANDLER_H

#include <sstream>
#include <boost/asio.hpp>
#include "constants.h"

#include "posix_serial.h"

class ti_serial_handler {
private:
    boost::asio::io_service io;
    boost::asio::serial_port serial;
    const char * port;
protected:
    // Check here first, maybe what you're waiting for is already received..
    // Note - cleared every time read is called
    std::stringstream cache;

    ti_serial_handler();

    ti_serial_handler(const char *port, int baud);

    estts::Status initialize_serial_port(int baud);

    ~ti_serial_handler();

    ssize_t write_serial_uc(unsigned char *data, int size);

    unsigned char *read_serial_uc();

    estts::Status write_serial_s(const std::string &data);

    std::string read_serial_s();

    void clear_serial_fifo();

    int check_serial_bytes_avail();
};

#endif //ESTTS_TI_SERIAL_HANDLER_H

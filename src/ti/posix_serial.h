//
// Created by Hayden Roszell on 3/17/22.
//

#ifndef ESTTS_TI_POSIX_SERIAL_H
#define ESTTS_TI_POSIX_SERIAL_H

#include <sstream>
#include "constants.h"


class posix_serial {
    // Private variables
    int serial_port;

    // Private functions
    estts::Status open_port();

    estts::Status initialize_serial_port();

    const char *port;
    int baud;
protected:
    // Check here first, maybe what you're waiting for is already received..
    // Note - cleared every time read is called
    std::stringstream cache;

    posix_serial(const char *port, int baud);

    ~posix_serial();

    virtual ssize_t write_serial_uc(unsigned char *data, int size) const;

    virtual unsigned char *read_serial_uc();

    virtual estts::Status write_serial_s(const std::string &data) const;

    virtual std::string read_serial_s();

    virtual void clear_serial_fifo();

    virtual int check_serial_bytes_avail() const;
};


#endif //ESTTS_TI_POSIX_SERIAL_H

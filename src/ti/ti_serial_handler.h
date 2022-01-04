//
// Created by Hayden Roszell on 12/10/21.
//

#ifndef ESTTS_TI_SERIAL_HANDLER_H
#define ESTTS_TI_SERIAL_HANDLER_H

#include <string>
#include <fcntl.h> // Contains file controls like O_RDWR
#include <cerrno> // Error integer and strerror() function
#include <unistd.h> // write(), read(), close()
#include <termios.h> // Contains POSIX terminal control definitions

#include "constants.h"

using std::string;

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
    ti_serial_handler(const char *port, int baud);

    ~ti_serial_handler();

    ssize_t write_serial_uc(unsigned char *data, int size) const;

    unsigned char *read_serial_uc() const;

    estts::Status write_serial_s(const std::string &data) const;

    std::string read_serial_s() const;

    unsigned char * read_serial_us_async();
};


#endif //ESTTS_TI_SERIAL_HANDLER_H

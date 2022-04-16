//
// Created by Hayden Roszell on 12/10/21.
//

#ifndef ESTTS_SERIAL_HANDLER_H
#define ESTTS_SERIAL_HANDLER_H

#include <sstream>
#include <boost/asio.hpp>
#include "constants.h"

#include "posix_serial.h"

class serial_handler {
private:
    boost::asio::io_service io;
    boost::asio::serial_port serial;
    std::string port;
    int baud, restarts;
    unsigned char * sync_buf;

    estts::Status find_serial_port();
protected:
    // Check here first, maybe what you're waiting for is already received..
    // Note - cleared every time read is called
    std::stringstream cache;

    char async_buf[MAX_SERIAL_READ];

    serial_handler();

    estts::Status initialize_serial_port();

    ~serial_handler();

    size_t write_serial_uc(unsigned char *data, int size);

    unsigned char *read_serial_uc();





    unsigned char *read_serial_uc(int bytes);

    std::string read_serial_s(int bytes);

    std::function<void(boost::system::error_code, size_t)> get_generic_async_read_lambda(const std::function<estts::Status(char *, size_t)>& estts_callback);

    void clear_serial_fifo();

    void clear_serial_fifo(const std::function<estts::Status(std::string)>& cb);

    int check_serial_bytes_avail();

public:
    void read_serial_async(const std::function<estts::Status(char *, size_t)>& cb);

    estts::Status write_serial_s(const std::string &data);

    std::string read_serial_s();
};

#endif //ESTTS_SERIAL_HANDLER_H

//
// Created by Hayden Roszell on 12/10/21.
//

#ifndef ESTTS_SERIAL_HANDLER_H
#define ESTTS_SERIAL_HANDLER_H

#include <sstream>
#include <boost/asio.hpp>
#include <future>
#include "constants.h"

#include "posix_serial.h"

class serial_handler {
private:
    int failures;
    boost::asio::io_service io;
    boost::asio::serial_port serial;
    std::string port;
    int baud, restarts;
    unsigned char * sync_buf;
    unsigned int delimiter_timeout_ms;

    estts::Status handle_failure();

    estts::Status find_serial_port();

    size_t internal_read_serial_uc(int bytes);
protected:
    // Check here first, maybe what you're waiting for is already received..
    // Note - cleared every time read is called
    std::stringstream cache;

    unsigned int last_transmission_byte_count;

    char async_buf[MAX_SERIAL_READ];

    std::chrono::time_point<std::chrono::high_resolution_clock> tx_trace_timestamp;

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

    void reset_sync_buf() { delete sync_buf; sync_buf = new unsigned char[MAX_SERIAL_READ]; }

public:
    void set_delimiter_timeout_ms(unsigned int timeout) { delimiter_timeout_ms = timeout; }

    std::string read_to_delimeter(const unsigned char * delimiter, int size);

    std::string read_to_delimeter(unsigned char delimiter);

    estts::Status write_serial_s(const std::string &data);

    std::string read_serial_s();

    serial_handler();
};

#endif //ESTTS_SERIAL_HANDLER_H

//
// Created by Hayden Roszell on 10/28/21.
//

#include <iostream>
#include "frame_constructor.h"
#include "ti_esttc.h"
#include "constants.h"
#include "spdlog/spdlog.h"

int main() {

    // Important note: Always use the 'cu.' port. TTY is used for reading from UNIX sockets.
    const char * port = "/dev/cu.usbserial-A10JVB3P";
    int baud = 115200;
    /*
    auto serial = new ti_serial_handler(port, baud);
    unsigned char msg[] = { 'H', 'e', 'l', 'l', 'o', '\r', '\0' };
    unsigned char* bruh = serial->read_serial_uc();
    std::cout << bruh << std::endl;
    std::string bruh2 = serial->read_serial_s();
    std::cout << bruh2 << std::endl;
    // serial->write_serial_uc(msg);
    delete serial;
    */
    try {
        auto esttc_handler = new ti_esttc(port, baud);
        esttc_handler->get_temp();
        delete esttc_handler;
    } catch (const std::exception& e) {
        spdlog::error("Failed to open serial port");
    }
}
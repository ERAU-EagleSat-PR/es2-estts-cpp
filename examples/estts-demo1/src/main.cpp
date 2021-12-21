//
// Created by Hayden Roszell on 10/28/21.
//

#include <iostream>
#include "ti_serial_handler.h"
#include "ti_esttc.h"

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

    auto esttc_handler = new ti_esttc(port, baud);
    if (!esttc_handler->successful_init)
        return -1;
    esttc_handler->get_temp();
    delete esttc_handler;
}
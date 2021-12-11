//
// Created by Hayden Roszell on 10/28/21.
//

#include <iostream>
#include "ti_serial_handler.h"

int main() {

    const char * port = "/dev/tty.usbmodem2101";
    int baud = 115200;

    auto serial = new ti_serial_handler(port, baud);
    unsigned char msg[] = { 'H', 'e', 'l', 'l', 'o', '\r', '\0' };

    unsigned char* bruh = serial->read_serial_uc();
    std::cout << bruh << std::endl;

    std::string bruh2 = serial->read_serial_s();
    std::cout << bruh2 << std::endl;
    // serial->write_serial_uc(msg);

    delete serial;

}
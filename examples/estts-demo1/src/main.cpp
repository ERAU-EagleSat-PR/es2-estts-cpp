//
// Created by Hayden Roszell on 10/28/21.
//

#include <iostream>
#include "system_api.h"

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

    auto command_data = new sapi::SAPICommandStructure;
    command_data->address = estts::ES_ACS;
    command_data->method = estts::ES_READ;
    command_data->command = estts::ACS_GET_POS;
    command_data->body = nullptr;

    auto command1 = new System_API(command_data);
    cout << command1->buildFrameHeader();
    delete command1;
}
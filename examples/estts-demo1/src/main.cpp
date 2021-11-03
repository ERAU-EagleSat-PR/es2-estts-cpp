//
// Created by Hayden Roszell on 10/28/21.
//

#include <iostream>
#include "system_api.h"

int main() {

    auto command_data = new sapi::SAPICommandStructure;
    command_data->address = estts::ES_ACS;
    command_data->method = estts::ES_READ;
    command_data->command = estts::ACS_GET_POS;
    command_data->body = nullptr;

    auto command1 = new System_API(command_data);
    cout << command1->buildFrameHeader();
    delete command1;
}
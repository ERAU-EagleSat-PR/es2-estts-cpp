//
// Created by Hayden Roszell on 10/28/21.
//

#include "system_api.h"

estts::Status System_API::buildAX25Frame() {

    return estts::ES_OK;
}

std::string System_API::buildFrameHeader() {
    std::stringstream header;
    header << destAddr << ssid0 << srcAddr << ssid1 << control << pid;
    return header.str();
}

/* Copyright © EagleSat II - Embry Riddle Aeronautical University - All rights reserved - 2022 */
//
// Created by Hayden Roszell on 1/11/22.
//

#include "constants.h"
#include "cosmos_handler.h"

int main() {
    // todo main should fork a child and exec the other processes and immediately terminate
    spdlog::set_level(spdlog::level::trace); // This setting is missed in the wiki
    auto cosmos = new cosmos_handler();
    if (estts::ES_OK != cosmos->cosmos_init())
        return -1;
    cosmos->initialize_cosmos_daemon();
    return 0;
}
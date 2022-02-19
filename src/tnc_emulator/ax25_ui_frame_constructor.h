//
// Created by Cody on 12/17/2021.
//

#ifndef ES2_ESTTS_CPP_FRAME_CONSTRUCTOR_H
#define ES2_ESTTS_CPP_FRAME_CONSTRUCTOR_H

#include <string>
#include <sstream>
#include <unordered_map>
#include "info_field.h"
#include "bin_converter.h"

class ax25_ui_frame_constructor : virtual public info_field {
private:

    /* Getters for Header Field */
    static std::string getFlag();

    static std::string getDestAddr();

    static std::string getSSID0();

    static std::string getSrcAddr();

    static std::string getSSID1();

    static std::string getControl();

    static std::string getPID();

    std::string getInfoField();

    static std::string perform_nrzi_encoding(std::string raw);

    static std::string calculate_crc16_ccit(std::string value);

    static std::string scramble_frame(std::string raw);

protected:

    std::string encode_ax25_frame(std::string raw);

public:
    /* Constructors */
    explicit ax25_ui_frame_constructor(estts::command_object *command) : info_field(command) {}

    /* Encoded AX.25 Frame Constructor */
    std::string construct_ax25();
};

#endif
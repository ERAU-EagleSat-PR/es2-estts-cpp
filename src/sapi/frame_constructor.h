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

class frame_constructor : virtual public info_field {
private:
    /* Binary converter */
    bin_converter binConverter;

    /* Constant sizes */
    static const unsigned int INFO_FIELD_SIZE = 616;
    static const unsigned char FCS_SIZE = 16;

    /* Getters for Header Field */
    static std::string getFlag();

    static std::string getDestAddr();

    static std::string getSSID0();

    static std::string getSrcAddr();

    static std::string getSSID1();

    static std::string getControl();

    static std::string getPID();

    std::string getInfoField();

    std::string getFCSBits();

public:
    /* Constructors */
    explicit frame_constructor(estts::command_object *command) : info_field(command) {}

    /* Encoded AX.25 Frame Constructor */
    std::string construct_ax25();
};

#endif
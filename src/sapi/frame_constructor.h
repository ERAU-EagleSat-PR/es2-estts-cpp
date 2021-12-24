//
<<<<<<< HEAD
// Created by Cody on 12/17/2021.
=======
// Created by Cody Park on 12/17/2021.
>>>>>>> main
//

#ifndef ES2_ESTTS_CPP_FRAME_CONSTRUCTOR_H
#define ES2_ESTTS_CPP_FRAME_CONSTRUCTOR_H

#include <string>
<<<<<<< HEAD
#include <sstream>
#include <unordered_map>
#include "info_field.h"
#include "bin_converter.h"

class frame_constructor : virtual public info_field {
=======
#include <unordered_map>
#include "info_field.h"
#include "bin_converter.h"
#include "constants.h"

class frame_constructor {
>>>>>>> main
private:
    /* Binary converter */
    bin_converter binConverter;

<<<<<<< HEAD
=======
    /* Frame Header Constants */
    estts::ax25::header headerConsts;

>>>>>>> main
    /* Constant sizes */
    static const unsigned int INFO_FIELD_SIZE = 616;
    static const unsigned char FCS_SIZE = 16;

<<<<<<< HEAD
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
=======
    /* Information Field */
    info_field* informationField;

    /* Frame Check Sequence */
    std::string FCS;

    /* Getters for Header Field */
    std::string getFlag();
    std::string getDestAddr();
    std::string getSSID0();
    std::string getSrcAddr();
    std::string getSSID1();
    std::string getControl();
    std::string getPID();
    std::string getInfoField();
    std::string getFCSBits();
public:
    /* Constructors */
    frame_constructor() : frame_constructor(nullptr) {}
    frame_constructor(info_field* informationField) : informationField(informationField) {}

    /* Setters */
    inline void setInfoField(info_field* informationField) { this->informationField = informationField; }
    inline void setFCS(const std::string FCS) { this->FCS = FCS; }

    /* Encoded AX.25 Frame Getter */
    std::string encode();
};

#endif //ES2_ESTTS_CPP_FRAME_CONSTRUCTOR_H
>>>>>>> main

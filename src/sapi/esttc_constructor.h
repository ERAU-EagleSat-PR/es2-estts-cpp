//
// Created by bstri on 2/7/2022.
//

#ifndef ESTTS_ESTTC_CONSTRUCTOR_H
#define ESTTS_ESTTC_CONSTRUCTOR_H

#include <string>
#include <sstream>
#include <unordered_map>
#include "info_field.h"
#include "bin_converter.h"
#include "esttc.h"

class esttc_constructor {

public:
    std::string construct_esttc(esttc command);
};


#endif //ESTTS_ESTTC_CONSTRUCTOR_H

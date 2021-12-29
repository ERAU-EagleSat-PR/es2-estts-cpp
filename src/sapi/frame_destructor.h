//
// Created by Hayden Roszell on 12/28/21.
//

#ifndef ESTTS_FRAME_DESTRUCTOR_H
#define ESTTS_FRAME_DESTRUCTOR_H

#include <string>
#include "constants.h"

class frame_destructor {
private:
    std::vector<estts::telemetry_object *> telemetry;

    estts::Status decode_frame(const std::string& frame);

    static estts::Status validate_header(const std::string& frame);

    static estts::Status check_source(const std::string& source);

    static estts::Status check_destination(const std::string& dest);

    static estts::Status check_ssid0(const std::string& ssid);

    static estts::Status check_ssid1(const std::string& ssid);

    static estts::Status check_control(const std::string& control);

    static estts::Status check_pid(const std::string& pid);

    static estts::Status check_crc(const std::string& frame, const std::string &crc);

public:
    explicit frame_destructor(std::string raw);

    std::vector<estts::telemetry_object *> get_telemetry() {return telemetry;}
};


#endif //ESTTS_FRAME_DESTRUCTOR_H

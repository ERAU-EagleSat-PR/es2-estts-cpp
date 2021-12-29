//
// Created by Hayden Roszell on 12/28/21.
//

#include <iostream>
#include <sstream>
#include "spdlog/spdlog.h"
#include "frame_destructor.h"
#include "helper.h"

frame_destructor::frame_destructor(std::string raw) {
    // First, find the start of the frame using the flag.
    // Assuming that the frame transmission had no loss, this method should work.
    unsigned long frame_start_flag;
    for (int i = 0; i < raw.length(); i++) {
        if (raw[i] == estts::ax25::AX25_FLAG[0] && raw[i + 1] == estts::ax25::AX25_FLAG[1]) {
            i++;
        } else {
            frame_start_flag = i;
            break;
        }
    }

    // Now, we need to find the end of the frame. We can do this using the flag, and safely assume that if the
    // trailing flag is missing, the frame is likely corrupted.
    unsigned long frame_end;
    for (int i = 0; i < raw.length(); i++) {
        if (raw[i] == estts::ax25::AX25_FLAG[0] && raw[i + 1] == estts::ax25::AX25_FLAG[1]) {
            if (estts::ax25::AX25_FLAG[1] != raw[i - 1] && i > 0){
                frame_end = i;
                break;
            }
        }
    }

    // Try decoding the frame.
    auto frame = raw.substr(frame_start_flag, frame_end - frame_start_flag);
    decode_frame(frame);

    // Then, find the start of the frame using the src/dest callsigns. This is a failsafe
    // method in case the transmission had loss
    std::stringstream callsign_and_ssid;
    // Remember that we're expecting the satellite to be the source and ground to be the destination
    callsign_and_ssid << ascii_to_hex(estts::ax25::AX25_SOURCE_ADDRESS) << estts::ax25::AX25_SSID0 <<
                      ascii_to_hex(estts::ax25::AX25_DESTINATION_ADDRESS) << estts::ax25::AX25_SSID1 << std::endl;
    auto temp = raw.find(callsign_and_ssid.str());
    auto frame_start_callsign = (temp != std::string::npos) ? -1 : temp;
}

estts::Status frame_destructor::decode_frame(const std::string& frame) {
    spdlog::info("{}", frame);

    if (estts::ES_OK != validate_header(frame))
        return estts::ES_UNSUCCESSFUL;

    int crc_size = 0; // Usually 16 bits todo when CRC is checked and tests are added including a CRC, subtract it here

    auto info_field = frame.substr(32);

    auto resp = new estts::telemetry_object;

    // TODO check the response code

    // TODO put info field stuff into a new info field constructor

    resp->address = info_field.substr(0, 2).c_str();

    int timestamp;
    std::istringstream (info_field.substr(2, 4)) >> timestamp;
    resp->timeStamp = timestamp;

    int sequence;
    std::istringstream (info_field.substr(6, 2)) >> sequence;
    resp->sequence = sequence;

    resp->commandID = info_field.substr(8, 2).c_str();

    int response_code;
    std::istringstream (info_field.substr(10, 2)) >> response_code;
    resp->response_code = response_code;

    // resp->data = info_field.substr(12).c_str(); todo figure this shit out

    telemetry.push_back(resp);

    return estts::ES_OK;
}

estts::Status frame_destructor::check_source(const std::string &source) {
    if (hex_to_ascii(source) == estts::ax25::AX25_DESTINATION_ADDRESS)
        return estts::ES_OK;
    return estts::ES_UNSUCCESSFUL;
}

estts::Status frame_destructor::check_destination(const std::string &dest) {
    if (hex_to_ascii(dest) == estts::ax25::AX25_SOURCE_ADDRESS)
        return estts::ES_OK;
    return estts::ES_UNSUCCESSFUL;
}

estts::Status frame_destructor::check_ssid0(const std::string &ssid) {
    if (ssid == estts::ax25::AX25_SSID0)
        return estts::ES_OK;
    return estts::ES_UNSUCCESSFUL;
}

estts::Status frame_destructor::check_ssid1(const std::string &ssid) {
    if (ssid == estts::ax25::AX25_SSID1)
        return estts::ES_OK;
    return estts::ES_UNSUCCESSFUL;
}

estts::Status frame_destructor::check_control(const std::string &control) {
    if (control == estts::ax25::AX25_CONTROL)
        return estts::ES_OK;
    return estts::ES_UNSUCCESSFUL;
}

estts::Status frame_destructor::check_pid(const std::string &pid) {
    if (pid == estts::ax25::AX25_PID)
        return estts::ES_OK;
    return estts::ES_UNSUCCESSFUL;
}

estts::Status frame_destructor::check_crc(const std::string& frame, const std::string &crc) {
    // TODO check the CRC against the calculated CRC of the entire frame
    return estts::ES_OK;
}

estts::Status frame_destructor::validate_header(const std::string &frame) {
    if (estts::ES_OK != check_source(frame.substr(0, 12)))
        return estts::ES_UNSUCCESSFUL;
    if (estts::ES_OK != check_ssid0(frame.substr(12, 2)))
        return estts::ES_UNSUCCESSFUL;
    if (estts::ES_OK != check_destination(frame.substr(14, 12)))
        return estts::ES_UNSUCCESSFUL;
    if (estts::ES_OK != check_ssid1(frame.substr(26, 2)))
        return estts::ES_UNSUCCESSFUL;
    if (estts::ES_OK != check_control(frame.substr(28, 2)))
        return estts::ES_UNSUCCESSFUL;
    if (estts::ES_OK != check_pid(frame.substr(30, 2)))
        return estts::ES_UNSUCCESSFUL;

    return estts::ES_OK;
}

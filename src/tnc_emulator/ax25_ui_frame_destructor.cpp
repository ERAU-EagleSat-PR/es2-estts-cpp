//
// Created by Hayden Roszell on 12/28/21.
//

#include <iostream>
#include <sstream>
#include "ax25_ui_frame_destructor.h"
#include "helper.h"

estts::Status ax25_ui_frame_destructor::decode_frame(const std::string &frame) {
    SPDLOG_DEBUG("Found frame: {}", frame);

    if (estts::ES_OK != validate_header(frame)) {
        spdlog::error("Frame validation failed");
        return estts::ES_UNSUCCESSFUL;
    }

    int crc_size = 0; // Usually 16 bits todo when CRC is checked and tests are added including a CRC, subtract it here

    auto resp_object = build_telemetry_object(frame.substr(32));

    telemetry.push_back(resp_object);

    return estts::ES_OK;
}

estts::Status ax25_ui_frame_destructor::check_source(const std::string &source) {
    if (hex_to_ascii(source) == estts::ax25::AX25_DESTINATION_ADDRESS)
        return estts::ES_OK;
    spdlog::error("Source: Expected {}; Got {}", estts::ax25::AX25_DESTINATION_ADDRESS, hex_to_ascii(source));
    return estts::ES_UNSUCCESSFUL;
}

estts::Status ax25_ui_frame_destructor::check_destination(const std::string &dest) {
    if (hex_to_ascii(dest) == estts::ax25::AX25_SOURCE_ADDRESS)
        return estts::ES_OK;
    spdlog::error("Destination: Expected {}; Got {}", estts::ax25::AX25_SOURCE_ADDRESS, hex_to_ascii(dest));
    return estts::ES_UNSUCCESSFUL;
}

estts::Status ax25_ui_frame_destructor::check_ssid0(const std::string &ssid) {
    if (ssid == estts::ax25::AX25_SSID0)
        return estts::ES_OK;
    spdlog::error("SSID0: Expected {}; Got {}", estts::ax25::AX25_SSID0, ssid);
    return estts::ES_UNSUCCESSFUL;
}

estts::Status ax25_ui_frame_destructor::check_ssid1(const std::string &ssid) {
    if (ssid == estts::ax25::AX25_SSID1)
        return estts::ES_OK;
    spdlog::error("SSID1: Expected {}; Got {}", estts::ax25::AX25_SSID1, ssid);
    return estts::ES_UNSUCCESSFUL;
}

estts::Status ax25_ui_frame_destructor::check_control(const std::string &control) {
    if (control == estts::ax25::AX25_CONTROL)
        return estts::ES_OK;
    spdlog::error("Control: Expected {}; Got {}", estts::ax25::AX25_CONTROL, control);
    return estts::ES_UNSUCCESSFUL;
}

estts::Status ax25_ui_frame_destructor::check_pid(const std::string &pid) {
    if (pid == estts::ax25::AX25_PID)
        return estts::ES_OK;
    spdlog::error("PID: Expected {}; Got {}", estts::ax25::AX25_PID, pid);
    return estts::ES_UNSUCCESSFUL;
}

estts::Status ax25_ui_frame_destructor::check_crc(const std::string &frame, const std::string &crc) {
    // TODO check the CRC against the calculated CRC of the entire frame
    return estts::ES_OK;
}

estts::Status ax25_ui_frame_destructor::validate_header(const std::string &frame) {
    if (estts::ES_OK != check_destination(frame.substr(0, 12)))
        return estts::ES_UNSUCCESSFUL;
    if (estts::ES_OK != check_ssid0(frame.substr(12, 2)))
        return estts::ES_UNSUCCESSFUL;
    if (estts::ES_OK != check_source(frame.substr(14, 12)))
        return estts::ES_UNSUCCESSFUL;
    if (estts::ES_OK != check_ssid1(frame.substr(26, 2)))
        return estts::ES_UNSUCCESSFUL;
    if (estts::ES_OK != check_control(frame.substr(28, 2)))
        return estts::ES_UNSUCCESSFUL;
    if (estts::ES_OK != check_pid(frame.substr(30, 2)))
        return estts::ES_UNSUCCESSFUL;
    SPDLOG_TRACE("Frame header validated. Continuing");
    return estts::ES_OK;
}

estts::Status ax25_ui_frame_destructor::build_telemetry_objects() {
    try {
        for (;;) {
            // First, find the start of the frame using the flag.
            // Assuming that the frame transmission had no loss, this method should work.
            unsigned long frame_start_flag;
            auto raw_length = raw_frame.length();
            if (raw_length < 1) {
                spdlog::error("Trying to decode empty frame. Exiting");
                return estts::ES_UNSUCCESSFUL;
            }
            for (int i = 0; i < raw_length; i++) {
                if (raw_frame[i] == estts::ax25::AX25_FLAG[0] && raw_frame[i + 1] == estts::ax25::AX25_FLAG[1]) {
                    i++;
                } else {
                    frame_start_flag = i;
                    break;
                }
            }

            // Now, we need to find the end of the frame. We can do this using the flag, and safely assume that if the
            // trailing flag is missing, the frame is likely corrupted.
            unsigned long frame_end;
            for (int i = 0; i < raw_length; i++) {
                if (i + 1 >= raw_length) {
                    SPDLOG_TRACE("No more frames found.");
                    return estts::ES_OK;
                }
                if (raw_frame[i] == estts::ax25::AX25_FLAG[0] && raw_frame[i + 1] == estts::ax25::AX25_FLAG[1]) {
                    if (estts::ax25::AX25_FLAG[1] != raw_frame[i - 1] && i > 0) {
                        frame_end = i;
                        break;
                    }
                }
            }

            // Try decoding the frame.
            auto frame = raw_frame.substr(frame_start_flag, frame_end - frame_start_flag);
            if (estts::ES_OK == decode_frame(frame)) {
                SPDLOG_TRACE("Trimming at index {} and looking for more frames", frame_end);
                raw_frame.erase(0, frame_end);
            } else {
                // Then, find the start of the frame using the src/dest callsigns. This is a failsafe
                // method in case the transmission had loss
                std::stringstream callsign_and_ssid;
                // Remember that we're expecting the satellite to be the source and ground to be the destination
                callsign_and_ssid << ascii_to_hex(estts::ax25::AX25_SOURCE_ADDRESS) << estts::ax25::AX25_SSID0 <<
                                  ascii_to_hex(estts::ax25::AX25_DESTINATION_ADDRESS) << estts::ax25::AX25_SSID1
                                  << std::endl;
                auto temp = raw_frame.find(callsign_and_ssid.str());
                auto frame_start_callsign = (temp != std::string::npos) ? -1 : temp;
                break;
            }
        }
    }
    catch (const std::exception &e) {
        spdlog::error("Failed to find frame, found exception {}", e.what());
        return estts::ES_UNSUCCESSFUL;
    }
}

std::vector<estts::telemetry_object *> ax25_ui_frame_destructor::destruct_ax25() {
    if (estts::ES_OK != build_telemetry_objects()) {
        return {};
    }
    return this->telemetry;
}
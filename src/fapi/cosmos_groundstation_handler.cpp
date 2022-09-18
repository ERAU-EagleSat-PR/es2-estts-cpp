/* Copyright © EagleSat II - Embry Riddle Aeronautical University - All rights reserved - 2022 */

#include <sstream>
#include <iostream>
#include <iomanip>
#include "cosmos_groundstation_handler.h"
#include "socket_handler.h"
#include "constants.h"
#include "session_manager_modifier.h"
#include "helper.h"

#define TWO_TO_THE_19th 524288

using namespace std::this_thread; // sleep_for, sleep_until
using namespace std::chrono; // nanoseconds, system_clock, seconds
using namespace estts;
using namespace estts::endurosat;

std::function<Status(std::string)> get_groundstation_command_callback_lambda(const std::string& command, socket_handler * sock);
std::function<Status(std::string)> get_groundstation_telemetry_callback_lambda(socket_handler * sock);
std::function<Status(std::string)> get_groundstation_session_transmit_func(groundstation_manager * gm);
std::function<std::string()> get_groundstation_session_receive_func(groundstation_manager * gm);
std::function<Status()> get_groundstation_session_start_session_func();
std::function<Status()> get_groundstation_session_end_session_func();

std::function<estts::Status(std::string)> get_set_txvr_freq_modifier(cosmos_groundstation_handler * cgsh);
std::function<estts::Status(std::string)> get_read_txvr_scw_modifier(cosmos_groundstation_handler * cgsh);
std::function<estts::Status(std::string)> get_set_txvr_scw_modifier(cosmos_groundstation_handler * cgsh);
std::function<estts::Status(std::string)> get_set_satellite_nominal_frequency_modifier(cosmos_groundstation_handler * cgsh);

cosmos_groundstation_handler::cosmos_groundstation_handler() {
    gm = nullptr;
    sock = new socket_handler(estts::cosmos::COSMOS_SERVER_ADDR, estts::cosmos::COSMOS_GROUNDSTATION_CMD_TELEM_PORT);
    sm = nullptr;
}

void cosmos_groundstation_handler::groundstation_cosmos_worker() {
    auto config = new session_config;
    config->receive_func = get_groundstation_session_receive_func(gm);
    config->end_session_func = get_groundstation_session_end_session_func();
    config->start_session_func = get_groundstation_session_start_session_func();
    config->transmit_func = get_groundstation_session_transmit_func(gm);
    config->priority = 1;
    config->satellite_range_required_for_execution = false;
    config->endpoint = GROUND_STATION;

    sm = gm->generate_session_manager(config);
    if (sm == nullptr)
        throw std::runtime_error("Primary COSMOS worker failed to request a session manager.");

    sm->set_session_modifier(build_session_modifier());
    std::string command;
    for (;;) {
        command = sock->read_socket_s();
        if (not command.empty()) {
            sm->schedule_command(command, get_groundstation_command_callback_lambda(command, sock));
        }
    }
}

estts::Status cosmos_groundstation_handler::cosmos_groundstation_init(groundstation_manager * temp_gm) {
    while (sock->init_socket_handle() != ES_OK) {
        SPDLOG_WARN("Socket handler init failed. Retry in 1 second.");
        sleep_until(system_clock::now() + seconds(1));
    }

    sock->init_socket_handle();
    this->gm = temp_gm;

    gm->set_groundstation_telemetry_callback(get_groundstation_telemetry_callback_lambda(sock));

    cosmos_worker = std::thread(&cosmos_groundstation_handler::groundstation_cosmos_worker, this);
    SPDLOG_TRACE("Created groundstation COSMOS worker thread with ID {}", std::hash<std::thread::id>{}(cosmos_worker.get_id()));

    return estts::ES_OK;
}

double get_fc_frac_conv(double fc_frac) {
    return fc_frac / TWO_TO_THE_19th;
}

estts::Status cosmos_groundstation_handler::set_transceiver_frequency(double frequency) {
    // Frequency is in Hz

    SPDLOG_INFO("Setting UHF frequency to {}Hz", frequency);

    auto f_req_xo = 26000000;
    auto outdiv = 8.0;
    auto npresc = 2;

    int fc_inte = 30.0;
    int fc_frac = TWO_TO_THE_19th;

    auto left_side = (frequency * outdiv) / (npresc * f_req_xo);

    long iterations = 0;

    for (;;) {
        fc_frac++;
        iterations++;

        if (get_fc_frac_conv(fc_frac) > 2) {
            fc_inte++;
            fc_frac = TWO_TO_THE_19th;
        }

        if (left_side < fc_inte + get_fc_frac_conv(fc_frac) + 0.000001 && left_side > (fc_inte + get_fc_frac_conv(fc_frac)) - 0.000001)
            break;
    }

    auto fc_frac_little = __builtin_bswap32(fc_frac);

    SPDLOG_DEBUG("Calculated fc_frac = {} and fc_inte = {} in {} iterations", fc_frac, fc_inte, iterations);
    SPDLOG_TRACE("These values calculate to a frequency of {}MHz", ((fc_inte + get_fc_frac_conv(fc_frac)) * npresc * (f_req_xo/outdiv)) / 1000000);

    std::stringstream fc_inte_ss;
    fc_inte_ss << std::setfill ('0') << std::setw(2) << std::uppercase << std::hex << fc_inte;

    std::stringstream fc_frac_little_ss;
    fc_frac_little_ss << std::uppercase << std::hex << fc_frac_little;
    std::string fc_frac_little_s = fc_frac_little_ss.str();
    fc_frac_little_s.resize(6);

    std::stringstream freq_equiv;
    freq_equiv << fc_frac_little_s << fc_inte_ss.str();

    SPDLOG_DEBUG("Converted {}Hz to {} in accordance with SiliconLabs Si4463 PLL synth", frequency, freq_equiv.str());
    auto resp = sm->default_command_executor("ES+W2201" + freq_equiv.str(), "");

    if (resp.empty()) {
        SPDLOG_ERROR("Failed to receive response to set frequency command.");
        return ES_UNSUCCESSFUL;
    }

    if (resp.find("ERR") != std::string::npos) {
        SPDLOG_ERROR("Failed to set frequency. Got back: {}", resp);
        return ES_UNSUCCESSFUL;
    }

    SPDLOG_INFO("Successfully set UHF frequency to {}Hz", frequency);

    return estts::ES_OK;
}

session_manager_modifier *cosmos_groundstation_handler::build_session_modifier() {
    auto modifier = new session_manager_modifier();
    // Set transceiver frequency
    modifier->insert_execution_modifier("ES+W2201", get_set_txvr_freq_modifier(this));
    // Read status control word
    modifier->insert_execution_modifier("ES+R2200", get_read_txvr_scw_modifier(this));
    // Write status control word
    modifier->insert_execution_modifier("ES+W2200", get_set_txvr_scw_modifier(this));
    // Set nominal satellite frequency for dynamic doppler accommodation
    modifier->insert_execution_modifier("ES+W6903", get_set_satellite_nominal_frequency_modifier(this));
    // Read nominal satellite frequency for dynamic doppler accommodation
    modifier->insert_execution_modifier("ES+R6903", [this] (const std::string& command) -> estts::Status {
        std::stringstream s;
        s << "OK+" << satellite_txvr_nominal_frequency_hz / 1000000.0;
        return get_groundstation_command_callback_lambda("ES+W6903", this->get_socket_handler())(s.str());
    });
    // Set dynamic doppler mode
    modifier->insert_execution_modifier("ES+W6904", [this] (const std::string& command) -> estts::Status {
        std::string mode = command;
        mode.erase(0, 8);
        if (mode == "0") dynamic_doppler_mode = false;
        else dynamic_doppler_mode = true;
        return get_groundstation_command_callback_lambda("ES+W6904", this->get_socket_handler())("OK+" + mode);
    });
    // Get dynamic doppler mode
    modifier->insert_execution_modifier("ES+R6904", [this] (const std::string& command) -> estts::Status {
        std::stringstream s;
        if (dynamic_doppler_mode) s << "OK+1";
        else s << "OK+0";
        return get_groundstation_command_callback_lambda("ES+W6904", this->get_socket_handler())(s.str());
    });

    return modifier;
}

std::function<estts::Status(std::string)> get_set_txvr_scw_modifier(cosmos_groundstation_handler * cgsh) {
    return [cgsh] (const std::string& command) -> estts::Status {
        std::string original_command = "ES+W2200";
        spdlog::trace("Modifier found raw packet {}", command);

        std::string scw = command;
        scw.erase(0, original_command.length());
        auto scw_uc = const_cast<unsigned char *>(reinterpret_cast<const unsigned char *>(scw.c_str()));

        auto hex = new char[2];
        sprintf(hex, "%02X%02X", scw_uc[0], scw_uc[1]);

        spdlog::trace("Encoded SCW to {}", hex);
        std::string encoded_scw = original_command + hex;
        spdlog::debug("Complete command modifier: {}", encoded_scw);

        auto resp = cgsh->get_session_manager()->default_command_executor(encoded_scw, "");

        if (resp.empty()) {
            spdlog::error("Failed to receive response to set SCW command.");
            get_groundstation_command_callback_lambda(original_command, cgsh->get_socket_handler())("ERR+Failed to set SCW");
            return ES_UNSUCCESSFUL;
        }

        if (resp.find("ERR") != std::string::npos) {
            spdlog::error("Failed to get SCW. Got back: {}", resp);
            get_groundstation_command_callback_lambda(original_command, cgsh->get_socket_handler())("ERR+Failed to set SCW");
            return ES_UNSUCCESSFUL;
        }

        // Lazy, just call read SCW
        return get_read_txvr_scw_modifier(cgsh)("ES+R2200");
    };
}

std::function<estts::Status(std::string)> get_read_txvr_scw_modifier(cosmos_groundstation_handler * cgsh) {
    return [cgsh] (const std::string& command) -> estts::Status {
        std::string original_command = "ES+R2200";

        auto resp = cgsh->get_session_manager()->default_command_executor(original_command, "");

        if (resp.empty()) {
            spdlog::error("Failed to receive response to get SCW.");
            get_groundstation_command_callback_lambda(original_command, cgsh->get_socket_handler())("ERR+Failed to get SCW");
            return ES_UNSUCCESSFUL;
        }

        if (resp.find("ERR") != std::string::npos) {
            spdlog::error("Failed to get SCW. Got back: {}", resp);
            get_groundstation_command_callback_lambda(original_command, cgsh->get_socket_handler())("ERR+Failed to get SCW");
            return ES_UNSUCCESSFUL;
        }

        std::string scw = resp.substr(9, 4);

        unsigned char *scw_uc;
        if (scw.empty()) {
            spdlog::warn("SCW is blank!");
            return get_groundstation_command_callback_lambda(original_command, cgsh->get_socket_handler())("ERR+Failed to get SCW");
        }

        scw_uc = const_cast<unsigned char *>(reinterpret_cast<const unsigned char *>(scw.c_str()));
        spdlog::trace("string scw: {} uc scw: {}", scw, scw_uc);

        for (int i = 0, j = 0; i < 4; i += 2, j++)
            scw_uc[j] = HexToBin(scw_uc[i], scw_uc[i + 1]);
        scw_uc[3] = '\0';

        std::string new_scw(reinterpret_cast<char const *>(scw_uc));

        std::stringstream reconstructed_scw;
        reconstructed_scw << "OK+" << resp.substr(3, 6);

        reconstructed_scw << ((scw_uc[0] >> 7) & 0x01); // Res
        reconstructed_scw << ((scw_uc[0] >> 6) & 0x01); // HFXT
        reconstructed_scw << ((scw_uc[0] >> 4) & 0x03); // UartBaud
        reconstructed_scw << ((scw_uc[0] >> 3) & 0x01); // Reset
        reconstructed_scw << ((scw_uc[0] >> 0) & 0x07); // RfMode
        for (int i = 7; i >= 0; i--) {
            reconstructed_scw << ((scw_uc[1] >> i) & 0x01);
        }

        spdlog::debug("Reconstructed SCW: {}", reconstructed_scw.str());

        return get_groundstation_command_callback_lambda(original_command, cgsh->get_socket_handler())(reconstructed_scw.str());
    };
}

std::function<estts::Status(std::string)> get_set_txvr_freq_modifier(cosmos_groundstation_handler * cgsh) {
    return [cgsh] (const std::string& command) -> estts::Status {
        std::string original_command = "ES+W2201";

        spdlog::trace("Modifier found raw packet {}", command);

        std::string freq = command;
        freq.erase(0, original_command.length());

        double freq_d = 0.0;
        try {
            freq_d = stod(freq) * 1000000;
        } catch (...) { spdlog::error("{} is not a frequency!"); }


        auto resp = cgsh->set_transceiver_frequency(freq_d);
        std::string cosmos_resp;
        if (resp == ES_OK)
            cosmos_resp = "OK+";
        else
            cosmos_resp = "ERR+Failed to set radio frequency";

        resp = get_groundstation_command_callback_lambda(original_command, cgsh->get_socket_handler())(cosmos_resp + freq);

        return resp;
    };
}

std::function<estts::Status(std::string)> get_set_satellite_nominal_frequency_modifier(cosmos_groundstation_handler * cgsh) {
    return [cgsh] (const std::string& command) -> estts::Status {
        std::string original_command = "ES+W6903";
        spdlog::trace("Modifier found raw packet {}", command);

        std::string freq = command;
        freq.erase(0, original_command.length());

        double freq_d = 0.0;
        try {
            freq_d = stod(freq) * 1000000;
        } catch (...) { spdlog::error("{} is not a frequency!"); }

        cgsh->set_satellite_txvr_nominal_frequency_hz(freq_d);
        return get_groundstation_command_callback_lambda(original_command, cgsh->get_socket_handler())("OK+" + freq);
    };
}

/**
 * Function that returns a function pointer that takes argument for a command received by COSMOS and the local socket
 * handler for dealing with any responses. This function is passed as the command callback to the schedule_command function
 * @param command String command passed by COSMOS
 * @param sock Pointer to socket handler used to handle the command response.
 * @return Function pointer with form std::function<estts::Status(std::string)>
 */
std::function<estts::Status(std::string)> get_groundstation_command_callback_lambda(const std::string& command, socket_handler *sock) {
    return [command, sock] (const std::string& telem) -> estts::Status {
        if (telem.empty() || sock == nullptr) {
            return estts::ES_UNINITIALIZED;
        }
        std::stringstream temp;
        temp << command << telem;
        sock->write_socket_s(temp.str());
        return estts::ES_OK;
    };
}

/**
 * Function that returns a function pointer that takes argument for the local socket
 * handler for dealing with telemetry received by ESTTS.
 * @param sock Pointer to socket handler used to handle the telemetry response.
 * @return Function pointer with form std::function<estts::Status(std::string)>
 */
std::function<estts::Status(std::string)> get_groundstation_telemetry_callback_lambda(socket_handler *sock) {
    return [sock] (const std::string& telem) -> estts::Status {
        if (telem.empty() || sock == nullptr) {
            return estts::ES_UNINITIALIZED;
        }
        sock->write_socket_s(telem);
        return estts::ES_OK;
    };
}

std::function<Status(std::string)> get_groundstation_session_transmit_func(groundstation_manager * gm) {
    return [gm] (std::string value) -> Status {
        // Route
        if (value.rfind("ES+", 0) == 0) {
            value.append("\r");
        }
        return gm->gs_transmit(value);
    };
}

std::function<std::string()> get_groundstation_session_receive_func(groundstation_manager * gm) {
    return [gm] () -> std::string {
        return gm->receive();
    };
}

std::function<Status()> get_groundstation_session_start_session_func() {
    return [] () -> Status { return ES_OK; };
}

std::function<Status()> get_groundstation_session_end_session_func() {
    return [] () -> Status { return ES_OK; };
}

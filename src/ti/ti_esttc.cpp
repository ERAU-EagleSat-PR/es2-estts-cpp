//
// Created by Hayden Roszell on 12/10/21.
//

#include <string>
#include <sstream>
#include "ti_esttc.h"


using std::stringstream;

/**
 * @brief ti_esttc default constructor that initializes ti_serial_handler
 * @param port Serial port (EX "/dev/tty.usbmodem")
 * @param baud Serial baud rate (EX 115200)
 * @return None
 */
ti_esttc::ti_esttc(const char *es_transmitter_port, int baud) : ti_serial_handler(es_transmitter_port, baud) {
    esttc_symbols = new estts::endurosat::esttc;
}

/**
 * @brief Enables transparent pipe mode on Endurosat UHF Transceiver module.
 * @return #ES_OK if pipe mode enabled successfully, or #ES_UNSUCCESSFUL if not
 */
estts::Status ti_esttc::enable_pipe() {
    // TODO Stephen's task for Sprint #3
    return estts::ES_OK;
}

/**
 * @brief Get the radio frequency and the last RSSI (Received signal strength indication) of the UHF Transceiver module.
 * @return //TODO - This should return a struct to the specific answer format of this command
 */
char* ti_esttc::get_radio_freq() {
    auto command = build_esttc_command(esttc_symbols->METHOD_READ, esttc_symbols->COMMAND_RFC, nullptr);

    auto response = this->read_serial_s();

    // TODO - Figure out how to return the data (directly from here or using an answer decoder)

}

/**
 * @brief Writes (configures) the radio frequency of the Endurosat UHF Transceiver module.
 * @param fractional Fractional part of the radio PLL synthesizer in HEX format (default = "")
 * @param divider Integer divider of the radio PLL synthesizer in HEX format (default = "")
 * @return #ES_OK if radio frequency was configured successfully, or #ES_UNSUCCESSFUL if not
 */
estts::Status ti_esttc:: ti_esttc::config_radio_freq(const char *fractional, const char *divider) {
    std::string command_body = fractional;
    command_body += divider;

    auto command = build_esttc_command(esttc_symbols->METHOD_WRITE, esttc_symbols->COMMAND_RFC, command_body.c_str());

    if (this->write_serial_s(command) < 0) {
        spdlog::error("Failed to transmit command");
        return estts::ES_UNSUCCESSFUL;
    }

    return estts::ES_OK;
}

/**
 * @brief Gets internal IMU temperature of EnduroSat UHF Transceiver module
 * @return Double representing internal IMU temperature in Celsius
 */
double ti_esttc::get_temp() {
#ifdef __TI_DEV_MODE__
    return 26;
#else
    auto command = build_esttc_command(esttc_symbols->METHOD_READ, esttc_symbols->COMMAND_TEMP_SENSOR, nullptr);
    if (this->write_serial_s(command) < 0) {
        spdlog::error("Failed to transmit command");
        return -1;
    }
    auto ret = this->read_serial_s();
    ret.replace(ret.find("OK +"), 4, "");
    SPDLOG_INFO("Transceiver internal temperature is {}°C", ret);
    // TODO make this actually return the temperature
    return 0;
#endif
}

/**
 * @brief Builds ESTTC command in format specified by EnduroSat ESTTC Specification
 * @param method Command method ('r' or 'w')
 * @param command_code EnduroSat ESTTC command code
 * @param body Request body for command if writing
 * @return String containing constructed ESTTC command frame (including \\r)
 */
std::string ti_esttc::build_esttc_command(char method, const char *command_code, const char *body) {
    stringstream command;
    command << esttc_symbols->HEADER;
    command << method;
    command << esttc_symbols->ADDRESS;
    command << command_code;
    if (body != nullptr)
        command << body;
    command << esttc_symbols->END;
    SPDLOG_TRACE("build_esttc_command: built command '{}'", command.str());
    return command.str();
}

/**
 * @brief Calculates CRC32 of command string specified by EnduroSat
 * @param string String input to calculate CRC32
 * @return Calculated CRC32 of inputted string
 */
std::string ti_esttc::calculate_crc32(std::string string) {
    // TODO Taylor's task for sprint #3
    return string;
}

ti_esttc::~ti_esttc() {
    delete esttc_symbols;
}

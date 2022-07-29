/* Copyright © EagleSat II - Embry Riddle Aeronautical University - All rights reserved - 2022 */

#ifndef ESTTS_CONSTANTS_H
#define ESTTS_CONSTANTS_H

// Configure spdlog
#undef SPDLOG_ACTIVE_LEVEL
#define SPDLOG_ACTIVE_LEVEL 0
#include "spdlog/spdlog.h"

#define MAX_SERIAL_READ 256
namespace estts {
    const char REMOVABLE_STORAGE_NAME[] = "EagleSat";

    const int ESTTS_MAX_RETRIES = 2;
    const int ESTTS_RETRY_WAIT_SEC = 1;
    const int ESTTS_AWAIT_RESPONSE_PERIOD_SEC = 10;
    const int ESTTS_MAX_SATELLITE_RANGE_CHECK_INTERVAL_SEC = 60;
    const int ESTTS_MIN_SATELLITE_RANGE_CHECK_INTERVAL_SEC = 20;
    const int ESTTS_SATELLITE_CONNECTION_TIMEOUT_MIN = 90;
    const int ESTTS_CHECK_SATELLITE_INRANGE_INTERVAL_SEC = 30;
    const int ESTTS_REQUEST_SESSION_TIMEOUT_SECONDS = 300;

    namespace cosmos {
        const char COSMOS_SERVER_ADDR[] = "172.19.35.150"; // 172.30.95.164 172.19.35.160
        const int COSMOS_PRIMARY_CMD_TELEM_PORT = 65432;
        const int COSMOS_GROUNDSTATION_CMD_TELEM_PORT = 8046;
        const int COSMOS_SATELLITE_TXVR_CMD_TELEM_PORT = 55927;
        const int COSMOS_PRIMARY_AX25_TELEM_PORT = 55482;
    }

    namespace ti_serial {
        const char TI_SERIAL_ADDRESS[] = "/dev/cu.usbserial-A10JVB3P";
    }

    namespace ti_socket {
        const int MAX_RETRIES = 2;
        const int WAIT_TIME_SEC = 2;
        const int TI_SOCKET_BUF_SZ = 1024;
        const char TI_SOCKET_ADDRESS[] = "127.0.0.1";
        const int TI_SOCKET_PORT = 65548;
    }

    /* AX.25 Related constants */
    namespace ax25 {
        const char AX25_FLAG[] = "7E"; // Flag is constant
        const char AX25_DESTINATION_ADDRESS[] = "NABCDE"; // Max 48-bit (6-byte)
        const char AX25_SSID0[] = "E0";
        const char AX25_SOURCE_ADDRESS[] = "NEDCBA"; // Max 48-bit (6-byte)
        const char AX25_SSID1[] = "E1";
        const char AX25_CONTROL[] = "03"; // 03 = Unnumbered Information
        const char AX25_PID[] = "F0"; // F0 = No layer 3 protocol implemented

        const char NEW_SESSION_FRAME[] = "AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA";
        const char END_SESSION_FRAME[] = "BBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBB";
    }

    namespace telem_handler {
        const char TELEM_HANDLER_STATE_FILE[] = "es2_state.json";
    }

    namespace estts_response_code {
        const int SUCCESS = 0;
        const int UNRECOGNIZED_REQUEST = 1;
        const int OBC_FAILURE = 2;
    }

    /* Endpoint names for all communication systems */
    namespace es2_endpoint {
        const int ES_OBC = 01;
        const int ES_EPS = 02;
        const int ES_ACS = 03;
        const int ES_CRP = 05;
        const int ES_MDE = 04;
        const int ES_OFFLINE_LOG = 05;
        const int ES_TELEMETRY = 06;
    }

    /* Generic response code enumeration for return codes */
    enum Status {
        ES_OK = 0,
        ES_SUCCESS = 0,
        ES_UNSUCCESSFUL = 1,
        ES_UNINITIALIZED = 2,
        ES_MEMORY_ERROR = 3,
        ES_WAITING = 3,
        ES_BAD_OPTION = 405,
        ES_UNAUTHORIZED = 403,
        ES_SERVER_ERROR = 500,
        ES_INPROGRESS = 300,
        ES_NOTFOUND = 404
    };

    namespace dispatcher {
        const int MAX_COMPLETED_CACHE = 20; // Maximum number of completed commands to remember
    }

    namespace endurosat {
        const int PIPE_DURATION_SEC = 2;
        const int MAX_RETRIES = 2;
        const int WAIT_TIME_SEC = 2;
        const int ES_BAUD = 115200;
        const int MAX_ES_TXVR_TEMP = 50;
        enum PIPE_State {
            PIPE_OFF = 0,
            PIPE_WAITING = 1,
            PIPE_ON = 2
        };
        class esttc_const {
        public:
            const uint8_t NUM_OF_RETRIES = 5;
            const char *HEADER = "ES+";
            const char METHOD_READ = 'R';
            const char METHOD_WRITE = 'W';
            const char METHOD_FIRMWARE_UPDATE = 'D';
            const char *ADDRESS = "22";
            const char *BLANK = " ";
            const char *END = "\r";
            const char* DOWNLINK_XOR = "AB7563CD";
            const char* UPLINK_XOR = "6ACD3B57";
            const char *CMD_SCW = "00"; // Status Control Word
            const char *CMD_RADIO_FREQ_CONFIG = "01"; //  Radio Frequency Configuration
            const char *CMD_READ_UPTIME = "02"; // Read Uptime E
            const char *CMD_READ_TRANS_PCKTS = "03"; // Read Number of Transmitted Packets C
            const char *CMD_READ_RECEIV_PCKTS = "04"; // Read Number of Received Packets E
            const char *CMD_READ_TRANS_PCKTS_CRC = "05"; // Read Number of Transmitted Packets With CRC Error C
            const char *CMD_PIPE_MODE_TMOUT_CONFIG = "06"; // Transparent (Pipe) Mode Timeout Period Configuration E
            const char *CMD_BCN_MSG_TRANS_CONFIG = "07"; //  Beacon Message Transmission Period Configuration C
            const char *CMD_AUDIO_BCN_P_TRANS = "08"; // Audio Beacon Period Between Transmissions E
            const char *CMD_RESTORE = "09"; // Restore Default Values C
            const char *CMD_TEMP_VAL = "0A"; // Internal Temperature Sensor Measurement Value E
            const char *CMD_I2C_RESIST_CONFIG = "0B"; // I2C Pull-Up Resistors Configuration Read/Write C
            const char *CMD_TERM_RESIST_CONFIG = "EC"; // Terminating Resistor Configuration Read/Write E
            const char *CMD_ENABLE_DISABLE_RADIO_CRC = "ED"; // Enabling/Disabling Radio Packet CRC16 C
            const char *CMD_FORCE_BCN_CMD = "EE"; // Force Beacon Command E
            const char *CMD_AUTO_AX25_DECODE = "EF"; // Enabling/Disabling Automatic AX.25 Decoding C
            const char *CMD_READ_WRITE_I2C = "F1"; // Generic Write and/or Read From an I2C Device E
            const char *CMD_ANT_RELEASE_CONFIG = "F2"; // UHF Antenna Release Configuration C
            const char *CMD_ANT_READ_WRITE = "F3"; // UHF Antenna Read/Write E
            const char *CMD_LOW_PWR_MODE = "F4"; // Low Power Mode C
            const char *CMD_DEST_CALL_SIGN = "F5"; // Destination Call Sign E
            const char *CMD_SRC_CALL_SIGN = "F6"; // Source Call Sign C
            const char *CMD_READ_SFTWR_VER = "F9"; // Read Software Version Build E
            const char *CMD_READ_DVC_PAYLOAD = "FA"; // Read Device Payload Size C
            const char *CMD_BCN_MSG_CONFIG = "FB"; // Beacon Message Content Configuration E
            const char *CMD_DVC_ADDR_CONFIG = "FC"; // Device Address Configuration C
            const char *CMD_FRAM_MEM_READ_WRITE = "FD"; // FRAM Memory Read/Write E
            const char *CMD_RADIO_TRANS_PROP_CONFIG = "FE"; // Radio Transceiver Property Configuration C
            const char *CMD_SECURE_MODE = "FF"; // Secure Mode E
            const char *CMD_FRMWR_UPDATE = "AA"; // Firmware Update C

            enum SCW_Commands {
                default_mode,
                enable_pipe,
                scw_stopper
            };

            const char *scw_body[scw_stopper] = {
                    "4343", // default_mode - 0011 0011 0000 0011
                    "3323" // enable_pipe - 0011 0011 0010 0011
            };
        };
    }

    typedef struct dispatched_command {
        std::string frame;
        Status response_code;
        std::string serial_number;
        std::function<estts::Status(std::string)> str_callback;
    } dispatched_command;

    typedef struct waiting_command {
        std::string frame;
        std::string serial_number;
        std::function<estts::Status(std::string)> str_callback;
    } waiting_command;

    enum SessionEndpoint {
        EAGLESAT2_OBC = 1,
        EAGLESAT2_TRANSCEIVER = 2,
        GROUND_STATION = 3,
    };

    typedef struct {
        bool satellite_range_required_for_execution;
        int priority;
        SessionEndpoint endpoint;
        std::function<estts::Status(std::string)> transmit_func;
        std::function<std::string()> receive_func;
        std::function<estts::Status()> start_session_func;
        std::function<estts::Status()> end_session_func;
    } session_config;
}

#endif //ESTTS_CONSTANTS_H

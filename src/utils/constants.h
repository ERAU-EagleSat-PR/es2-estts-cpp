//
// Created by Hayden Roszell on 10/31/21.
//

#ifndef ESTTS_CONSTANTS_H
#define ESTTS_CONSTANTS_H

// Configure spdlog
#undef SPDLOG_ACTIVE_LEVEL
#define SPDLOG_ACTIVE_LEVEL 0
#include "spdlog/spdlog.h"

#define MAX_SERIAL_READ 256
namespace estts {
    const int ESTTS_MAX_RETRIES = 2;
    const int ESTTS_RETRY_WAIT_SEC = 1;
    const int ESTTS_AWAIT_RESPONSE_PERIOD_SEC = 10;
    /* AX.25 Related constants */
    namespace ax25 {
        const char AX25_FLAG[] = "7E"; // Flag is constant
        const char AX25_DESTINATION_ADDRESS[] = "NABCDE"; // Max 48-bit (6-byte)
        const char AX25_SSID0[] = "E0";
        const char AX25_SOURCE_ADDRESS[] = "NEDCBA"; // Max 48-bit (6-byte)
        const char AX25_SSID1[] = "E1";
        const char AX25_CONTROL[] = "03"; // 03 = Unnumbered Information
        const char AX25_PID[] = "F0"; // F0 = No layer 3 protocol implemented
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
        ES_BAD_OPTION = 405,
        ES_UNAUTHORIZED = 403,
        ES_SERVER_ERROR = 500
    };

    namespace es2_commands {
        namespace acs {
            const int ACS_GET_GPS_LAT = 01;
            const int ACS_GET_GPS_LONG = 02;
            const int ACS_GET_POS = 03;
        }
        namespace eps {
            const int EPS_GET_HEALTH = 01;
        }
        namespace mde {
            const int MDE_GET_STATUS = 01;
        }
        namespace crp {
            const int CRP_GET_DATA = 01;
        }
        namespace obc {
            const int OBC_GET_HEALTH = 01;
        }
        namespace method {
            const int ES_READ = 0;
            const int ES_WRITE = 1;
        }
    }

    namespace endurosat {
        const int MAX_RETRIES = 2;
        const int WAIT_TIME_SEC = 2;
        const int ES_BAUD = 115200;
        const int MAX_ES_TXVR_TEMP = 50;
        class esttc {
        public:
            const uint8_t NUM_OF_RETRIES = 5;
            const char *HEADER = "ES+";
            const char METHOD_READ = 'R';
            const char METHOD_WRITE = 'W';
            const char *ADDRESS = "22";
            const char *BLANK = " ";
            const char *END = "\r";
            const char *CMD_SCW = "00"; // Status Control Word
            const char *CMD_RADIO_FREQ_CONFIG = "01"; //  Radio Frequency Configuration
            const char *CMD_READ_UPTIME = "02"; // Read Uptime
            const char *CMD_READ_TRANS_PCKTS = "03"; // Read Number of Transmitted Packets
            const char *CMD_READ_RECEIV_PCKTS = "04"; // Read Number of Received Packets
            const char *CMD_READ_TRANS_PCKTS_CRC = "05"; // Read Number of Transmitted Packets With CRC Error
            const char *CMD_PIPE_MODE_TMOUT_CONFIG = "06"; // Transparent (Pipe) Mode Timeout Period Configuration
            const char *CMD_BCN_MSG_TRANS_CONFIG = "07"; //  Beacon Message Transmission Period Configuration
            const char *CMD_AUDIO_BCN_P_TRANS = "08"; // Audio Beacon Period Between Transmissions
            const char *CMD_RESTORE = "09"; // Restore Default Values
            const char *CMD_TEMP_VAL = "0A"; // Internal Temperature Sensor Measurement Value
            const char *CMD_I2C_RESIST_CONFIG = "0B"; // I2C Pull-Up Resistors Configuration Read/Write
            const char *CMD_TERM_RESIST_CONFIG = "EC"; // Terminating Resistor Configuration Read/Write
            const char *CMD_ENABLE_DISABLE_RADIO_CRC = "ED"; // Enabling/Disabling Radio Packet CRC16
            const char *CMD_FORCE_BCN_CMD = "EE"; // Force Beacon Command
            const char *CMD_AUTO_AX25_DECODE = "EF"; // Enabling/Disabling Automatic AX.25 Decoding
            const char *CMD_READ_WRITE_I2C = "F1"; // Generic Write and/or Read From an I2C Device
            const char *CMD_ANT_RELEASE_CONFIG = "F2"; // UHF Antenna Release Configuration
            const char *CMD_ANT_READ_WRITE = "F3"; // UHF Antenna Read/Write
            const char *CMD_LOW_PWR_MODE = "F4"; // Low Power Mode
            const char *CMD_DEST_CALL_SIGN = "F5"; // Destination Call Sign
            const char *CMD_SRC_CALL_SIGN = "F6"; // Source Call Sign
            const char *CMD_READ_SFTWR_VER = "F9"; // Read Software Version Build
            const char *CMD_READ_DVC_PAYLOAD = "FA"; // Read Device Payload Size
            const char *CMD_BCN_MSG_CONFIG = "FB"; // Beacon Message Content Configuration
            const char *CMD_DVC_ADDR_CONFIG = "FC"; // Device Address Configuration
            const char *CMD_FRAM_MEM_READ_WRITE = "FD"; // FRAM Memory Read/Write
            const char *CMD_RADIO_TRNS_PROP_CONFIG = "FE"; // Radio Transceiver Property Configuration
            const char *CMD_SECURE_MODE = "FF"; // Secure Mode
            const char *CMD_FRMWRE_UPDATE = "AA"; // Firmware Update

            enum SCW_Commands {
                enable_pipe,
                scw_stopper
            };

            const char *scw_body[scw_stopper] = {
                    "3323" // enable_pipe - 0011 0011 0010 0011
            };
        };
    }

    namespace ti_serial {
        const char TI_SERIAL_ADDRESS[] = "/dev/cu.";
    }
    
    namespace ti_socket {
        const int MAX_RETRIES = 2;
        const int WAIT_TIME_SEC = 2;
        const int TI_SOCKET_BUF_SZ = 1024;
        const char TI_SOCKET_ADDRESS[] = "127.0.0.1";
        const int TI_SOCKET_PORT = 8080;
    }

    typedef struct estts_command {
        int address{};
        int timeStamp{};
        int sequence{};
        int commandID{};
        int method{};
        const char *data{};
    } command_object;

    typedef struct estts_telemetry {
        int address{};
        int timeStamp{};
        int sequence{};
        int commandID{};
        int response_code{};
        const char *data{};
    } telemetry_object;
}


#endif //ESTTS_CONSTANTS_H

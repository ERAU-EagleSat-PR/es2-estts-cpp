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
    const int ESTTS_AWAIT_RESPONSE_PERIOD_SEC = 5;

    namespace cosmos {
        const char COSMOS_SERVER_ADDR[] = "172.30.95.164"; // 172.30.95.164
        const int COSMOS_PORT = 65432;
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

    namespace es2_commands {
        namespace acs {
            const int ACS_GET_GPS_LAT = 01;
            const int ACS_GET_GPS_LONG = 02;
            const int ACS_GET_POS = 03;
            const int ACS_DEP_MAG_BOOM = 07;
            const int ACS_ENABLE = 10;
            const int ACS_POWER = 11;
            const int ACS_SET_CTRL_MODE = 13;
            const int ACS_SET_EST_MODE = 14;
            const int ACS_SET_MAG_MNT = 33;
            const int ACS_SET_MAG_MNT_MTRX = 34;
            const int ACS_SET_INERTIA = 41;
            const int ACS_SAVE_CONFIG = 63;
            const int ACS_SET_ATT_ANG = 146;
            const int ACS_SET_ANG_RATE = 147;
            const int ACS_GET_MAGNET = 151;
            const int ACS_RATE_SENSE_RATE = 155;
            const int ACS_SET_MAGNETORQUER = 157;
            const int ACS_GET_MAGNETO = 170;
            const int ACS_GET_CC_CURRENT = 172;
            const int ACS_EST_ANG_RATES_FINE = 201;
        }
        namespace eps {
            const int EPS_GET_HEALTH = 01;
            const int EPS_GET_COMMAND_43 = 43;
            const int EPS_GET_BATTERY_VOLTAGE = 1;
            const int EPS_GET_BATTERY_CURRENT = 2;
            const int EPS_GET_5VBUS_CURRENT = 15;
            const int EPS_GET_3VBUS_CURRENT = 14;
            const int EPS_GET_TEMP_SENSOR5 = 38;
            const int EPS_GET_TEMP_SENSOR6 = 39;
            const int EPS_GET_TEMP_SENSOR7 = 40;
            const int EPS_GET_BATTERY_TEMP_SENSOR1 = 19;
            const int EPS_GET_BATTERY_TEMP_SENSOR2 = 20;
            const int EPS_GET_BATTERY_TEMP_SENSOR3 = 21;
            const int EPS_GET_BATTERY_TEMP_SENSOR4 = 22;

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

    namespace es2_telemetry {
        namespace eps {
            struct vitals {
                double battery_voltage;
                double brownouts;
                double charge_time_mins;
            };
            struct eps_voltage {
                double battery_voltage;
            };
            struct eps_current {
                double battery_current;
            };
            struct eps_5Vbus_current {
                double bus_current;
            };
            struct eps_3Vbus_current {
                double bus_current;
            };
            struct eps_externalTemp_sensor5{
                double external_temperature;
            };
            struct eps_externalTemp_sensor6{
                double external_temperature;
            };
            struct eps_externalTemp_sensor7{
                double external_temperature;
            };
            struct eps_batteryTemp_sensor1{
                double battery_temperature;
            };
            struct eps_batteryTemp_sensor2{
                double battery_temperature;
            };
            struct eps_batteryTemp_sensor3{
                double battery_temperature;
            };
            struct eps_batteryTemp_sensor4{
                double battery_temperature;
            };
        }
        namespace acs {
        }
    }

    namespace endurosat {
        const int PIPE_DURATION_SEC = 10;
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

    typedef struct command_object {
        int address{};
        int timeStamp{}; // deprecated
        int sequence{};
        int commandID{};
        int method{};
        const char *data{};
    } command_object;

    typedef struct telemetry_object {
        int address{};
        int timeStamp{}; // deprecated
        int sequence{};
        int commandID{};
        int response_code{};
        const char *data{};
    } telemetry_object;

    typedef struct dispatched_command {
        unsigned char * frame;
        command_object * command;
        std::vector<telemetry_object *> telem_obj;
        std::string telem_str;
        Status response_code;
        std::string serial_number;
        std::function<estts::Status(std::vector<estts::telemetry_object *>)> obj_callback;
        std::function<estts::Status(std::string)> str_callback;
    } dispatched_command;

    typedef struct waiting_command {
        unsigned char * frame;
        command_object * command;
        std::string serial_number;
        std::function<estts::Status(std::vector<estts::telemetry_object *>)> obj_callback;
        std::function<estts::Status(std::string)> str_callback;
    } waiting_command;

    typedef std::function<std::string(estts::command_object *, std::function<estts::Status(std::vector<estts::telemetry_object *>)>)> dispatch_fct;
}

#endif //ESTTS_CONSTANTS_H

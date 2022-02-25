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
    }

    namespace endurosat {
        const int MAX_RETRIES = 2;
        const int WAIT_TIME_SEC = 2;
        const int ES_BAUD = 115200;
        const int MAX_ES_TXVR_TEMP = 50;
        class esttc {
        public:
            const char *HEADER = "ES+";
            const char METHOD_READ = 'R';
            const char METHOD_WRITE = 'W';
            const char *ADDRESS = "22";
            const char *BLANK = " ";
            const char *END = "\r";
            const char *COMMAND_SOFTWARE_BUILD = "F9";
            const char *COMMAND_TEMP_SENSOR = "0A";
            const char *COMMAND_SCW = "00";
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
        std::vector<command_object *> command;
        std::vector<telemetry_object *> telem;
        Status response_code;
        std::string serial_number;
        std::function<estts::Status(std::vector<estts::telemetry_object *>)> callback;
    } dispatched_command;

    typedef struct waiting_command {
        std::vector<command_object *> command;
        std::string serial_number;
        std::function<estts::Status(std::vector<estts::telemetry_object *>)> callback;
    } waiting_command;

    typedef std::function<std::string(std::vector<estts::command_object *>, std::function<estts::Status(std::vector<estts::telemetry_object *>)>)> dispatch_fct;
}


#endif //ESTTS_CONSTANTS_H

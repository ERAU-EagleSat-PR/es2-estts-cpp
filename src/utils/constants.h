//
// Created by Hayden Roszell on 10/31/21.
//

#ifndef ESTTS_CONSTANTS_H
#define ESTTS_CONSTANTS_H

#define MAX_SERIAL_READ 256
namespace estts {
    const int ESTTS_MAX_RETRIES = 2;
    const int ESTTS_RETRY_WAIT_SEC = 1;
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

    /* Endpoint names for all communication systems */
    namespace es2_endpoint {
        const char ES_OBC[] = "01";
        const char ES_EPS[] = "02";
        const char ES_ACS[] = "03";
        const char ES_CRP[] = "05";
        const char ES_MDE[] = "04";
        const char ES_OFFLINE_LOG[] = "0A";
        const char ES_TELEMETRY[] = "0B";
    }

    /* Generic response code enumeration for return codes */
    enum Status {
        ES_OK = 0,
        ES_SUCCESS = 0,
        ES_UNSUCCESSFUL = 1,
        ES_UNINITIALIZED = 2,
        ES_BAD_OPTION = 405,
        ES_UNAUTHORIZED = 403
    };

    namespace es2_commands {
        namespace acs {
            const char ACS_GET_GPS_LAT[] = "01";
            const char ACS_GET_GPS_LONG[] = "02";
            const char ACS_GET_POS[] = "03";
        }
        namespace eps {
            const char EPS_GET_HEALTH[] = "01";
        }
        namespace mde {
            const char MDE_GET_STATUS[] = "01";
        }
        namespace crp {
            const char CRP_GET_DATA[] = "01";
        }
        namespace obc {
            const char OBC_GET_HEALTH[] = "01";
        }
        namespace method {
            const char ES_READ = 'r'; // ASCII 'r'
            const char ES_WRITE = 'w'; // ASCII 'w'
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

    typedef struct estts_command {
        const char *address{};
        int timeStamp{};
        int sequence{};
        const char *commandID{};
        char method{};
        const char *data{};
    } command_object;

    typedef struct estts_telemetry {
        const char *address{};
        int response_code{};
        int timeStamp{};
        int sequence{};
        const char *commandID{};
        char method{};
        const char *data{};
    } telemetry_object;
}


#endif //ESTTS_CONSTANTS_H
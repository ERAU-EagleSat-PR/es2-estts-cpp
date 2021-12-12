//
// Created by Hayden Roszell on 10/31/21.
//

#ifndef ESTTS_CONSTANTS_H
#define ESTTS_CONSTANTS_H

#define MAX_SERIAL_READ 256

namespace estts {
    /*
     * AX.25 Related constants
     */
    namespace ax25 {
        class header {
        public:
            const unsigned char AX25_FLAG = 0x7E; // Flag is constant
            const char * AX25_DESTINATION_ADDRESS = "NABC  "; // Max 48-bit (6-byte)
            const unsigned char AX25_SSID0 = 0xE0;
            const char * AX25_SOURCE_ADDRESS = "NBCA  "; // Max 48-bit (6-byte)
            const unsigned char AX25_SSID1 = 0xE1;
            const unsigned char AX25_CONTROL = 0x03; // 03 = Unnumbered Information
            const unsigned char AX25_PID = 0xF0; // F0 = No layer 3 protocol implemented
        };
    }

    // TODO Change enumeration types to SCOPED BASED enumerations
    /* Generic response code enumeration for return codes */
    enum Status {
        ES_OK = 0,
        ES_SUCCESS = 0,
        ES_UNSUCCESSFUL = 1,
        ES_BAD_OPTION = 405,
        ES_UNAUTHORIZED = 403
    };

    /* Endpoint names for all communication systems */
    enum Endpoint {
        ES_OBC = 0x01,
        ES_EPS = 0x02,
        ES_ACS = 0x03,
        ES_CRP = 0x05,
        ES_MDE = 0x04,
        ES_OFFLINE_LOG = 0x0A,
        ES_TELEMETRY = 0x0B
    };

    enum Method {
        ES_READ = 0x72, // ASCII 'r'
        ES_WRITE = 0x77 // ASCII 'w'
    };

    enum Command {
        /* OBC commands */
        OBC_GET_HEALTH = 0x01,

        /* EPS commands */
        EPS_GET_SYS_BATVOLTAGE = 0x01,

        /* ACS commands */
        ACS_GET_GPS_LAT = 0x01,
        ACS_GET_GPS_LONG = 0x02,
        ACS_GET_POS = 0X03,

        /* MDE commands */
        MDE_GET_STATUS = 0x01,

        /* CRP commands */
        CRP_GET_DATA = 0x01,
    };

    enum LogLevel {
        TRACE,
        DEBUG,
        ERROR,
        FORCE,
        INFO,
        WARN,
        OFF
    };

    /* EnduroSat specific */

    enum EnduroSatReturn {
        EnduroSat_OK,
        EnduroSat_Fail
    };

    namespace endurosat {
        class esttc {
        public:
            const char * HEADER = "ES+";
            const char METHOD_READ = 'R';
            const char METHOD_WRITE = 'W';
            const char * ADDRESS = "22";
            const char * BLANK = " ";
            const char * END = "\r";
            const char * COMMAND_SOFTWARE_BUILD = "F9";
            const char * COMMAND_TEMP_SENSOR = "0A";
            const char * COMMAND_SCW = "00";
        };
    }
}


#endif //ESTTS_CONSTANTS_H

//
// Created by Hayden Roszell on 10/31/21.
//

#ifndef ESTTS_CONSTANTS_H
#define ESTTS_CONSTANTS_H

/*
 * AX.25 Related constants
 */

const unsigned char AX25_DESTINATION_ADDRESS[] = "sat"; // Max 48-bit
const unsigned char AX25_SSID0[] = {0xE0};
const unsigned char AX25_SSID1[] = {0xE1};
const unsigned char AX25_SOURCE_ADDRESS[] = "gnd"; // Max 48-bit
const unsigned char AX25_CONTROL[] = {0x03};
const unsigned char AX25_PID[] = {0xF0};

/* Generic response code enumeration for return codes */
enum esStatus {
    ES_OK = 0,
    ES_SUCCESS = 0,
    ES_UNSUCCESSFUL = 1,
    ES_BAD_OPTION = 405,
    ES_UNAUTHORIZED = 403
};

/* Endpoint names for all communication systems */
enum  esEndpoint {
    ES_OBC = 0x01,
    ES_EPS = 0x02,
    ES_ACS = 0x03,
    ES_CRP = 0x05,
    ES_MDE = 0x04,
    ES_OFFLINE_LOG = 0x0A,
    ES_TELEMETRY = 0x0B
};

enum esttcCommand {
    ES_EPS_BATVOLTAGE,

};

enum LogLevel {
    ES_TRACE,
    ES_DEBUG,
    ES_ERROR,
    ES_FORCE,
    ES_INFO,
    ES_WARN,
    ES_OFF
};

/* EnduroSat specific */

enum EnduroSatReturn {
    EnduroSat_OK,
    EnduroSat_Fail
};

#endif //ESTTS_CONSTANTS_H

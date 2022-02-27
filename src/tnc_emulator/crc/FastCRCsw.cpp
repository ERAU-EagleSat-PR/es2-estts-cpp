/* FastCRC library code is placed under the MIT license
 * Copyright (c) 2014 - 2021 Frank Bösing
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files (the
 * "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sublicense, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject to
 * the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS
 * BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
 * ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

//
// Thanks to:
// - Catalogue of parametrised CRC algorithms, CRC RevEng
// http://reveng.sourceforge.net/crc-catalogue/
//
// - Danjel McGougan (CRC-Table-Generator)
//

#if defined(ARDUINO)
#include <Arduino.h>
#endif

#if !defined(KINETISK)

#if !defined(ARDUINO)
#define PROGMEM
//errors caused by typeof if not linux
#define pgm_read_byte(addr) (*(const unsigned char *)(addr))
#define pgm_read_word(addr) ({ \
	typeof(addr) _addr = (addr); \
	*(const unsigned short *)(_addr); \
})
#define pgm_read_dword(addr) ({ \
	typeof(addr) _addr = (addr); \
	*(const unsigned long *)(_addr); \
})
#endif



#include "FastCRC.h"
#include "FastCRC_tables.h"


static inline
uint32_t REV16( uint32_t value)
{
    return (value >> 8) | ((value & 0xff) << 8);
}

static inline
uint32_t REV32( uint32_t value)
{
    value = (value >> 16) | ((value & 0xffff) << 16);
    return ((value >> 8) & 0xff00ff) | ((value & 0xff00ff) << 8);
}


// ================= 16-BIT CRC ===================
/** Constructor
 */
FastCRC16::FastCRC16(){}

#define crc_n4(crc, data, table) crc ^= data; \
	crc = pgm_read_word(&table[(crc & 0xff) + 0x300]) ^		\
	pgm_read_word(&table[((crc >> 8) & 0xff) + 0x200]) ^	\
	pgm_read_word(&table[((data >> 16) & 0xff) + 0x100]) ^	\
	pgm_read_word(&table[data >> 24]);

/** CCITT
 * Alias "false CCITT"
 * @param data Pointer to Data
 * @param datalen Length of Data
 * @return CRC value
 */
uint16_t FastCRC16::ccitt_upd(const uint8_t *data, size_t len)
{

    uint16_t crc = seed;
    while (((uintptr_t)data & 3) && len) {
        crc = (crc >> 8) ^ pgm_read_word(&crc_table_ccitt[(crc & 0xff) ^ *data++]);
        len--;
    }

    while (len >= 16) {
        len -= 16;
        crc_n4(crc, ((uint32_t *)data)[0], crc_table_ccitt);
        crc_n4(crc, ((uint32_t *)data)[1], crc_table_ccitt);
        crc_n4(crc, ((uint32_t *)data)[2], crc_table_ccitt);
        crc_n4(crc, ((uint32_t *)data)[3], crc_table_ccitt);
        data += 16;
    }

    while (len--) {
        crc = (crc >> 8) ^ pgm_read_word(&crc_table_ccitt[(crc & 0xff) ^ *data++]);
    }

    seed = crc;
    crc = REV16(crc);

    return crc;
}
uint16_t FastCRC16::ccitt(const uint8_t *data,const size_t datalen)
{
    // poly=0x1021 init=0xffff refin=false refout=false xorout=0x0000 check=0x29b1
    seed = 0xffff;
    return ccitt_upd(data, datalen);
}

/** MCRF4XX
 * equivalent to _crc_ccitt_update() in crc16.h from avr_libc
 * @param data Pointer to Data
 * @param datalen Length of Data
 * @return CRC value
 */

// ================= 32-BIT CRC ===================
/** Constructor
 */
FastCRC32::FastCRC32(){}

#define crc_n4d(crc, data, table) crc ^= data; \
	crc = pgm_read_dword(&table[(crc & 0xff) + 0x300]) ^	\
	pgm_read_dword(&table[((crc >> 8) & 0xff) + 0x200]) ^	\
	pgm_read_dword(&table[((crc >> 16) & 0xff) + 0x100]) ^	\
	pgm_read_dword(&table[(crc >> 24) & 0xff]);

#define crcsm_n4d(crc, data, table) crc ^= data; \
	crc = (crc >> 8) ^ pgm_read_dword(&table[crc & 0xff]); \
	crc = (crc >> 8) ^ pgm_read_dword(&table[crc & 0xff]); \
	crc = (crc >> 8) ^ pgm_read_dword(&table[crc & 0xff]); \
	crc = (crc >> 8) ^ pgm_read_dword(&table[crc & 0xff]);

/** CRC32
 * Alias CRC-32/ADCCP, PKZIP, Ethernet, 802.3
 * @param data Pointer to Data
 * @param datalen Length of Data
 * @return CRC value
 */
#if CRC_BIGTABLES
#define CRC_TABLE_CRC32 crc_table_crc32_big
#else
#define CRC_TABLE_CRC32 crc_table_crc32
#endif

uint32_t FastCRC32::crc32_upd(const uint8_t *data, size_t len)
{

    uint32_t crc = seed;

    while (((uintptr_t)data & 3) && len) {
        crc = (crc >> 8) ^ pgm_read_dword(&CRC_TABLE_CRC32[(crc & 0xff) ^ *data++]);
        len--;
    }

    while (len >= 16) {
        len -= 16;
#if CRC_BIGTABLES
        crc_n4d(crc, ((uint32_t *)data)[0], CRC_TABLE_CRC32);
        crc_n4d(crc, ((uint32_t *)data)[1], CRC_TABLE_CRC32);
        crc_n4d(crc, ((uint32_t *)data)[2], CRC_TABLE_CRC32);
        crc_n4d(crc, ((uint32_t *)data)[3], CRC_TABLE_CRC32);
#else
        crcsm_n4d(crc, ((uint32_t *)data)[0], CRC_TABLE_CRC32);
		crcsm_n4d(crc, ((uint32_t *)data)[1], CRC_TABLE_CRC32);
		crcsm_n4d(crc, ((uint32_t *)data)[2], CRC_TABLE_CRC32);
		crcsm_n4d(crc, ((uint32_t *)data)[3], CRC_TABLE_CRC32);
#endif
        data += 16;
    }

    while (len--) {
        crc = (crc >> 8) ^ pgm_read_dword(&CRC_TABLE_CRC32[(crc & 0xff) ^ *data++]);
    }

    seed = crc;
    crc = ~crc;

    return crc;
}

uint32_t FastCRC32::crc32(const uint8_t *data, const size_t datalen)
{
    // poly=0x04c11db7 init=0xffffffff refin=true refout=true xorout=0xffffffff check=0xcbf43926
    seed = 0xffffffff;
    return crc32_upd(data, datalen);
}

/** CKSUM
 * Alias CRC-32/POSIX
 * @param data Pointer to Data
 * @param datalen Length of Data
 * @return CRC value
 */
#if CRC_BIGTABLES
#define CRC_TABLE_CKSUM crc_table_cksum_big
#else
#define CRC_TABLE_CKSUM crc_table_cksum
#endif
uint32_t FastCRC32::cksum_upd(const uint8_t *data, size_t len)
{

    uint32_t crc = seed;

    while (((uintptr_t)data & 3) && len) {
        crc = (crc >> 8) ^ pgm_read_dword(&CRC_TABLE_CKSUM[(crc & 0xff) ^ *data++]);
        len--;
    }

    while (len >= 16) {
        len -= 16;
#if CRC_BIGTABLES
        crc_n4d(crc, ((uint32_t *)data)[0], CRC_TABLE_CKSUM);
        crc_n4d(crc, ((uint32_t *)data)[1], CRC_TABLE_CKSUM);
        crc_n4d(crc, ((uint32_t *)data)[2], CRC_TABLE_CKSUM);
        crc_n4d(crc, ((uint32_t *)data)[3], CRC_TABLE_CKSUM);
#else
        crcsm_n4d(crc, ((uint32_t *)data)[0], CRC_TABLE_CKSUM);
		crcsm_n4d(crc, ((uint32_t *)data)[1], CRC_TABLE_CKSUM);
		crcsm_n4d(crc, ((uint32_t *)data)[2], CRC_TABLE_CKSUM);
		crcsm_n4d(crc, ((uint32_t *)data)[3], CRC_TABLE_CKSUM);
#endif
        data += 16;
    }

    while (len--) {
        crc = (crc >> 8) ^ pgm_read_dword(&CRC_TABLE_CKSUM[(crc & 0xff) ^ *data++]);
    }

    seed = crc;
    crc = ~REV32(crc);
    return crc;
}

uint32_t FastCRC32::cksum(const uint8_t *data, const size_t datalen)
{
    // width=32 poly=0x04c11db7 init=0x00000000 refin=false refout=false xorout=0xffffffff check=0x765e7680
    seed = 0x00;
    return cksum_upd(data, datalen);
}

#endif // #if !defined(KINETISK)
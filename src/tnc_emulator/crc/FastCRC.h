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


// Teensy 3.0, Teensy 3.1:
// See K20P64M72SF1RM.pdf (Kinetis), Pages 638 - 641 for documentation of CRC Device
// See KINETIS_4N30D.pdf for Errata (Errata ID 2776)
//
// So, ALL HW-calculations are done as 32 bit.
//
//
//
// Thanks to:
// - Catalogue of parametrised CRC algorithms, CRC RevEng
// http://reveng.sourceforge.net/crc-catalogue/
//
// - Danjel McGougan (CRC-Table-Generator)
//


// Set this to 0 for smaller 32BIT-CRC-Tables:
#if !defined(CRC_BIGTABLES)
#define CRC_BIGTABLES 1
#endif

#if !defined(FastCRC_h)
#define FastCRC_h

#if defined(ARDUINO)
#include <Arduino.h>
#endif

#include <inttypes.h>


// ================= DEFINES ===================
#if defined(KINETISK)
#define CRC_SW 0
#define CRC_FLAG_NOREFLECT         (((1<<31) | (1<<30)) | ((0<<29) | (0<<28))) //refin=false refout=false
#define CRC_FLAG_REFLECT           (((1<<31) | (0<<30)) | ((1<<29) | (0<<28))) //Reflect in- and outgoing bytes (refin=true refout=true)
#define CRC_FLAG_XOR               (1<<26)                                     //Perform XOR on result
#define CRC_FLAG_NOREFLECT_8       (0)                                         //For 8-Bit CRC
#define CRC_FLAG_REFLECT_SWAP      (((1<<31) | (0<<30)) | ((0<<29) | (1<<28))) //For 16-Bit CRC (byteswap)
#else
#define CRC_SW 1
#endif

// ================= 16-BIT CRC ===================

class FastCRC16
{
public:
  FastCRC16();
  uint16_t ccitt(const uint8_t *data, const size_t datalen);      // Alias "false CCITT"


  uint16_t ccitt_upd(const uint8_t *data, size_t len);			// Call for subsequent calculations with previous seed

#if !CRC_SW
  uint16_t generic(const uint16_t polyom, const uint16_t seed, const uint32_t flags, const uint8_t *data, const size_t datalen); //Not available in non-hw-variant (not T3.x)
#endif
private:
#if CRC_SW
  uint16_t seed;
#else
  uint16_t update(const uint8_t *data, const size_t datalen);
#endif
};

// ================= 32-BIT CRC ===================

class FastCRC32
{
public:
  FastCRC32();
  uint32_t crc32(const uint8_t *data, const size_t datalen);		// Alias CRC-32/ADCCP, PKZIP, Ethernet, 802.3
  uint32_t cksum(const uint8_t *data, const size_t datalen);		// Alias CRC-32/POSIX

  uint32_t crc32_upd(const uint8_t *data, size_t len);			// Call for subsequent calculations with previous seed
  uint32_t cksum_upd(const uint8_t *data, size_t len);			// Call for subsequent calculations with previous seed
#if !CRC_SW
  uint32_t generic(const uint32_t polyom, const uint32_t seed, const uint32_t flags, const uint8_t *data, const size_t datalen); //Not available in non-hw-variant (not T3.x)
#endif
private:
#if CRC_SW
  uint32_t seed;
#else
  uint32_t update(const uint8_t *data, const size_t datalen);
#endif
};

#endif

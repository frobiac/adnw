/*
    This file is part of the AdNW keyboard firmware.

    Copyright 2017-2020 Stefan Fröbe, <frobiac /at/ gmail [d0t] com>

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/


#pragma once

#include <stdint.h>
#include <stdbool.h>

#define TR_COLS 26 // 20 to treat ij pq vwxyz as one column
#define TR_ROWS 13

#define SHA1_B64_BYTES 27 // base64 encoded 20 byte sha1 hash. Only 26 usable as last always contains '=' !

// HMAC: is about 1.5k larger
#define HMAC  1
// XOR : No need to get fancy with hmac or random string:
//       Reading out Flash and EEPROM or binary is not protected anyways.
#define XOR   2

#define TR_ALGO XOR

#if TR_ALGO == XOR
    #include "xor.h"
#elif TR_ALGO == HMAC
    #include <src/external/avr-cryptolib/hmac-sha1.h>
#else
    #error TR_ALGO undefined
#endif

void lock(void);
bool unlocked(void);

uint16_t pwfingerprint(void);

int8_t encrypt(uint8_t * data, uint8_t len);
int8_t decrypt(uint8_t * data, uint8_t len);
void tabula_recta(uint8_t * dst, char row, uint8_t col, uint8_t dig);
void hmac_tag(uint8_t * result, uint8_t result_len, char * tag, uint8_t tag_len, uint8_t offs);
void unlock(uint8_t * code, uint8_t len);



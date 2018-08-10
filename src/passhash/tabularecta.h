/*
    This file is part of the AdNW keyboard firmware.

    Copyright 2017-2018 Stefan Fröbe, <frobiac /at/ gmail [d0t] com>

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

#define HMAC  1
#define XOR   4

#define TR_ALGO 1

#define SHA1_B64_BYTES 27 // base64 encoded 20 byte sha1 hash. Only 26 usable!

#if TR_ALGO == XOR
    #include "xor.h"
#elif TR_ALGO == HMAC
    #include "hmac-sha1.h"
#else
    #error TR_ALGO undefined
#endif


bool unlocked(void);

uint16_t pwfingerprint(void);

int8_t encrypt(uint8_t * data, uint8_t len);
int8_t decrypt(uint8_t * data, uint8_t len);
void tabula_recta(uint8_t * dst, char row, uint8_t col, uint8_t dig);
void unlock(uint8_t * code, uint8_t len);



/*
    This file is part of the AdNW keyboard firmware.

    Copyright 2017 Stefan Fröbe, <frobiac /at/ gmail [d0t] com>

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

#include "b64.h"

#define XOR_BITS 32
#define XOR_BYTES (XOR_BITS/8)


// uses 42 66 126 630 bytes
#if   (XOR_BITS == 8)
    typedef uint8_t  xor_size_t;
#elif (XOR_BITS == 16)
    typedef uint16_t xor_size_t;
#elif (XOR_BITS == 32)
    typedef uint32_t xor_size_t;
#elif (XOR_BITS == 64)
    typedef uint64_t xor_size_t;
#else
    #error "Invalid XOR_BITS"
#endif



/// must call this to seed generator with state != 0
void xor_init(char * seed, uint8_t len);
void xorshift(void);
// void number2str(char dst[XOR_BITS/8], xor_size_t number);
void tr_code(char *dst, uint8_t len, uint8_t row, uint8_t col);

xor_size_t xor_result(void);

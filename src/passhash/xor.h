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
void set_xor_seed(xor_size_t state);
/// generates enough random data to create LEN characters, of which #cnt @pos are put into result _without_ terminating '\0'
void genLine(char *result,  const uint8_t cnt, const uint8_t pos,const uint8_t LEN, const bool hexNotRnd);
/// Loops through line generation for two lines and offset.
void genPass(char *result, const char c1, const uint8_t d, const char c2);
/// Wrapper around genPass with args in first 3 elements
void genPass3(char * result);



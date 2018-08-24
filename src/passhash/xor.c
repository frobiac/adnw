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


// gh.com/tomjennings/SR_Resources  SRPNG.h
// -> different shift for xor32
//
#include "xor.h"
#include "../helpers.h"   // mapAscii()
#include "tabularecta.h"   // mapAscii()

#if defined(__AVR__)
    #include "../print.h"
#else
    #include <stdio.h>
    #include <string.h>
    #include <stdint.h>
    #include <stdlib.h>
    #define xprintf printf
#endif


/// The seed to init everything with on each run, to set externally.
static xor_size_t g_xor_seed;

/// Must be initialized with seed before each run, and will hold result of xorshifts.
static xor_size_t g_xor_result;

/// internal functions
//void xorshift(void);

// re-init generator with seed
void xor_reset(void);

xor_size_t xor_result(void) { return g_xor_result; };

#if (XOR_BITS == 8)
// returns values from 1 to 255 inclusive, period is 255
void xorshift(void)
{
    g_xor_result ^= (g_xor_result << 7);
    g_xor_result ^= (g_xor_result >> 5);
    g_xor_result ^= (g_xor_result << 3);
}

#elif (XOR_BITS == 16)
// returns values from 1 to 65535 inclusive, period is 65535
void xorshift(void)
{
    g_xor_result ^= (g_xor_result << 13);
    g_xor_result ^= (g_xor_result >> 9);
    g_xor_result ^= (g_xor_result << 7);
}

#elif (XOR_BITS == 32)
// The state word must be initialized to non-zero
void xorshift(void)
{
    g_xor_result ^= g_xor_result << 13;
    g_xor_result ^= g_xor_result >> 17;
    g_xor_result ^= g_xor_result << 5; //@TODO 15 here?
}
#elif (XOR_BITS == 64)
// static xor_size_t s[2]; // s[0] -> g_xor_result, s[1] -> y1_xor
static xor_size_t s[2];
void xorshift(void)  // xorshift128plus
{
    xor_size_t x = s[0];
    xor_size_t const y = s[1];
    s[0] = y;
    x ^= x << 23; // a
    s[1] = x ^ y ^ (x >> 17) ^ (y >> 26); // b, c
    g_xor_result = s[1] + y;
}
#endif

/**
 * Set seed value of generator.
 */
void set_xor_seed(xor_size_t state)
{
    // Don't allow all zero!
    if (state==0)
        state=1;

    g_xor_seed = state;
    xor_reset();
}

/**
 * Reset generator to stored g_xor_seed value
 */
void xor_reset()
{
    g_xor_result = g_xor_seed;
#if (XOR_BITS==64)
    /// @TODO fixme: 64bit needs two init states.
    s[0] = g_xor_seed;
    s[1] = g_xor_seed+1864562;
#endif
}

/**
 * Seed from string:
 * Add character to seed, then run xorshift() and use result as new seed.
 */
void xor_init(char * seed, uint8_t len)
{
    g_xor_result = 0;
    for (uint8_t i = 0; i<len; ++i) {
        g_xor_result += seed[i];
        xorshift();
    }
    set_xor_seed(g_xor_result);
}

// random avr:
// https://gist.github.com/endolith/2568571
// avr-hardware-random-number-generation
// http://electronics.stackexchange.com/questions/3653/avr-random-number-generator/206605/
// https://sites.google.com/site/astudyofentropy/project-definition/timer-jitter-entropy-sources/entropy-library/arduino-random-seed


/**
 * Convert each Byte of number to a printable character.
 * Using only part of the [0-255] range for mapping also makes the output of the generator
 * less predictable:
 *
 * 32bit generator has 2^32 outputs of 4 Bytes, each occuring only once.
 * Using only 6 out of each Byte there are (2^6)^4 combinations, each 256 times.
 * A single observed number is thus not leaking the internal state.
 *
 * This is all highly theoretical, as the xorshift-generator is used to create a secret
 * TabulaRecta that still depends on other factors like row/col/len and should be used
 * only as part of a password.
 *
 * Similar to base64-encode, but _loosing_ information on purpose.
 *
 */
void number2str(char dst[XOR_BITS/8], xor_size_t number)
{
    /*
        // use upper 6bit of each Byte
        number = number >> 2;
        // order is reversed, but does not matter
        for(uint8_t i=0; i<XOR_BITS/8; ++i) {
            *dst++ = b64map[number&0x3F];
            number = number >> 8;
        }
        */
}

/**
 * Read string[len] at 0-based row x col into *dst
 * @TODO wrap around? last row / end undefined.
 *
 */
void tr_code(char *dst, uint8_t len, uint8_t row, uint8_t col)
{
    xor_reset();

    for(uint16_t i=0; i<(row*TR_COLS +col); ++i)
        xorshift();


    for(uint8_t i=0; i<len; ++i) {
        xorshift();
        dst[i] = b64map[g_xor_result&0x3F];
    }
    // printf("\n%2d @ %2dx%2d: %s", len, row, col, dst);
}


#ifndef __AVR__

int main(int argc, char ** argv)
{
    char teststr[30];

    if(argc>1) {
        xor_init(argv[1], strlen(argv[1]));
        printf("\nDONE:h(%s)=%08lX -> %08lX \n%s", argv[1], g_xor_result, g_xor_seed, FW_VERSION);

        uint32_t sum=0;
        for(int i=0; i<strlen(argv[1]); ++i) {
            sum *= 95;
            sum += argv[1][i] - 31;
            printf("\n%d sum = %08lX", i, sum);
        }


    } else {
        printf("\nMust provide initial seed as arg.");
        return 5;
    }

    xor_reset();
    // settle for 24 chars (XYZ together)
    for(uint8_t r=0; r<TR_ROWS; ++r) {
        tr_code(&teststr[0], TR_COLS, r,0);
        teststr[TR_COLS]='\0';
        printf("\n%2d: %s", r, teststr);
        /*
                //for(uint8_t c=0; c*(XOR_BITS/8)<TR_COLS; ++c) {
                for(uint8_t c=0; c<TR_COLS; ++c) {
                    xorshift();
                    //number2str(res_str, g_xor_result);
                    printf("%c", b64map[g_xor_result&0x3f] ); //b64map[g_xor_result&0x3F];
                }
                */
    }

    tr_code(&teststr[0], 4,  0,0);
    tr_code(&teststr[0], 4,  0,4);
    tr_code(&teststr[0], 4, 12,0);
    tr_code(&teststr[0], 12,12,0);
    tr_code(&teststr[0], 11,12,1);
    tr_code(&teststr[0], 10,12,2);
    tr_code(&teststr[0],  9,12,3);
    tr_code(&teststr[0],  9,11,21);
}
#endif // not __AVR__

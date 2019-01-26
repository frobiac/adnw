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
#include "tabularecta.h"

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

void xor_reset(void);                   // re-init generator with seed
void set_xor_seed(xor_size_t state);    // store state as seed
void xor_advance(char * str);           // for each character in str, add to state and advance once to randomize state.

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
        state=XOR_RND_INIT; // different on each compile

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
void xor_init(char * str, uint8_t len)
{
    g_xor_result = 0;
    xor_advance(str);
    set_xor_seed(g_xor_result);
}

/**
 *  Add each character to current state and advance generator
 *  Used to geneate random output
 */
void xor_advance(char * str)
{
    while(*str != '\0') {
        g_xor_result += *str;
        str++;
        xorshift();
    }
}

// random avr:
// https://gist.github.com/endolith/2568571
// avr-hardware-random-number-generation
// http://electronics.stackexchange.com/questions/3653/avr-random-number-generator/206605/
// https://sites.google.com/site/astudyofentropy/project-definition/timer-jitter-entropy-sources/entropy-library/arduino-random-seed

/**
 * Read string[len] at 0-based row x col into *dst
 * @TODO wrap around? last row / end undefined.
 *
 * if *dst[0] != 0 use content of dst as extra offset similar to init.
 *
 */
void tr_code(char *dst, uint8_t len, uint8_t row, uint8_t col)
{
    xor_reset();

    // advance generator
    if( dst[0] == '\0' ) {
        for(uint16_t i=0; i<(row*TR_COLS +col); ++i)
            xorshift();
    } else {
        xor_advance(dst);
    }

    for(uint8_t i=0; i<len; ++i) {
        xorshift();
        dst[i] = b64(g_xor_result&0x3F);
    }
}


#ifndef __AVR__
void xor_test(const char * str)
{
    char teststr[30];
    xor_reset();
    sprintf(teststr, str);
    printf("\nNEW : %s", teststr);
    tr_code(teststr, 8, 0, 0);
    teststr[8]='\0';
    printf(" = %s", teststr);

}

int main(int argc, char ** argv)
{
    char teststr[30];

    if(argc>1) {
        xor_init(argv[1], strlen(argv[1]));
        printf("\nseed(%s) = %08lX  FW=%s", argv[1], g_xor_seed, FW_VERSION);
    } else {
        printf("\nMust provide initial seed as arg.");
        return 5;
    }

    xor_reset();
    for(uint8_t r=0; r<TR_ROWS; ++r) {
        teststr[0]='\0'; // to force tabula recta mode
        tr_code(&teststr[0], TR_COLS, r,0);
        teststr[TR_COLS]='\0';
        printf("\n%2d: %s", r, teststr);
    }

    xor_test("test");
    xor_test("abc");
    xor_test("ttt");
    xor_test("tttt");

}
#endif // not __AVR__


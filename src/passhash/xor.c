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
/// Must be initialized with seed before each invocation and will hold result of xorshifts.
static xor_size_t g_xor_result;

/// internal functions
void gen4char(char * dst, xor_size_t rnd);
void xorshift(void);

// re-init generator with seed
void xor_reset(void);


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


void set_xor_seed(xor_size_t state)
{
    g_xor_seed = state;
    xor_reset();
}

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
 * Generates one random character per input byte.
 */
void gen4char(char * dst, xor_size_t rnd)
{
    for(int s=0; s < XOR_BITS/8; ++s) {
        *dst++ = mapAscii(rnd&0xFF);
        rnd = rnd >> 8;
    }
    *dst='\0';
}


/**
 *  generate a random line of rnd[LEN] hex[LEN] and pick cnt characters at pos in each one.
 *  Keep LEN>pos+cnt to avoid looping, otherwise pos is set to pos % LEN
 *
 *  Call with cnt=0 to skip output but advance xorshift generator..
 *  @param result : char array of [cnt+1] that will hold result, NOT zero-terminated!.
 *  @param LEN : length in characters of random and hex strings to create
 *  @param pos : zero-based position to start picking characters at in each of both substrings
 *  @param cnt : number of characters to pick. Will loop around if requrired.
 */
void genLine(char *result, const uint8_t cnt, const uint8_t pos,const uint8_t LEN, const bool hexNotRnd)
{
    uint8_t idx;
    xor_size_t r = 0; // result of xorshift that will be shifted out to obtain characters

    for(int i=0; i<LEN; ++i) {
        if(0==i%XOR_BYTES) {
            xorshift();
            r = g_xor_result;
        }

        if(cnt!=0) {
            idx=(LEN+i-pos)%LEN;  // wrap around if necessary (pos+cnt)>=LEN
            if(idx < cnt) {
                // for hex output, we waste half the input data to keep same loop structure here
                result[idx] = mapAscii(r&(hexNotRnd?0x0F:0xFF));
            }
            r>>=8;
        }
    }
}


void genPass3(char * result)
{
    genPass(result, (26+result[0]-'a') % 26 + 'a',
            (10+result[1]-'0') % 10,
            (26+result[2]-'a') % 26 + 'a');
}

void genPass(char * result, const char c1, const uint8_t d, const char c2)
{
    const uint8_t len_line = 14;
    const uint8_t len_code =  4;
    const uint8_t add_args =  5;
//    char result[2*len_code+1+add_args];

    set_xor_seed(g_xor_seed);
    // could early-exit if both characters were found, but not really worth it.
    for(char c='a'; c<='z'; ++c) {
        if(c==c1)
            genLine(result, len_code,d,len_line,0);
        else
            genLine(result, 0,0,len_line,0);
        if(c==c2)
            genLine(&result[len_code+add_args], len_code,d,len_line,1);
        else
            genLine(&result[len_code+add_args], 0,0,len_line,1);
    }

    if(add_args==5) { // add string to center
        result[len_code+0] = '.';
        result[len_code+1] = c1;
        result[len_code+2] = '0'+d%10;
        result[len_code+3] = c2;
        result[len_code+4] = '-';
    }
    result[2*len_code+add_args]='\0';

    // xprintf("%c%d%c: %s", c1, d, c2, result);
}



// random avr:
// https://gist.github.com/endolith/2568571
// avr-hardware-random-number-generation
// http://electronics.stackexchange.com/questions/3653/avr-random-number-generator/206605/
// https://sites.google.com/site/astudyofentropy/project-definition/timer-jitter-entropy-sources/entropy-library/arduino-random-seed

#ifndef __AVR__

int main(int argc, char ** argv)
{
    xor_size_t hash=1;
    xor_size_t xor_res;

    if(argc>1) {
        set_xor_seed(str2hash(argv[1]));
        printf("\n%s -> %08lX \n%s", argv[1], g_xor_seed, FW_VERSION);
    } else {
        printf("\nMust provide initial seed as arg.");
        return 5;
    }

    xor_reset();

    char str[2*4+5+1];

    const uint8_t len=14;

    genPass(str,'a', 0, 'a'); printf("\n%s ", str);
    genPass(str,'a', len-4, 'a'); printf("\n%s ", str);
    genPass(str,'b', 0, 'b'); printf("\n%s ", str);
    genPass(str,'b', len-4, 'b'); printf("\n%s ", str);

    xor_reset();

    printf("\n   0123456789---- | 0123456789----");
    for(char c='a'; c<='z'; ++c) {
        char result[len+1];
        result[len]='\0';
        printf("\n%c: ",c);
        genLine(result,  len, 0,len, 0);
        printf("%s | ", result);
        genLine(result,  len, 0,len, 1);
        printf("%s", result);
    }
    printf("\n---------------------\n");

    genPass(str,'z', 0, 'z'); printf("\n%s ", str);
    genPass(str,'z', len-4, 'z'); printf("\n%s ", str);
    genPass(str,'y', 0, 'y'); printf("\n%s ", str);
    genPass(str,'y', len-4, 'y'); printf("\n%s ", str);

    str[0]='y';
    str[1]='8';
    str[2]='y';
    genPass3(str); printf("\n%s <- GEN3", str);


    printf("\n\n");
}
#endif // not __AVR__


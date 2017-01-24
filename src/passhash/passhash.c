/*
    This file is part of the AdNW keyboard firmware.

    Copyright 2010-2016 Stefan Fröbe, <frobiac /at/ gmail [d0t] com>

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


    Base64 encoding based on code from base64.sf.net/b64.c
    Copyright (c) 2001 Bob Trower, Trantor Standard Systems Inc.
    MIT license

    Password hash generation is implemented after the algorithm from the Android apps
    Twik and HashIt!, which in turn are based on the chrome extension Password Hasher Plus.
    See http://github.com/coxande/Twik for a python implementation.

    It uses hmac-sha1 and sha1 implementations from avr-crypto-lib licensed under GPL3:
    https://github.com/cantora/avr-crypto-lib
*/

#include "sha1.h"
#include "hmac-sha1.h"
#include "passhash.h"
#include <string.h>
#include <ctype.h>     // isdigit()


static const char b64[]="ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

/// encode 3 8-bit binary bytes as 4 '6-bit' characters
void encodeblock( unsigned char *in, unsigned char *out, int len )
{
    out[0] = (unsigned char) b64[ (int)(in[0] >> 2) ];
    out[1] = (unsigned char) b64[ (int)(((in[0] & 0x03) << 4) | ((in[1] & 0xf0) >> 4)) ];
    out[2] = (unsigned char) (len > 1 ? b64[ (int)(((in[1] & 0x0f) << 2) | (((len>2?in[2]:0) & 0xc0) >> 6)) ] : '=');
    out[3] = (unsigned char) (len > 2 ? b64[ (int)(in[2] & 0x3f) ] : '=');
}

// minimum result length : i*4/3 + (i%3>0?1:0)
void b64enc(const unsigned char* data, size_t len, char* result, size_t resultSize)
{
    size_t x;
    for(x=0; x*3<len; ++x)
        encodeblock( (unsigned char *)(&data[x*3]), (unsigned char *)(&result[x*4]), (len-x*3));
}

void injectChar( char * mhash, uint8_t offset, uint8_t reserved,
                 uint32_t seed, uint8_t length, char cstart, uint8_t cnum)
{
    uint8_t pos0 = seed % length;
    uint8_t pos = (pos0+offset)%length;
    uint8_t i, tmp;

    uint8_t cstartI = (uint8_t)(cstart);

    // check if desired character is already present (in special range)
    for(i=0; i<length-reserved; ++i) {
        tmp= (pos0+reserved+i)%length;
        uint8_t ci = (int)(mhash[tmp]);
        if( ci >= cstartI && ci < cstartI + cnum) {
            return;
        }
    }

    uint8_t injectC = (((seed+(int)(mhash[pos]))%cnum)+cstartI);
    mhash[pos] = (char)(injectC);
}


void remPunct( char * mhash, uint16_t seed, uint8_t length)
{
    uint8_t pivot=0;
    uint8_t i;

    for(i=0; i<strlen(mhash); ++i) {
        if( ! isalnum(mhash[i]) ) {
            mhash[i] = (char)( (seed+pivot)%26 + 65 /*'A'*/);
            pivot=i+1;
        }
    }
}

void conv2digits( char * mhash, uint16_t seed, uint8_t length)
{
    uint8_t pivot=0;
    uint8_t i;

    for(i=0; i<length; ++i) {
        if( ! isdigit(mhash[i]) ) {
            mhash[i] = (char)( (seed+(int)(mhash[pivot]))%10 + 48 /*'0'*/);
            pivot=i+1;
        }
    }
}


uint16_t letterSum(const char * str)
{
    uint8_t i;
    uint16_t sum=0;
    for(i=0; i<strlen(str); i++) {
        sum+=(int)(str[i]);
    }
    return sum;
}

void genHash(char * result, char * key, char * tag, uint8_t len, uint8_t type)
{
    char dest[HMAC_SHA1_BYTES];
    char b64result[30]; // 22 Byte hash -> 7*4+2
    uint16_t seed = 0;  // max is 27*122

    memset(b64result, 0, 30);
    memset(dest, 0, HMAC_SHA1_BYTES);

    hmac_sha1((void*)(&dest), tag, 8*strlen(tag), key, 8*strlen(key));

    b64enc((unsigned char*)dest, HMAC_SHA1_BYTES, b64result, sizeof(b64result));
    // remove last 2 characters ('=' padding and end)
    b64result[27] ='\0';

    seed=letterSum(b64result);

    if(type==3) { // numeric
        conv2digits(b64result,seed,len);
    } else {
        injectChar( b64result, 0, 4, seed, len, '0', 10); // digit
        if(type==1) // digit + punctuation
            injectChar( b64result, 1, 4, seed, len, '!', 15); // punctuation
        injectChar( b64result, 2, 4, seed, len, 'A', 26); // uppercase
        injectChar( b64result, 3, 4, seed, len, 'a', 26); // lowercase
        if(type==2) // alnum only
            remPunct( b64result, seed, len);
    }

    uint8_t i;
    for(i=0; i<len; ++i)
        result[i]=b64result[i];
    result[len] = '\0';
}
/**
 *  password array must be one longer then requested length!
 */
uint8_t passHash(char * password, uint8_t len, uint8_t type, char * secret, char * key, char * tag)
{
    if(len<4 || len>26) {
        return(3);
    }

    if(type<1 || type>3) {
        return(4);
    }

    char mhash[24+1];

    /* Naming conventions form twik / passhash
      - master_key  is read in Gui (or run.py)   (=key   )
      - private_key is from config               (=secret)
      - tag         is domain name
     */
    genHash(mhash,    secret, tag, 24,  1);
    genHash(password, mhash,  key, len, type);

    return 0;
}



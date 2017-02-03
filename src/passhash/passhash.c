/*
    This file is part of the AdNW keyboard firmware.

    Copyright 2010-2017 Stefan Fröbe, <frobiac /at/ gmail [d0t] com>

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

//#include "sha1.h"
#include "hmac-sha1.h"
#include "passhash.h"
#include <string.h>
#include "../helpers.h"


static const char b64[]="ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

/// encode 3 8-bit binary bytes as 4 '6-bit' characters
void encodeblock( unsigned char *in, unsigned char *out, int len )
{
    out[0] = (unsigned char) b64[ (int)(in[0] >> 2) ];
    out[1] = (unsigned char) b64[ (int)(((in[0] & 0x03) << 4) | ((in[1] & 0xf0) >> 4)) ];
    out[2] = (unsigned char) (len > 1 ? b64[ (int)(((in[1] & 0x0f) << 2) | (((len>2?in[2]:0) & 0xc0) >> 6)) ] : '=');
    out[3] = (unsigned char) (len > 2 ? b64[ (int)(in[2] & 0x3f) ] : '=');
}

//
/** Encode data of size len to base64 result
 *  Result must be at least len*4.0/3 (rounded up!)
 *
 *  A SHA1 hash of 20 Bytes will thus occupy 27 Bytes (20*4/3=26.666)
 */
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

/**
 * key must have room for 27 chars (20Byte SHA1_HASH to base64)
 */
void genHash(char * key, char * tag, uint8_t len, uint8_t type)
{
    //fprintf(stderr, "\nKey: %s Tag: %s", key, tag);
    char sha1hash[HMAC_SHA1_BYTES];
    uint16_t seed = 0;  // max is 27*122

    memset(sha1hash, 0, HMAC_SHA1_BYTES);

    hmac_sha1((void*)(&sha1hash), tag, 8*strlen(tag), key, 8*strlen(key));

    b64enc((unsigned char*)sha1hash, HMAC_SHA1_BYTES, key, sizeof(key));

    // b64 of 20 Byte sha1hash -> 27 Bytes ending in '=' which must be removed
    key[27] ='\0';

    seed=letterSum(key);

    if(type==3) { // numeric
        conv2digits(key,seed,len);
    } else {
        injectChar( key, 0, 4, seed, len, '0', 10); // digit
        if(type==1) // digit + punctuation
            injectChar( key, 1, 4, seed, len, '!', 15); // punctuation
        injectChar( key, 2, 4, seed, len, 'A', 26); // uppercase
        injectChar( key, 3, 4, seed, len, 'a', 26); // lowercase
        if(type==2) // alnum only
            remPunct( key, seed, len);
    }

    // truncate result to requested length
    key[len] = '\0';
}
/**
 *  key array must be 27 chars and will contain '\0' terminated passhash upon completion
 *
 *  @ret key
 */
uint8_t passHash(uint8_t len, uint8_t type, char * private_key, char * master_pw, char * tag)
{
    if(len<4 || len>26) {
        return(3);
    }

    if(type<1 || type>3) {
        return(4);
    }

    /* Naming conventions from twik / passhash
      - master_key  is read in Gui (or run.py)   (=key   )
      - private_key is from config               (=secret)
      - tag         is domain name
     */
    genHash(private_key, tag, 24,  1);
    // now key contains first run result
    genHash(private_key, master_pw, len, type);
    // now key contains final passhash

    return 0;
}

bool verifyHash(char * private_key, char * master_pw, char * tag, uint8_t len, uint8_t type, char * expected_hash )
{
    passHash( len, type, private_key, master_pw, tag);
    if( memcmp( expected_hash, private_key, len) == 0)
        return true;

    return false;
}



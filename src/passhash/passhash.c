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

#include <string.h>

#include "hmac-sha1.h"
#include "passhash.h"
#include "../helpers.h"

#ifdef __AVR__
#include "../_private_data.h"   // don't include for testing on x86 where private key is defined separately
#include "../macro.h"           // for direct output via setOutputString()
#else
// these are only required for external testing with different private keys.
#include <stdio.h>

char * testing_pk;
void ph_setPrivateKey(char * k) { testing_pk = k; }
void setOutputString(char * str) { printf("%s", str); }
#endif

//################################################################################
// Parsing additions
//################################################################################
// longest possible tag is input minus length, mode and two spaces
#define PH_TAGLEN PH_INPUT_LEN - 5 // tag_XY_M

// master password: filled during runtime and never stored persistently.
static char ph_master_pw[PH_PW_LEN+1];

// buffer for input and passhash generation
char ph_input[PH_INPUT_LEN];

uint8_t type, len, read_field;
//################################################################################

char b64( uint8_t i )
{
    //static const char b64[]="ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
    if(i<26)
        return 'A'+i;
    if(i<52)
        return 'a'+ i-26;
    if(i<62)
        return '0'+ i-52;
    if(i==62)
        return '+';
    if(i==63)
        return '/';

    // should never get here!!!
    return '-';
}

/// encode 3 8-bit binary bytes as 4 '6-bit' characters
void encodeblock( unsigned char *in, unsigned char *out, int len )
{
    out[0] = (unsigned char) b64( (int)(in[0] >> 2) );
    out[1] = (unsigned char) b64( (int)(((in[0] & 0x03) << 4) | ((in[1] & 0xf0) >> 4)) );
    out[2] = (unsigned char) (len > 1 ? b64( (int)(((in[1] & 0x0f) << 2) | (((len>2?in[2]:0) & 0xc0) >> 6)) ) : '=');
    out[3] = (unsigned char) (len > 2 ? b64( (int)(in[2] & 0x3f) ) : '=');
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

    for(i=0; i<length; ++i) {
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


uint16_t letterSum(const char * str, uint8_t len)
{
    uint8_t i;
    uint16_t sum=0;
    for(i=0; i<len; i++) {
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

    seed=letterSum(key, 27);

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

void fillPrivateKey(char * pk)
{
#ifdef __AVR__
    memcpy(pk, PH_PRIVATE_KEY, PH_INPUT_LEN);
#else
    if(testing_pk)
        strncpy(pk, testing_pk, PH_INPUT_LEN);
#endif
}

/**
 *  ph_result must have size for 27 chars.
 *  Input tag there
 *  Will contain '\0' terminated passhash upon completion
 *
 *  @ret 0
 */
uint8_t passHash(uint8_t len, uint8_t type)
{
    if(len<4 || len>26)
        return(3);

    if(type<1 || type>3)
        return(4);

    char tag[PH_TAGLEN+1];
    memcpy(tag, ph_input, PH_TAGLEN);

    fillPrivateKey(ph_input);
    /* Naming conventions from twik / passhash
      - master_key  is read in Gui (or run.py)   (=key   )
      - ph_result is from config               (=secret)
      - tag         is domain name
     */
    // ph_result = real private key defined
    genHash(ph_input, tag, 24,  1);
    // ph_result = first runs hmac_sha1
    genHash(ph_input, ph_master_pw, len, type);
    // ph_result = pk = final passhash

    return 0;
}



/**
 * Parser called from command mode
 */
uint8_t ph_parse(char c)
{
    // on first call enter password, or press return to clear previously entered one
    // on subsequent runs enter tag [len [type]]
    //
    // read until return=10 is pressed or maximum length reached
    if( (uint8_t)(c) != 10 && strlen(ph_input) < PH_INPUT_LEN) {
        if ( c == ' ' ) { // space -> advance to next field
            ++read_field;
            return PH_READING;
        }
        if (read_field == 0) {      // still reading tag
            ph_input[strlen(ph_input)]= c;
            return PH_READING;
        }

        if( c<='9' && c >= '0') {   // digits for length or type
            if(read_field==1)
                len=len*10+c-'0';
            else if(read_field==2)
                type=c-'0';
        }
        return PH_READING;                     // character was handled

    } else { // Done reading any input:

        // only return was pressed -> clear master password and return
        if(strlen(ph_input) == 0) {
            memset(ph_master_pw,0,PH_PW_LEN);
            return PH_PW_CLEARED;
        }

        // entered string is initial entry of master password
        if( ph_master_pw[0] == 0 ) {
            memcpy(ph_master_pw, ph_input, strlen(ph_input));
            ph_reset();
            return PH_PW_ENTERED;
        }
        // getting here means password had been set, all data was entered, so passhash is ready
        {
            // xprintf("\nPW:%s: T:%s %u/%u", ph_master_pw, ph_input, len, type);
            //memset(ph_input,0,PH_INPUT_LEN);

            // defaults if invalid input
            if(len<PH_MIN_LEN || len>PH_MAX_LEN)
                len=12;
            if(type<PH_TYPE_ALNUMSYM || type>PH_TYPE_NUM)
                type=PH_TYPE_ALNUMSYM;

            // reuse ph_input buffer in passhash calculation
            passHash( (uint8_t)len, (uint8_t)type);
            setOutputString( ph_input );
            ph_reset();
            return PH_DONE;
        }
    }
}

/**
 *  Reset all fields after successful parsing.
 */
void ph_reset()
{
    memset(ph_input,0,PH_INPUT_LEN);
    read_field=0; len=0; type=0;
}


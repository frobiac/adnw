/*
    This file is part of the AdNW keyboard firmware.

    Copyright 2010-2018 Stefan Fröbe, <frobiac /at/ gmail [d0t] com>

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


    Password hash generation is implemented after the algorithm from the Android apps
    Twik and HashIt!, which in turn are based on the chrome extension Password Hasher Plus.
    See http://github.com/coxande/Twik for a python implementation.

    It uses hmac-sha1 and sha1 implementations from avr-crypto-lib licensed under GPL3:
    https://github.com/cantora/avr-crypto-lib

    @todo: ???
    https://github.com/Cathedrow/Cryptosuite
*/

/**
 * Naming conventions from twik / passhash
 * - master_key  is read in Gui (or run.py)   (=key   )
 * - ph_result   is from config               (=secret)
 * - tag         is domain name
 */

#include <string.h>

#include "hmac-sha1.h"
#include "passhash.h"
#include "../helpers.h"

#ifdef __AVR__
#include "../_private_data.h"   // don't include for testing on x86 where private key is defined separately
#include "../macro.h"           // for direct output via setOutputString()
#include "../global_config.h"   // g_pw
#else

// these are only required for external testing with different private keys.
#include <stdio.h>
char * PH_PRIVATE_KEY;
char g_pw[PWLEN+2];
void ph_setMasterPW(char * pk) { memset(g_pw, 0, PWLEN+2); snprintf(g_pw, PWLEN+1, "%s", pk); g_pw[PWLEN+1] = strlen(pk); }
void ph_setPrivateKey(char * pk) { PH_PRIVATE_KEY = pk; }
void setOutputString(char * str) { printf("%s", str); }
#endif

void genHash2(char * key, uint8_t key_len,  char * tag, uint8_t tag_len, uint8_t len, uint8_t type);

//################################################################################
// Parsing additions
//################################################################################
// longest possible tag is input minus length, mode and two spaces
#define PH_TAGLEN PH_INPUT_LEN - 5 // tag_XY_M

// buffer for input and passhash generation
char ph_input[PH_INPUT_LEN];

uint8_t type, len, read_field;
uint8_t ph_tag_len;
uint8_t ph_pk_len;
//################################################################################

#include "b64.h"
char sha1hash[HMAC_SHA1_BYTES]; // save Progmem vs Data compared to local definition


/**
 * key must have room for 27 chars (20Byte SHA1_HASH to base64)
 */
void genHash(char * key, char * tag, uint8_t len, uint8_t type)
{
    genHash2(key, strlen(key), tag, strlen(tag), len, type);
}

void genHash2(char * key, uint8_t key_len,  char * tag, uint8_t tag_len, uint8_t len, uint8_t type)
{
    memset(sha1hash, 0, HMAC_SHA1_BYTES);

    hmac_sha1((void*)(&sha1hash), tag, 8*tag_len, key, 8*key_len);

    b64enc((unsigned char*)sha1hash, HMAC_SHA1_BYTES, key, sizeof(key));

    // b64 of 20 Byte sha1hash -> 27 Bytes ending in '=' which must be removed
    key[27] ='\0';

    // truncate result to requested length
    key[len] = '\0';
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

    // only allocating dynamic ph_tag_len size here leads to larger code...
    // char tag[PH_TAGLEN+1]; memcpy(tag, ph_input, PH_TAGLEN);
    char tag[ph_tag_len]; memcpy(tag, ph_input, ph_tag_len);

    // load private key for first round
    memcpy(ph_input, PH_PRIVATE_KEY, PH_INPUT_LEN);
    ph_pk_len=strlen(ph_input);

    genHash2(ph_input, ph_pk_len, tag,          ph_tag_len,            24,  1);
    genHash2(ph_input, 24,  (char*) g_pw, (uint8_t) g_pw[PWLEN+1],    len, type);

    return 0;
}



/**
 * Parser called from command mode
 */

void ph_parse(char c)
{
    // Enter tag [len [type]]
    // len and type are set to default values if invalid so no need to check
    //
    // @TODO could implement early exits and more checks
    //
    // read until return=10 is pressed or maximum length reached
    if( (uint8_t)(c) != 10 && ph_tag_len < PH_INPUT_LEN) {
        if ( c == ' ' ) {           // space -> advance to next field
            ++read_field;
            if(read_field>2)  // too many spaces
                goto exit;
            return;
        }
        if (read_field == 0) {      // still reading tag
            ph_input[ph_tag_len]= c;
            ++ph_tag_len;
            return;
        } else if(read_field==1) {
            len=len*10+c-'0';
            return;
        } else if(read_field==2) {
            type=c-'0';
        }
    }

    if(ph_tag_len==0) // no tag
        goto exit;

    // getting here means password had been set, all data was entered, so passhash is ready
    if(len<PH_MIN_LEN || len>PH_MAX_LEN)
        len=12;
    if(type<PH_TYPE_ALNUMSYM || type>PH_TYPE_NUM)
        type=PH_TYPE_ALNUMSYM;

    // reuse ph_input buffer in passhash calculation
    passHash( (uint8_t)len, (uint8_t)type);
    setOutputString( ph_input );

exit:
    ph_reset();
#ifdef __AVR__
    setCommandMode(false);
#endif
}

/**
 *  Reset all fields after successful parsing.
 */
void ph_reset()
{
    ph_tag_len=0;
    memset(ph_input,0,PH_INPUT_LEN);
    read_field=0; len=0; type=0;
}

#ifndef __AVR__

uint8_t passHashArgs(char * priv_key, char * master_pw, char * tag, uint8_t len, uint8_t type)
{
    ph_reset();
    ph_setPrivateKey(priv_key);
    ph_setMasterPW(master_pw);

    char input[80];
    snprintf(input, 80, "%s %d %d\n", tag, len, type);

    for(uint8_t i=0; i<strlen(input); ++i) {
        ph_parse(input[i]);
    }
}
#endif
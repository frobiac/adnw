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
*/
#pragma once

#ifndef uint8_t
    #include <stdint.h>
#endif
#include <stdbool.h>

#define PH_PW_LEN       16
#define PH_INPUT_LEN PH_MAX_LEN + 1 // 27

#define PH_MIN_LEN       4
#define PH_MAX_LEN      26

#define PH_TYPE_NUM      3
#define PH_TYPE_ALNUM    2
#define PH_TYPE_ALNUMSYM 1

enum PH_STATES { PH_DONE, PH_READING, PH_PW_ENTERED, PH_PW_CLEARED, PH_PW_ERROR, PH_FAIL, PH_UNUSED_LAST };

/**
 *  Calculate passhash from given values.
 *
 *  ph_result must fit 27 chars for final result and is used for tag.
 *
 *  it will contain calculated hash, '\0'-delimited at requested length.
 *
 *  @ret 0
 */
uint8_t passHash(char * ph_result, char * master_pw, uint8_t len, uint8_t type);

/**
 *  Compare expected_hash to calculated result.
 */
bool verifyHash(char * master_pw, char * tag, uint8_t len, uint8_t type, char * expected_hash);


/**
 * Parser additions
 */
/** Upon very first run, everything up to return is read as master password
 *  Subsequent runs are interpreted as tag len type
 *  Immediate entry of return clears master password
 */
uint8_t ph_parse(char c);
void    ph_reset(void);

#ifndef __AVR__
    // allows to set otherwise hardcoded private key for tests
    void ph_setPrivateKey(char * privateKey);
#endif

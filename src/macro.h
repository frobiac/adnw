/*
    Copyright 2010-2012 Stefan Fröbe, <frobiac /at/ gmail [d0t] com>

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

#ifndef MACRO_H
#define MACRO_H 1

/** Macro mode
 *  Set g_macro_mode to 1 and activate wanted macro.
 *  Then getMacroChars while possible and assemble a report.
 */

#include <stdint.h>
#include <stdbool.h>
#include <avr/eeprom.h>

#include "hhstdio.h"


#include "keymap.h"


#ifdef _private_macros_h_NOT_FOUND
/**
 * _private_macros.h
 * Put private macro declaration in this header file and define LEN and COUNT:
 * - use ascii characters/strings, or the corresponding codes from ascii_table directly
 * - any modifier(s) for the following character should be ORed with 0x80,
 *   value is 1 for CTRL, 2 for ALT, 4 SHIFT 8 GUI and +16 for right hand modifiers
 */

 */
/// Increasing either too much leads to keyboard not registering at all!
#define MACROCOUNT   3
#define MACROLEN    20

static const char EEmacrosC[MACROCOUNT][MACROLEN]  =  {
    "SampleString!",
    // CTRL+ALT+DEL
    { ((1<<0)|(1<<2)/*L_CTL|L_ALT*/)+0x80, 127, 'p','w','\n' },
    // ^c ^t ^v return for firefox
    { 0x81, c , 0x81, t, 0x81, v, '\0' },
};
#endif

#include "_private_macros.h"

/**
 *  Macros are stored in eeprom.
 *  Content has to be put into a separate file to keep sensitive data
 *  out of normal sources :)
 *  Addresses are therefore fixed here.
*/
#define EEPROM_DEF 0xFF

#define EE_ADDR_START       100
#define EE_ADDR_MACROS      (EE_ADDR_START+100)
#define EE_ADDR_MACRO(idx)  (EE_ADDR_MACROS + (idx*MACROLEN))

uint8_t writeMacro(const char    macro[MACROLEN], uint8_t idx);
uint8_t readMacro (uint8_t macro[MACROLEN], uint8_t idx);

bool initMacros(void);
void printMacros(void);

bool macroMode(void);
void setMacroMode(bool on);
void endMacro(void);

bool activateMacro(uint8_t id);
bool getMacroChar(struct keycode *kc);
bool getMacroCharacter(char *c);
bool getMacroReport(USB_KeyboardReport_Data_t *report);


#endif

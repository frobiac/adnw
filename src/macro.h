/*
    Copyright 2010-2013 Stefan Fröbe, <frobiac /at/ gmail [d0t] com>

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

#include <LUFA/Drivers/USB/Class/Device/HIDClassDevice.h>
#include "keymap.h"

#include "_private_macros.h"


#define MACRO_PAUSE 0x80

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

uint8_t updateEEMacro(const char    macro[MACROLEN], uint8_t idx);
uint8_t readEEMacro (uint8_t macro[MACROLEN], uint8_t idx);

bool initMacros(void);
void printMacros(void);

void endMacro(void);

bool macroMode(void);
void setMacroMode(bool on);

bool activateMacro(uint8_t id);
bool getMacroChar(keycode *kc);
bool getMacroCharacter(char *c);
bool getMacroReport(USB_KeyboardReport_Data_t *report);

bool macroRecording(void);
void setMacroRecording( bool on );
void macro_key(uint8_t mod, uint8_t hid);

char hid2asciicode(uint8_t hid, uint8_t mod);

#endif

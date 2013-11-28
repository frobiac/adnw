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

#include <stdint.h>
#include <stdbool.h>
#include <avr/eeprom.h>

#include <LUFA/Drivers/USB/Class/Device/HIDClassDevice.h>
#include "keymap.h"


/**
 *  Macros are stored in eeprom. To avoid excessive rewrites, a fixed maximum length is set.
 *  Addresses are fixed.
 *  .
 *  @todo Enable pre-loading under certain conditions via *.eep like with _private_macros.c previously?
*/
#define MACROCOUNT   6
#define MACROLEN    40

#define EEPROM_DEF 0xFF

#define EE_ADDR_START       100
#define EE_ADDR_MACROS      (EE_ADDR_START+100)
#define EE_ADDR_MACRO(idx)  (EE_ADDR_MACROS + (idx*(MACROLEN+1)))

uint8_t updateEEMacroHID(const uint8_t macro[MACROLEN], uint8_t idx);
uint8_t readEEMacroHID  (uint8_t macro[MACROLEN], uint8_t idx);

bool initMacros(void);
void printMacros(void);

void endMacro(void);

bool macroMode(void);
void setMacroMode(bool on);

bool activateMacro(uint8_t id);
bool getMacroReport(USB_KeyboardReport_Data_t *report);

bool macroRecording(void);
void setMacroRecording( uint8_t id );
void macro_key(uint8_t mod, uint8_t hid);

char hid2asciicode(uint8_t hid, uint8_t mod);

#endif

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
*/

#pragma once

#include <stdint.h>
#include <stdbool.h>
#include <avr/eeprom.h>

#include <LUFA/Drivers/USB/Class/Device/HIDClassDevice.h>
#include "keymap.h"
#include "command.h"

/**
 *  Macros are stored in eeprom. To avoid excessive rewrites, a fixed maximum length is set.
 *  Addresses are fixed.
 *  .
 *  @todo Enable pre-loading under certain conditions via *.eep like with _private_macros.c previously?
*/
#define MACROCOUNT       12
#define MACRO_MAX_LEN    40

uint8_t updateEEMacroHID(const uint8_t macro[MACRO_MAX_LEN], uint8_t idx);
uint8_t readEEMacroHID  (uint8_t macro[MACRO_MAX_LEN], uint8_t idx);
/// shortcut to put macro directly in print buffer
uint8_t printMacro(char macro_char);

void printMacros(void);

void setMacroMode(bool on);

bool macroRecording(void);
bool setMacroRecording(char macro_char, uint8_t hid, uint8_t mod);
void macro_key(uint8_t hid, uint8_t mod);

uint8_t setOutputString(char * str);
uint8_t setOutputHIDCodes(uint8_t * hidcodes);
uint8_t printOutstr(USB_KeyboardReport_Data_t * report);



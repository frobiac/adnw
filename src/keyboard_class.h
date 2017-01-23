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

#ifndef __KEYBOARD_CLASS_H__
#define __KEYBOARD_CLASS_H__

#include <LUFA/Drivers/USB/Class/Device/HIDClassDevice.h>

#ifdef ANALOGSTICK
    #include "analog.h"
#endif

#include "config.h"


void      initKeyboard(void);
void      init_active_keys(void);

uint8_t   getKeyboardReport(USB_KeyboardReport_Data_t *report);

void analogDataAcquire(void);

struct sfrActiveKeys activeKeys;
struct sfrActiveKeys secondUse_Prev_activeKeys;

struct Key {
    uint8_t row;
    uint8_t col;
    bool normalKey;
};

#define MAX_ACTIVE_KEYS 10

struct sfrActiveKeys {
    struct Key keys[MAX_ACTIVE_KEYS];
    uint8_t keycnt;
};


void clearActiveKeys(void);

bool    isModifierKey(uint8_t row, uint8_t col);
bool    isLayerKey   (uint8_t row, uint8_t col);
bool    isNormalKey  (uint8_t row, uint8_t col);
uint8_t getMouseKey  (uint8_t row, uint8_t col);
void    getSecondaryUsage(uint8_t r, uint8_t c, uint8_t *hid);

uint8_t getModifier(uint8_t row, uint8_t col, uint8_t layer);

uint8_t getActiveModifiers(void);
uint8_t getActiveLayer(void);
uint8_t getActiveKeyCodeModifier(void);

void set_led(void);
void set_led_color(uint8_t r, uint8_t g, uint8_t b);

void scan_matrix(void);

uint8_t fillReport(USB_KeyboardReport_Data_t *report_data);
uint8_t fillMacroReport(USB_KeyboardReport_Data_t *report_data);

void printCurrentKeys(void);
void clearRowData(void);

void handleSecondaryKeyUsage(USB_KeyboardReport_Data_t* report_data);
void hostLEDChange(uint8_t leds);

#endif // __KEYBOARD_CLASS_H__

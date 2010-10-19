/**

  Copyright 2010, frobiac |At| goolemail /d0t\ com

  This project is free software: you can redistribute
  it and/or modify it under the terms of the GNU General Public License as
  published by the Free Software Foundation, either version 3 of the
  License, or (at your option) any later version.

  This project is distributed in the hope that it will
  be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General
  Public License for more details.

  You should have received a copy of the GNU General Public License along
  with this project.  If not, see  <http://www.gnu.org/licenses/>.

*/


#ifndef __KEYBOARD_CLASS_H__
#define __KEYBOARD_CLASS_H__

#include <LUFA/Drivers/USB/Class/HID.h>
#include "analog.h"

void      initKeyboard(void);
uint8_t   getKeyboardReport(USB_KeyboardReport_Data_t *report);


uint8_t g_mouse_keys;
uint8_t g_mouse_mode;

void analogDataAcquire(void);

struct sfrActiveKeys activeKeys;

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

bool isModifierKey(uint8_t row, uint8_t col);
bool isLayerKey(uint8_t row, uint8_t col);
bool isNormalKey(uint8_t row, uint8_t col);

uint8_t getKeyCode(uint8_t row, uint8_t col, uint8_t layer);
uint8_t getModifier(uint8_t row, uint8_t col, uint8_t layer);
uint8_t getActiveModifiers(void);
uint8_t getActiveLayer(void);
uint8_t getActiveKeyCodeModifier(void);

void scan_matrix(void);

uint8_t fillReport(USB_KeyboardReport_Data_t *report_data);

void copyCurrentKeys(void);
bool keysChanged(void);
void printCurrentKeys(void);

#endif // __KEYBOARD_CLASS_H__

/*
    This file is part of the AdNW keyboard firmware.

    Copyright 2010-2019 Stefan Fröbe, <frobiac /at/ gmail [d0t] com>

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

#include <LUFA/Drivers/USB/Class/Device/HIDClassDevice.h>

#ifdef ANALOGSTICK
    #include "analog.h"
#endif

#include "config.h"
#include "keymap.h" //column_size_t


volatile uint32_t idle_count;   ///< interupt-incremented timer used for timeouts of MKT and mousekeys

void      initKeyboard(void);

void set_led(void);
void set_led_color(uint8_t r, uint8_t g, uint8_t b);

uint8_t getKeyboardReport(USB_KeyboardReport_Data_t *report_data);

bool suspend_wakeup_condition(void);
void SetupHardware(void);

uint8_t     g_mouse_keys;       ///< stores keys to be interpreted as mouse buttons
uint8_t     g_mouse_keys_enabled;       ///< temporarily active while mouse in in use,

void enable_mouse_keys(uint8_t on);
void initPWM(void);

inline void zeroReport(USB_KeyboardReport_Data_t *report_data) { memset(report_data, 0, 8); }

bool useAsMouseReport(void);
void fillKeyboardReport(USB_KeyboardReport_Data_t * report);
uint8_t activeKeysCount(void);
void keyChange(uint8_t row, column_size_t p, column_size_t h, column_size_t r);

// void hostLEDChange(uint8_t leds);


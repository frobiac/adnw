/*
    This file is part of the AdNW keyboard firmware.

    Copyright 2010-2016 Stefan Fröbe, <frobiac /at/ gmail [d0t] com>

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

#ifndef PS2MOUSE_H
#define PS2MOUSE_H

#include <stdbool.h>

//#include "pstwo.h"
#include "tmk_ps2/ps2_mouse.h"
#include "tmk_ps2/ps2.h"
#include "Keyboard.h"  // for USB_Mouse_Report_Data_t
#include "config.h"

#define PS2_ACK 0xFA

#define ACC_RAMPTIME 400 // acc incrementation time till maximum
#define ACC_MAX      2.5 // maximum accelleration factor reachable

uint8_t errcnt;
uint8_t g_mouse_enabled; ///< >0 if mouse/trackpoint is active and reports shall be generated
uint8_t g_ps2_connected; ///< >0 if a PS/2 device was detected.

void ps2_read_mouse(int *dx, int *dy, uint8_t *BTNS );
bool ps2_init_mouse(void);

uint8_t getMouseReport(USB_WheelMouseReport_Data_t *report_data);


#endif // PS2MOUSE_H

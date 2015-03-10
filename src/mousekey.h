/*
    Based on mousekey implementation from tmk_keyboard project,
    Copyright 2011 Jun Wako <wakojun@gmail.com>

    Copyright 2013 Stefan Fröbe, <frobiac /at/ gmail [d0t] com>

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

#ifndef MOUSEKEY_H
#define  MOUSEKEY_H

#include <stdbool.h>


void mousekey_task(void);
void mousekey_on(uint8_t code);
void mousekey_off(uint8_t code);
void mousekey_clear(void);
void mousekey_send(void);

void mousekey_activate(uint16_t mask);

uint8_t getMouseKeyReport(USB_WheelMouseReport_Data_t *MouseReport);

#endif

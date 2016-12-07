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

#include "global_config.h"

/**
 * Configuration data of keyboard:
 *  - Storage in eeprom
 *  - Modification via command mode
 *  - Hardware configuration of GPIOs
 */


/**
 * Setup initial config variables from defined values or EEPROM storage.
 *
 */
void init_config()
{
    g_led = (led_t) { .brightness=5, .on=0, .off=60 };
}

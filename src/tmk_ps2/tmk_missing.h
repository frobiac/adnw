/*
    This file is part of the AdNW keyboard firmware.

    Copyright 2016-2017 Stefan Fröbe, <frobiac /at/ gmail [d0t] com>

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


#ifndef TMK_MISSING_H
#define TMK_MISSING_H

/** Missing includes or defines from TMK PS/2 import.
 * These are simply put here to not clutter the originl sources too much
 */


// #include "wait.h"
#if defined(__AVR__)
    #include <util/delay.h>
    #define wait_ms(ms)  _delay_ms(ms)
    #define wait_us(us)  _delay_us(us)
#endif

#include "../config.h"
#endif

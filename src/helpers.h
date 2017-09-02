/*
    This file is part of the AdNW keyboard firmware.

    Copyright 2017-2018 Stefan Fröbe, <frobiac /at/ gmail [d0t] com>

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
// smaller than including ctype.h

#define isalnum(c) (('0' <= (c) && (c) <= '9') || ('a' <= (c) && (c) <= 'z') || ('A' <= (c) && (c) <= 'Z'))
#define isdigit(c) ('0' <= (c) && (c) <= '9')

char mapAscii(uint8_t tmp);
uint32_t str2hash(const char * str);


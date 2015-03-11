/*
    This file is part of the AdNW keyboard firmware.

    Copyright 2010-2015 Stefan Fröbe, <frobiac /at/ gmail [d0t] com>

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

#ifndef uint8_t
    #include <stdint.h>
#endif

#define PH_MIN_LEN       4
#define PH_MAX_LEN      26

#define PH_TYPE_NUM      3
#define PH_TYPE_ALNUM    2
#define PH_TYPE_ALNUMSYM 1

///  password array must be one longer then requested length!
uint8_t passHash(char * password, uint8_t len, uint8_t type, char * secret, char * key, char * tag);


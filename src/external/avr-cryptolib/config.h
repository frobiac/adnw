/*
    This file is part of the AdNW keyboard firmware.

    Copyright 2015-2020 Stefan Fröbe, <frobiac /at/ gmail [d0t] com>

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


// nearly empty file to be compatible with unmodified avr-crypto-lib sources
// re-defined in standard avr-crypto-lib sha1.c
#ifdef LITTLE_ENDIAN
    #undef LITTLE_ENDIAN
#endif
#define DEBUG_S(a)  ((void)0)

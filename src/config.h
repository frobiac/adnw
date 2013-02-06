/*
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


#ifndef CONFIG_H
#define CONFIG_H

// Unicode strings describing this keyboard
#define STR_MANUFACTURER L"frobiac"
#define STR_PRODUCT      L"AdNW TP and mouse"

// thanks to Openmoko, Inc we now have a dedicated USB Product ID
// Change this if you indend to make changes to the firmware, or distribute it
// in any way as these are specifically acquired for this setup!
#define VENDOR_ID             0x1D50
#define PRODUCT_ID            0x6033
#define RELEASE_NUMBER        0x0001


#endif // CONFIG_H


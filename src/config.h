/*
    This file is part of the AdNW keyboard firmware.

    Copyright 2013-2016 Stefan Fröbe, <frobiac /at/ gmail [d0t] com>

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

// *INDENT-OFF*

#ifndef CONFIG_H
#define CONFIG_H

/**
 * @file config.h
 *
 * User specific configuration of the firmware.
 * Debug and PS/2 options must be set in makefile.
 * Do not forget to adjust matrix.h to the hardware wiring used.
 *
 * @todo : Still need to configure command keys here.
 */

/// define DEBUG_OUTPUT in makefile to enable printf() for hid_listen.
#ifdef DEBUG_OUTPUT
    #include "hhstdio.h"
    //#define TRACE printf
    #define TRACE(...)
#else
    #define TRACE(...)
    #define printf(...)
#endif

// Unicode strings describing this keyboard
#define STR_MANUFACTURER L"frobiac"
#if defined BLUECUBE
    #define STR_PRODUCT      L"AdNW BlueCube"
#elif defined REDTILT
    #define STR_PRODUCT      L"AdNW RedTilt"
#elif defined HYPERNANO
    #define STR_PRODUCT      L"AdNW HyperNano"
#elif defined HYPERMICRO
    #define STR_PRODUCT      L"AdNW HyperMicro"
#elif defined BLACKFLAT
    #define STR_PRODUCT      L"AdNW BlackFlat"
#else
    #define STR_PRODUCT      L"AdNW Keyboard"
#endif


// thanks to Openmoko, Inc we now have a dedicated USB Product ID
// Change this if you indend to make changes to the firmware, or distribute it
// in any way as these are specifically acquired for this setup!
#define VENDOR_ID             0x1D50
#define PRODUCT_ID            0x6033
#define RELEASE_NUMBER        0x0001

#ifdef BLUECUBE
    #define DAPORT  B
    #define DBIT    7
    #define CLKPORT E
    #define CBIT    6
    #define RSTPORT B
    #define RBIT    3

#elif defined HYPERNANO || defined REDTILT
    #define DAPORT  B
    #define DBIT    1
    #define CLKPORT B
    #define CBIT    2
    #define RSTPORT B
    #define RBIT    0

#elif defined BLACKFLAT
    // Trackpoint from new batch of 3 with dedicated reset circuitry 
    // TP init: Bat:54 Id:aaTP 2nd ID=010e Ext.ID=M 19990623($IBM3780))

    // DATA green, CLK blue, RESET Yellow
    // Working: E6/D4.  C6/C7 works but in use by rows. D6 ok but LED.
    // Not ok: B7 is fried.
    #define DAPORT  D
    #define DBIT    4
    #define CLKPORT B
    #define CBIT    3
    // no RESET pin, dedicated circuitry instead
    #define RSTPORT B
    #define RBIT    0

/// @TODO Hypermicro defines for trackpoint
#else 
    #ifdef PS2MOUSE
        #error PS2MOUSE selected, but no pins defined
    #endif
#endif

#endif // CONFIG_H


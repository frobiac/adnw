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
#include "LUFA/Common/Common.h"  // CONCAT

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


/** Teensy 2.0 pinout, much nicer here:
 * https://github.com/benblazak/ergodox-firmware/blob/master/src/keyboard/ergodox/controller/teensy-2-0.md
 *
 *  GND            VCC   [SPI]
 *  [B0]           F0    (I2C)
 *  [B1]           F1    {UART}
 *  [B2]           F4    ?LED?
 *  [B3]  E6  AR   F5
 *   B7            F6
 *  (D0)           F7
 *  (D1)           B6
 *  {D2}           B5
 *  {D3}           B4
 *   C6            D7
 *   C7           ?D6?
 *      D V G R D
 *      5 C N S 4
 *        C D T
 *
*/


/*************************************************
 * TRACKPOINT SETUP
 *************************************************/
/*
 * Trackpoint configuration can be performed via the integrated config subcommand in command mode.
 * A default axis configuration should be provided based on a bitmask for the
 * TrackPoint config register as described in page33 of YKT3Eext.pdf
 * #define TP_AXES 0b01011001 // SWAPXY FLIPY FLIPX PTS
 */
enum { TP_PTS=0, TP_RES, TP_BTN2, TP_FLIPX, TP_FLIPY, TP_FLIPZ, TP_SWAPXY, TP_FTRANS };

#ifdef BLUECUBE
    #define TP_AXES (1<<TP_PTS | 1<<TP_FLIPY) // ???

    #define PS2_DATA_PORT_LETTER  B
    #define PS2_DATA_BIT    7
    #define PS2_CLOCK_PORT_LETTER E
    #define PS2_CLOCK_BIT    6
    #define PS2_RESET_PORT_LETTER B
    #define PS2_RESET_BIT    3

#elif defined HYPERNANO || defined REDTILT
    // trackpoint sideways, nub to the left
    #define TP_SWAP_XY
    #define TP_FLIP_X
    #define TP_FLIP_Y
    #define TP_AXES (1<<TP_PTS | 1<<TP_FLIPX | 1<<TP_FLIPY | 1<<TP_SWAPXY)

    #define PS2_DATA_PORT_LETTER  B
    #define PS2_DATA_BIT    1
    #define PS2_CLOCK_PORT_LETTER B
    #define PS2_CLOCK_BIT    2
    #define PS2_RESET_PORT_LETTER B
    #define PS2_RESET_BIT    0

#elif defined BLACKFLAT
    #define HAS_LED //@TODO LED port configurable

    // Trackpoint from new batch of 3 with dedicated reset circuitry
    // TP init: Bat:54 Id:aaTP 2nd ID=010e Ext.ID=M 19990623($IBM3780))
    // normal mount, TrackPoint nub at top of PCB and long side vertically
    #define TP_AXES (1<<TP_PTS | 1<<TP_FLIPY)

    // DATA green, CLK blue, RESET Yellow
    // Working: E6/D4.  C6/C7 works but in use by rows. D6 ok but LED.
    // Not ok: B7 is fried.
    #define PS2_DATA_PORT_LETTER  D
    #define PS2_DATA_BIT    4
    #define PS2_CLOCK_PORT_LETTER B
    #define PS2_CLOCK_BIT    3
    #define PS2_RESET_PORT_LETTER B
    #define PS2_RESET_BIT    0

/// @TODO Hypermicro defines for trackpoint
#else 
    #ifdef PS2MOUSE
        #error PS2MOUSE selected, but no pins defined
    #endif
#endif


// Create defines for TMK compatibility
#define CONCAT_PORT(name)    CONCAT(PORT,name)
#define CONCAT_PIN(name)     CONCAT(PIN,name)
#define CONCAT_DDR(name)     CONCAT(DDR,name)

#define PS2_DATA_PORT   CONCAT_PORT(PS2_DATA_PORT_LETTER)
#define PS2_DATA_PIN    CONCAT_PIN(PS2_DATA_PORT_LETTER)
#define PS2_DATA_DDR    CONCAT_DDR(PS2_DATA_PORT_LETTER)

#define PS2_CLOCK_PORT   CONCAT_PORT(PS2_CLOCK_PORT_LETTER)
#define PS2_CLOCK_PIN    CONCAT_PIN(PS2_CLOCK_PORT_LETTER)
#define PS2_CLOCK_DDR    CONCAT_DDR(PS2_CLOCK_PORT_LETTER)

#define PS2_RESET_PORT   CONCAT_PORT(PS2_RESET_PORT_LETTER)
#define PS2_RESET_PIN    CONCAT_PIN(PS2_RESET_PORT_LETTER)
#define PS2_RESET_DDR    CONCAT_DDR(PS2_RESET_PORT_LETTER)

#endif // CONFIG_H


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

#ifndef TRACKPOINT_H
#define TRACKPOINT_H

#include "ps2mouse.h"
#include "tmk_ps2/ps2.h"

#define PS2_DELAY 150

void    tp_reset( void );
bool    tp_init( void );
bool    tp_id( void );
uint8_t tp_read_config(void);
uint8_t tp_sensitivity(uint8_t sensitivity);
void    tp_ram_toggle(uint8_t addr, uint8_t mask);

uint8_t g_tp_sens;
uint8_t g_tp_sens_low;


/// https://git.kernel.org/cgit/linux/kernel/git/stable/linux-stable.git/plain/drivers/input/mouse/trackpoint.h
/*
 * These constants are from the TrackPoint System
 * Engineering documentation Version 4 from IBM Watson
 * research:
 *    http://wwwcssrv.almaden.ibm.com/trackpoint/download.html
 */

#define TP_COMMAND           0xE2    /* Commands start with this */

#define TP_READ_ID           0xE1    /* Sent for device identification */
#define TP_MAGIC_IDENT       0x01    /* Sent after a TP_READ_ID followed by the firmware ID */


/*
 * Commands
 */
#define TP_RECALIB           0x51    /* Recalibrate */
#define TP_POWER_DOWN        0x44    /* Can only be undone through HW reset */
#define TP_EXT_DEV           0x21    /* Determines if external device is connected (RO) */
#define TP_EXT_BTN           0x4B    /* Read extended button status */
#define TP_POR               0x7F    /* Execute Power on Reset */
#define TP_POR_RESULTS       0x25    /* Read Power on Self test results */
#define TP_DISABLE_EXT       0x40    /* Disable external pointing device */
#define TP_ENABLE_EXT        0x41    /* Enable external pointing device */

/*
 * Mode manipulation
 */
#define TP_SET_SOFT_TRANS    0x4E    /* Set mode */
#define TP_CANCEL_SOFT_TRANS 0xB9    /* Cancel mode */
#define TP_SET_HARD_TRANS    0x45    /* Mode can only be set */


/*
 * Register oriented commands/properties
 */
#define TP_WRITE_MEM         0x81
#define TP_READ_MEM          0x80    /* Not used in this implementation */

/*
* RAM Locations for properties
 */
#define TP_SENS              0x4A    /* Sensitivity */
#define TP_MB                0x4C    /* Read Middle Button Status (RO) */
#define TP_INERTIA           0x4D    /* Negative Inertia */
#define TP_SPEED             0x60    /* Speed of TP Cursor */
#define TP_REACH             0x57    /* Backup for Z-axis press */
#define TP_DRAGHYS           0x58    /* Drag Hysteresis (how hard it is to drag with Z-axis pressed) */
#define TP_MINDRAG           0x59    /* Minimum amount of force needed to trigger dragging */

#define TP_THRESH            0x5C    /* Minimum value for a Z-axis press */
#define TP_UP_THRESH         0x5A    /* Used to generate a 'click' on Z-axis */
#define TP_Z_TIME            0x5E    /* How sharp of a press */
#define TP_JENKS_CURV        0x5D    /* Minimum curvature for double click */
#define TP_DRIFT_TIME        0x5F    /* How long a 'hands off' condition must last (x*107ms) for drift correction to occur */

/*
 * Toggling Flag bits
 */
#define TP_TOGGLE            0x47    /* Toggle command */

#define TP_TOGGLE_MB         0x23    /* Disable/Enable Middle Button */
#define TP_MASK_MB           0x01
#define TP_TOGGLE_EXT_DEV    0x23    /* Disable external device */
#define TP_MASK_EXT_DEV      0x02
#define TP_TOGGLE_DRIFT      0x23    /* Drift Correction */
#define TP_MASK_DRIFT        0x80
#define TP_TOGGLE_BURST      0x28    /* Burst Mode */
#define TP_MASK_BURST        0x80
#define TP_TOGGLE_PTSON      0x2C    /* Press to Select */
#define TP_MASK_PTSON        0x01
#define TP_TOGGLE_HARD_TRANS 0x2C    /* Alternate method to set Hard Transparency */
#define TP_MASK_HARD_TRANS   0x80
#define TP_TOGGLE_TWOHAND    0x2D    /* Two handed */
#define TP_MASK_TWOHAND      0x01
#define TP_TOGGLE_STICKY_TWO 0x2D    /* Sticky two handed */
#define TP_MASK_STICKY_TWO   0x04
#define TP_TOGGLE_SKIPBACK   0x2D    /* Suppress movement after drag release */
#define TP_MASK_SKIPBACK     0x08
#define TP_TOGGLE_SOURCE_TAG 0x20    /* Bit 3 of the first packet will be set to the origin of the packet (external or TP) */
#define TP_MASK_SOURCE_TAG   0x80
#define TP_TOGGLE_EXT_TAG    0x22    /* Bit 3 of the first packet coming from the external device will be forced to 1 */
#define TP_MASK_EXT_TAG      0x04


/* Power on Self Test Results */
#define TP_POR_SUCCESS       0x3B

/*
 * Default power on values
 */
#define TP_DEF_SENS          0x80
#define TP_DEF_INERTIA       0x06
#define TP_DEF_SPEED         0x61
#define TP_DEF_REACH         0x0A

#define TP_DEF_DRAGHYS       0xFF
#define TP_DEF_MINDRAG       0x14

#define TP_DEF_THRESH        0x08
#define TP_DEF_UP_THRESH     0xFF
#define TP_DEF_Z_TIME        0x26
#define TP_DEF_JENKS_CURV    0x87
#define TP_DEF_DRIFT_TIME    0x05

/* Toggles */
#define TP_DEF_MB            0x00
#define TP_DEF_PTSON         0x00
#define TP_DEF_SKIPBACK      0x00
#define TP_DEF_EXT_DEV       0x00    /* 0 means enabled */
#define TP_DEF_TWOHAND       0x00
#define TP_DEF_SOURCE_TAG    0x00


#endif // TRACKPOINT_H

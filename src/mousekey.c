/*
    This file is part of the AdNW keyboard firmware.

    Based on mousekey implementation from tmk_keyboard project,
    Copyright 2011 Jun Wako <wakojun@gmail.com>

    Copyright 2013-2015 Stefan Fröbe, <frobiac /at/ gmail [d0t] com>

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


#include <stdint.h>
#include <util/delay.h>
#include "hid_usage.h"
#include "Keyboard.h" // idle_count, should go into timer
#include "mousekey.h"

// Mouse keys acceleration algorithm : http://en.wikipedia.org/wiki/Mouse_keys
// speed = delta * max_speed * (repeat / time_to_max)**((1000+curve)/1000)

/*
   Current PS/2 Trackpoint values and timing information:
   X/Y -35..35    Min or max reached in about 180ms (idlecount incremented by 11 or 15 calls to getMouseReport())
   V/H  -9.. 9    twice as fast: 5 idlecount = 8 calls to getMouseReport()

   Code below used to be (with XY_DELAY 200 VH_DELAY 700)
   X/Y -27..27    reached in 70 idlecount =17 calls to getMousekeyReport()
   V/H  -5.. 5....           83 idlecount =40 calls to getMousekeyReport()

   The settings below seem to be a good approximation for mousekey pointer movement.
   It is however hard to navigate around fast and accurate at the same time, and
   any acceleration must be carefully fine-tuned.
*/
#define MK_XY_DELAY     200 ///< delay [ms] until start of acceleration
#define MK_XY_TTM      1200 ///< time  [ms] until maximum acceleration is reached
#define MK_XY_MAX        35 ///< maximum move increment per event
#define MK_XY_MIN         3 ///< minimal move incerment per event

#define MK_VH_DELAY     500 ///< like above, but for scroll movements
#define MK_VH_TTM      2000
#define MK_VH_MAX         9
#define MK_VH_MIN         1

#define MS_PER_REPORT    30 ///< time [ms] until next report is filled with data

/// local working copy of mouse key report to send on getMousekeyReport()
USB_WheelMouseReport_Data_t mkr = {0};

static uint8_t mousekey_accel = 0;

static uint32_t first_press_timer = 0;
static uint32_t last_sent = 0;
static uint32_t delta_in_ms;

static void mousekey_debug(void);

static uint8_t move_unit(void)
{
    uint16_t unit;

    if (mousekey_accel & (1<<0)) {
        unit = MK_XY_MAX/mousekey_accel;
    } else if (delta_in_ms < MK_XY_DELAY) {
        unit = MK_XY_MIN;
    } else if (delta_in_ms >= MK_XY_TTM) {
        unit = MK_XY_MAX;
    } else {
        unit=MK_XY_MIN+(MK_XY_MAX-MK_XY_MIN)*(delta_in_ms-MK_XY_DELAY)/MK_XY_TTM;
    }

    return (unit > MK_XY_MAX ? MK_XY_MAX : (unit == 0 ? 1 : unit));
}

static uint8_t wheel_unit(void)
{
    uint16_t unit;

    if (mousekey_accel & (1<<0)) {
        unit = MK_VH_MAX/mousekey_accel;
    } else if (delta_in_ms < MK_VH_DELAY) {
        unit = MK_VH_MIN;
    } else if (delta_in_ms >= MK_VH_TTM) {
        unit = MK_VH_MAX;
    } else {
        unit=MK_VH_MIN+(MK_VH_MAX-MK_VH_MIN)*(delta_in_ms-MK_VH_DELAY)/MK_VH_TTM;
    }

    return (unit > MK_VH_MAX ? MK_VH_MAX : (unit == 0 ? 1 : unit));
}

/** this is called periodically from outside.
*  we need to set the appropriate values in the mousekey report that later gets send via getMouseReport.
*/
void mousekey_activate(uint16_t mask)
{
    // if no key is pressed, reset!
    if(mask==0) {
        mousekey_clear();
        return;
    }
    if(first_press_timer == 0)
        first_press_timer = idle_count;

    delta_in_ms = (idle_count-first_press_timer)*1000/61;

    for(uint8_t c=0; c<16; ++c) {
        if(mask & (1<<c))
            mousekey_on(MS_BEGIN+1+c);
        else
            mousekey_off(MS_BEGIN+1+c);
    }
}

void mousekey_on(uint8_t code)
{
    if(code < MS_U || code > MS_ACC2)
        return;

    switch(code) {
        case MS_U:    mkr.Y = move_unit() * -1; break;
        case MS_D:    mkr.Y = move_unit(); break;
        case MS_L:    mkr.X = move_unit() * -1; break;
        case MS_R:    mkr.X = move_unit(); break;
        case MS_W_U:  mkr.V = wheel_unit(); break;
        case MS_W_D:  mkr.V = wheel_unit() * -1; break;
        case MS_W_L:  mkr.H = wheel_unit() * -1; break;
        case MS_W_R:  mkr.H = wheel_unit(); break;
        case MS_BTN1: mkr.Button |= HID_MOUSEBTN_1; break;
        case MS_BTN2: mkr.Button |= HID_MOUSEBTN_2; break;
        case MS_BTN3: mkr.Button |= HID_MOUSEBTN_3; break;
        case MS_BTN4: mkr.Button |= HID_MOUSEBTN_4; break;
        case MS_BTN5: mkr.Button |= HID_MOUSEBTN_5; break;
        case MS_ACC0:
        case MS_ACC1:
        case MS_ACC2: mousekey_accel |= (1<<(code-MS_ACC0)); break;
        default: mousekey_debug();
    }
}

void mousekey_off(uint8_t code)
{
    if(code < MS_U || code > MS_ACC2)
        return;

    switch(code) {
        case MS_U   : if(mkr.Y < 0) mkr.Y = 0; break;
        case MS_D   : if(mkr.Y > 0) mkr.Y = 0; break;
        case MS_L   : if(mkr.X < 0) mkr.X = 0; break;
        case MS_R   : if(mkr.X > 0) mkr.X = 0; break;
        case MS_W_U : if(mkr.V > 0) mkr.V = 0; break;
        case MS_W_D : if(mkr.V < 0) mkr.V = 0; break;
        case MS_W_L : if(mkr.H < 0) mkr.H = 0; break;
        case MS_W_R : if(mkr.H > 0) mkr.H = 0; break;
        case MS_BTN1: mkr.Button &= ~HID_MOUSEBTN_1; break;
        case MS_BTN2: mkr.Button &= ~HID_MOUSEBTN_2; break;
        case MS_BTN3: mkr.Button &= ~HID_MOUSEBTN_3; break;
        case MS_BTN4: mkr.Button &= ~HID_MOUSEBTN_4; break;
        case MS_BTN5: mkr.Button &= ~HID_MOUSEBTN_5; break;
        case MS_ACC0: mousekey_accel &= ~(1<<0); break;
        case MS_ACC1: mousekey_accel &= ~(1<<1); break;
        case MS_ACC2: mousekey_accel &= ~(1<<2); break;
        default: mousekey_debug();
    }
}

/**
 * @brief getMouseKeyReport
 * @param MouseReport
 * @return 0 unless MouseReport was changed
 */
uint8_t getMouseKeyReport(USB_WheelMouseReport_Data_t *MouseReport)
{
    if(mkr.Button == 0) {
        if(mkr.X == 0 && mkr.Y == 0 && mkr.V == 0 && mkr.H == 0) {
            return 0;
        } else if ((idle_count-last_sent)*1000/61 < MS_PER_REPORT ) {
            return 0;
        }
    }

    last_sent=idle_count;

    if (mkr.X && mkr.Y) {
        // lower speed on diagonal movement: 3/4 close enough to sqrt(2) for 45deg angles, and others are fine with this, too.
        MouseReport->X = (mkr.X*3) >> 2;
        MouseReport->Y = (mkr.Y*3) >> 2;
    } else {
        MouseReport->X= mkr.X;
        MouseReport->Y= mkr.Y;
    }
    MouseReport->Button = mkr.Button;
    MouseReport->V= mkr.V;
    MouseReport->H= mkr.H;

    return sizeof(USB_WheelMouseReport_Data_t);
}

void mousekey_clear(void)
{
    for(uint8_t c=0; c<16; ++c)
        mousekey_off(MS_BEGIN+1+c);
    mkr = (USB_WheelMouseReport_Data_t) {};
    mousekey_accel = 0;
    first_press_timer=0;
}

static void mousekey_debug(void)
{
    printf("\n%02x |%d,%d %d,%d (acc %d)] ", mkr.Button, mkr.X, mkr.Y, mkr.V, mkr.H, mousekey_accel);
    printf("%lu", (idle_count-first_press_timer));
}

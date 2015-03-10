/*
    Based on mousekey implementation from tmk_keyboard project,
    Copyright 2011 Jun Wako <wakojun@gmail.com>

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

#include <stdint.h>
#include <util/delay.h>
#include "hid_usage.h"
#include "Keyboard.h" // idle_count, should go into timer
#include "mousekey.h"

USB_WheelMouseReport_Data_t mouse_report = {0};

static uint8_t mousekey_accel = 0;

static void mousekey_debug(void);
/*
 * Mouse keys  acceleration algorithm
 *  http://en.wikipedia.org/wiki/Mouse_keys
 *
 *  speed = delta * max_speed * (repeat / time_to_max)**((1000+curve)/1000)
 */
/*

*/

#define MK_XY_DELAY     200 ///< delay [ms] until start of acceleration
#define MK_XY_TTM      1200 ///< time  [ms] until maximum acceleration is reached
#define MK_XY_MAX        27 ///< maximum move increment per event
#define MK_XY_MIN         3 ///< minimal move incerment per event
#define MK_VH_DELAY     700
#define MK_VH_TTM      2000
#define MK_VH_MAX         5
#define MK_VH_MIN         1

#define MS_PER_REPORT    30 ///< time [ms] until next report is filled with data

static uint32_t first_press_timer = 0;
static uint32_t last_sent = 0;

static uint8_t move_unit(void)
{
    uint16_t unit;
    uint32_t delta_in_ms=(idle_count-first_press_timer)*1000/61;

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
    uint32_t delta_in_ms=(idle_count-first_press_timer)*1000/61;

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
*  we need to set the appropriate values in mouse_report that later get send via getMouseReport.
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
//mousekey_debug();
    switch(code) {
        case MS_U:    mouse_report.Y = move_unit() * -1; break;
        case MS_D:    mouse_report.Y = move_unit(); break;
        case MS_L:    mouse_report.X = move_unit() * -1; break;
        case MS_R:    mouse_report.X = move_unit(); break;
        case MS_W_U:  mouse_report.V = wheel_unit(); break;
        case MS_W_D:  mouse_report.V = wheel_unit() * -1; break;
        case MS_W_L:  mouse_report.H = wheel_unit() * -1; break;
        case MS_W_R:  mouse_report.H = wheel_unit(); break;
        case MS_BTN1: mouse_report.Button |= HID_MOUSEBTN_1; break;
        case MS_BTN2: mouse_report.Button |= HID_MOUSEBTN_2; break;
        case MS_BTN3: mouse_report.Button |= HID_MOUSEBTN_3; break;
        case MS_BTN4: mouse_report.Button |= HID_MOUSEBTN_4; break;
        case MS_BTN5: mouse_report.Button |= HID_MOUSEBTN_5; break;
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
        case MS_U   : if(mouse_report.Y < 0) mouse_report.Y = 0; break;
        case MS_D   : if(mouse_report.Y > 0) mouse_report.Y = 0; break;
        case MS_L   : if(mouse_report.X < 0) mouse_report.X = 0; break;
        case MS_R   : if(mouse_report.X > 0) mouse_report.X = 0; break;
        case MS_W_U : if(mouse_report.V > 0) mouse_report.V = 0; break;
        case MS_W_D : if(mouse_report.V < 0) mouse_report.V = 0; break;
        case MS_W_L : if(mouse_report.H < 0) mouse_report.H = 0; break;
        case MS_W_R : if(mouse_report.H > 0) mouse_report.H = 0; break;
        case MS_BTN1: mouse_report.Button &= ~HID_MOUSEBTN_1; break;
        case MS_BTN2: mouse_report.Button &= ~HID_MOUSEBTN_2; break;
        case MS_BTN3: mouse_report.Button &= ~HID_MOUSEBTN_3; break;
        case MS_BTN4: mouse_report.Button &= ~HID_MOUSEBTN_4; break;
        case MS_BTN5: mouse_report.Button &= ~HID_MOUSEBTN_5; break;
        case MS_ACC0: mousekey_accel &= ~(1<<0); break;
        case MS_ACC1: mousekey_accel &= ~(1<<1); break;
        case MS_ACC2: mousekey_accel &= ~(1<<2); break;
        default: mousekey_debug();
    }
}

uint8_t getMouseKeyReport(USB_WheelMouseReport_Data_t *MouseReport)
{
// @note: If sending of reports is not forced in calling function,
// return an empty report every second time - else it stalls when max accel is reached.
    if (mouse_report.Button == 0) {
        if (mouse_report.X == 0 && mouse_report.Y == 0 && mouse_report.V == 0 && mouse_report.H == 0)
            return sizeof(USB_WheelMouseReport_Data_t);

        if (mouse_report.V == 0 && mouse_report.H == 0 &&
            (idle_count-last_sent)*1000/61 < MS_PER_REPORT )
            return sizeof(USB_WheelMouseReport_Data_t);
        if (mouse_report.X == 0 && mouse_report.Y == 0 &&
            (idle_count-last_sent)*1000/61 < 100)
            return sizeof(USB_WheelMouseReport_Data_t);
    }

    last_sent=idle_count;

    MouseReport->X= mouse_report.X;
    MouseReport->Y= mouse_report.Y;
    MouseReport->Button = mouse_report.Button;
    MouseReport->V= mouse_report.V;
    MouseReport->H= mouse_report.H;

    // lower speed on diagonal movement:
    // 3/4 close enough to sqrt(2) for 45deg angles, and others are fine with this, too.
    if (mouse_report.X && mouse_report.Y) {
        MouseReport->X = (MouseReport->X*3) >> 2;
        MouseReport->Y = (MouseReport->Y*3) >> 2;
    }

    return sizeof(USB_WheelMouseReport_Data_t);
}

void mousekey_clear(void)
{
    for(uint8_t c=0; c<16; ++c)
        mousekey_off(MS_BEGIN+1+c);
    mouse_report = (USB_WheelMouseReport_Data_t) {};
    mousekey_accel = 0;
    first_press_timer=0;
}

static void mousekey_debug(void)
{
    printf("\n%02x |%d,%d %d,%d (acc %d)] ", mouse_report.Button, mouse_report.X, mouse_report.Y, mouse_report.V, mouse_report.H, mousekey_accel);
    printf("%lu", (idle_count-first_press_timer));
}

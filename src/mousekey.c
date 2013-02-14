/*
Copyright 2011 Jun Wako <wakojun@gmail.com>

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 2 of the License, or
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


typedef struct {
    uint8_t buttons;
    int8_t x;
    int8_t y;
    int8_t v;
    int8_t h;
} __attribute__ ((packed)) report_mouse_t;

report_mouse_t mouse_report = {0};

static uint8_t mousekey_repeat =  0;
static uint8_t mousekey_accel = 0;
//static uint8_t mk_repeat =  0;
//static uint8_t mk_accel = 0;
static uint8_t mk_acc_timer = 0;
static uint8_t count=0;

static void mousekey_debug(void);


/*
 * Mouse keys  acceleration algorithm
 *  http://en.wikipedia.org/wiki/Mouse_keys
 *
 *  speed = delta * max_speed * (repeat / time_to_max)**((1000+curve)/1000)
 */
/* milliseconds between the initial key press and first repeated motion event (0-2550) */
uint8_t mk_delay = MOUSEKEY_DELAY/10;
/* milliseconds between repeated motion events (0-255) */
uint8_t mk_interval = MOUSEKEY_INTERVAL;
/* steady speed (in action_delta units) applied each event (0-255) */
uint8_t mk_max_speed = MOUSEKEY_MAX_SPEED;
/* number of events (count) accelerating to steady speed (0-255) */
uint8_t mk_time_to_max = MOUSEKEY_TIME_TO_MAX;
/* ramp used to reach maximum pointer speed (NOT SUPPORTED) */
//int8_t mk_curve = 0;
/* wheel params */
uint8_t mk_wheel_max_speed = MOUSEKEY_WHEEL_MAX_SPEED;
uint8_t mk_wheel_time_to_max = MOUSEKEY_WHEEL_TIME_TO_MAX;


static uint16_t last_timer = 0;
static bool prev_empty=true;

static uint8_t move_unit(void)
{
    return (1+(mousekey_repeat*7/255));

    uint16_t unit;
    if (mousekey_accel & (1<<0)) {
        unit = (MOUSEKEY_MOVE_DELTA * mk_max_speed)/4;
    } else if (mousekey_accel & (1<<1)) {
        unit = (MOUSEKEY_MOVE_DELTA * mk_max_speed)/2;
    } else if (mousekey_accel & (1<<2)) {
        unit = (MOUSEKEY_MOVE_DELTA * mk_max_speed);
    } else if (mousekey_repeat == 0) {
        unit = MOUSEKEY_MOVE_DELTA;
    } else if (mousekey_repeat >= mk_time_to_max) {
        unit = MOUSEKEY_MOVE_DELTA * mk_max_speed;
    } else {
        unit = (MOUSEKEY_MOVE_DELTA * mk_max_speed * mousekey_repeat) / mk_time_to_max;
    }
    return (unit > MOUSEKEY_MOVE_MAX ? MOUSEKEY_MOVE_MAX : (unit == 0 ? 1 : unit));
}

static uint8_t wheel_unit(void)
{
    return (1+(mousekey_repeat*3/255));

    uint16_t unit;
    if (mousekey_accel & (1<<0)) {
        unit = (MOUSEKEY_WHEEL_DELTA * mk_wheel_max_speed)/4;
    } else if (mousekey_accel & (1<<1)) {
        unit = (MOUSEKEY_WHEEL_DELTA * mk_wheel_max_speed)/2;
    } else if (mousekey_accel & (1<<2)) {
        unit = (MOUSEKEY_WHEEL_DELTA * mk_wheel_max_speed);
    } else if (mousekey_repeat == 0) {
        unit = MOUSEKEY_WHEEL_DELTA;
    } else if (mousekey_repeat >= mk_wheel_time_to_max) {
        unit = MOUSEKEY_WHEEL_DELTA * mk_wheel_max_speed;
    } else {
        unit = (MOUSEKEY_WHEEL_DELTA * mk_wheel_max_speed * mousekey_repeat) / mk_wheel_time_to_max;
    }
    return (unit > MOUSEKEY_WHEEL_MAX ? MOUSEKEY_WHEEL_MAX : (unit == 0 ? 1 : unit));
}

/** this is called periodically from outside.
 *  we need to set the appropriate values in mouse_report that later get send via getMouseReport.
 */
void mk_activate(uint16_t mask)
{
    // if no key is pressed, reset!
    if(mask==0) {
        mousekey_clear();
        return;
    }
    for(uint8_t c=0; c<16; ++c) {
        if(mask & (1<<c))
            mousekey_on(MS_BEGIN+1+c);
        else
            mousekey_off(MS_BEGIN+1+c);
    }

    // increment acceleration slowler
    if((idle_count - mk_acc_timer) / 20 > 1)
    {
        if (mousekey_repeat != UINT8_MAX)
            mousekey_repeat++;
        // reset accel increase timer
        mk_acc_timer=idle_count;

    }
    //mousekey_debug();
}

void mousekey_on(uint8_t code)
{
    if(code < MS_U || code > MS_ACC2)
        return;
    //mousekey_debug();
    if      (code == MS_U)    mouse_report.y = move_unit() * -1;
    else if (code == MS_D)    mouse_report.y = move_unit();
    else if (code == MS_L)    mouse_report.x = move_unit() * -1;
    else if (code == MS_R)    mouse_report.x = move_unit();
    else if (code == MS_W_U)  mouse_report.v = wheel_unit();
    else if (code == MS_W_D)  mouse_report.v = wheel_unit() * -1;
    else if (code == MS_W_L)  mouse_report.h = wheel_unit() * -1;
    else if (code == MS_W_R)  mouse_report.h = wheel_unit();
    else if (code == MS_BTN1) mouse_report.buttons |= MOUSE_BTN1;
    else if (code == MS_BTN2) mouse_report.buttons |= MOUSE_BTN2;
    else if (code == MS_BTN3) mouse_report.buttons |= MOUSE_BTN3;
    else if (code == MS_BTN4) mouse_report.buttons |= MOUSE_BTN4;
    else if (code == MS_BTN5) mouse_report.buttons |= MOUSE_BTN5;
    else if (code == MS_ACC0) mousekey_accel |= (1<<0);
    else if (code == MS_ACC1) mousekey_accel |= (1<<1);
    else if (code == MS_ACC2) mousekey_accel |= (1<<2);
}

void mousekey_off(uint8_t code)
{
    if(code < MS_U || code > MS_ACC2)
        return;
    if      (code == MS_U     && mouse_report.y < 0) mouse_report.y = 0;
    else if (code == MS_D     && mouse_report.y > 0) mouse_report.y = 0;
    else if (code == MS_L     && mouse_report.x < 0) mouse_report.x = 0;
    else if (code == MS_R     && mouse_report.x > 0) mouse_report.x = 0;
    else if (code == MS_W_U   && mouse_report.v > 0) mouse_report.v = 0;
    else if (code == MS_W_D   && mouse_report.v < 0) mouse_report.v = 0;
    else if (code == MS_W_L   && mouse_report.h < 0) mouse_report.h = 0;
    else if (code == MS_W_R   && mouse_report.h > 0) mouse_report.h = 0;
    else if (code == MS_BTN1) mouse_report.buttons &= ~MOUSE_BTN1;
    else if (code == MS_BTN2) mouse_report.buttons &= ~MOUSE_BTN2;
    else if (code == MS_BTN3) mouse_report.buttons &= ~MOUSE_BTN3;
    else if (code == MS_BTN4) mouse_report.buttons &= ~MOUSE_BTN4;
    else if (code == MS_BTN5) mouse_report.buttons &= ~MOUSE_BTN5;
    else if (code == MS_ACC0) mousekey_accel &= ~(1<<0);
    else if (code == MS_ACC1) mousekey_accel &= ~(1<<1);
    else if (code == MS_ACC2) mousekey_accel &= ~(1<<2);

    if (mouse_report.x == 0 && mouse_report.y == 0 && mouse_report.v == 0 && mouse_report.h == 0)
        mousekey_repeat = 0;
}

uint8_t getMouseKeyReport(USB_MouseReport_Data_t *MouseReport)
{
    // If sending of reports is not forced in calling function,
    // return an empty report every second time - else it stalls when max accel is reached.
    if (mouse_report.x == 0 && mouse_report.y == 0 && mouse_report.v == 0 && mouse_report.h == 0  && mouse_report.buttons==0)
        return sizeof(USB_MouseReport_Data_t);

    MouseReport->X= mouse_report.x;
    MouseReport->Y= mouse_report.y;
    MouseReport->V= mouse_report.v;
    MouseReport->H= mouse_report.h;
    MouseReport->Button = mouse_report.buttons;

    // same speed diagonally
    if (mouse_report.x && mouse_report.y) {
        MouseReport->X *= 0.7;
        MouseReport->Y *= 0.7;
    }

    //mousekey_clear();
    //last_timer = idle_count; //timer_read();

    return sizeof(USB_MouseReport_Data_t);

}

void mousekey_clear(void)
{
    for(uint8_t c=0; c<16; ++c) 
        mousekey_off(MS_BEGIN+1+c);
    mouse_report = (report_mouse_t) {};
    mousekey_repeat = 0;
    mousekey_accel = 0;
    mk_acc_timer=idle_count;
    last_timer=idle_count;
}

static void mousekey_debug(void)
{
    printf("\n%02x |%d,%d %d,%d (%d %d)] ", mouse_report.buttons, mouse_report.x, mouse_report.y, mouse_report.v, mouse_report.h, mousekey_repeat, mousekey_accel);
    printf("0x%02x%02x %d", (uint8_t)((idle_count&0xff00)>>8),(uint8_t)(idle_count&0x00ff), (uint8_t)(idle_count-last_timer)&0x00ff);
}

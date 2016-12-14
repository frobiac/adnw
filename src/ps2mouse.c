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

#include <util/delay.h>

#include "ps2mouse.h"
#include "trackpoint.h"

volatile uint8_t     scrollcnt;
volatile uint32_t    mouse_timer; /// toggle mouse mode for a specified time
volatile uint16_t    accel; /// toggle mouse mode for a specified time

/**
 * Setup PS/2 connection
 */
bool ps2_init_mouse(void)
{
    uint8_t rcv __attribute__((unused)); // only for debug output

    g_ps2_connected=1;
    g_mouse_enabled = 0;
    scrollcnt = 0;

#ifndef BLACKFLAT
    tp_reset();
#endif

    ps2_host_init();

    _delay_ms(1000);    // wait for powering up

    // send Reset
    rcv = ps2_host_send(0xFF);
    printf("\nps2 RST: %02x %02x", rcv, ps2_error);

    // read completion code of BAT
    rcv = ps2_host_recv_response();
    printf("\nps2 BAT: %02x %02x", rcv, ps2_error);

    // read Device ID
    rcv = ps2_host_recv_response();
    printf("\nps2 DEV: %02x %02x", rcv, ps2_error);

    //Enable Data reporting
    //rcv = ps2_host_send(0xF4);
    //printf("\nps2 REP: %02x %02x", rcv, ps2_error);

    //ps2_send_expect(0xe8, PS2_ACK); // set resolution
    //ps2_send_expect(0x01, PS2_ACK); // 8 counts/mm
    //ps2_send_expect(0xf3, PS2_ACK); // set sample rate
    //ps2_send_expect(0x01, PS2_ACK);
    //ps2_host_send(0x64); //200 smaples a second

    // send Set Remote mode
    rcv = ps2_host_send(0xF0);
    printf("\nps2 REM: %02x %02x", rcv, ps2_error);


    tp_init();
    tp_id();

    /// @todo Set only on successful init
    g_mouse_enabled = 1;

    return true;
}

/*
 *
 */
void ps2_read_mouse(int *dx, int *dy, uint8_t *BTNS )
{
    if(!g_ps2_connected)
        return;

    *BTNS=0;
    int mouseinf;
    {
        uint8_t rcv;
        rcv = ps2_host_send(PS2_MOUSE_READ_DATA);
        if(rcv == PS2_ACK) {
            mouseinf=ps2_host_recv_response();
            /// Bits 2 1 0 correspond to  M R L button so swap M&R for RML
            /// @todo Make mouse button order mapping configurable
            *BTNS = ((mouseinf & 0x01) /*LMB*/ << 0) |
                    ((mouseinf & 0x04) /*MMB*/ << 1) |
                    ((mouseinf & 0x02) /*RMB*/ << 2);

            *dx= ps2_host_recv_response();
            *dy= ps2_host_recv_response();

            if(mouseinf&0x10)
                *dx=-(256-*dx);

            if(mouseinf&0x20)
                *dy=-(256-*dy);

            return;
        }
    }
}

/**
 * Rotate 90° by exchanging x and y and changing sign of new x
 */
void rotateXY(int8_t *x, int8_t *y)
{
#ifdef TP_ROTATE
    int8_t tmp = *y;
    *y = *x;
    *x = -tmp;
#endif
}

/**
 * @brief getMouseReport
 * @param MouseReport
 * @return 0 unless mouse report data was changed
 */
uint8_t getMouseReport(USB_WheelMouseReport_Data_t *MouseReport)
{
    if(!g_mouse_enabled)
        return 0;

    int16_t dx=0, dy=0;
    uint8_t ps2_buttons=0;

    /// @TODO why another check here??
#ifdef PS2MOUSE
    ps2_read_mouse(&dx, &dy, &ps2_buttons);
#endif

    if( (g_mouse_keys & 0x0F) || (ps2_buttons & 0x07) || (dx!=0) || (dy!=0) ) {
        if(g_mouse_keys_enabled==0) {
            enable_mouse_keys(1);
            accel=0;
        }

        mouse_timer=idle_count;
        if(accel<ACC_RAMPTIME)
            accel++;

        // reset mouse mode after inactivity: idle_count is incremented 61 times per second
        // After last detected trackpoint movement that much time remains to press a button.
    } else {
        if(idle_count-mouse_timer > 1/*seconds*/ *61 ) {
            enable_mouse_keys(0);
            accel=0;
            scrollcnt=0;
        }
        return 0; // no data to send
    }

    if(g_mouse_keys_enabled || ps2_buttons) {
        // factor = 1 + [0..400]*1.5/400 = 1..2.5
        //factor= 1 + accel * (ACC_MAX-1) / ACC_RAMPTIME;

        // keyboard mouse buttons only in mousemode
        if( (ps2_buttons & 0x05)==0x05 || (g_mouse_keys & 0x08)) {
            int8_t sx = (dx&0xFF00) ? -(256-(dx+0x100)) : dx;
            int8_t sy = (dy&0xFF00) ? -(256-(dy+0x100)) : dy;

            // scrollcnt = scrollcnt+abs(sy)+abs(sx); abs(v) = v*((v<0)*(-1)+(v>0));
            scrollcnt += sy*((sy<0)*(-1)+(sy>0)) + sx*((sx<0)*(-1)+(sx>0));

            // limit 10 and emiting as is way to fast in windows.
            if(scrollcnt>10) {
                scrollcnt=0;
                MouseReport->X=0;
                MouseReport->Y=0;
                // only move by 1 ?!
                // factor here had been mapped to 0..0.5 -> use 0.25
                MouseReport->H = -sx>>2;
                MouseReport->V = -sy>>2;
                rotateXY(&MouseReport->V, &MouseReport->H);
            }
        } else { // no scroll-wheel support or not active
            MouseReport->X = -dx;
            MouseReport->Y = dy;
            rotateXY(&(MouseReport->X), &(MouseReport->Y));
            MouseReport->Button = g_mouse_keys & ~(0x08); // do not emit the scroll button
            MouseReport->Button |= ps2_buttons;           // PS/2 buttons if set
        }
        g_mouse_keys=0;

        return sizeof(USB_WheelMouseReport_Data_t);
    }
    return 0;
}


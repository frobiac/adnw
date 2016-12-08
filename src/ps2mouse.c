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

volatile uint32_t    mouse_timer; /// toggle mouse mode for a specified time
volatile uint16_t    accel; /// toggle mouse mode for a specified time

/**
 * Setup PS/2 connection
 */
bool ps2_init_mouse(void)
{
    uint8_t rcv __attribute__((unused)); // only for debug output

    g_ps2_connected=1;
    g_cfg.mouse_enabled = 0;

#ifndef BLACKFLAT
    tp_reset();
#endif

    ps2_host_init();

    // send Reset
    rcv = ps2_host_send(0xFF); // printf("\nps2 RST: %02x %02x", rcv, ps2_error);
    if(rcv != PS2_ACK)
        return false;

    // read completion code = AA
    rcv = ps2_host_recv_response(); // printf("\nps2 BAT: %02x %02x", rcv, ps2_error);
    // @TODO check for error (FC 00 instead of AA 00) (p9: 2.3.1 RESET)

    // read I.D. = 00
    rcv = ps2_host_recv_response(); // printf("\nps2 DEV: %02x %02x", rcv, ps2_error);

    // send Set Remote mode
    rcv = ps2_host_send(0xF0); // printf("\nps2 REM: %02x %02x", rcv, ps2_error);

    // Enable Data reporting (not needed for remote mode)
    // rcv = ps2_host_send(0xF4); //printf("\nps2 REP: %02x %02x", rcv, ps2_error);
    //
    // disable external PS/2 device: not required in remote mode
    // ps2_host_send(TP_COMMAND); ps2_host_send(TP_DISABLE_EXT);

    // Further options: 0xE8 (resolution), 0xF3 (sample rate)

    if(! tp_id())
        return false;

    if(! tp_init()) {
        g_ps2_connected=0;
        return false;
    };

    /// @todo Set only on successful init
    g_cfg.mouse_enabled = 1;

    return true;
}

/*
 *
 */
#define X_IS_NEG  (mouseinf & (1<<4))
#define Y_IS_NEG  (mouseinf & (1<<5))
#define X_IS_OVF  (mouseinf & (1<<6))
#define Y_IS_OVF  (mouseinf & (1<<7))

void ps2_read_mouse(int8_t *dx, int8_t *dy, int8_t *BTNS )
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

            *dx=  X_IS_NEG ?
                  ((!X_IS_OVF && -127 <= *dx && *dx <= -1) ?  *dx : -127) :
                  ((!X_IS_OVF && 0 <= *dx && *dx <= 127) ? *dx : 127);
            *dy=  Y_IS_NEG ?
                  ((!Y_IS_OVF && -127 <= *dy && *dy <= -1) ?  *dy : -127) :
                  ((!Y_IS_OVF && 0 <= *dy && *dy <= 127) ? *dy : 127);

            return;
        } else {
            printf("\nERR read_mouse %0x", rcv);
        }
    }
}

/**
 * @brief getMouseReport
 * @param MouseReport
 * @return 0 unless mouse report data was changed
 */
uint8_t getMouseReport(USB_WheelMouseReport_Data_t *MouseReport)
{
    if(!g_cfg.mouse_enabled)
        return 0;

    int8_t dx=0, dy=0;
    int8_t ps2_buttons=0;

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
        }
        return 0; // no data to send
    }

    if(g_mouse_keys_enabled || ps2_buttons) {
        // factor = 1 + [0..400]*1.5/400 = 1..2.5
        //factor= 1 + accel * (ACC_MAX-1) / ACC_RAMPTIME;
        if(g_cfg.swapxy) {
            // swapxy does not work with trackpoint.
            int8_t tmp;
            tmp = dx;
            dx = dy;
            dy = tmp;
        }
        // keyboard mouse buttons only in mousemode
        if( (ps2_buttons & 0x05)==0x05 || (g_mouse_keys & 0x08)) {
            MouseReport->X=0;
            MouseReport->Y=0;
            // Divide by 8 to slow down scrolling with higher sensitivities and speed
            MouseReport->H = dx>>3;
            MouseReport->V = -dy>>3;
        } else { // no scroll-wheel support or not active
            MouseReport->X = dx;
            MouseReport->Y = dy;
            MouseReport->Button = g_mouse_keys & ~(0x08); // do not emit the scroll button
            MouseReport->Button |= ps2_buttons;           // PS/2 buttons if set
        }
        g_mouse_keys=0;

        return sizeof(USB_WheelMouseReport_Data_t);
    }
    return 0;
}


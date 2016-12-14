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


// see drivers/input/mouse/trackpoint.h in linux source for config registers
// or
// wwwcssrv.almaden.ibm.com/trackpoint/files/YKT3Eext.pdf

/// @TODO: configure based on model detected?

#include <util/delay.h>

#include "trackpoint.h"

void tp_reset()
{
    PS2_RESET_DDR |= (1 << PS2_RESET_BIT);
    PS2_RESET_PORT |= (1 << PS2_RESET_BIT);
    _delay_us(150 /*PS2_DELAY*/);
    PS2_RESET_PORT &= ~(1 << PS2_RESET_BIT);
    return;
}



void tp_ram_toggle(uint8_t addr, uint8_t mask)
{
    /*
    // Check prior to toggle
    uint8_t tmp;
    ps2_host_send(0xe2);
    ps2_host_send(addr);
    tmp = ps2_host_recv_response();
    if( (tmp & mask) != 0x00) {
        printf("\nToggle %02x -> %02x", tmp, mask);
    }
    */

    ps2_host_send(0xe2);
    ps2_host_send(0x47);
    ps2_host_send(addr);
    ps2_host_send(mask);
}

uint8_t tp_ram_read(uint8_t addr)
{
    ps2_host_send(0xe2);
    ps2_host_send(0x80);
    return ps2_host_send(addr);
}

void tp_ram_write(uint8_t addr, uint8_t val)
{
    ps2_host_send(0xe2);
    ps2_host_send(0x81);
    ps2_host_send(addr);
    ps2_host_send(val);
}

// TP config register: See p33 of YKT3Eext.pdf
enum { TP_PTS=0, TP_RES, TP_BTN2, TP_FLIPX, TP_FLIPY, TP_FLIPZ, TP_SWAPXY, TP_FTRANS };

/**
 * Print out trackpoint config register
 * read config byte at 2C: E2 2C or E2 80 2C
 * bit  0   1   2    3    4    5    6    7
 * Pts res 2clk invX invY invZ ExXY HardTrans
 */
uint8_t tp_read_config()
{
    //@TODO: check return status in whole file and abort
    //       KB must work without trackpoint connected even when activated!
    uint8_t config;
    ps2_host_send(0xe2);
    ps2_host_send(0x2c);
    config = ps2_host_recv_response();
    printf("\nTP cfg=0x%02x", config);
    return config;
}

/** Read id from trackpoint.
 *  smaller TP: 2nd ID=010e  Ext.ID= M 19990623($IBM3780)
 */
void tp_id(void)
{
    uint8_t tmp='0';

    // read secondary ID
    tmp=ps2_host_send(0xE1);
    tmp=ps2_host_recv_response();
    printf("\n2nd ID=%02x", tmp);
    tmp=ps2_host_recv_response();
    printf("%02x", tmp);

    // read extended ID, which ends in ')'
    tmp=ps2_host_send(0xD0);
    tmp=ps2_host_recv_response();
    printf("\nExt.ID=%c", tmp);
    while(tmp != (uint8_t)')') {
        tmp=ps2_host_recv_response();
        printf("%c",tmp);
    }
    printf("\n");
}

/** Set TrackPoint sensitivity.
 * Tested values were 0x60 for RedTilt, 0x40 for BlackFlat.
 *
 * @return Old sensitivity setting.
 */
uint8_t tp_sensitivity(uint8_t sens)
{
    uint8_t old = tp_ram_read(0x4A);
    tp_ram_write(0x4A, sens);
    return old;
}

bool tp_init(void)
{
    /* RAM locations:
     * - Read with E2 80 ADDR
     * - Read with E2 81 ADDR VAL
     *
     * 41,42,43 pts btn mask
     * 5C PtS thres
     * 4A sensitivity
     * 60 speed
     */
    g_tp_sens = TP_SENS_DEF;
    g_tp_sens_low = TP_SENS_LOW;

    // setup PressToSroll by enabling PTS, setting button masks and increasing threshold
    tp_ram_toggle(0x2c, (1<<TP_PTS) );
    tp_ram_write(0x41, 0xff);
    tp_ram_write(0x42, 0xff);

    // sensitivity, speed
    tp_ram_write(0x4A, g_tp_sens);
#ifdef REDTILT
    tp_ram_write(0x60, 0x53);
    tp_ram_write(0x5c, 0x0A); // 08 is default, 10 too hard
#elif defined BLACKFLAT
    // activate higher sensitivity via PtS
    tp_ram_write(0x60, 0x53);
    tp_ram_write(0x5c, 0x09);
#endif

    uint8_t tp_config = tp_read_config();
#ifdef TP_PTS_ENABLE
    tp_config |= (1<<TP_PTS);
#endif
#ifdef TP_FLIP_X
    tp_config |= (1<<TP_FLIPX);
#endif
#ifdef TP_FLIP_Y
    tp_config |= (1<<TP_FLIPY);
#endif
#ifdef TP_SWAP_XY
    tp_config |= (1<<TP_SWAPXY);
#endif

    // now write back config
    tp_ram_write(0x2c, tp_config);

    return true;
}

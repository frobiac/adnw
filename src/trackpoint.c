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

#include <util/delay.h>

#include "trackpoint.h"

void tp_reset()
{
    RDDR |= (1 << RBIT);
    RPORT |= (1 << RBIT);
    _delay_us(PS2_DELAY);
    RPORT &= ~(1 << RBIT);
    return;
}



void tp_ram_toggle(uint8_t addr, uint8_t val)
{
    tp_send_read_ack(0xe2);
    tp_send_read_ack(0x2c);
    uint8_t tmp=read_packet();
    if( (tmp & val) != 0x00) {
        //printf("\nAlready set");
    }

    tp_send_read_ack(0xe2);
    tp_send_read_ack(0x47);
    tp_send_read_ack(addr);
    tp_send_read_ack(val);
}

uint8_t tp_ram_read(uint8_t addr)
{
    tp_send_read_ack(0xe2);
    tp_send_read_ack(0x80);
    tp_send_read_ack(addr);
    return( read_packet() );
}

void tp_ram_write(uint8_t addr, uint8_t val)
{
    tp_send_read_ack(0xe2);
    tp_send_read_ack(0x81);
    tp_send_read_ack(addr);
    tp_send_read_ack(val);
}

bool tp_send_read_ack(uint8_t val)
{
    return ps2_send_expect(val, 0xfa);
}


// TP config register: See p33 of YKT3Eext.pdf
enum { TP_PTS=0, TP_RES, TP_BTN2, TP_FLIPX, TP_FLIPY, TP_FLIPZ, TP_SWAPXY, TP_FTRANS };

/**
 * Print out trackpoint config register
 */
uint8_t tp_read_config()
{
    tp_send_read_ack(0xe2);
    tp_send_read_ack(0x2c);
    uint8_t config = read_packet();
    printf("TP cfg=0x%02x\n", config);
    return config;
}

/** Read id from trackpoint.
 *  smaller TP: 2nd ID=010e  Ext.ID= M 19990623($IBM3780)
 */
void tp_id(void)
{
    // read secondary ID
    if( tp_send_read_ack(0xe1) ) {
        uint8_t id[2];
        id[0]=read_packet();
        id[1]=read_packet();
        printf("2nd ID=%02x%02x\nExt.ID=", id[0],id[1]);
    }
    // read extended ID, which ends in ')'
    if( tp_send_read_ack(0xd0) ) {
        uint8_t tmp='0';
        while(tmp != (uint8_t)')') {
            tmp=read_packet();
            printf("%c",tmp);
        }
        printf(")\n");
    }
}


bool tp_init(void)
{
    // read config byte at 2C: E2 2C or E2 80 2C
    /* bit  0   1   2    3    4    5    6    7
            Pts res 2clk invX invY invZ ExXY HardTrans  */
    if(!tp_read_config())
        return false;
    tp_ram_toggle(0x2c, (1<<TP_PTS) );
    tp_read_config();

    /* RAM locations:
     * - Read with E2 80 ADDR
     * - Read with E2 81 ADDR VAL
     *
     * 41,42,43 pts btn mask
     * 5C PtS thres
     * 4A sensitivity
     * 60 speed
     */

    // setup PressToSroll by enabling PTS, setting button masks and increasing threshold
    //printf("\nPTS btn masks: %02x %02x %02x ", tp_ram_read(0x41), tp_ram_read(0x42), tp_ram_read(0x43) );
    tp_ram_write(0x41, 0xff);
    tp_ram_write(0x42, 0xff);
    //printf("\nPTS btn masks: %02x %02x %02x ", tp_ram_read(0x41), tp_ram_read(0x42), tp_ram_read(0x43) );
    tp_ram_write(0x5c, 0x0A); // 08 is default, 10 too hard
    //printf("\nPTS thres: %02x", tp_ram_read(0x5c));

    //printf("\nSens/Speed (orig): %02x %02x ", tp_ram_read(0x4A), tp_ram_read(0x60) );
#ifdef REDTILT
    tp_ram_write(0x4A, 0x60);
    tp_ram_write(0x60, 0x53);
#endif

    // flip axis
    // tp_ram_write(0x2c, 1<<TP_FLIPX);
    // tp_ram_write(0x2c, 1<<TP_FLIPY);

    //printf("\nSens  (orig=80): %02x ", tp_ram_read(0x4A) );
    //printf("\nSpeed (orig=61): %02x ", tp_ram_read(0x60) );
    return true;
}

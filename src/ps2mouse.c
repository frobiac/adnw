/*********************************************************************************
 * Title: PS/2 Library
 * Author: Akshay Srinivasan<akshaysrinivasan@nitk.ac.in>
 * Date: 22 June 2009
 * Software: AVR-GCC 4.3.2

 *-------------------------------------------------------------------------------------
 * "THE BEER-WARE LICENSE" (Revision 42):
 * <akshaysrinivasan@gmail.com> wrote this file.  As long as you retain this notice you
 * can do whatever you want with this stuff. If we meet some day, and you think
 * this stuff is worth it, you can buy me a beer in return.        Akshay Srinivasan
 *-------------------------------------------------------------------------------------

 DESCRIPTION
    This library provides an abstraction layer for the PS/2 protocol. This library
    provides functions for receiving and sending bytes via PS/2. This library can
    be used to read and write data in PS/2 devices, although interpreting the data
    is left for you to do.

    For more information goto,
    http://computer-engineering.org/ps2protocol/
**********************************************************************************/

#include <avr/io.h>
#include <util/delay.h>
#include <stdlib.h>

#include "Keyboard.h"
#include "ps2mouse.h"

#define ACK 0
#define DELAY 150

static uint8_t scrollcnt=0;


void data(uint8_t x) {
    DDDR |= (1 << DBIT);
    if(x==0)
        DPORT &= ~(1 << DBIT);
    else if(x==1)
        DPORT |= (1 << DBIT);
    return;
}

void clk(uint8_t x) {
	CDDR |= (1 << CBIT);
	if(x==0)
		CPORT &= ~(1 << CBIT);
	else if(x==1)
		CPORT |= (1 << CBIT);
	return;
}

void tp_reset(){

    RDDR |= (1 << RBIT);
    RPORT |= (1 << RBIT);
    _delay_us(DELAY);
    RPORT &= ~(1 << RBIT);

    return;

    while(CLK){
        __asm__("nop");
    }
    while(!CLK) {
        __asm__("nop");
    }
    while(CLK){
        __asm__("nop");
    }
    while(!CLK) {
        __asm__("nop");
    }
    RPORT &= ~(1 << RBIT);
}


#define CNT 500
void serout(uint8_t bit) {
    uint8_t cnt;
    cnt=0;
    while(CLK && cnt++ < CNT){
        __asm__("nop");
    }
    data(bit);
    cnt = 0;
    while(!CLK && cnt++ < CNT) {
        __asm__("nop");
    }
}

uint8_t serin() {
    uint8_t state;
    uint8_t cnt;
    cnt=0;
    while(CLK && cnt++ < CNT){
        __asm__("nop");
    }
    state = DATA;
    cnt = 0;
    while(!CLK && cnt++ < CNT){
        __asm__("nop");
    }
    return state;
}

uint8_t oparity(uint8_t byte) {
    uint8_t par=1;
    par ^= ((byte & (1 << 0)) >> 0);
    par ^= ((byte & (1 << 1)) >> 1);
    par ^= ((byte & (1 << 2)) >> 2);
    par ^= ((byte & (1 << 3)) >> 3);
    par ^= ((byte & (1 << 4)) >> 4);
    par ^= ((byte & (1 << 5)) >> 5);
    par ^= ((byte & (1 << 6)) >> 6);
    par ^= ((byte & (1 << 7)) >> 7);
    return par;
}



bool send_packet(uint8_t byte) {

    uint8_t parity;

    errcnt=0;
    do {
    parity = oparity(byte);
    clk(0);
    _delay_us(DELAY);
    data(0); //Start
    clk(1);
    CDDR &= ~(1 << CBIT); // Release clock
    CPORT |= (1 << CBIT); //Set the pull up on Clock

    /////////////
    serout((byte & (1 << 0)) >> 0);
    serout((byte & (1 << 1)) >> 1);
    serout((byte & (1 << 2)) >> 2);
    serout((byte & (1 << 3)) >> 3);
    serout((byte & (1 << 4)) >> 4);
    serout((byte & (1 << 5)) >> 5);
    serout((byte & (1 << 6)) >> 6);
    serout((byte & (1 << 7)) >> 7);

/////////////
    serout(parity);
    /////////////
    serout(1); //Stop

    DDDR &= ~(1 << DBIT); //Release the Data line
    DPORT |= (1 << DBIT); //Set the pull up on Data


    /////////////
    //if(serin() != ACK )
    //    send_packet(byte); // Try again if ACK has not been received
    errcnt++;

    } while (serin() != ACK && errcnt < 5 );

    if(errcnt >= 5)
        return false;
    return true;
}

uint8_t read_packet(void) {
    uint8_t byte=0,par;
    serin(); //Start
    byte |= (serin() << 0);
    byte |= (serin() << 1);
    byte |= (serin() << 2);
    byte |= (serin() << 3);
    byte |= (serin() << 4);
    byte |= (serin() << 5);
    byte |= (serin() << 6);
    byte |= (serin() << 7);
    par = serin(); //Parity
    serin(); //Stop

    if(par != oparity(byte))
        send_packet(0xfe); //Resend

    return byte;
}


bool ps2_init_mouse(void) {
	g_trackpoint = 0;

	tp_reset();

	if ( ! send_packet(0xff) )
		return false;
	read_packet(); //Ack
	read_packet(); //Bat
	read_packet(); //dev ID
	//Enable Data reporting
	if ( !send_packet(0xf4) )
		return false;
	read_packet();	// Ack
	////
	//send_packet(0xe8); //Set Resolution
	//read_packet(); //Ack
	//send_packet(0x01); //8counts/mm
	//read_packet(); //Ack
	////
	//send_packet(0xf3); //SetSample rate
	//read_packet(); //Ack
	//send_packet(0x64); //200 smaples a second

	//Set remote mode
	if( ! send_packet(0xf0) )
		return false;
	read_packet(); //Ack

	return true;
}


/*
 *
 */
void ps2_read_mouse(int *dx, int *dy, uint8_t *BTNS )
{

        uint8_t ack;
        uint8_t LMB,MMB,RMB;
	int mouseinf;
	{
		send_packet(0xeb);
		ack=read_packet(); //Ack
		if(ack==0xfa) 
		{
			mouseinf=read_packet();
			*dx= read_packet();
			*dy= read_packet();

			// raw *dx is of 0xXX
			// int x = *dx; int y = *dy;

			if(mouseinf&0x10)
				*dx-=0x100; // Add sign bit to dx
			if(mouseinf&0x20)
				*dy-=0x100; // Add sign bit to dy

			/*
			if( x!=0 ){
				//printf("\n%4x %4x | %4x %4x", x,*dx,y,*dy);
				printf("\nX %4x %4x: ", x,*dx);
				if(*dx&0xFF00)
					printf("-%d" , 256-(*dx+0x100));
				else
					printf("+%d" , *dx);
			}
			*/

			LMB=0;MMB=0;RMB=0;

                        if(mouseinf & 0x01)  //0x09
                            LMB=1;         // Get leftbutton status
                        if(mouseinf & 0x02)
                            RMB=1;        // Get rightbutton status
                        if(mouseinf & 0x04)
                            MMB=1;       // Get middlebutton status


/*			// emulate 3 buttons
			if( RMB & LMB ) {
				MMB=1;
				RMB=LMB=0;
			}
*/
			*BTNS = (LMB<<3) | (MMB<<2) | (RMB << 1);
		}
	}
}

volatile uint32_t mouse_timer; /// toggle mouse mode for a specified time

uint8_t getMouseReport(USB_MouseReport_Data_t *MouseReport)
{
	int16_t dx=0, dy=0;
	uint8_t btns=0;

#ifdef PS2MOUSE
    if(g_trackpoint){
        ps2_read_mouse(&dx, &dy, &btns);
        //printf("\nBtns: %d ", btns);
    }
#endif

    // reset mouse mode after inactivity: idle_count is incremented 61 times per second
    if( dx!=0 || dy!=0 ) {
        g_mouse_mode=1;
        mouse_timer=idle_count;
    } else if(idle_count-mouse_timer > 1/*seconds*/ *61 ) {
        g_mouse_mode=0;
    }
    
	led((g_mouse_mode!=0));
    if(g_mouse_mode) {
        if( g_mouse_keys & 0x08 )
        {
            int8_t sx=0, sy=0;

            if( dx!=0 ){
                if(dx&0xFF00)
                    sx= -(256-(dx+0x100));
                else
                    sx=dx;
            }
            if( dy!=0 ){
                if(dy&0xFF00)
                    sy= -(256-(dy+0x100));
                else
                    sy=dy;
            }

            MouseReport->Button=0;

            scrollcnt = scrollcnt+abs(sy)+abs(sx);

            if(scrollcnt>40){
                scrollcnt=0;
                MouseReport->X=0;
                MouseReport->Y=0;
                // only move by 1 ?!
                MouseReport->V = sx;
                MouseReport->H = -sy;
                MouseReport->Button=0;
            }
        } else {
            MouseReport->V=0;
            MouseReport->H=0;
            // original
            // MouseReport->Y = -dy;
            // MouseReport->X = dx;

            // rotated clockwise 90°
            MouseReport->Y = dx;
            MouseReport->X = dy;
            MouseReport->Button=g_mouse_keys & ~(0x08);
        }
        g_mouse_keys=0;

        return sizeof(USB_MouseReport_Data_t);

    } else {
        // no g_mouse_mode;
        int8_t old = g_mouse_modifier;

        // reset after a certain time
        if(dx==0 && dy==0) {
            if( g_tp_counter++ > 12 ){
                g_mouse_modifier=0;
                g_tp_counter=0;
                for(int i=0; i<4;++i) {
                    g_mouse_mode_sum[i] = 0;
                }
            }
        }

        if( dx > 0 )
            g_mouse_mode_sum[0] += dx;
        else if( dx & 0xFF00 )
            g_mouse_mode_sum[1] += (256-(dx+0x100)) ;
        if( dy > 0 )
            g_mouse_mode_sum[2] += dy;
        else if( dy & 0xFF00)
            g_mouse_mode_sum[3] += (256-(dy+0x100));


        uint8_t maxid;
        for(int i=1; i<4;++i) {
            if(g_mouse_mode_sum[i]>g_mouse_mode_sum[maxid]){
                maxid=i;
            }
        }

        if( g_mouse_mode_sum[maxid] > 30)
            g_mouse_modifier=(1<<maxid);

       // if(old != g_mouse_modifier)
       //     printf("\n%5d %5d %5d %5d : %d -> %d | ",g_mouse_mode_sum[0],g_mouse_mode_sum[1],g_mouse_mode_sum[2],g_mouse_mode_sum[3], old, g_mouse_modifier );

    }
    return 0;
}

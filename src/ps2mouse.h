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

#include <stdbool.h>

uint8_t     g_trackpoint;

void ps2_read_mouse(int *dx, int *dy, uint8_t *BTNS );
bool ps2_init_mouse(void);


#ifndef F_CPU
#define F_CPU 16000000UL
#endif

//Data
#define DAPORT C
#define DBIT  6

//Clock
#define CLKPORT E
#define CBIT  6

//reset trackpoimt
#define RSTPORT C
#define RBIT 7

/////////////////////////////
///General purpose macros////
#define CONCAT(a,b)     a ## b
#define CCPORT(name)    CONCAT(PORT,name)
#define CCPIN(name)     CONCAT(PIN,name)
#define CCDDR(name)     CONCAT(DDR,name)
/////////////////////////////

#define DPORT   CCPORT(DAPORT)
#define DPIN    CCPIN(DAPORT)
#define DDDR    CCDDR(DAPORT)

#define CPORT   CCPORT(CLKPORT)
#define CPIN    CCPIN(CLKPORT)
#define CDDR    CCDDR(CLKPORT)

#define RPORT   CCPORT(RSTPORT)
#define RPIN    CCPIN(RSTPORT)
#define RDDR    CCDDR(RSTPORT)
//////////////////////////////

#define DATA ((DPIN & (1 << DBIT)) >> DBIT)
#define CLK  ((CPIN & (1 << CBIT)) >> CBIT)
#define RESET ((RPIN & (1 << RBIT)) >> RBIT)))
///////////////////////////////
/*
#define DELX (dapack[1]-(((dapack[0] & (1 << 4)) >> 4) << 8))
#define DELY (dapack[2]-(((dapack[0] & (1 << 5)) >> 5) << 8))
*/

bool send_packet(uint8_t);
uint8_t read_packet(void);

void reset(void);
void tp_reset(void);

void serout(uint8_t bit);
uint8_t serin(void);

uint8_t getMouseReport(USB_MouseReport_Data_t *report_data);

uint8_t errcnt;

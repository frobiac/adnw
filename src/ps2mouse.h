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

#ifndef PS2MOUSE_H
#define PS2MOUSE_H

#include <stdbool.h>
#include "Keyboard.h"  // for USB_Mouse_Report_Data_t
#include "config.h"

#define PS2_ACK 0xFA
#define DELAY 150

#define ACC_RAMPTIME 400 // acc incrementation time till maximum
#define ACC_MAX      2.5 // maximum accelleration factor reachable


#ifdef BLUECUBE
    #define DAPORT  B
    #define DBIT    7
    #define CLKPORT E
    #define CBIT    6
    #define RSTPORT B
    #define RBIT    3

#elif defined HYPERNANO || defined REDTILT
    #define DAPORT  B
    #define DBIT    1
    // d4 to break
    #define CLKPORT B
    #define CBIT    2
    #define RSTPORT B
    #define RBIT    0

#else
    #error "Trackpoint defined only for BLUECUBE and HYPER"
#endif

/////////////////////////////
///General purpose macros////
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


uint8_t errcnt;
uint8_t     g_mouse_enabled; ///< >0 if mouse/trackpoint is active and reports shall be generated

void ps2_read_mouse(int *dx, int *dy, uint8_t *BTNS );
bool ps2_init_mouse(void);

bool     send_packet(uint8_t byte);
uint8_t  read_packet(void);
bool ps2_send_expect(uint8_t send, uint8_t expect);

uint8_t getMouseReport(USB_WheelMouseReport_Data_t *report_data);


#endif // PS2MOUSE_H

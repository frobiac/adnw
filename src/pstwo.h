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

#include "config.h" // data, clock and reset ports/pins
#include <LUFA/LUFA/Common/Common.h>

#define PS2_DELAY 150

#define DATA ((PS2_DATA_PIN & (1 << PS2_DATA_BIT)) >> PS2_DATA_BIT)
#define CLK  ((PS2_CLOCK_PIN & (1 << PS2_CLOCK_BIT)) >> PS2_CLOCK_BIT)
#define RESET ((PS2_RESET_PIN & (1 << PS2_RESET_BIT)) >> PS2_RESET_BIT)))
///////////////////////////////

/*
#define DELX (dapack[1]-(((dapack[0] & (1 << 4)) >> 4) << 8))
#define DELY (dapack[2]-(((dapack[0] & (1 << 5)) >> 5) << 8))
*/

bool send_packet(uint8_t);
uint8_t read_packet(void);
uint8_t serin(void);


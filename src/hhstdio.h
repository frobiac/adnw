/*
                   The HumbleHacker Keyboard Project
                 Copyright © 2008-2010, David Whetstone
               david DOT whetstone AT humblehacker DOT com

  This file is a part of The HumbleHacker Keyboard Project.

  The HumbleHacker Keyboard Project is free software: you can redistribute
  it and/or modify it under the terms of the GNU General Public License as
  published by the Free Software Foundation, either version 3 of the
  License, or (at your option) any later version.

  The HumbleHacker Keyboard Project is distributed in the hope that it will
  be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General
  Public License for more details.

  You should have received a copy of the GNU General Public License along
  with The HumbleHacker Keyboard Project.  If not, see
  <http://www.gnu.org/licenses/>.

*/

#ifndef __HHSTDIO_H__
#define __HHSTDIO_H__

#include <stdio.h>
#include <stdbool.h>
#include <LUFA/Drivers/USB/Class/Device/HIDClassDevice.h>


void stdio_init(void);
void stdio_fill_report(char ch, USB_KeyboardReport_Data_t *report);
bool stdout_is_empty(void);
bool stdout_is_full(void);
char stdout_popchar(void);

//static const USB_KeyboardReport_Data_t ascii_table[];

/*
   The largest HID usage code is AC_Distribute_Vertically from the Consumer
   Control usage page.  Its id is 0x29c, which requires 10 bits.  The largest
   usage page used by a keyboard, on the other hand, is 0x0c, which requires
   only 4 bits.  So we pack the usage id in the lower 10 bits of a 16 bit int,
   and the usage page in the upper 6 bits.
*/

#define MAKE_USAGE(page,id) (((page)<<10)|(id))
#define USAGE_ID(usage)     (usage&~(0xffff<<10))
#define USAGE_PAGE(usage)   (usage>>10)

typedef uint8_t  UsagePage;
typedef uint8_t  UsageID;


#endif // __HHSTDIO_H__

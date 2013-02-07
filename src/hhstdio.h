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

#include "hid_usage.h"

#include <stdbool.h>
#include <LUFA/Drivers/USB/Class/Device/HIDClassDevice.h>


void stdio_init(void);
void stdio_fill_report(char ch, USB_KeyboardReport_Data_t *report);
bool stdout_is_empty(void);
bool stdout_is_full(void);
char stdout_popchar(void);

#endif // __HHSTDIO_H__

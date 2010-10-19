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

#ifndef __IDENTIFIERS_H__
#define __IDENTIFIERS_H__

#define MANUFACTURER_NAME     L"frobiac"
#define MANUFACTURER_NAME_LEN USB_STRING_LEN(7)

#define PRODUCT_NAME          L"Left Numpad blue=C0 red=A7"
#define PRODUCT_NAME_LEN      USB_STRING_LEN(26)

#define SERIAL_NUMBER         L"000000000000"
#define SERIAL_NUMBER_LEN     USB_STRING_LEN(12)

#define VENDOR_ID             0x05AF
#define PRODUCT_ID            0x2031
#define RELEASE_NUMBER        0xF000

#endif // __IDENTIFIERS_H__

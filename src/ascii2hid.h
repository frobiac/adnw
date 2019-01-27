/*
    This file is part of the AdNW keyboard firmware.

    Copyright 2010-2019 Stefan Fröbe, <frobiac /at/ gmail [d0t] com>

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


/**
 * Return ascii char of given hid code and modifier combination.
 *
 * Only ASCII printable characters - no umlauts or other specials like € or °.
 *
 * @arg hid HID code to map to ascii
 * @arg mod modifier necessary [SHIFT|ALTGR]
 *
 * @ret printable ascii char if found, or '\0' on unprintable characters
 *
 * @todo backslash does it work?
 * @todo handle ESC, BS and the like?
 */

#pragma once

#include <stdio.h>
#include "hid_usage.h"

// @TODO @see https://github.com/Nurrl/LocaleKeyboard.js/blob/master/locales/de_DE.lang

//#define TABLE // 20260 / 775
//#undef  TABLE // 20466 / 519
#undef USE_FUNCTION_FOR_LOOKUP


void ascii2hid(uint8_t ascii, uint8_t *hid, uint8_t *mod);

/**
 * Return ascii char of given hid code and modifier combination.
 *
 * Only ASCII printable characters - no umlauts or other specials like € or °.
 *
 * @arg hid HID code to map to ascii
 * @arg mod modifier necessary [SHIFT|ALTGR]
 *
 * @ret printable ascii char if found, or '\0' on unprintable characters
 *
 * @todo backslash does it work?
 * @todo handle ESC, BS and the like?
 */
char hid2asciicode(uint8_t hid, uint8_t mod);


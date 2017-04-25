/*
    This file is part of the AdNW keyboard firmware.

    Copyright 2017 Stefan Fröbe, <frobiac /at/ gmail [d0t] com>

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


#include <stdint.h>
#include "hid_usage.h"
#include "extra.h"
#include "print.h"

/** Suspend with System Control 0x82 = 130 works under linux.
 *  WakeUp with any key.
 *
 */
uint8_t getExtraReport(USB_ExtraReport_Data_t *ExtraReport)
{
//    if(g_extra_data==0)
//        return 0;

    ExtraReport->report_id = REPORT_ID_CONSUMER;

    if(g_extra_data >= 0x81 || g_extra_data <= 0xB7 )
        ExtraReport->report_id = REPORT_ID_SYSTEM;

    ExtraReport->usage     = g_extra_data;

    g_extra_data = 0;

    return sizeof(USB_ExtraReport_Data_t);
}


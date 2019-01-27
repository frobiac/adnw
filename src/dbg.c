/*
    This file is part of the AdNW keyboard firmware.

    Copyright 2010-2015 Stefan Fröbe, <frobiac /at/ gmail [d0t] com>

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


#include "dbg.h"
#include "hhstdio.h"

uint8_t
DBG__get_report(USB_DBGReport_Data_t *report_data)
{
    /// @todo Remove DBGReport altogether if no debug
#ifdef DEBUG_OUTPUT
    if (!stdout_is_empty()) {
        char ch;
        int index = 0;
        while(index < DBG_EPSIZE && (ch = stdout_popchar())) {
            report_data->data[index++] = ch;
        }
    }

    return sizeof(USB_DBGReport_Data_t);
#else
    return 0;
#endif
}

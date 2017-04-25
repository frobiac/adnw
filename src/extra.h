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

#pragma once

/* report id */
#define REPORT_ID_MOUSE     1
#define REPORT_ID_SYSTEM    2
#define REPORT_ID_CONSUMER  3

uint16_t g_extra_data;

/* extra report structure */
typedef struct {
    uint8_t  report_id;
    uint16_t usage;
} __attribute__ ((packed)) USB_ExtraReport_Data_t;
// } __attribute__ ((packed)) report_extra_t; -TMK
/*
typedef struct {
    unsigned Play           : 1;
    unsigned Pause          : 1;
    unsigned FForward       : 1;
    unsigned Rewind         : 1;
    unsigned NextTrack      : 1;
    unsigned PreviousTrack  : 1;
    unsigned Stop           : 1;
    unsigned PlayPause      : 1;
    unsigned Mute           : 1;
    unsigned VolumeUp       : 1;
    unsigned VolumeDown     : 1;
    unsigned RESERVED       : 5;
} __attribute__ ((packed)) USB_ExtraReport_Data_t;
*/
uint8_t getExtraReport(USB_ExtraReport_Data_t *ExtraReport);



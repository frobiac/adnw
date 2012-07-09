/*
    Copyright 2010-2012 Stefan Fröbe, <frobiac /at/ gmail [d0t] com>

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

#include "command.h"
#include "hhstdio.h"
#include "hid_usage.h"
#include "keyboard_class.h"

bool command=false;

void setCommandMode(bool on) {
    printf("\nCommandMode %s", on ? "active" : "off" );
    command=on;
    clearActiveKeys();
}


bool commandMode(void){ return command; }


void handleCommand(void){
    if(!commandMode())
        return;

    if(activeKeys.keycnt==0)
        return;

    struct Key k=activeKeys.keys[0];
    uint8_t hid =getKeyCode(k.row, k.col, 0);
    clearActiveKeys();

    //if(hid == HID_ESC)
    //    setCommandMode(false);
    switch(hid){
        case HID_P:
            printf("\nPrint:"); break;
            clearActiveKeys();
        case HID_T:
            printf("\nTrackpoint:"); break;
        case HID_ESC:
            setCommandMode(false); break;
        default:
            printf("\nUnknown:");
            //setCommandMode(false);
            break;
    }
}



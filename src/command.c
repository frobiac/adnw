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
#include "keymap.h"
#include "jump_bootloader.h"
#include "trackpoint.h"

bool command=false;

void setCommandMode(bool on) {
    printf("\nCMD %s ", on ? "active [qtp] " : "off" );
    command=on;
    clearActiveKeys();
}


bool commandMode(void){ return command; }

static uint8_t layer=0;

void handleCommand(void){
    if(!commandMode())
        return;

    if(activeKeys.keycnt==0)
        return;

    struct Key k=activeKeys.keys[0];
    uint8_t hid =getKeyCode(k.row, k.col, 0);
    clearActiveKeys();
    clearRowData();

    //if(hid == HID_ESC)
    //    setCommandMode(false);
    switch(hid){
        case HID_J:
            printf("\nBootloader::");
            jump_bootloader();
            break;;
        case HID_P:
            printLayout(layer);
            layer=(layer+1)%LAYERS;
            if(layer==0)
                setCommandMode(false);
            break;
        case HID_T:
            printf("\nTrackpoint:");
            tp_id();
            setCommandMode(false);
            break;
        case HID_Q:
            setCommandMode(false);
            break;
        default:
            printf("\nUnknown:");
            //setCommandMode(false);
            break;
    }
}



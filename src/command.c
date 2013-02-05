/*
    Copyright 2010-2013 Stefan Fröbe, <frobiac /at/ gmail [d0t] com>

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
#include "hid_usage.h"
#include "keyboard_class.h"
#include "keymap.h"
#include "jump_bootloader.h"
#include "trackpoint.h"

bool command=false;
static uint8_t layer=0;

/// possible subcommands
enum {
    SUB_NONE=0,
    SUB_TP,
    SUB_READ,
    SUB_LAYOUT,
    SUB_GEOAREA,
    SUB_PC_MAC
};
static uint8_t subcmd;           ///< currently active subcommand

#define MAXLEN 20
static uint8_t idx =0;
static uint8_t str[MAXLEN];

void setCommandMode(bool on) {
    printf("\nCMD %s ", on ? "active [qtp] " : "off" );
    command=on;
    clearActiveKeys();
    subcmd = SUB_NONE;
}

bool commandMode(void) { return command; }
void handleSubCmd(struct Key k);

void PrintConfiguration() {
    // G:GeoArea umschalten
    printf("\nC:Config::\n");
    printf("\nLayout:  %u (0=QWERTZ_DE, 1=KO.,Y, 2=Malt90, 3=QWERTY_GB)",
           eeprom_read_byte(&alternateLayoutNr));
    printf("\nGeoArea: %u (0=DE,        1=GB,    2=US)",
           eeprom_read_byte(&alternateGeoArea));
    printf("\nMac/PC:  %u (0=PC,        1=Mac)\n", eeprom_read_byte(&altMacOrPC));
}

void handleCommand(void) {
    if(!commandMode())
        return;

    if(activeKeys.keycnt==0)
        return;

    struct Key k=activeKeys.keys[0];
    uint8_t hid = getKeyCode(k.row, k.col, 0);

    if(subcmd) {
        handleSubCmd(k);
        return;
    }
    clearActiveKeys();
    clearRowData();

    switch(hid) {
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
        case HID_L:
            g_alternateLayer=!g_alternateLayer;
            printf("\nAlternate layer %s", g_alternateLayer ? "selected." : "off.");
            setCommandMode(false);
            break;
        case HID_A:
            for(uint8_t i=32; i<255; ++i) {
                if(i%16==0)
                    printf("\n %03d", i);
                printf("%c ", (char)(i));
                if(i==127)
                    i+=33;
            }
            setCommandMode(false);
            break;
        case HID_M:
            g_mouse_enabled = g_mouse_enabled > 0 ? 0 : 1;
            printf("\nMouse %sabled", g_mouse_enabled ? "en" : "dis");
            setCommandMode(false);
            break;
        case HID_R:
            printf("\nRead active.");
            subcmd=SUB_READ;
            idx=0;
            break;
        default:
            printf("\nUnknown:");
            //setCommandMode(false);
            break;
    }
}

void handleSubCmd(struct Key k) {
    uint8_t h =getKeyCode(k.row, k.col, getActiveLayer());
    switch( subcmd )
    {
        case SUB_READ:
            if(h == HID_ESC) {
                printf("\nSubRead terminated");
                setCommandMode(false);
            } else {
                if(idx>=MAXLEN) {
                    idx=0;
                    setCommandMode(false);
                }
                str[idx++]=h;
                printf("\n%2d %d: ",idx,h);
                for(uint8_t i=0; i<idx; ++i)
                    printf("%02x", str[i]);
            }
            break;
        default:
            break;
    }
    clearActiveKeys();
    clearRowData();
}



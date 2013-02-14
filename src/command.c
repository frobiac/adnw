/*
    Copyright 2010-2013 Stefan Fr√∂be, <frobiac /at/ gmail [d0t] com>

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
#include "macro.h"

bool command=false;
static uint8_t layer=0;

/// possible subcommands
enum {
    SUB_NONE=0,
    SUB_TP,
    SUB_READ,
    SUB_LAYOUT,
    SUB_GEOAREA,
    SUB_PC_MAC,
    SUB_MACRO
};

static uint8_t subcmd;           ///< currently active subcommand

#define MAXLEN 20
static uint8_t idx =0;
static uint8_t str[MAXLEN];

void setCommandMode(bool on) {
    if(on!=command)
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

/** Called when command mode is active.
 *
 *  First pressed key is mapped to first layer defined and evaluated. Use only a-z and 0-9 for commands, others exit this mode. 
 *
 *  @todo: leave automatically on unknown command or timeout, or signal mode through leds.
 *
 */
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

    // Char with Meaning:
    //  A:ASCIIPrint, B:BootL, C:PrintConfig, G:GeoArea, H:HardwarePC/Mac, L: SwitchLayout, M:Macro, P:PrintLayout, Q:QuitCommand,  T: Trackpoint
    // Char without Meaning:
    //  O:MouseMode, R:PrintKeyHID
    switch(hid) {
        case HID_V:
            printf("\nAdNW %s", FW_VERSION);
            setCommandMode(false);
            break;
        case HID_C:
            PrintConfiguration();
            break;
        case HID_G:
            // G:GeoArea umschalten
            printf("\nG:GeoArea::");
            subcmd=SUB_GEOAREA;
            break;
        case HID_H:
            // HardwarePC/Mac umschalten
            printf("\nHardwarePC/Mac::");
            subcmd=SUB_PC_MAC;
            break;
        case HID_Q:
        case HID_ESC:
            printf("\nLeaving command mode::");
            setCommandMode(false);
            break;
        case HID_B:
            printf("\nBootloader::");
            jump_bootloader();
            break;
        case HID_P:
            // Print Layout: one layer per press on key 'p'
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
        case HID_L:
            g_alternateLayer=!g_alternateLayer;
            printf("\nAlternate layer %s", g_alternateLayer ? "selected." : "off.");
            setCommandMode(false);
            /*
            // Layout umschalten
            printf("\nSwitch layout::");
            subcmd=SUB_LAYOUT;
            */
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
            // print HID code of pressed key
            printf("\nHID code read active.");
            subcmd=SUB_READ;
            idx=0;
            break;
        case HID_X:
            subcmd=SUB_MACRO;
            printf("Macro mode true\n");
            break;
        default:
            printf("\nUnknown command.");
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
        case SUB_LAYOUT:
        {
            // das nächste Layout auf die gedrückte Spalte setzen, also 12 verschiedene Layouts verfügbar
            /*
            Layout nextLayout = k.col < LAYOUTS ? k.col : eeprom_read_byte (&alternateLayoutNr);
            eeprom_write_byte(&alternateLayoutNr, nextLayout);
            PrintConfiguration();
            */
            printf("\nLAYOUTS not yet implemented");
            setCommandMode(false);
            break;
        }
        case SUB_GEOAREA:
        {
            // den geografischen Bereich auf die gedrückte Spalte setzen, DE, GB und US verfügbar
            GeoArea nextAlternateGeoArea = k.col < 3 ? k.col : eeprom_read_byte (&alternateGeoArea);
            eeprom_write_byte(&alternateGeoArea, nextAlternateGeoArea);
            PrintConfiguration();
            setCommandMode(false);
            break;
        }
        case SUB_PC_MAC:
        {
            // die Hardware auf die gedrückte Spalte setzen, PC/Mac verfügbar
            MacOrPC  nextAltMacOrPC = k.col < 2 ? k.col : eeprom_read_byte (&altMacOrPC);
            eeprom_write_byte(&altMacOrPC, nextAltMacOrPC);
            PrintConfiguration();
            setCommandMode(false);
            break;
        }
        case SUB_MACRO:
            setMacroMode(true);
            activateMacro(k.row*ROWS+k.col);
            setCommandMode(false);
            break;
        default:
            break;
    }
    clearActiveKeys();
    clearRowData();
}



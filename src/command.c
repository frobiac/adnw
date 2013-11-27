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
#include "macro.h"

bool command=false;
// static uint8_t layer=0;

/// possible subcommands
enum {
    SUB_NONE=0,
    SUB_TP,
    SUB_LAYOUT,
    SUB_MACRO,
    SUB_MACRO_REC
};

static uint8_t subcmd;           ///< currently active subcommand

void setCommandMode(bool on) {
    if(on!=command)
        printf("\nCMD %s ", on ? "active [qtp] " : "off" );
    command=on;
    clearActiveKeys();
    subcmd = SUB_NONE;
}

bool commandMode(void) { return command; }
void handleSubCmd(struct Key k);

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
        case HID_D:
            g_pinkydrop = (++g_pinkydrop%2);
            printf("\nPinkydrop %d", g_pinkydrop);
            eeprom_write_byte(&ee_pinkyDrop, g_pinkydrop);
            setCommandMode(false);
            break;
        case HID_V:
            printf("\nAdNW %s", FW_VERSION);
            setCommandMode(false);
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
/*
        case HID_P:
            // Print Layout: one layer per press on key 'p'
            printLayout(layer);
            layer=(layer+1)%LAYERS;
            if(layer==0)
                setCommandMode(false);
            break;
*/
#ifdef MOUSE_HAS_SCROLL_WHEELS
        case HID_T:
            printf("\nTrackpoint:");
            tp_id();
            setCommandMode(false);
            break;
#endif
        case HID_L:
            g_alternateLayer=(++g_alternateLayer%2);
            eeprom_write_byte(&ee_alternateLayer,g_alternateLayer);
            printf("\nAlternate layer %s", g_alternateLayer ? "selected." : "off.");
            setCommandMode(false);
            break;
#ifdef PS2MOUSE
        case HID_M:
            g_mouse_enabled = g_mouse_enabled > 0 ? 0 : 1;
            printf("\nMouse %sabled", g_mouse_enabled ? "en" : "dis");
            setCommandMode(false);
            break;
#endif
        case HID_X:
            subcmd=SUB_MACRO;
            printf("Macro mode true\n");
            break;
        case HID_R:
            printf("Macro recording\n");
            subcmd=SUB_MACRO_REC;
            break;
        default:
            printf("\nUnknown command.");
            break;
    }
}

void handleSubCmd(struct Key k) {
    switch( subcmd )
    {
        case SUB_MACRO:
            setMacroMode(true);
            activateMacro(k.row*ROWS+k.col);
            setCommandMode(false);
            break;
        case SUB_MACRO_REC:
            setMacroRecording(1+k.row*ROWS+k.col);
            setCommandMode(false);
            break;
        default:
            setCommandMode(false);
            break;
    }
    clearActiveKeys();
    clearRowData();
}



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
#include "passhash/passhash.h"

#ifdef PS2MOUSE
    #include "trackpoint.h"
#endif
#include "macro.h"

bool command=false;

/// possible subcommands
enum {
    SUB_NONE=0,
    SUB_TP,
    SUB_LAYOUT,
    SUB_MACRO,
    SUB_MACRO_REC,
    SUB_PASSHASH
};

static uint8_t subcmd;           ///< currently active subcommand

void setCommandMode(bool on)
{
    if(on!=command)
        printf("CMD %s\n", on ? "on" : "off" );
    command=on;
    subcmd = SUB_NONE;
}

bool commandMode(void) { return command; }
void handleSubCmd(char c);


/** Called when command mode is active.
 *
 *  First pressed key is mapped to first layer defined and evaluated. Use only a-z and 0-9 for commands, others exit this mode.
 *
 *  @todo: leave automatically on unknown command or timeout, or signal mode through leds.
 *
 *  @return: bool whether we handled the command, e.g. false means print character normally
 *
 */
bool handleCommand(uint8_t hid_now, uint8_t mod_now)
{
    if( !commandMode() )
        return false;

    // generally return "true" below as we are in command mode which should not echo.

    static uint8_t hid_prev, mod_prev, act_prev;
    uint8_t act_now = activeKeys.keycnt;

    if((hid_now==hid_prev) && (mod_now==mod_prev) && (act_now==act_prev))
        return true;

    // ok, something changed:
    uint8_t hid = hid_prev;
    uint8_t mod = mod_prev; // these may be used later
    hid_prev=hid_now; mod_prev=mod_now; act_prev = act_now;

    if( !(hid_now==0 && hid != 0) ) {
        // no release detected
        return true;
    }

    if(macroRecording()) {
        macro_key(hid,mod);
        return false; // macro should be echoed!
    }

    char curChar = hid2asciicode(hid, mod);

    if(subcmd) {
        handleSubCmd(curChar);
        return true;
    }

    switch(curChar) {
#ifdef PINKYDROP
        case 'd':
            g_pinkydrop = g_pinkydrop ? 0 : 1;
            printf("Pinkydrop %d\n", g_pinkydrop);
            eeprom_write_byte(&ee_pinkyDrop, g_pinkydrop);
            setCommandMode(false);
            break;
#endif
        case 'v':
            printf("AdNW %s\n", FW_VERSION);
            setCommandMode(false);
            break;
        case 'q':
            setCommandMode(false);
            break;
        case 'b':
            jump_bootloader();
            break;

#ifdef MOUSE_HAS_SCROLL_WHEELS
        case 't':
            printf("TP:\n");
            tp_id();
            setCommandMode(false);
            break;
#endif

#ifdef ALTERNATE_LAYER
        case 'l':
            g_alternateLayer = g_alternateLayer ? 0 : 1;
            eeprom_write_byte(&ee_alternateLayer,g_alternateLayer);
            printf("AltL %s\n", g_alternateLayer ? "on" : "off");
            setCommandMode(false);
            break;
#endif

#ifdef PS2MOUSE
        case 'm':
            g_mouse_enabled = g_mouse_enabled > 0 ? 0 : 1;
            printf("Mouse %sabled\n", g_mouse_enabled ? "en" : "dis");
            setCommandMode(false);
            break;
#endif
        case 'x':
            subcmd=SUB_MACRO;
            break;
        case 'r':
            printf("Rec macro\n");
            subcmd=SUB_MACRO_REC;
            break;
        case 'h':
            subcmd=SUB_PASSHASH;
            break;
        default:
            printf("%c",curChar);
            break;
    }

    return true;
}


void handleSubCmd(char c)
{
    uint8_t type=PH_TYPE_ALNUMSYM;
    uint8_t len=12;
    char password[PH_MAX_LEN+1];
    uint8_t ret;

    switch( subcmd ) {
        case SUB_MACRO:
            printMacro( (((uint8_t)c)%MACROCOUNT) );
            setCommandMode(false);
            break;
        case SUB_MACRO_REC:
            // stay in command mode until macro is read.
            if(!setMacroRecording(((uint8_t)c)%MACROCOUNT))
                setCommandMode(false);
            break;
        case SUB_PASSHASH:
            ret = passHash(password, len, type, "secret", "key", "tag");
            printf("\nPH len=%d type=%d = %s", len, type, password);
            setCommandMode(false);
            break;

        default:
            setCommandMode(false);
            break;
    }
}



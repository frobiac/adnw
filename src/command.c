/*
    This file is part of the AdNW keyboard firmware.

    Copyright 2010-2017 Stefan Fröbe, <frobiac /at/ gmail [d0t] com>

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

#include "global_config.h"
#include "command.h"
#include "hid_usage.h"
#include "keyboard_class.h"
#include "keymap.h"
#include "trackpoint.h"
#include "jump_bootloader.h"

#ifdef PH_ENABLED
    #include "passhash/passhash.h"
#endif

#include "print.h"

#ifdef PS2MOUSE
    #include "trackpoint.h"
#endif
#include "macro.h"

bool g_cmd_mode_active=false;


led_t led_save;


static uint8_t subcmd;           ///< currently active subcommand

void setCommandMode(bool on)
{
    if(on!=g_cmd_mode_active) {
        if(on) {
            led_save = g_cfg.led;
            g_cfg.led = (led_t) { .r=5, .g=0, .b=0, .on=30,  .off=30 };
        } else {
            g_cfg.led = led_save;
        }

        g_cmd_mode_active=on;
        subcmd = SUB_NONE;
    }
}

bool commandMode(void) { return g_cmd_mode_active; }
uint8_t commandModeSub(void) { return subcmd; }



/** Called when command mode is active.
 *
 *  Key is mapped according to modifiers, but better stick to ascii?
 *  First pressed key is mapped to first layer defined and evaluated. Use only a-z and 0-9 for commands, others exit this mode.
 *
 *  Leave by calling setCommandMode(false) on one-off commands, otherwise any unmapped key or
 *  'q' quit the loop. Don't for adjusting config variables through multiple presses.
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
            g_cfg.fw.pinkydrop = ! g_cfg.fw.pinkydrop;
            xprintf("Pinkydrop %d\n", g_cfg.fw.pinkydrop);
            setCommandMode(false);
            break;
#endif
        case 'v':
            xprintf("AdNW %s\n", FW_VERSION);
            setCommandMode(false);
            break;

        case 'b':
            jump_bootloader();
            break;
        case 'c':
            subcmd=SUB_CONFIG;
            break;

/// @todo Trackpoint availability
#ifdef PS2MOUSE
        case 't':
            ps2_init_mouse();
            setCommandMode(false);
            break;
#endif

#ifdef ALTERNATE_LAYER
        case 'l':
            g_cfg.fw.alt_layer = !g_cfg.fw.alt_layer;
            //xprintf("AltL %s\n", g_cfg.fw.alt_layer ? "on" : "off");
            setCommandMode(false);
            break;
#endif

#ifdef PS2MOUSE
        case 'm': {
            // @TODO  only activate if init worked!
            g_cfg.fw.mouse_enabled = !g_cfg.fw.mouse_enabled;

            if(g_cfg.fw.mouse_enabled) {
                g_cfg.fw.mouse_enabled = ps2_init_mouse();
            }

            //xprintf("\nMouse %sabled", g_cfg.fw.mouse_enabled ? "en" : "dis");
            setCommandMode(false);
        }
        break;
#endif
        case 'x':
            subcmd=SUB_MACRO;
            break;
        case 'r':
            //xprintf("Rec macro\n");
            subcmd=SUB_MACRO_REC;
            break;
#ifdef PH_ENABLED
        case 'h':
            subcmd=SUB_PASSHASH;
            break;
#endif


        case 'q':
        default:
            setCommandMode(false);
            break;
    }

    return true;
}


void handleSubCmd(char c)
{
#ifdef PH_ENABLED
    uint8_t type=PH_TYPE_ALNUMSYM;
    uint8_t len=12;
    char password[PH_MAX_LEN+1];
#endif

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
#ifdef PH_ENABLED
        case SUB_PASSHASH:
            passHash(password, len, type, "secret", "key", "tag");
            xprintf("\nPH len=%d type=%d = %s", len, type, password);
            setCommandMode(false);
            break;
#endif

        case SUB_CONFIG:
            switch(c) {
                case 'I': init_config();       tp_init(); break;
                case 'S': save_config(&g_cfg); tp_init(); break;
                case 'R': invalidate_config(); init_config(); tp_init(); break;
                case 'L': load_config(&g_cfg); tp_init(); break;
                case 'm': xprintf("\nMEM: %d/%d", get_mem_unused_simple(), get_mem_unused()); break;
#ifdef PS2MOUSE
                // change sensitivity for initial and normal operation
                ///@TODO generic interface, always allow '0' (no %256)
                case 'p': g_cfg.tp_config.speed -= 10; tp_init(); break;
                case 'P': g_cfg.tp_config.speed += 10; tp_init(); break;
                case 'd': g_cfg.tp_config.sens  -= 10; tp_init(); break;
                case 'D': g_cfg.tp_config.sens  += 10; tp_init(); break;
                case 't': g_cfg.tp_config.sensL -= 10; tp_init(); break;
                case 'T': g_cfg.tp_config.sensL += 10; tp_init(); break;
                case 'c': g_cfg.tp_config.thres -=  5; tp_init(); break;
                case 'C': g_cfg.tp_config.thres +=  5; tp_init(); break;

                case 'x': g_cfg.tp_axis.flipx  = !g_cfg.tp_axis.flipx;  tp_init(); break;
                case 'y': g_cfg.tp_axis.flipy  = !g_cfg.tp_axis.flipy;  tp_init(); break;
                case 'z': g_cfg.tp_axis.flipz  = !g_cfg.tp_axis.flipz;  tp_init(); break;
                case 'X': g_cfg.fw.swap_xy     = !g_cfg.fw.swap_xy;                break;
                case 'Y': g_cfg.tp_axis.pts    = !g_cfg.tp_axis.pts;    tp_init(); break;
#endif

#ifdef HAS_LED
                // Operate on the saved config that will be restored when leaving command mode
                case 'j': led_save.off = (led_save.off-5) % 256; break;
                case 'J': led_save.off = (led_save.off+5) % 256; break;
                case 'o': led_save.on = (led_save.on-5) % 256; break;
                case 'O': led_save.on = (led_save.on+5) % 256; break;
#endif

                case 'q':
                default:
                    setCommandMode(false);
                    break;
            }
            print_config();
            break; // allow consecutive changes to variables

        default:
            setCommandMode(false);
            break;
    }
}



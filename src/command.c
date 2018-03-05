/*
    This file is part of the AdNW keyboard firmware.

    Copyright 2010-2018 Stefan Fröbe, <frobiac /at/ gmail [d0t] com>

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
#include "ascii2hid.h"
#include "print.h"
#include "helpers.h"

#ifdef PS2MOUSE
    #include "trackpoint.h"
#endif
#include "macro.h"

bool g_cmd_mode_active=false;

#ifdef EXTRA
    #include "extra.h"
#endif

#ifdef XXTEA
    #include "crypt/xxtea.h"
#endif

#include "passhash/hmac-sha1.h"
#include "passhash/b64.h"


static uint8_t subcmd;           ///< currently active subcommand


void setCommandMode(bool on)
{
    if(on!=g_cmd_mode_active) {
        g_cmd_mode_active=on;
        subcmd = SUB_NONE;
        if(on) {
            memset(g_cmd_buf, 0, CMD_BUF_SIZE+2);
        }
    }
}

bool unlocked(void)
{
    return(g_cfg.unlock_check == ((g_pw[0]<<8)|(g_pw[1]&0xFE)));
}

void subcmdIfUnlocked(uint8_t cmd) { unlocked() ? subcmd = cmd : setCommandMode(false); }
bool commandMode(void) { return g_cmd_mode_active; }
uint8_t commandModeSub(void) { return subcmd; }



/**
 *  Called when command mode is active.
 *
 *  Will wait until a key that would generate output on press is released and then passes its
 *  hid code and modifiers to switch statement below.
 *  This allows to use any printable character with modifiers in switch statement below.
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
    uint8_t act_now = activeKeysCount();

    if((hid_now==hid_prev) && (mod_now==mod_prev) && (act_now==act_prev))
        return true;

    // ok, something changed:
    uint8_t hid_released = hid_prev;
    uint8_t mod_released = mod_prev; // these may be used later
    hid_prev=hid_now; mod_prev=mod_now; act_prev = act_now;

    if( !(hid_now==0 && hid_released != 0) ) {
        // no release detected
        return true;
    }

    char curChar = hid2asciicode(hid_released, mod_released);

    uint8_t len = g_cmd_buf[CMD_BUF_SIZE+1];
    if(len < CMD_BUF_SIZE) {
        g_cmd_buf[len] = curChar;
        //g_cmd_buf[len+1]='\0';
        g_cmd_buf[CMD_BUF_SIZE+1]=len+1;
    } else { // not really required.
        g_cmd_buf[CMD_BUF_SIZE] = '\0';
    }

    if(subcmd) {
        return handleSubCmd(curChar, hid_released, mod_released);
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

        case 'l':
            subcmd=SUB_LED;
            break;

/// @todo Trackpoint availability
#ifdef PS2MOUSE
        case 't':
            ps2_init_mouse();
            setCommandMode(false);
            break;
#endif

#ifdef ALTERNATE_LAYER
        case 'L':
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
            subcmdIfUnlocked(SUB_MACRO);
            break;

        case 'r':
            subcmdIfUnlocked(SUB_MACRO_REC);
            break;

#ifdef EXTRA
        case 'e':
            subcmd=SUB_EXTRA;
            break;
#endif


        case 'u':
            memset(g_pw, 0, PWLEN);
            subcmd=SUB_UNLOCK;
            break;

#ifdef XXTEA
        case 'X':
            subcmd=SUB_XXTEA;
            break;
#endif

        case 'U':
            subcmdIfUnlocked(SUB_SET_TAG);
            break;

        case 'h':
            subcmdIfUnlocked(SUB_HMAC);
            break;

        case 'q':
        default:
            setCommandMode(false);
            break;
    }

    return true;
}


/**
Several subcommands intercept entered data for consumption:
- Macro rec  [MACROLEN] : C-Return or C-Esc
- Macro play [1]        : reads single macro selector character
- Passhash   [PH_INPUT] : until Return - reads tag[ <len>[ <type>]
- Unlock     [PWLEN+2]  : until Return - reads g_pw
 */

bool handleSubCmd(char c, uint8_t hid, uint8_t mod)
{
    // len minus first (subcmd id) and last (return)
    uint8_t len = g_cmd_buf[CMD_BUF_SIZE+1] -2 ;

    switch( subcmd ) {
        case SUB_SET_TAG:
            // read until return=10 is pressed or maximum length reached
            if( (uint8_t)(c) != 10 )
                return false;

            g_cmd_buf[len+1]='\0';
            for(uint8_t i=0; i<len+1; ++i)
                g_cmd_buf[1+i] ^= g_pw[i%PWLEN];

            eeprom_busy_wait();
            eeprom_update_block (( const void *) (&g_cmd_buf[1]), (void *) EE_ADDR_TAG, len+1);
            setCommandMode(false);
            break;


        case SUB_HMAC:
            // read until return=10 is pressed or maximum length reached
            if( (uint8_t)(c) != 10 )
                return false;

            // expect args: row(a-z) col(a-z) len
            char    row=g_cmd_buf[1]; // a-z -> a-m
            uint8_t col=g_cmd_buf[2] - 'a';
            uint8_t dig=g_cmd_buf[3] - '0';

            // verify args
            if(row>'m')
                row-=13;

            if(row<'a' || row>'m' || col > 26) {
                setCommandMode(false);
                break;
            }

            if(dig < 2 || dig > 8)
                dig=4;

            uint8_t sha[20];

            // KEY=$(echo -n $PW | sha1sum | head -c 40)
            // echo -n "${ROW}${TAG}" | openssl dgst -sha1 -mac HMAC -macopt hexkey:$KEY -binary | base64 | head -c 27

            g_cmd_buf[0]=row;
#define TAG "0123456789"
            //full hmac-sha1 with key=g_pw and tag in g_cmd_buf
            memcpy(&g_cmd_buf[1], TAG, 10);
            // memcpy(&g_cmd_buf[1+10], g_pw, 10);
            // hmac_sha1(sha, &g_cmd_buf[11], 8*10, &g_cmd_buf[0], 8*(1+10));
            hmac_sha1(sha, g_pw, 8*PWLEN, &g_cmd_buf[0], 8*(1+10));

            b64enc( sha, 20, (char*)g_cmd_buf, 27);
            g_cmd_buf[27]='\0'; // full 27 usable, but only 26 used
            // xprintf("\n%s|", g_cmd_buf);

            for(uint8_t i=0; i<dig; ++i) {
                //xprintf("%c", g_cmd_buf[(col+i)%26]) ;
                sha[i]=g_cmd_buf[(col+i)%26];
            }
            sha[dig]='\0';

            eeprom_busy_wait();
            eeprom_read_block (( void *) (&g_cmd_buf[dig]), ( const void *) (EE_ADDR_TAG), EE_TAG_LEN);
            for(uint8_t i=0; i<EE_TAG_LEN; ++i)
                g_cmd_buf[dig+i] ^= g_pw[i%PWLEN];
            memcpy(g_cmd_buf, sha, dig);

            g_cmd_buf[27]='\0'; // full 27 usable, but only 26 used

            setOutputString((char*)g_cmd_buf);

            memset(g_cmd_buf, 0, CMD_BUF_SIZE+2);

            setCommandMode(false);
            break;


        case SUB_UNLOCK:
            // Initialize g_pw with sha1 hash of entered string.

            // read until return=10 is pressed or maximum length reached
            if( (uint8_t)(c) != 10 )
                return false;

            sha1(g_pw, &g_cmd_buf[1], 8*len);
            // save config to store current password as correct

            setCommandMode(false); // length limit reached

            break;

        case SUB_MACRO:
            printMacro(c);
            setCommandMode(false);
            break;

        case SUB_MACRO_REC:
            // stay in command mode until macro is read.
            if(!setMacroRecording(c, hid,mod))
                setCommandMode(false);
            return false; // printing character ?
            break;

#ifdef XXTEA
        case SUB_XXTEA:
            if(c==10) {
                // len minus return
                uint8_t len = g_cmd_buf[CMD_BUF_SIZE+1] -1 ;
                memcpy(g_xxtea_txt, &g_cmd_buf[1], len-1);

                // xprintf("\n"); for(uint8_t i=1; i<len; ++i) xprintf("%02X", g_xxtea_txt[i]);
                xxtea_fixed_encrypt(len);
                // xprintf("\n"); for(uint8_t i=1; i<len; ++i) xprintf("%02X", g_xxtea_txt[i]);
                len = xxtea_fixed_decrypt();
                // xprintf("\n"); for(uint8_t i=1; i<len; ++i) xprintf("%02X", g_xxtea_txt[i]);

                setCommandMode(false);
            }
            break;

#endif

#ifdef EXTRA
            static uint16_t data;
        case SUB_EXTRA:
            if(c==10) {
                xprintf("\nExtra 0x%0X", data);
                g_extra_data = data;
                data=0;
                setCommandMode(false);
            } else if( c>='0' && c<='9') {
                data*=16;
                data+=(c-'0');
            } else if(c>='a'&& c<='f') {
                data*=16;
                data+= (10+c-'a');
            }

            break;
#endif


        case SUB_CONFIG: {
            // 120
            switch(c) {
                case 'I': init_config();       break;
                case 'S':
                    // overwrite unlock verification (0xFE to prevent empty eeprom collision)·
                    g_cfg.unlock_check = ((g_pw[0]<<8)|(g_pw[1]&0xFE));
                    save_config(&g_cfg);
                    break;
                case 'R': invalidate_config(); init_config(); break;
                case 'L': load_config(&g_cfg); break;
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
                case 'q':
                default:
                    setCommandMode(false);
                    break;
            }
            print_config();
            break;
        }

        case SUB_LED: {
            // +120 byte
            //set_led_color(r,g,b);
            switch(c) {
#ifdef HAS_LED
                // @TODO 8 bit resolution, map to curve for lower brightnesses
                case 'r': g_cfg.led.r-=16; break;
                case 'g': g_cfg.led.g-=16; break;
                case 'b': g_cfg.led.b-=16; break;
                case 'R': g_cfg.led.r+=16; break;
                case 'G': g_cfg.led.g+=16; break;
                case 'B': g_cfg.led.b+=16; break;

                // Operate on the saved config that will be restored when leaving command mode
                case 'f': g_cfg.led.off--; break;
                case 'F': g_cfg.led.off++; break;
                case 'n': g_cfg.led.on--; break;
                case 'N': g_cfg.led.on++; break;
                case '0': g_cfg.led = (led_t) { .r=0, .g=0, .b=0, .on=0,  .off=255 }; break;
#endif

                case 'q':
                default:
                    setCommandMode(false);
                    set_led_color(0,0,0);
                    break;
            }
            print_config();
            break; // allow consecutive changes to variables
        }

        // quit SUBCOMMAND on invalid
        default:
            setCommandMode(false);
            break;
    }

    // by default, signal we handled command.
    return true;
}


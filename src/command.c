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


#ifdef EXTRA
    #include "extra.h"
#endif

bool g_cmd_mode_active=false;

/// global unlock password used "descramble" stored EEPROM content and init seed
#define PWLEN 20  // Must be >=20 for sha1
uint8_t g_pw[PWLEN];

#define CMD_BUF_SIZE 27
uint8_t g_cmd_buf[CMD_BUF_SIZE+2]; // last two will contain length and '\0'


static uint8_t subcmd;           ///< currently active subcommand



#define HMAC  1
#define XXTEA 2
#define XOR   4

#define TR_ALGO  4

#if TR_ALGO == XXTEA
    #include "crypt/xxtea.h"
    uint32_t * g_xxtea_key = (void*)g_pw;
#elif TR_ALGO == XOR
    #include "passhash/xor.h"
#elif TR_ALGO == HMAC
    #include "passhash/hmac-sha1.h"
#else
    //#error TR_ALGO undefined
#endif

#include "passhash/b64.h"


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
    return true;
    return(g_cfg.unlock_check == ((g_pw[0]<<8)|(g_pw[1]&0xFE)));
}

void subcmdIfUnlocked(uint8_t cmd) { unlocked() ? subcmd = cmd : setCommandMode(false); }
bool commandMode(void) { return g_cmd_mode_active; }
uint8_t commandModeSub(void) { return subcmd; }

void tabula_recta(uint8_t * dst, char row, uint8_t col, uint8_t dig);
void unlock(uint8_t * str, uint8_t len);



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
            encrypt(&g_cmd_buf[1], len+1);

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
            uint8_t col=g_cmd_buf[2];
            uint8_t dig=g_cmd_buf[3] - '0';

            // Print whole tabula recta on "zz0"
            if(row=='z' && col == 'z' && dig==0) {
                xprintf("\n   abcdefgh klmno rstu ");
                for( row='a'; row<='m'; ++row) {
                    tabula_recta(g_cmd_buf, row, 'a', 20);
                    //tr_code((char*)g_cmd_buf, 20, row-'a', 0);
                    g_cmd_buf[20]='\0';
                    xprintf("\n%c%c %s", row, row+13, g_cmd_buf);
                }
                setCommandMode(false);
                break;
            }

            // verify args
            if(row>'m')
                row-=13;

            // 26 -> 20 chars: (ij) (pq) (vwxyz)
            if(col>='v') col='v';
            if(col>='q') col--;
            if(col>='j') col--;
            col -= 'a';

            if(row<'a' || row>'m' || col > 20) {
                setCommandMode(false);
                break;
            }

            if(dig < 2 || dig > 8)
                dig=4;

            tabula_recta(g_cmd_buf, row, col, dig);

            // read encrypted TabulaRecta password from EEPROM
            eeprom_busy_wait();
            eeprom_read_block (( void *) (&g_cmd_buf[dig]), ( const void *) (EE_ADDR_TAG), EE_TAG_LEN);
            // ... and decrypt after random string from TabulaRecta
            decrypt(&g_cmd_buf[dig], EE_TAG_LEN);

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

            unlock(&g_cmd_buf[1],len);

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

/**
 * HMAC-SHA1:
 * + Optimized asm available
 * + Generate arbitrary hashes from variable length input
 * + Compatible with CLI tools
 * - Needs arrays for key, tag and sha hash
 * - Needs base64 and its array
 * -/+ Output fixed at 20 -> 26 in base64
 *
 * XXTEA:
 * + True encryption, Two-Way
 * + Operate in place
 * - Fixed sizes, or at least uint32_t based
 *
 * TABULA_RECTA:
 * Given length, row and col the 13x26 matrix is queried for code[len] @ (row, col)
 * @TODO: direction
 * @TODO: only 20 selector cols: ij / pq / vwxyz same.
 *
 * When using hmac-sha1:
 * Unlock string is read and hashed via sha1. hash is stored as g_pw[20].
 * - The first two elements of currently entered unlock hash are stored in EEPROM for
 *   verification purposes on config save.
 * - Hash is used to Un-XOR stored EEPROM data like macros and the hmac-sha1 tag base.
 * - It is also used as key for tabula recta hmac-sha1 calls.
 *
 * When using XXTEA:
 * - Unlock string is read into xxtea_key[16]
 * - Any content can be freely en/decrypted with it
 */

#define TAG "0123456789"


int8_t encrypt(uint8_t * data, uint8_t len)
{
    if(!unlocked())
        return -1;
#if TR_ALGO == XXTEA
    if(len > XXTEA_DATA_LEN)
        return -2;
    memcpy(g_xxtea_txt, data, len);
    xxtea_fixed_encrypt(len);
#elif TR_ALGO == HMAC || TR_ALGO == XOR
    // xor same as decrypt
    decrypt(data,len);
#endif
    return 0;
}

int8_t decrypt(uint8_t * data, uint8_t len)
{
    if(!unlocked())
        return -1;
#if TR_ALGO == XXTEA
    if(len > XXTEA_DATA_LEN)
        return -2;
    memcpy(g_xxtea_txt, data, len);
    return xxtea_fixed_decrypt();
#elif TR_ALGO == HMAC || TR_ALGO == XOR
    for(uint8_t i=0; i<len; ++i)
        data[i] ^= g_pw[i%PWLEN];
    return len;
#endif
    return 0;
}

void tabula_recta(uint8_t * dst, char row, uint8_t col, uint8_t dig)
{

#if TR_ALGO == HMAC
    uint8_t sha[20];
    // read row + TAG into textbuffer for hmac-sha
    g_cmd_buf[0]=row;
    memcpy(&g_cmd_buf[1], TAG, 10);

    hmac_sha1(sha, g_pw, 8*PWLEN, g_cmd_buf, 8*(1+10));

    //b64enc( sha, 20, (char*)g_cmd_buf, 27);
    for(uint8_t i=0; i<dig; ++i) {
        dst[i] = b64map[sha[(col+i)%20]&0x3f];
    }

#elif TR_ALGO == XXTEA
    memset(g_xxtea_txt, 0, XXTEA_DATA_LEN);
    g_xxtea_txt[0]=row;
    memcpy(&g_xxtea_txt[1], TAG, 10);
    xxtea_fixed_encrypt(10);
    for(uint8_t i=0; i<dig; ++i) {
        dst[i] = b64map[g_xxtea_txt[(col+i)%20]&0x3f];
    }

#elif TR_ALGO == XOR
    tr_code((char*)dst, dig, row-'a', col);
#endif
}

void unlock(uint8_t * code, uint8_t len)
{
#if TR_ALGO == XXTEA
    memset(g_pw, 0, PWLEN);
    memcpy(g_pw, &g_cmd_buf[1], len);
    // @TODO XXTEA key should be expanded from input.
    //xxtea_fixed_encrypt(10);

#elif TR_ALGO == XOR
    // set seed from input
    xor_init((char*)&g_cmd_buf[1], len);
    for(uint8_t i=0; i<PWLEN; ++i) {
        xorshift();
        g_pw[i] = g_xor_result & 0xFF;
    }


#elif TR_ALGO == HMAC
    memset(g_pw, 0, PWLEN);
    // store hash of entered string as unlock password
    sha1(g_pw, &g_cmd_buf[1], 8*len);
    // save config to store current password as correct
#endif
}

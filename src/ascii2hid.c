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


/**
 * Return ascii char of given hid code and modifier combination.
 *
 * Only ASCII printable characters - no umlauts or other specials like € or °.
 *
 * @arg hid HID code to map to ascii
 * @arg mod modifier necessary [SHIFT|ALTGR]
 *
 * @ret printable ascii char if found, or '\0' on unprintable characters
 *
 * @todo backslash does it work?
 * @todo handle ESC, BS and the like?
 */

#include <stdio.h>
#include "hid_usage.h"
#include "ascii2hid.h"

//#define TABLE // 20260 / 775
//#undef  TABLE // 20466 / 519


// @TODO @see https://github.com/Nurrl/LocaleKeyboard.js/blob/master/locales/de_DE.lang
//
//
#ifndef USE_FUNCTION_FOR_LOOKUP
/// @todo .data usage: this adds 128*2 bytes and is needed only for macro and printf
static const uint8_t ascii2hid_array[128][2] = {
// KEYMAP         // ASCII   dec  HID mod
    _no,          // ( nul)   0
    _no,          // ( soh)   1
    _no,          // ( stx)   2
    _no,          // ( etx)   3
    _no,          // ( eot)   4
    _no,          // ( enq)   5
    _no,          // ( ack)   6
    _no,          // ( bel)   7
    _no,          // ( bs)    8   42
    _TAB,         // ( ht)    9   43
    _ENTER,       // ( nl)   10   40, 88KP
    _no,          // ( vt)   11
    _no,          // ( np)   12
    _no,          // ( cr)   13
    _no,          // ( so)   14
    _no,          // ( si)   15
    _no,          // ( dle)  16
    _no,          // ( dc1)  17
    _no,          // ( dc2)  18
    _no,          // ( dc3)  19
    _no,          // ( dc4)  20
    _no,          // ( nak)  21
    _no,          // ( syn)  22
    _no,          // ( etb)  23
    _no,          // ( can)  24
    _no,          // ( em)   25
    _no,          // ( sub)  26
    _ESC,         // ( esc)  27
    _no,          // ( fs)   28
    _no,          // ( gs)   29
    _no,          // ( rs)   30
    _no,          // ( us)   31
    _SPACE,       // ( sp)   32   44
    _EXCLAM,      // !       33   30 S  (S+1)
    _DQUOTE,      // "       34   31 S  (S+2)
    _HASH,        // #       35   49    (BSLASH)
    _DOLLAR,      // $       36   33 S  (S+4)
    _PERCENT,     // %       37   34 S  (S+5)
    _AMPERSAND,   // &       38   35 S  (S+6)
    _SQUOTE,      // '       39   50 S  (S+NonUS1)
    _L_PAREN,     // (       40   37 S  (S+8)
    _R_PAREN,     // )       41   57 S  (S+9)
    _ASTERIX,     // *       42   48 S  (S+R_Bracket)
    _PLUS,        // +       43   48    (R_Bracket)
    _COMMA,       // ,       44   54
    _MINUS,       // -       45   56    (Slash)
    _PERIOD,      // .       46   55
    _SLASH,       // /       47   36 S  (S+7)
    _0,           // 0       48   39
    _1,           // 1       49   30
    _2,           // 2       50   31
    _3,           // 3       51   32
    _4,           // 4       52   33
    _5,           // 5       53   34
    _6,           // 6       54   35
    _7,           // 7       55   36
    _8,           // 8       56   37
    _9,           // 9       57   38
    _COLON,       // :       58   55  S
    _SCOLON,      // ;       59   54  S
    _LESS,        // <       60  100      NonUS2
    _EQUAL,       // =       61   39  S  (S+0)
    _GREATER,     // >       62  100  S   NonUS2
    _QUESTION,    // ?       63   45  S   Minus
    _AT,          // @       64   20  AG (Ag+q)
    _A,           // A       65    4  S
    _B,           // B       66    5  S
    _C,           // C       67    6  S
    _D,           // D       68    7  S
    _E,           // E       69    8  S
    _F,           // F       70    9  S
    _G,           // G       71   10  S
    _H,           // H       72   11  S
    _I,           // I       73   12  S
    _J,           // J       74   13  S
    _K,           // K       75   14  S
    _L,           // L       76   15  S
    _M,           // M       77   16  S
    _N,           // N       78   17  S
    _O,           // O       79   18  S
    _P,           // P       80   19  S
    _Q,           // Q       81   20  S
    _R,           // R       82   21  S
    _S,           // S       83   22  S
    _T,           // T       84   23  S
    _U,           // U       85   24  S
    _V,           // V       86   25  S
    _W,           // W       87   26  S
    _X,           // X       88   27  S
    _Y,           // Y       89   28  S
    _Z,           // Z       90   29  S
    _L_BRACKET,   // [       91   37  AG (Ag+8)
    _BSLASH,      // \       92   45  AG (Ag+ß)
    _R_BRACKET,   // ]       93   38  AG
    _CARET,       // ^       94   53     (Grave)
    _USCORE,      // _       95   56  S  (S+Slash)
    _GRAVE,       // `       96   46  S (S+Equal)
    _a,           // a       97    4
    _b,           // b       98    5
    _c,           // c       99    6
    _d,           // d      100    7
    _e,           // e      101    8
    _f,           // f      102    9
    _g,           // g      103   10
    _h,           // h      104   11
    _i,           // i      105   12
    _j,           // j      106   13
    _k,           // k      107   14
    _l,           // l      108   15
    _m,           // m      109   16
    _n,           // n      110   17
    _o,           // o      111   18
    _p,           // p      112   19
    _q,           // q      113   20
    _r,           // r      114   21
    _s,           // s      115   22
    _t,           // t      116   23
    _u,           // u      117   24
    _v,           // v      118   25
    _w,           // w      119   26
    _x,           // x      120   27
    _y,           // y      121   28
    _z,           // z      122   29
    _L_BRACE,     // {      123   36  AG (Ag+7)
    _PIPE,        // |      124  100  AG (Ag+NonUS2)
    _R_BRACE,     // }      125   39  AG (Ag+0)
    _TILDE,       // ~      126   48  AG (Ag+R_Bracket)
    _DEL,         // ( del) 127
}               ;
#else
void ascii2hid_DE(uint8_t ascii, uint8_t * hid, uint8_t * mod)
{
    *mod=0x00;
    *hid=0x00;
    if( 'a' <= ascii && ascii <= 'x' ) {
        *hid=4+ascii-'a';
    } else if( 'A' <= ascii && ascii <= 'X' ) {
        *hid=4+ascii-'A';
        *mod=0x02; // shift
    } else if( '1' <= ascii && ascii <= '9' ) {
        *hid=30+ascii-'1';
    } else {
        switch(ascii) {
            case 'Y' : *mod = 0x02;
            case 'y' : *hid = 29; break;
            case 'Z' : *mod = 0x02;
            case 'z' : *hid = 28; break;
            case '}':  *mod=0x40;
            case '0' : *hid = 39; break;
            case '\t': *hid = 43; break;
            case '\n': *hid = 40; break;
            case 27  : *hid = 41; break; // ESC
            case ' ' : *hid = 44; break;
            case '\\': *hid = 45; *mod=0x40; break;
            case '!': *hid =  30; *mod=0x02; break;
            case '"': *hid =  31; *mod=0x02; break;
            case '#': *hid =  49; break;
            case '$': *hid =  33; *mod=0x02; break;
            case '%': *hid =  34; *mod=0x02; break;
            case '&': *hid =  35; *mod=0x02; break;
            case '\'': *hid =  50; *mod=0x02; break;
            case '(': *hid =  37; *mod=0x02; break;
            case ')': *hid =  38; *mod=0x02; break;
            case '*': *mod=0x02;
            case '+': *hid =  48; break;
            case ',': *hid =  54; break;
            case '_': *mod=0x02;
            case '-': *hid =  56; break;
            case '.': *hid =  55; break;
            case '/': *hid =  36; *mod=0x02; break;
            case ':': *hid =  55; *mod=0x02; break;
            case ';': *hid =  54; *mod=0x02; break;
            case '>': *mod=0x02;
            case '<': *hid = 100; break;
            case '=': *hid =  39; *mod=0x02; break;
            case '?': *hid =  45; *mod=0x02; break;
            case '@': *hid =  20; *mod=0x40; break;
            case '[': *hid =  37; *mod=0x40; break;
            case ']': *hid =  38; *mod=0x40; break;
            case '^': *hid =  53; break;
            case '`': *hid =  46; *mod=0x02; break;
            case '{': *hid =  36; *mod=0x40; break;
            case '|': *hid = 100; *mod=0x40; break;
            case '~': *hid =  48; *mod=0x40; break;
            case 127: *hid =  76; break; // delete
        }
    }
}
#endif


void ascii2hid(uint8_t ascii, uint8_t *hid, uint8_t *mod)
{
#ifndef USE_FUNCTION_FOR_LOOKUP
    *hid=ascii2hid_array[ascii][0];
    *mod=HID_MOD_MASK(ascii2hid_array[ascii][1]);
#else
    ascii2hid_DE(ascii,hid, mod);
#endif
}


/**
 * Return ascii char of given hid code and modifier combination.
 *
 * Only ASCII printable characters - no umlauts or other specials like € or °.
 *
 * @arg hid HID code to map to ascii
 * @arg mod modifier necessary [SHIFT|ALTGR]
 *
 * @ret printable ascii char if found, or '\0' on unprintable characters
 *
 * @todo backslash does it work?
 * @todo handle ESC, BS and the like?
 */
char hid2asciicode(uint8_t hid, uint8_t mod)
{
    if(hid==0)
        return('\0');

    uint8_t i, m, h;
    for(i=127; i>0; --i) {
        ascii2hid(i, &h, &m);
        if( h == hid) {
            // either no modifier is needed and neither shift or altgr are pressed
            // or only the required modifier is pressed
            if(0==(m^mod)) {
                return((char)(i));
            }
        }
    }
    return ('\0');
}

#ifndef __AVR__
void main(void)
{
    for(uint8_t i=0; i<128; ++i) {
        uint8_t h=ascii2hid_array[i][0];
        uint8_t m=HID_MOD_MASK(ascii2hid_array[i][1]);

        ascii2hid_DE(i, &H, &M);
        if(h!=H || m!=M) {
            printf("\nERR:%3d %c %3d/%3d != %3d/%3d", i, i, h, m, H, M);
        }


    }
}
#endif


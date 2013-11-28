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

#ifndef _HID_USAGE_H
#define _HID_USAGE_H 1

#include <stdint.h>

// see also ../LUFA/LUFA/Drivers/USB/Class/Common/HIDClassCommon.h
// or http://www.freebsddiary.org/APC/usb_hid_usages

#define HID_NO_KEY			0
#define HID_ERROR_ROLLOVER  1
#define HID_POST_FAIL       2
#define HID_ERROR_UNDEFINED 3
#define HID_A               4
#define HID_B               5
#define HID_C               6
#define HID_D               7
#define HID_E               8
#define HID_F               9
#define HID_G               10
#define HID_H               11
#define HID_I               12
#define HID_J               13
#define HID_K               14
#define HID_L               15
#define HID_M               16
#define HID_N               17
#define HID_O               18
#define HID_P               19
#define HID_Q               20
#define HID_R               21
#define HID_S               22
#define HID_T               23
#define HID_U               24
#define HID_V               25
#define HID_W               26
#define HID_X               27
#define HID_Y               28
#define HID_Z               29
#define HID_1               30
#define HID_2               31
#define HID_3               32
#define HID_4               33
#define HID_5               34
#define HID_6               35
#define HID_7               36
#define HID_8               37
#define HID_9               38
#define HID_0               39
#define HID_ENTER           40
#define HID_ESC             41
#define HID_BACKSPACE       42
#define HID_TAB             43
#define HID_SPACE           44
#define HID_MINUS           45
#define HID_EQUAL           46
#define HID_L_BRACKET       47
#define HID_R_BRACKET       48
#define HID_BSLASH          49
#define HID_NON_US_1        50
#define HID_SEMICOLON       51
#define HID_QUOTE           52
#define HID_GRAVE           53
#define HID_COMMA           54
#define HID_PERIOD          55
#define HID_SLASH           56
#define HID_CAPS_LOCK       57
#define HID_F1              58
#define HID_F2              59
#define HID_F3              60
#define HID_F4              61
#define HID_F5              62
#define HID_F6              63
#define HID_F7              64
#define HID_F8              65
#define HID_F9              66
#define HID_F10             67
#define HID_F11             68
#define HID_F12             69
#define HID_PRINTSCREEN     70
#define HID_SCROLL_LOCK     71
#define HID_PAUSE           72
#define HID_INSERT          73
#define HID_HOME            74
#define HID_PAGE_UP         75
#define HID_DELETE          76
#define HID_END             77
#define HID_PAGE_DOWN       78
#define HID_RIGHT           79
#define HID_LEFT            80
#define HID_DOWN            81
#define HID_UP              82
#define HID_NUM_LOCK        83
#define HID_KEYPAD_SLASH    84
#define HID_KEYPAD_ASTERIX  85
#define HID_KEYPAD_MINUS    86
#define HID_KEYPAD_PLUS     87
#define HID_KEYPAD_ENTER    88
#define HID_KEYPAD_1        89
#define HID_KEYPAD_2        90
#define HID_KEYPAD_3        91
#define HID_KEYPAD_4        92
#define HID_KEYPAD_5        93
#define HID_KEYPAD_6        94
#define HID_KEYPAD_7        95
#define HID_KEYPAD_8        96
#define HID_KEYPAD_9        97
#define HID_KEYPAD_0        98
#define HID_KEYPAD_PERIOD   99
#define HID_NON_US_2        100
#define HID_APPLICATION     101
#define HID_MAC_POWER       102
#define HID_KEYPAD_EQUAL    103
#define HID_F13             104
#define HID_F14             105
#define HID_F15             106

/*
   The largest HID usage code is AC_Distribute_Vertically from the Consumer
   Control usage page.  Its id is 0x29c, which requires 10 bits.  The largest
   usage page used by a keyboard, on the other hand, is 0x0c, which requires
   only 4 bits.  So we pack the usage id in the lower 10 bits of a 16 bit int,
   and the usage page in the upper 6 bits.
*/

#define MAKE_USAGE(page,id) (((page)<<10)|(id))
#define USAGE_ID(usage)     (usage&~(0xffff<<10))
#define USAGE_PAGE(usage)   (usage>>10)

typedef enum {
    page_keyboard = 0x07,
    page_system = 0x01,
    page_consumer = 0x0c
} usage_page;

// ... till F24             ...
#define HID_F24             115

#define HID_L_CONTROL       0xE0
#define HID_L_SHIFT         0xE1
#define HID_L_ALT           0xE2
#define HID_L_GUI           0xE3
#define HID_R_CONTROL       0xE4
#define HID_R_SHIFT         0xE5
#define HID_R_ALT           0xE6
#define HID_R_GUI           0xE7

#define HID_SYSRQ           0x9A

/* mouse buttons hid codes */
#define HID_MOUSEBTN_1 (1<<0)
#define HID_MOUSEBTN_2 (1<<1)
#define HID_MOUSEBTN_3 (1<<2)
#define HID_MOUSEBTN_4 (1<<3)
#define HID_MOUSEBTN_5 (1<<4)


typedef enum {
//##########################################
    /* Mousekey */
    MS_BEGIN=0xEF,
    MS_U, //  = 0xF0,
    MS_D,
    MS_L,
    MS_R,
    MS_BTN1,
    MS_BTN2,
    MS_BTN3,
    MS_BTN4,
    MS_BTN5,         /* 0xF8 */
    /* Mousekey wheel */
    MS_W_U,
    MS_W_D,
    MS_W_L,
    MS_W_R,     /* 0xFC */
    /* Mousekey accel */
    MS_ACC0,
    MS_ACC1,
    MS_ACC2,        /* 0xFF */
//##########################################

    // modifier bits to use, actual HID-modifier is (1<<(modbit-MOD_CTRL))
    MOD_L_CTRL=0xE0, // E0 is rather arbitrarily taken to be same as HID
    MOD_L_SHIFT,
    MOD_L_ALT,
    MOD_L_GUI,
    MOD_R_CTRL,
    MOD_R_SHIFT,
    MOD_R_ALT,
    MOD_R_GUI,
    MOD_END,

    // layer modifiers
    MOD_LAYER_0=0xF2,
    MOD_LAYER_1,
    MOD_LAYER_2,
    MOD_LAYER_3,
    MOD_MOUSEKEY,
    MOD_COMPOSE,
    MOD_LAYER_LAST
} modbits;

#define _MS_U     { MS_U, 0,    ' ' }
#define _MS_D     { MS_D, 0,    ' ' }
#define _MS_L     { MS_L, 0,    ' ' }
#define _MS_R     { MS_R, 0,    ' ' }
#define _MS_W_U     { MS_W_U, 0,    ' ' }
#define _MS_W_D     { MS_W_D, 0,    ' ' }
#define _MS_W_L     { MS_W_L, 0,    ' ' }
#define _MS_W_R     { MS_W_R, 0,    ' ' }
#define _MS_ACC0    { MS_ACC0, 0,    ' ' }
#define _MS_ACC1    { MS_ACC1, 0,    ' ' }
#define _MS_ACC2    { MS_ACC2, 0,    ' ' }
#define _MS_BTN1    { MS_BTN1, 0,    ' ' }
#define _MS_BTN2    { MS_BTN2, 0,    ' ' }
#define _MS_BTN3    { MS_BTN3, 0,    ' ' }
#define _MS_BTN4    { MS_BTN4, 0,    ' ' }
#define _MS_BTN5    { MS_BTN5, 0,    ' ' }

/// @todo : cleanup modifiers everywhere
#define NONE  0
#define CTRL  (1<<0)
#define SHIFT (1<<1)
#define ALT   (1<<2)
#define GUI   (1<<3)
#define ALTGR (1<<6)

// *************************************************************
// mapping of characters and HID codes
// differences between qwert_Y and qwert_Z are separate at the bottom
// **************************************************************

/*
QWERTY US                                QWERTZ DE
---------                                ---------
                                AltGr      ¹ ² ³ ¼ ½   { [ ] } \ ¸
~ ! @ # $ % ^ & * ( ) - +       shift    ° ! " § $ % & / ( ) = ? `
` 1 2 3 4 5 6 7 8 9 0 _ =       ROW1     ^ 1 2 3 4 5 6 7 8 9 0 ß ´
-----                                    -----
                                           @   €
  Q W E R T Y U I O P { } |     shift      Q W E R T Z U I O P Ü *
            y         [ ] \     ROW2                 z         ü +
-----                                    -----

  A S D F G H J K L : "         shift    A S D F G H J K L Ö Ä '
                    ; '         ROW3                         ö ä #
-----                                    -----
                                         |             µ
  Z X C V B N M < > ?           shift    > Y X C V B N M ; : _
  z             , . /           ROW4     < y             , . -
-----                                    -----

-> Different keys in QWERTZ DE :
Normal : ^                 ß ´     z ü + ö ä # < y , . -
Shifted: ^ 2 3 4 6 7 8 9 0 ß ´     z ü + ö ä # < y , . -
AltGr  :           7 8 9 0 ß   q e     +       <

*/

#define _no         { 0,     0,      ' ' } // No key defined

#define _1          { HID_1, 0,      '1' }
#define _EXCLAM     { HID_1, SHIFT , '!' }
#define _2          { HID_2, 0,      '2' }
#define _3          { HID_3, 0,      '3' }
#define _4          { HID_4, 0,      '4' }
#define _DOLLAR     { HID_4, SHIFT , '$' }
#define _5          { HID_5, 0,      '5' }
#define _PERCENT    { HID_5, SHIFT , '%' }
#define _6          { HID_6, 0,      '6' }
#define _7          { HID_7, 0,      '7' }
#define _8          { HID_8, 0,      '8' }
#define _9          { HID_9, 0,      '9' }
#define _0          { HID_0, 0,      '0' }

#define _q          { HID_Q, 0,      'q' }
#define _Q          { HID_Q, SHIFT , 'Q' }
#define _w          { HID_W, 0,      'w' }
#define _W          { HID_W, SHIFT , 'W' }
#define _e          { HID_E, 0,      'e' }
#define _E          { HID_E, SHIFT , 'E' }
#define _r          { HID_R, 0,      'r' }
#define _R          { HID_R, SHIFT , 'R' }
#define _t          { HID_T, 0,      't' }
#define _T          { HID_T, SHIFT , 'T' }
#define _u          { HID_U, 0,      'u' }
#define _U          { HID_U, SHIFT , 'U' }
#define _i          { HID_I, 0,      'i' }
#define _I          { HID_I, SHIFT , 'I' }
#define _o          { HID_O, 0,      'o' }
#define _O          { HID_O, SHIFT , 'O' }
#define _p          { HID_P, 0,      'p' }
#define _P          { HID_P, SHIFT , 'P' }

#define _a          { HID_A, 0,      'a' }
#define _A          { HID_A, SHIFT , 'A' }
#define _s          { HID_S, 0,      's' }
#define _S          { HID_S, SHIFT , 'S' }
#define _d          { HID_D, 0,      'd' }
#define _D          { HID_D, SHIFT , 'D' }
#define _f          { HID_F, 0,      'f' }
#define _F          { HID_F, SHIFT , 'F' }
#define _g          { HID_G, 0,      'g' }
#define _G          { HID_G, SHIFT , 'G' }
#define _h          { HID_H, 0,      'h' }
#define _H          { HID_H, SHIFT , 'H' }
#define _j          { HID_J, 0,      'j' }
#define _J          { HID_J, SHIFT , 'J' }
#define _k          { HID_K, 0,      'k' }
#define _K          { HID_K, SHIFT , 'K' }
#define _l          { HID_L, 0,      'l' }
#define _L          { HID_L, SHIFT , 'L' }

#define _x          { HID_X, 0,      'x' }
#define _X          { HID_X, SHIFT , 'X' }
#define _c          { HID_C, 0,      'c' }
#define _C          { HID_C, SHIFT , 'C' }
#define _v          { HID_V, 0,      'v' }
#define _V          { HID_V, SHIFT , 'V' }
#define _b          { HID_B, 0,      'b' }
#define _B          { HID_B, SHIFT , 'B' }
#define _n          { HID_N, 0,      'n' }
#define _N          { HID_N, SHIFT , 'N' }
#define _m          { HID_M, 0,      'm' }
#define _M          { HID_M, SHIFT , 'M' }

#define _CAPS       { HID_CAPS_LOCK,0     , ' ' }  //  CapsLock
#define _BSPACE     { HID_BACKSPACE,0     , 171 }  //  Backspace
#define _TAB        { HID_TAB,      0     , 187 }  //  Tab

#define _NON_US_1   { HID_NON_US_1, 0     , ' ' }  //  non-US-1
#define _NON_US_2   { HID_NON_US_2, 0     , ' ' }  //  non-US-1
#define _ENTER      { HID_ENTER,    0     , 182 }  //  Enter

#define _COMMA      { HID_COMMA,    0     , ',' }
#define _PERIOD     { HID_PERIOD,   0     , '.' }
#define _SPACE      { HID_SPACE,    0     , ' ' }

//L/R 4 modifiers
#define _L_SHIFT    { HID_L_SHIFT,		MOD_L_SHIFT , ' ' }  //  LShift
#define _R_SHIFT    { HID_R_SHIFT,		MOD_R_SHIFT , ' ' }  //  RShift
#define _L_CTRL     { HID_L_CONTROL,	MOD_L_CTRL  , ' ' }  //  LCtrl
#define _R_CTRL     { HID_R_CONTROL,	MOD_R_CTRL  , ' ' }  //  RCtrl
#define _L_ALT      { HID_L_ALT,		MOD_L_ALT   , ' ' }  //  LAlt
#define _R_ALT      { HID_R_ALT,		MOD_R_ALT   , ' ' }  //  RAlt
#define _L_GUI      { HID_L_GUI,		MOD_L_GUI   , ' ' }  //  LWin
#define _R_GUI      { HID_R_GUI,		MOD_R_GUI   , ' ' }  //  RWin
//
#define _MOD_0      { HID_NO_KEY,		MOD_LAYER_0 , ' ' }
#define _MOD_1      { HID_NO_KEY,		MOD_LAYER_1 , ' ' }
#define _MOD_2      { HID_NO_KEY,		MOD_LAYER_2 , ' ' }
#define _MOD_3      { HID_NO_KEY,		MOD_LAYER_3 , ' ' }
#define _MOD_MOUSEKEY { HID_NO_KEY,		MOD_MOUSEKEY, ' ' }
#define _COMPOSE    { HID_NO_KEY,		MOD_COMPOSE , ' ' }

#define _MS_SCROLL   { MS_SCROLL,0,    ' ' }
#define _MS_BTN_1    { MS_BTN_1, 0,    ' ' }
#define _MS_BTN_2    { MS_BTN_2, 0,    ' ' }
#define _MS_BTN_3    { MS_BTN_3, 0,    ' ' }

#define _INS        { HID_INSERT,		0     , ' ' }  //  Insert
#define _DEL        { HID_DELETE,		0     , ' ' }  //  Delete
#define _LEFT       { HID_LEFT,			0     , ' ' }  //  Left
#define _HOME       { HID_HOME,			0     , ' ' }  //  Home
#define _END        { HID_END,			0     , ' ' }  //  End
#define _UP         { HID_UP,			0     , ' ' }  //  Up
#define _DOWN       { HID_DOWN,			0     , ' ' }  //  Down
#define _PGUP       { HID_PAGE_UP,		0     , ' ' }  //  PgUp
#define _PGDN       { HID_PAGE_DOWN,	0     , ' ' }  //  PgDn
#define _RIGHT      { HID_RIGHT,		0     , ' ' }  //  Right
#define _NUMLOCK    { HID_NUM_LOCK,   NUMLOCK , ' ' }  //  NumLock

#define _ESC        { HID_ESC, 0,    ' ' }  //  Esc
#define _F1         { HID_F1,  0,    ' ' }  //  F1
#define _F2         { HID_F2,  0,    ' ' }  //  F2
#define _F3         { HID_F3,  0,    ' ' }  //  F3
#define _F4         { HID_F4,  0,    ' ' }  //  F4
#define _F5         { HID_F5,  0,    ' ' }  //  F5
#define _F6         { HID_F6,  0,    ' ' }  //  F6
#define _F7         { HID_F7,  0,    ' ' }  //  F7
#define _F8         { HID_F8,  0,    ' ' }  //  F8
#define _F9         { HID_F9,  0,    ' ' }  //  F9
#define _F10        { HID_F10, 0,    ' ' }  //  F10
#define _F11        { HID_F11, 0,    ' ' }  //  F11
#define _F12        { HID_F12, 0,    ' ' }  //  F12



#ifdef QWERTY
/*---------                                ---------
                                AltGr      ¹ ² ³ ¼ ½   { [ ] } \ ¸
~ ! @ # $ % ^ & * ( ) - +       shift    ° ! " § $ % & / ( ) = ? `
` 1 2 3 4 5 6 7 8 9 0 _ =       ROW1     ^ 1 2 3 4 5 6 7 8 9 0 ß ´
-----                                    -----
                                           @   €
  Q W E R T Y U I O P { } |     shift      Q W E R T Z U I O P Ü *
            y         [ ] \     ROW2                 z         ü +
-----                                    -----

  A S D F G H J K L : "         shift    A S D F G H J K L Ö Ä '
                    ; '         ROW3                         ö ä #
-----                                    -----
                                         |             µ
  Z X C V B N M < > ?           shift    > Y X C V B N M ; : _
  z             , . /           ROW4     < y             , . -
-----                                    -----
*/

#define _AMPERSAND { HID_7,         SHIFT , '&' }
#define _ASTERIX   { HID_8,         SHIFT , '*' }
#define _AT        { HID_2,         SHIFT , '@' }
#define _BSLASH    { HID_BSLASH,    0,      '\\'}
#define _CARET     { HID_6,         SHIFT , '&' }
#define _COLON     { HID_SEMICOLON, SHIFT , 'Ö' }
#define _DQUOTE    { HID_QUOTE,     SHIFT , '"' }
#define _EQUAL     { HID_EQUAL,     0,      '=' }
#define _GREATER   { HID_PERIOD,    SHIFT , '>' }
#define _HASH      { HID_3,         SHIFT , '#' }
#define _L_BRACE   { HID_L_BRACKET, SHIFT , '[' }
#define _L_BRACKET { HID_L_BRACKET, 0,      '{' }
#define _LESS      { HID_COMMA ,    SHIFT , '<' }
#define _L_PAREN   { HID_9,         SHIFT , '(' }
#define _MINUS     { HID_MINUS,     0,      '-' }
#define _PLUS      { HID_EQUAL,     SHIFT , '+' }
#define _QUESTION  { HID_SLASH,     SHIFT , '?' }
#define _SQUOTE    { HID_QUOTE,     0,      '\''}
#define _R_BRACE   { HID_R_BRACKET, SHIFT , ']' }
#define _R_BRACKET { HID_R_BRACKET, 0,      '}' }
#define _R_PAREN   { HID_0,         SHIFT , ')' }
#define _SCOLON    { HID_SEMICOLON, 0,      ';' }
#define _SLASH     { HID_SLASH,     0,      '/' }
#define _USCORE    { HID_MINUS,     SHIFT , '_' }
#define _y         { HID_Y,         0,      'y' }
#define _Y         { HID_Y,         SHIFT , 'Y' }
#define _z         { HID_Z,         0,      'z' }
#define _Z         { HID_Z,         SHIFT , 'Z' }

#define _DEGREE     _no
#define _GRAVE      { HID_GRAVE,    0,      '^' }
#define _TILDE      { HID_GRAVE,    SHIFT , '~' }
#define _PIPE       { HID_BSLASH,   SHIFT , '|' }

// Umlauts not available
#define _SSHARP     _no
#define _a_UML      _no
#define _A_UML      _no
#define _O_UML      _no
#define _o_UML      _no
#define _U_UML      _no
#define _u_UML      _no


#else //qwert_Z_

#define _AMPERSAND  { HID_6,            SHIFT , '&' }
#define _ASTERIX    { HID_R_BRACKET,    SHIFT , '*' }
#define _AT         { HID_Q,            ALTGR , '@' }
#define _CARET      { HID_GRAVE,        0,      '`' }
#define _COLON      { HID_PERIOD,       SHIFT , ':' }
#define _DQUOTE     { HID_2,            SHIFT , '"' }
#define _EQUAL      { HID_0,            SHIFT , '=' }
#define _GREATER    { HID_NON_US_2,     SHIFT , '>' }
#define _HASH       { HID_BSLASH,       0,      '#' }
#define _L_BRACE    { HID_7,            ALTGR , '{' }
#define _L_BRACKET  { HID_8,            ALTGR , '[' }
#define _LESS       { HID_NON_US_2,     0,      '<' }
#define _L_PAREN    { HID_8,            SHIFT , '(' }
#define _MINUS      { HID_SLASH,        0,      '-' }
#define _PLUS       { HID_R_BRACKET,    0,      '+' }
#define _QUESTION   { HID_MINUS,        SHIFT , '?' }
#define _R_BRACE    { HID_0,            ALTGR , '}' }
#define _R_BRACKET  { HID_9,            ALTGR , ']' }
#define _R_PAREN    { HID_9,            SHIFT , ')' }
#define _SCOLON     { HID_COMMA,        SHIFT , ';' }
#define _SLASH      { HID_7,            SHIFT , '/' }
#define _SQUOTE     { HID_NON_US_1,     SHIFT , '\''}
#define _USCORE     { HID_SLASH,        SHIFT , '_' }
#define _y          { HID_Z,            0,      'y' }
#define _Y          { HID_Z,            SHIFT , 'Y' }
#define _z          { HID_Y,            0,      'z' }
#define _Z          { HID_Y,            SHIFT , 'Z' }

#define _BSLASH     { HID_MINUS,        ALTGR , '\\'}
#define _TILDE      { HID_R_BRACKET,    ALTGR , '~' }
#define _PIPE       { HID_NON_US_2,     ALTGR , '|' }
#define _DEGREE     { HID_GRAVE,        SHIFT , ' ' }
#define _GRAVE      { HID_EQUAL,        SHIFT , '^' }

#define _SSHARP     { HID_MINUS,        0,      'S' }
#define _a_UML      { HID_QUOTE,        0,      228 }
#define _A_UML      { HID_QUOTE,        SHIFT , 196 }
#define _o_UML      { HID_SEMICOLON,    0,      246 }
#define _O_UML      { HID_SEMICOLON,    SHIFT , 214 }
#define _u_UML      { HID_L_BRACKET,    0,      252 }
#define _U_UML      { HID_L_BRACKET,    SHIFT , 220 }

#endif // QWERTY or QWERTZ

#ifdef DEBUG_OUTPUT
/// @todo .data usage: this adds 3*128 bytes and is needed only for macro and printf
// needed for debug printf and macro replay
static const uint8_t ascii2hid[128][3] = {
    _no , // ( nul)   0
    _no , // ( soh)   1
    _no , // ( stx)   2
    _no , // ( etx)   3
    _no , // ( eot)   4
    _no , // ( enq)   5
    _no , // ( ack)   6
    _no , // ( bel)   7
    _no , // ( bs)    8
    _no , // ( ht)    9
    _ENTER , // ( nl)   10
    _no , // ( vt)   11
    _no , // ( np)   12
    _no , // ( cr)   13
    _no , // ( so)   14
    _no , // ( si)   15
    _no , // ( dle)  16
    _no , // ( dc1)  17
    _no , // ( dc2)  18
    _no , // ( dc3)  19
    _no , // ( dc4)  20
    _no , // ( nak)  21
    _no , // ( syn)  22
    _no , // ( etb)  23
    _no , // ( can)  24
    _no , // ( em)   25
    _no , // ( sub)  26
    _no , // ( esc)  27
    _no , // ( fs)   28
    _no , // ( gs)   29
    _no , // ( rs)   30
    _no , // ( us)   31
    _no , // ( sp)   32
    _EXCLAM ,   // !       33
    _DQUOTE ,   // "       34
    _HASH ,     // #       35
    _DOLLAR ,   // $       36
    _PERCENT ,  // %       37
    _AMPERSAND, // &       38
    _SQUOTE ,   // '       39
    _L_PAREN ,  // (       40
    _R_PAREN ,  // )       41
    _ASTERIX ,  // *       42
    _PLUS ,     // +       43
    _COMMA ,    // ,       44
    _MINUS ,    // -       45
    _PERIOD ,   // .       46
    _SLASH ,    // /       47
    _0 , // 0       48
    _1 , // 1       49
    _2 , // 2       50
    _3 , // 3       51
    _4 , // 4       52
    _5 , // 5       53
    _6 , // 6       54
    _7 , // 7       55
    _8 , // 8       56
    _9 , // 9       57
    _COLON ,    // :       58
    _SCOLON ,   // ;       59
    _LESS ,     // <       60
    _EQUAL ,    // =       61
    _GREATER ,  // >       62
    _QUESTION,  // ?       63
    _AT, // @       64
    _A , // A       65
    _B , // B       66
    _C , // C       67
    _D , // D       68
    _E , // E       69
    _F , // F       70
    _G , // G       71
    _H , // H       72
    _I , // I       73
    _J , // J       74
    _K , // K       75
    _L , // L       76
    _M , // M       77
    _N , // N       78
    _O , // O       79
    _P , // P       80
    _Q , // Q       81
    _R , // R       82
    _S , // S       83
    _T , // T       84
    _U , // U       85
    _V , // V       86
    _W , // W       87
    _X , // X       88
    _Y , // Y       89
    _Z , // Z       90
    _L_BRACKET, // [       91
    _BSLASH ,   // \       92
    _R_BRACKET, // ]       93
    _CARET ,    // ^       94
    _USCORE ,   // _       95
    _GRAVE ,    // `       96
    _a , // a       97
    _b , // b       98
    _c , // c       99
    _d , // d      100
    _e , // e      101
    _f , // f      102
    _g , // g      103
    _h , // h      104
    _i , // i      105
    _j , // j      106
    _k , // k      107
    _l , // l      108
    _m , // m      109
    _n , // n      110
    _o , // o      111
    _p , // p      112
    _q , // q      113
    _r , // r      114
    _s , // s      115
    _t , // t      116
    _u , // u      117
    _v , // v      118
    _w , // w      119
    _x , // x      120
    _y , // y      121
    _z , // z      122
    _L_BRACE ,  // {      123
    _PIPE ,     // |      124
    _R_BRACE ,  // }      125
    _TILDE ,    // ~      126
    _DEL        // ( del) 127
} ;
#endif DEBUG_OUTPUT

#endif // _HID_USAGE_H


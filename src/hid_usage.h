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

#ifndef _HID_USAGE_H
#define _HID_USAGE_H 1
// see also ../LUFA/LUFA/Drivers/USB/Class/Common/HIDClassCommon.h
// -> bCountryCode=09 german?

// or http://www.freebsddiary.org/APC/usb_hid_usages

#define MOD_BEGIN 0xE8

/// @todo LEFT CTRL not functional
#define MOD_L_CTRL  MOD_BEGIN +0 // 0x01
#define MOD_L_SHIFT MOD_BEGIN +1 // 0x02
#define MOD_L_ALT   MOD_BEGIN +2 // 0x04
#define MOD_L_GUI   MOD_BEGIN +3 // 0x08
#define MOD_R_CTRL  MOD_BEGIN +4 // 0x10
#define MOD_R_SHIFT MOD_BEGIN +5 // 0x20
#define MOD_R_ALT   MOD_BEGIN +6 // 0x40
#define MOD_R_GUI   MOD_BEGIN +7 // 0x80
//#define MOD_ALTGR MOD_R_ALT | MOD_R_CTRL

#define MOD_LAYER_0     0xF2
#define MOD_LAYER_1     (1 + MOD_LAYER_0)
#define MOD_LAYER_2     (2 + MOD_LAYER_0)
#define MOD_LAYER_3     (3 + MOD_LAYER_0)
#define MOD_MOUSE       (4 + MOD_LAYER_0)
#define MOD_COMPOSE     (5 + MOD_LAYER_0)
#define MOD_LAYER_LAST  (6 + MOD_LAYER_0)

#define MS_BTNS        0x90
#define MS_BTN_1       (0 + MS_BTNS)
#define MS_BTN_2       (2 + MS_BTNS)
#define MS_BTN_3       (1 + MS_BTNS)
#define MS_SCROLL      (3 + MS_BTNS)

#define HID_NO_KEY      0
#define HID_ERROR_ROLLOVER  1
#define HID_POST_FAIL       2
#define HID_ERROR_UNDEFINED 3
#define HID_A       4
#define HID_B       5
#define HID_C       6
#define HID_D       7
#define HID_E       8
#define HID_F       9
#define HID_G       10
#define HID_H       11
#define HID_I       12
#define HID_J       13
#define HID_K       14
#define HID_L       15
#define HID_M       16
#define HID_N       17
#define HID_O       18
#define HID_P       19
#define HID_Q       20
#define HID_R       21
#define HID_S       22
#define HID_T       23
#define HID_U       24
#define HID_V       25
#define HID_W       26
#define HID_X       27
#define HID_Y       28
#define HID_Z       29
#define HID_1       30
#define HID_2       31
#define HID_3       32
#define HID_4       33
#define HID_5       34
#define HID_6       35
#define HID_7       36
#define HID_8       37
#define HID_9       38
#define HID_0       39
#define HID_ENTER   40
#define HID_ESC     41
#define HID_BACKSPACE   42
#define HID_TAB     43
#define HID_SPACE   44
#define HID_MINUS   45
#define HID_EQUAL   46
#define HID_L_BRACKET 47
#define HID_R_BRACKET 48
#define HID_BSLASH   49
#define HID_NON_US_1    50    // # & ~
#define HID_SEMICOLON   51
#define HID_QUOTE   52
#define HID_GRAVE   53      // grave & tilde
#define HID_COMMA   54
#define HID_PERIOD  55
#define HID_SLASH   56
#define HID_CAPS_LOCK   57
#define HID_F1      58
#define HID_F2      59
#define HID_F3      60
#define HID_F4      61
#define HID_F5      62
#define HID_F6      63
#define HID_F7      64
#define HID_F8      65
#define HID_F9      66
#define HID_F10     67
#define HID_F11     68
#define HID_F12     69
#define HID_PRINTSCREEN 70
#define HID_SCROLL_LOCK 71
#define HID_PAUSE   72
#define HID_INSERT  73
#define HID_HOME    74
#define HID_PAGE_UP 75
#define HID_DELETE  76
#define HID_END     77
#define HID_PAGE_DOWN   78
#define HID_RIGHT   79
#define HID_LEFT    80
#define HID_DOWN    81
#define HID_UP      82
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

#define HID_NON_US_2        100         // \ & |
#define HID_APPLICATION     101
#define HID_MAC_POWER       102
#define HID_KEYPAD_EQUAL    103
#define HID_F13             104
#define HID_F14             105
#define HID_F15             106
// till F24
#define HID_F24             115

#define HID_CUT             0x7b
#define HID_COPY            0x7c
#define HID_PASTE           125
#define HID_FIND            0x7e

/*
KB_EXECUTE,
KB_HELP,
KB_MENU,
KB_SELECT,
KB_STOP,
KB_AGAIN,
KB_UNDO,
KB_CUT,
KB_COPY,
KB_PASTE,
KB_FIND,
KB_MUTE,
KB_VOLUP,           //0x80
KB_VOLDOWN,
KB_LOCKING_CAPS,      // locking Caps Lock
KB_LOCKING_NUM,       // locking Num Lock
KB_LOCKING_SCROLL,    // locking Scroll Lock
KP_COMMA,
KP_EQUAL_AS400,       // equal sign on AS/400
*/


#define HID_INTERNATIONAL_1_RO          0x87
#define HID_INTERNATIONAL_2_KATAKANA    0x88
#define HID_INTERNATIONAL_3_YEN         0x89
#define HID_INTERNATIONAL_4_HENKAN      0x8A
#define HID_INTERNATIONAL_5_MUHENKAN    0x8B
#define HID_INTERNATIONAL_6             0x8C
#define HID_INTERNATIONAL_7             0x8D
#define HID_INTERNATIONAL_8             0x8E
#define HID_INTERNATIONAL_9             0x8E

#define HID_LANG_1_ 0x90
#define HID_LANG_2_ 0x91
#define HID_LANG_3_ 0x92
#define HID_LANG_4_ 0x93
#define HID_LANG_5_ 0x94
#define HID_LANG_7_ 0x96
#define HID_LANG_6_ 0x95
#define HID_LANG_8_ 0x97
#define HID_LANG_9_ 0x98

// extensions for internal use
/*
FN_1 = 0xE8,
FN_2,
FN_3,
FN_4,
MS_UP = 0xF0,
MS_DOWN,
MS_LEFT,
MS_RIGHT,
MS_BTN1 = 0xF4,
MS_BTN2,
MS_BTN3,
MS_BTN4,
MS_BTN5,
MS_WH_UP,
MS_WH_DOWN,
MS_WH_LEFT,
MS_WH_RIGHT,
*/
#define HID_L_CONTROL   224
#define HID_L_SHIFT     225
#define HID_L_ALT       226
#define HID_L_GUI       227
#define HID_R_CONTROL   228
#define HID_R_SHIFT     229
#define HID_R_ALT       230
#define HID_R_GUI       231

#define HID_SYSRQ       0x9A

// KEYBOARD Execute/Menu/Help/Copy/paste

// KEYBOARD Media

// *************************************************************
// mapping of characters and HID codes
// differences between qwert_Y and qwert_Z are separate at the bottom
// **************************************************************

/// @todo : Also fix qwertz specifics in hhstdio.[ch]

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

#define NONE  0x00
#define CTRL  0x01
#define SHIFT 0x02
#define ALTGR 0x40

#define _no         { 0,     NONE  }      // No key defined

#define _1          { HID_1, NONE  }      //  1
#define _EXCLAM     { HID_1, SHIFT }      //  !
#define _2          { HID_2, NONE  }      //  2
#define _3          { HID_3, NONE  }      //  3
#define _4          { HID_4, NONE  }      //  4
#define _DOLLAR     { HID_4, SHIFT }      //  $
#define _5          { HID_5, NONE  }      //  5
#define _PERCENT    { HID_5, SHIFT }      //  %
#define _6          { HID_6, NONE  }      //  6
#define _7          { HID_7, NONE  }      //  7
#define _8          { HID_8, NONE  }      //  8
#define _9          { HID_9, NONE  }      //  9
#define _0          { HID_0, NONE  }      //  0



#define _BSPACE     { HID_BACKSPACE, NONE  }  //  Backspace
#define _TAB        { HID_TAB, NONE  }        //  Tab
#define _q      { HID_Q, NONE  }      //  q
#define _Q      { HID_Q, SHIFT }      //  Q
#define _w      { HID_W, NONE  }      //  w
#define _W      { HID_W, SHIFT }      //  W
#define _e      { HID_E, NONE  }      //  e
#define _E      { HID_E, SHIFT }      //  E
#define _r      { HID_R, NONE  }      //  r
#define _R      { HID_R, SHIFT }      //  R
#define _t      { HID_T, NONE  }      //  t
#define _T      { HID_T, SHIFT }      //  T
#define _u      { HID_U, NONE  }      //  u
#define _U      { HID_U, SHIFT }      //  U
#define _i      { HID_I, NONE  }      //  i
#define _I      { HID_I, SHIFT }      //  I
#define _o      { HID_O, NONE  }      //  o
#define _O      { HID_O, SHIFT }      //  O
#define _p      { HID_P, NONE  }      //  p
#define _P      { HID_P, SHIFT }      //  P

#define _CAPS       { HID_CAPS_LOCK,  NONE  }     //  CapsLock

#define _a      { HID_A, NONE  }      //  a
#define _A      { HID_A, SHIFT }      //  A
#define _s      { HID_S, NONE  }      //  s
#define _S      { HID_S, SHIFT }      //  S
#define _d      { HID_D, NONE  }      //  d
#define _D      { HID_D, SHIFT }      //  D
#define _f      { HID_F, NONE  }      //  f
#define _F      { HID_F, SHIFT }      //  F
#define _g      { HID_G, NONE  }      //  g
#define _G      { HID_G, SHIFT }      //  G
#define _h      { HID_H, NONE  }      //  h
#define _H      { HID_H, SHIFT }      //  H
#define _j      { HID_J, NONE  }      //  j
#define _J      { HID_J, SHIFT }      //  J
#define _k      { HID_K, NONE  }      //  k
#define _K      { HID_K, SHIFT }      //  K
#define _l      { HID_L, NONE  }      //  l
#define _L      { HID_L, SHIFT }      //  L

#define _NON_US_1   { HID_NON_US_1, NONE  }  //  non-US-1
#define _NON_US_2   { HID_NON_US_2, NONE  }  //  non-US-1

#define _ENTER      { HID_ENTER, NONE  }    //  Enter

#define _x      { HID_X, NONE  }      //  x
#define _X      { HID_X, SHIFT }      //  X
#define _c      { HID_C, NONE  }      //  c
#define _C      { HID_C, SHIFT }      //  C
#define _v      { HID_V, NONE  }      //  v
#define _V      { HID_V, SHIFT }      //  V
#define _b      { HID_B, NONE  }      //  b
#define _B      { HID_B, SHIFT }      //  B
#define _n      { HID_N, NONE  }      //  n
#define _N      { HID_N, SHIFT }      //  N
#define _m      { HID_M, NONE  }      //  m
#define _M      { HID_M, SHIFT }      //  M

#define _COMMA      { HID_COMMA,  NONE  }     //  ,
#define _PERIOD     { HID_PERIOD, NONE  }     //  .
#define _SPACE      { HID_SPACE,  NONE  }     //  space

//L/R 4 modifiers
#define _L_SHIFT    { HID_L_SHIFT,     MOD_L_SHIFT }       //  LShift
#define _R_SHIFT    { HID_R_SHIFT,     MOD_R_SHIFT }       //  RShift
#define _L_CTRL     { HID_L_CONTROL,   MOD_L_CTRL  }       //  LCtrl
#define _R_CTRL     { HID_R_CONTROL,   MOD_R_CTRL  }       //  RCtrl
#define _L_ALT      { HID_L_ALT,       MOD_L_ALT   }       //  LAlt
#define _R_ALT      { HID_R_ALT,       MOD_R_ALT   }       //  RAlt
#define _L_GUI      { HID_L_GUI,       MOD_L_GUI   }       //  LWin
#define _R_GUI      { HID_R_GUI,       MOD_R_GUI   }       //  RWin
//
#define _MOD_0      { HID_NO_KEY, MOD_LAYER_0 }
#define _MOD_1      { HID_NO_KEY, MOD_LAYER_1 }
#define _MOD_2      { HID_NO_KEY, MOD_LAYER_2 }
#define _MOD_3      { HID_NO_KEY, MOD_LAYER_3 }
#define _MOUSE      { HID_NO_KEY, MOD_MOUSE   }
#define _COMPOSE    { HID_NO_KEY, MOD_COMPOSE }

//
#define _INS        { HID_INSERT,  NONE  }     //  Insert
#define _DEL        { HID_DELETE,  NONE  }     //  Delete
#define _LEFT       { HID_LEFT,    NONE  }     //  Left
#define _HOME       { HID_HOME,    NONE  }     //  Home
#define _END        { HID_END,     NONE  }     //  End
#define _UP         { HID_UP,      NONE  }     //  Up
#define _DOWN       { HID_DOWN,    NONE  }     //  Down
#define _PGUP       { HID_PAGE_UP, NONE  }     //  PgUp
#define _PGDN       { HID_PAGE_DOWN,NONE }     //  PgDn
#define _RIGHT      { HID_RIGHT,   NONE  }     //  Right
#define _NUMLOCK    { HID_NUM_LOCK,NUMLOCK  }      //  NumLock
//
#define _KP_7       { HID_KEYPAD_7 , NONE  }       //  KP_7
#define _KP_HOME    { HID_KEYPAD_7 , SHIFT }       //  Home
#define _KP_4       { HID_KEYPAD_4 , NONE  }       //  KP_4
#define _KP_LEFT    { HID_KEYPAD_4 , SHIFT }       //  Left
#define _KP_1       { HID_KEYPAD_1 , NONE  }       //  KP_1
#define _KP_END     { HID_KEYPAD_1 , SHIFT }       //  End
#define _KP_SLASH   { HID_KEYPAD_SLASH, NONE  }        //  KP_/
#define _KP_8       { HID_KEYPAD_8 , NONE  }       //  KP_8
#define _KP_UP      { HID_KEYPAD_8 , SHIFT }       //  Up
#define _KP_5       { HID_KEYPAD_5 , NONE  }       //  KP_5
#define _KP_2       { HID_KEYPAD_2, NONE  }        //  KP_2
#define _KP_DOWN    { HID_KEYPAD_2 , SHIFT }       //  Down
#define _KP_0       { HID_KEYPAD_0 , NONE  }       //  KP_0
#define _KP_INS     { HID_KEYPAD_0 , SHIFT }       //  Ins
#define _KP_MUL     { HID_KEYPAD_ASTERIX, NONE  }      //  KP_*
#define _KP_9       { HID_KEYPAD_9, NONE  }        //  KP_9
#define _KP_PGUP    { HID_KEYPAD_9, SHIFT }        //  PgUp
#define _KP_6       { HID_KEYPAD_6, NONE  }        //  KP_6
#define _KP_RIGHT   { HID_KEYPAD_6, SHIFT }        //  Right
#define _KP_3       { HID_KEYPAD_3, NONE  }        //  KP_3
#define _KP_PGDN    { HID_KEYPAD_3, SHIFT }        //  PgDn
#define _KP_PERIOD  { HID_KEYPAD_PERIOD, NONE  }       //  KP_.
#define _KP_DEL     { HID_KEYPAD_PERIOD, SHIFT }       //  Del
#define _KP_MINUS   { HID_KEYPAD_MINUS, NONE  }        //  KP_-
#define _KP_PLUS    { HID_KEYPAD_PLUS, NONE  }     //  KP_+
#define _KP_ENTER   { HID_KEYPAD_ENTER, NONE  }        //  KP_Enter


#define _ESC        { HID_ESC, NONE  }     //  Esc
#define _F1         { HID_F1,  NONE  }     //  F1
#define _F2         { HID_F2,  NONE  }     //  F2
#define _F3         { HID_F3,  NONE  }     //  F3
#define _F4         { HID_F4,  NONE  }     //  F4
#define _F5         { HID_F5,  NONE  }     //  F5
#define _F6         { HID_F6,  NONE  }     //  F6
#define _F7         { HID_F7,  NONE  }     //  F7
#define _F8         { HID_F8,  NONE  }     //  F8
#define _F9         { HID_F9,  NONE  }     //  F9
#define _F10        { HID_F10, NONE  }     //  F10
#define _F11        { HID_F11, NONE  }     //  F11
#define _F12        { HID_F12, NONE  }     //  F12

// test for copy paste
#define _CUT        { HID_CUT, NONE }
#define _COPY       { HID_COPY, NONE }
//#define _PASTE      { HID_PASTE, NONE }
#define _PASTE      { HID_V, CTRL }

#define _MS_SCROLL   { MS_SCROLL,NONE  }
#define _MS_BTN_1    { MS_BTN_1, NONE  }
#define _MS_BTN_2    { MS_BTN_2, NONE  }
#define _MS_BTN_3    { MS_BTN_3, NONE  }


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

#define _AMPERSAND { HID_7, SHIFT }       //  &
#define _ASTERIX   { HID_8, SHIFT }      //  *
#define _AT        { HID_2, SHIFT }      //  @
#define _BSLASH    { HID_BSLASH,  NONE  }     //
#define _CARET     { HID_6, SHIFT }      //  &
#define _COLON     { HID_SEMICOLON, SHIFT }      //  Ö
#define _DQUOTE    { HID_QUOTE, SHIFT }     // "
#define _EQUAL     { HID_EQUAL, NONE  }      //  =
#define _GREATER   { HID_PERIOD, SHIFT }     //  >
#define _HASH      { HID_3, SHIFT }      //  #
#define _L_BRACE   { HID_L_BRACKET,    SHIFT  }     //
#define _L_BRACKET { HID_L_BRACKET,    NONE }     //  Ü
#define _LESS      { HID_COMMA , SHIFT }     //  <
#define _L_PAREN   { HID_9, SHIFT }      //  (
#define _MINUS     { HID_MINUS, NONE  }      //  Z:ß/?
#define _PLUS      { HID_EQUAL, SHIFT }      //  +
#define _QUESTION  { HID_SLASH,  SHIFT }     //  ?
#define _SQUOTE    { HID_QUOTE, NONE  }     //  '
#define _R_BRACE   { HID_R_BRACKET,    SHIFT }     //  ]
#define _R_BRACKET { HID_R_BRACKET,    NONE }     //  *
#define _R_PAREN   { HID_0, SHIFT }      //  )
#define _SCOLON    { HID_SEMICOLON, NONE  }      //  ö
#define _SLASH     { HID_SLASH,  NONE  }     //  /
#define _USCORE    { HID_MINUS, SHIFT }      //  _
#define _y         { HID_Y, NONE  }      //  y
#define _Y         { HID_Y, SHIFT }      //  Y
#define _z         { HID_Z, NONE  }      //  z
#define _Z         { HID_Z, SHIFT  }     //  z

#define _DEGREE     _no
#define _GRAVE      { HID_GRAVE, NONE  } //  =
#define _TILDE      { HID_GRAVE,    SHIFT }
#define _PIPE       { HID_BSLASH,   SHIFT }

// Umlauts not available
#define _SSHARP     _no
#define _a_UML      _no
#define _A_UML      _no
#define _O_UML      _no
#define _o_UML      _no
#define _U_UML      _no
#define _u_UML      _no


#else //qwert_Z_

#define _AMPERSAND  { HID_6, SHIFT }      //  &
#define _ASTERIX    { HID_R_BRACKET,    SHIFT }     //  *
#define _AT         { HID_Q, ALTGR }      //  @
#define _CARET      { HID_GRAVE, NONE  }  //  `
#define _COLON      { HID_PERIOD,    SHIFT }
#define _DQUOTE     { HID_2, SHIFT     }     // "
#define _EQUAL      { HID_0, SHIFT }          //  )
#define _GREATER    { HID_NON_US_2,SHIFT}     //  >
#define _HASH       { HID_BSLASH,  NONE  }
#define _L_BRACE    { HID_7, ALTGR }      //
#define _L_BRACKET  { HID_8, ALTGR }      //  Ü
#define _LESS       { HID_NON_US_2,NONE }     //  <
#define _L_PAREN    { HID_8, SHIFT }      //  *
#define _MINUS      { HID_SLASH, NONE  }      //  -
#define _PLUS       { HID_R_BRACKET,    NONE  }     //  ]
#define _QUESTION   { HID_MINUS,  SHIFT }     //  Z:ß/?
#define _R_BRACE    { HID_0, ALTGR }      //  ]
#define _R_BRACKET  { HID_9, ALTGR }      //  *
#define _R_PAREN    { HID_9, SHIFT }      //  )
#define _SCOLON     { HID_COMMA,     SHIFT }
#define _SLASH      { HID_7,      SHIFT }     //  &
#define _SQUOTE     { HID_NON_US_1, SHIFT }       //  #
#define _USCORE     { HID_SLASH, SHIFT }      //  _
#define _y          { HID_Z, NONE  }      //  z
#define _Y          { HID_Z, SHIFT  }     //  z
#define _z          { HID_Y, NONE  }      //  z
#define _Z          { HID_Y, SHIFT  }     //  z

#define _BSLASH     { HID_MINUS,     ALTGR }      //  Z:ß/?
#define _TILDE      { HID_R_BRACKET,   ALTGR }
#define _PIPE        { HID_NON_US_2,  ALTGR }
#define _DEGREE     { HID_GRAVE, SHIFT  } //  `
#define _GRAVE      { HID_EQUAL, SHIFT  } //  =

#define _SSHARP     { HID_MINUS,     NONE  }      //  Z:ß/?
#define _a_UML      { HID_QUOTE,     NONE  }     //  '
#define _A_UML      { HID_QUOTE,     SHIFT }     //  '
#define _O_UML      { HID_SEMICOLON, SHIFT }
#define _o_UML      { HID_SEMICOLON, NONE  }
#define _U_UML      { HID_L_BRACKET,   SHIFT }     //  Ü
#define _u_UML      { HID_L_BRACKET,   NONE }      //  Ü


#endif // QWERTY or QWERTZ

static const uint8_t ascii2hid[128][2] = {
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
    _no , // ( nl)   10
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
    _EXCLAM , // !       33
    _DQUOTE , // "       34
    _HASH , // #       35
    _DOLLAR , // $       36
    _PERCENT , // %       37
    _AMPERSAND , // &       38
    _SQUOTE , // '       39
    _L_PAREN , // (       40
    _R_PAREN , // )       41
    _ASTERIX , // *       42
    _PLUS , // +       43
    _COMMA , // ,       44
    _MINUS , // -       45
    _PERIOD , // .       46
    _SLASH , // /       47
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
    _COLON , // :       58
    _SCOLON , // ;       59
    _LESS , // <       60
    _EQUAL , // =       61
    _GREATER , // >       62
    _QUESTION , // ?       63
    _AT , // @       64
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
    _L_BRACKET , // [       91
    _BSLASH , // \       92
    _R_BRACKET , // ]       93
    _CARET , // ^       94
    _USCORE , // _       95
    _GRAVE , // `       96
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
    _L_BRACE , // {      123
    _PIPE , // |      124
    _R_BRACE , // }      125
    _TILDE , // ~      126
    _no  // ( del) 127
} ;

#endif // _HID_USAGE_H


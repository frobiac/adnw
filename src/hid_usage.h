#ifndef _HID_USAGE_H
#define _HID_USAGE_H 1
// see also ./trunk/LUFA/Drivers/USB/Class/Common/HID.h

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
#define HID_L_BRACE 47
#define HID_R_BRACE 48
#define HID_BACKSLASH   49
#define HID_NON_US_1    50    // # & ~
#define HID_SEMICOLON   51
#define HID_QUOTE   52
#define HID_TILDE   53      // grave & tilde
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




// **************************************************************
// *    QWERTZ mapping of characters and HID codes
// * prefixes with y_ are valid if computer is set-up for qwert_Y_
// **************************************************************
#define NONE  0x00
#define CTRL  0x01
#define SHIFT 0x02
#define ALTGR 0x40

#define _no         { ' ',  0,     NONE  }     // No key defined
#define _CARET      { '`',  HID_TILDE, NONE  } //  `
#define _DEGREE     { '`',  HID_TILDE, SHIFT  } //  `
#define _GRAVE      { '=',  HID_EQUAL, SHIFT  }     //  =

#define _1          { '1',  HID_1, NONE  }     //  1
#define _EXCLAM     { '!',  HID_1, SHIFT }     //  !
#define _2          { '2',  HID_2, NONE  }      //  2
#define y_AT        { '@',  HID_2, SHIFT }      //  @
#define _AT         { '@',  HID_Q, ALTGR }      //  @
#define _3          { '3',  HID_3, NONE  }      //  3
#define y_HASH      { '#',  HID_3, SHIFT }      //  #
#define _SQUOTE     { '\'', HID_NON_US_1, SHIFT }       //  #
#define _4          { '4',  HID_4, NONE  }      //  4
#define _DOLLAR     { '$',  HID_4, SHIFT }      //  $
#define _5          { '5',  HID_5, NONE  }      //  5
#define _PERCENT    { '%',  HID_5, SHIFT }      //  %
#define _6          { '6',  HID_6, NONE  }      //  6
#define y_CARET     { '^',  HID_6, SHIFT }//  &
#define _7          { '7',  HID_7, NONE  }      //  7
#define y_AMPERSAND { '&',  HID_7, SHIFT }      //  &
#define _AMPERSAND  { '&',  HID_6, SHIFT }      //  &
#define _8          { '8',  HID_8, NONE  }      //  8
#define y_ASTERIX   { '*',  HID_8, SHIFT }      //  *
#define _PLUS       { '+',  HID_R_BRACE,    NONE  }     //  ]
#define _ASTERIX    { '*',  HID_R_BRACE,    SHIFT }     //  *
#define _TILDE      { '~',  HID_R_BRACE,    ALTGR}

#define _9          { '9',  HID_9, NONE  }      //  9
#define y_L_PAREN   { '(',  HID_9, SHIFT }      //  (
#define _L_PAREN    { '[',  HID_8, SHIFT }      //  *
#define _0          { '0',  HID_0, NONE  }      //  0
#define y_R_PAREN   { ')',  HID_0, SHIFT }      //  )
#define _R_PAREN    { ')',  HID_9, SHIFT }      //  )

#define _L_BRACE    { '{',  HID_7, ALTGR  }     //
#define _L_BRACKET  { '[',  HID_8, ALTGR }      //  Ü
#define _R_BRACE    { '}',  HID_0, ALTGR  }     //  ]
#define _R_BRACKET  { ']',  HID_9, ALTGR }      //  *

#define y_MINUS     { '-',  HID_MINUS, NONE  }      //  Z:ß/?
//#define _SSHARP     { 'ß',  HID_MINUS, NONE  }     //  Z:ß/?
#define _SSHARP     { 's',  HID_MINUS, NONE  }     //  Z:ß/?
#define _BSLASH     { '\\', HID_MINUS, ALTGR  }     //  Z:ß/?
#define _MINUS      { '-',  HID_SLASH, NONE  }      //  -
#define y_USCORE    { '_',  HID_MINUS, SHIFT }      //  _
#define _USCORE     { '_',  HID_SLASH, SHIFT }      //  _
#define y_EQUAL     { '=',  HID_EQUAL, NONE  }      //  =
#define _EQUAL      { '=',  HID_0, SHIFT }      //  )
#define y_PLUS      { '+',  HID_EQUAL, SHIFT }      //  +
#define _BSPACE     { 'B',  HID_BACKSPACE, NONE  }      //  Backspace
#define _TAB        { 'T',  HID_TAB, NONE  }        //  Tab
#define _q      { 'q',  HID_Q, NONE  }      //  q
#define _Q      { 'Q',  HID_Q, SHIFT }      //  Q
#define _w      { 'w',  HID_W, NONE  }      //  w
#define _W      { 'W',  HID_W, SHIFT }      //  W
#define _e      { 'e',  HID_E, NONE  }      //  e
#define _E      { 'E',  HID_E, SHIFT }      //  E
#define _r      { 'r',  HID_R, NONE  }      //  r
#define _R      { 'R',  HID_R, SHIFT }      //  R
#define _t      { 't',  HID_T, NONE  }      //  t
#define _T      { 'T',  HID_T, SHIFT }      //  T
#define y_y     { 'y',  HID_Y, NONE  }      //  y
#define y_Y     { 'Y',  HID_Y, SHIFT }      //  Y
#define _y      { 'y',  HID_Z, NONE  }      //  y
#define _Y      { 'Y',  HID_Z, SHIFT }      //  Y
#define _u      { 'u',  HID_U, NONE  }      //  u
#define _U      { 'U',  HID_U, SHIFT }      //  U
#define _i      { 'i',  HID_I, NONE  }      //  i
#define _I      { 'I',  HID_I, SHIFT }      //  I
#define _o      { 'o',  HID_O, NONE  }      //  o
#define _O      { 'O',  HID_O, SHIFT }      //  O
#define _p      { 'p',  HID_P, NONE  }      //  p
#define _P      { 'P',  HID_P, SHIFT }      //  P

#define y_L_BRACE   { '{',  HID_L_BRACE,    NONE  }     //
#define y_L_BRACKET { '[',  HID_L_BRACE,    SHIFT }     //  Ü
#define _U_UML      { 'U',  HID_L_BRACE,    SHIFT }     //  Ü
#define _u_UML      { 'u',  HID_L_BRACE,    NONE }      //  Ü

#define y_R_BRACE   { '}',  HID_R_BRACE,    NONE  }     //  ]
#define y_R_BRACKET { ']',  HID_R_BRACE,    SHIFT }     //  *
#define y_BACKSLASH { '\\',  HID_BACKSLASH, NONE  }     //
#define _HASH       { '#',  HID_BACKSLASH,  NONE  }     //
#define _BAR        { '|',  HID_NON_US_2,   ALTGR}

#define _CAPS       { ' ',  HID_CAPS_LOCK,  NONE  }     //  CapsLock

#define _a      { 'a',  HID_A, NONE  }      //  a
#define _A      { 'A',  HID_A, SHIFT }      //  A
#define _s      { 's',  HID_S, NONE  }      //  s
#define _S      { 'S',  HID_S, SHIFT }      //  S
#define _d      { 'd',  HID_D, NONE  }      //  d
#define _D      { 'D',  HID_D, SHIFT }      //  D
#define _f      { 'f',  HID_F, NONE  }      //  f
#define _F      { 'F',  HID_F, SHIFT }      //  F
#define _g      { 'g',  HID_G, NONE  }      //  g
#define _G      { 'G',  HID_G, SHIFT }      //  G
#define _h      { 'h',  HID_H, NONE  }      //  h
#define _H      { 'H',  HID_H, SHIFT }      //  H
#define _j      { 'j',  HID_J, NONE  }      //  j
#define _J      { 'J',  HID_J, SHIFT }      //  J
#define _k      { 'k',  HID_K, NONE  }      //  k
#define _K      { 'K',  HID_K, SHIFT }      //  K
#define _l      { 'l',  HID_L, NONE  }      //  l
#define _L      { 'K',  HID_L, SHIFT }      //  L

#define y_SCOLON    { ';',  HID_SEMICOLON, NONE  }      //  ö
#define y_COLON     { ':',  HID_SEMICOLON, SHIFT }      //  Ö
#define _O_UML      { 'O',  HID_SEMICOLON, SHIFT  }
#define _o_UML      { 'o',  HID_SEMICOLON, NONE  }
#define _SCOLON     { ';',  HID_COMMA, SHIFT  }
#define _COLON      { ':',  HID_PERIOD, SHIFT }

#define y_QUOTE     { '\'',  HID_QUOTE, NONE  }     //  '
#define _a_UML      { 'a',  HID_QUOTE, NONE  }      //  '
#define _A_UML      { 'A',  HID_QUOTE, SHIFT  }      //  '
#define _DQUOTE     { '"',  HID_2, SHIFT }      // "
#define y_DQUOTE    { '\"',  HID_QUOTE, SHIFT }     // "

#define _NON_US_1   { ' ',  HID_NON_US_1, NONE  }       //  non-US-1
#define _NON_US_2   { ' ',  HID_NON_US_2, NONE  }       //  non-US-1

#define _ENTER      { 'E',  HID_ENTER, NONE  }      //  Enter

#define y_z         { 'z',  HID_Z, NONE  }      //  z
#define y_Z         { 'Z',  HID_Z, SHIFT  }     //  z
#define _z      { 'z',  HID_Y, NONE }       //  Z
#define _Z      { 'Z',  HID_Y, SHIFT }      //  Z
#define _x      { 'x',  HID_X, NONE  }      //  x
#define _X      { 'X',  HID_X, SHIFT }      //  X
#define _c      { 'c',  HID_C, NONE  }      //  c
#define _C      { 'C',  HID_C, SHIFT }      //  C
#define _v      { 'v',  HID_V, NONE  }      //  v
#define _V      { 'V',  HID_V, SHIFT }      //  V
#define _b      { 'b',  HID_B, NONE  }      //  b
#define _B      { 'B',  HID_B, SHIFT }      //  B
#define _n      { 'n',  HID_N, NONE  }      //  n
#define _N      { 'N',  HID_N, SHIFT }      //  N
#define _m      { 'm',  HID_M, NONE  }      //  m
#define _M      { 'M',  HID_M, SHIFT }      //  M

#define _COMMA      { ',',  HID_COMMA,  NONE  }     //  ,
#define y_LESS      { '<',  HID_COMMA , SHIFT }     //  <
#define _LESS       { '<',  HID_NON_US_2,NONE }     //  <
#define _PERIOD     { '.',  HID_PERIOD, NONE  }     //  .
#define y_GREATER   { '>',  HID_PERIOD, SHIFT }     //  >
#define _GREATER    { '>',  HID_NON_US_2,SHIFT}     //  >
#define y_SLASH     { '/',  HID_SLASH,  NONE  }     //  /
#define _SLASH      { '/',  HID_7,      SHIFT }     //  &
#define y_QUESTION  { '?',  HID_SLASH,  SHIFT }     //  ?
#define _QUESTION   { '?',  HID_MINUS,  SHIFT }     //  Z:ß/?
#define _SPACE      { ' ',  HID_SPACE,  NONE  }     //  space

//L/R 4 modifiers
#define _L_SHIFT    { 'S', HID_L_SHIFT,     MOD_L_SHIFT }       //  LShift
#define _R_SHIFT    { 'S', HID_R_SHIFT,     MOD_R_SHIFT }       //  RShift
#define _L_CTRL     { 'C', HID_L_CONTROL,   MOD_L_CTRL  }       //  LCtrl
#define _R_CTRL     { 'C', HID_R_CONTROL,   MOD_R_CTRL  }       //  RCtrl
#define _L_ALT      { 'A', HID_L_ALT,       MOD_L_ALT   }       //  LAlt
#define _R_ALT      { 'A', HID_R_ALT,       MOD_R_ALT   }       //  RAlt
#define _L_GUI      { 'G', HID_L_GUI,       MOD_L_GUI   }       //  LWin
#define _R_GUI      { 'G', HID_R_GUI,       MOD_R_GUI   }       //  RWin
//
#define _MOD_0      { '0', HID_NO_KEY, MOD_LAYER_0 }
#define _MOD_1      { '1', HID_NO_KEY, MOD_LAYER_1 }
#define _MOD_2      { '2', HID_NO_KEY, MOD_LAYER_2 }
#define _MOD_3      { '3', HID_NO_KEY, MOD_LAYER_3 }
#define _MOUSE  { 'M', HID_NO_KEY, MOD_MOUSE   }
#define _COMPOSE {'C', HID_NO_KEY, MOD_COMPOSE }

//
#define _INS        { 'I', HID_INSERT,  NONE  }     //  Insert
#define _DEL        { 'D', HID_DELETE,  NONE  }     //  Delete
#define _LEFT       { ' ', HID_LEFT,    NONE  }     //  Left
#define _HOME       { ' ', HID_HOME,    NONE  }     //  Home
#define _END        { ' ', HID_END,     NONE  }     //  End
#define _UP         { ' ', HID_UP,      NONE  }     //  Up
#define _DOWN       { '.', HID_DOWN,    NONE  }     //  Down
#define _PGUP       { '.', HID_PAGE_UP, NONE  }     //  PgUp
#define _PGDN       { ' ', HID_PAGE_DOWN,NONE }     //  PgDn
#define _RIGHT      { ' ', HID_RIGHT,   NONE  }     //  Right
#define _NUMLOCK    { ' ', HID_NUM_LOCK,NUMLOCK  }      //  NumLock
//
#define _KP_7       { '7', HID_KEYPAD_7 , NONE  }       //  KP_7
#define _KP_HOME    { '4', HID_KEYPAD_7 , SHIFT }       //  Home
#define _KP_4       { ' ', HID_KEYPAD_4 , NONE  }       //  KP_4
#define _KP_LEFT    { '1', HID_KEYPAD_4 , SHIFT }       //  Left
#define _KP_1       { ' ', HID_KEYPAD_! , NONE  }       //  KP_1
#define _KP_END     { ' ', HID_KEYPAD_1 , SHIFT }       //  End
#define _KP_SLASH   { '/', HID_KEYPAD_SLASH, NONE  }        //  KP_/
#define _KP_8       { '8', HID_KEYPAD_8 , NONE  }       //  KP_8
#define _KP_UP      { ' ', HID_KEYPAD_8 , SHIFT }       //  Up
#define _KP_5       { '5', HID_KEYPAD_5 , NONE  }       //  KP_5
#define _KP_2       { '2', HID_KEYPAD_2, NONE  }        //  KP_2
#define _KP_DOWN    { ' ', HID_KEYPAD_2 , SHIFT }       //  Down
#define _KP_0       { '0', HID_KEYPAD_0 , NONE  }       //  KP_0
#define _KP_INS     { ' ', HID_KEYPAD_0 , SHIFT }       //  Ins
#define _KP_MUL     { '*', HID_KEYPAD_ASTERIX, NONE  }      //  KP_*
#define _KP_9       { '9', HID_KEYPAD_9, NONE  }        //  KP_9
#define _KP_PGUP    { ' ', HID_KEYPAD_9, SHIFT }        //  PgUp
#define _KP_6       { '6', HID_KEYPAD_6, NONE  }        //  KP_6
#define _KP_RIGHT   { ' ', HID_KEYPAD_6, SHIFT }        //  Right
#define _KP_3       { '3', HID_KEYPAD_3, NONE  }        //  KP_3
#define _KP_PGDN    { ' ', HID_KEYPAD_3, SHIFT }        //  PgDn
#define _KP_PERIOD  { '.', HID_KEYPAD_PERIOD, NONE  }       //  KP_.
#define _KP_DEL     { ' ', HID_KEYPAD_PERIOD, SHIFT }       //  Del
#define _KP_MINUS   { '-', HID_KEYPAD_MINUS, NONE  }        //  KP_-
#define _KP_PLUS    { '+', HID_KEYPAD_PLUS, NONE  }     //  KP_+
#define _KP_ENTER   { 'E', HID_KEYPAD_ENTER, NONE  }        //  KP_Enter
//
#define _ESC        { ' ', HID_ESC, NONE  }     //  Esc
#define _F1         { ' ', HID_F1, NONE  }      //  F1
#define _F2         { ' ', HID_F2, NONE  }      //  F2
#define _F3         { ' ', HID_F3, NONE  }      //  F3
#define _F4         { ' ', HID_F4, NONE  }      //  F4
#define _F5         { ' ', HID_F5, NONE  }      //  F5
#define _F6         { ' ', HID_F6, NONE  }      //  F6
#define _F7         { ' ', HID_F7, NONE  }      //  F7
#define _F8         { ' ', HID_F8, NONE  }      //  F8
#define _F9         { ' ', HID_F9, NONE  }      //  F9
#define _F10        { ' ', HID_F10, NONE  }     //  F10
#define _F11        { ' ', HID_F11, NONE  }     //  F11
#define _F12        { ' ', HID_F12, NONE  }     //  F12

#define _CUT        { ' ', HID_CUT, NONE }
#define _COPY       { ' ', HID_COPY, NONE }
//#define _PASTE      { ' ', HID_PASTE, NONE }
#define _PASTE      { ' ', HID_V, CTRL }

#define _MS_SCROLL   { ' ', MS_SCROLL,NONE  }
#define _MS_BTN_1    { ' ', MS_BTN_1, NONE  }
#define _MS_BTN_2    { ' ', MS_BTN_2, NONE  }
#define _MS_BTN_3    { ' ', MS_BTN_3, NONE  }

#endif

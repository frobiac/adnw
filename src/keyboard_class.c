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


    Debounce code based on code from Peter Dannegger [danni/At/specs/d0t/de]
        described in German at bottom of page
            http://www.mikrocontroller.net/articles/Entprellung
        and discussed at
            http://www.mikrocontroller.net/topic/48465
    Array extensions inspired by Pult.zip
            http://www.mikrocontroller.net/topic/48465#1489399

*/

#include <string.h>
#include <limits.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <util/atomic.h>

#include "Keyboard.h"
#include "keyboard_class.h"
#include "hhstdio.h"

#include "keymap.h"
#include "macro.h"
#include "matrix.h"
#include "command.h"
#include "jump_bootloader.h"

// #define TRACE printf
// #define TRACE false || printf
#define TRACE(...)

uint8_t lastKeyCode;
uint8_t rowData[ROWS];
uint8_t prevRowData[ROWS];

/**
 * ModeKeyToggle is a simple state machine that enables keys to:
 * 1) emit a normal keycode, including its modifiers, if pressed without any other key being
 *         held down AND immediately released again.
 * 2) act as a modifier if pressed together with another normal key.

 * This enables keys in good locations (e.g. thumb buttons) to be used either as frequent keys
 * such as space, return or backspace to also act as layer or control modifiers, thus minimizing
 * finger travel across the board.
 *
 * @todo : several MKTs together do not work unless they all act as modifiers
 *
 */
enum { MKT_RESET, MKT_TOOMANYKEYS, MKT_YES , MKT_INIT };

#define MKT_TIMEOUT 30 // = x/61 [s]: a modekey will NOT send normal key if pressed this long

uint32_t    mkt_timer;
uint8_t     mkt_state;
struct Key  mkt_key;


/// debounce variables
volatile uint8_t kb_state[ROWS];    // debounced and inverted key state: bit = 1: key pressed
volatile uint8_t kb_press[ROWS];    // key press detect
volatile uint8_t kb_release[ROWS];  // key release detect
volatile uint8_t kb_rpt[ROWS];      // key long press and repeat

static uint8_t ct0[ROWS], ct1[ROWS];
static int32_t rpt[ROWS];

#define ALL_COLS_MASK ((1<<COLS)-1)  // 0x63 or all lower 6 bits
#define REPEAT_MASK    ALL_COLS_MASK // repeat: key0 = 0x3F = 63
#define REPEAT_START   31            // 61 = 1000ms
#define REPEAT_NEXT    15

/**
  * ISR that should get called 61 times a second.
  * Allows exact timers
  */
ISR(TIMER0_OVF_vect)
{
    idle_count++;
    /// @todo What happens on overflow? with 61/s and 16bit=65536
    // (2^15-1)/61/60        ~= 9 minutes till overflow
    // (2^31-1)/61/60/60/24   = 407 days should suffice...
    if(idle_count<0)
        idle_count=0;
}

/**
 * Some startup initialization is performed here.
 */
void initKeyboard()
{
    idle_count=0;
    // not strictly necessary
    for (uint8_t row = 0; row < ROWS; ++row) {
        rowData[row]=0;
        ct0[row]=0xFF;
        ct1[row]=0xFF;
    }

    g_mouse_keys_enabled = 0;
    g_mouse_keys = 0;
    mkt_timer=idle_count + MKT_TIMEOUT;

    stdio_init();
    init_cols();

    initMacros();
}

void clearRowData(){
    for (uint8_t row = 0; row < ROWS; ++row) {
        rowData[row]=0;
        ct0[row]=0xFF;
        ct1[row]=0xFF;
    }
}

uint8_t getKeyboardReport(USB_KeyboardReport_Data_t *report_data)
{
    // handle MKT: timeout resets state machine
    if(mkt_timer+MKT_TIMEOUT < idle_count)
        mkt_state = MKT_RESET;
    // no more keys pressed, so test whether last released key was MKT
    if(activeKeys.keycnt==0) {
        if(mkt_state == MKT_YES) {
            report_data->KeyCode[0] = SecondaryUsage[mkt_key.row][mkt_key.col]; //ModeKeyMatrix[mkt_key.row][mkt_key.col].hid;
            report_data->Modifier = 0;
            mkt_state = MKT_INIT;
            return sizeof(USB_KeyboardReport_Data_t);
        }
        mkt_state = MKT_INIT;
    }

    clearActiveKeys();
    scan_matrix();

    init_active_keys();

#ifdef ANALOGSTICK
    analogDataAcquire();
#endif

    if(commandMode()){
        handleCommand();
        report_data->Modifier=0;
        memset(&report_data->KeyCode[0], 0, 6);
        return sizeof(USB_KeyboardReport_Data_t);
        return 0;
    }

    // while macro mode is on, acquire data from it
    if(macroMode()) {
        if(getMacroReport(report_data)) {
            return sizeof(USB_KeyboardReport_Data_t);
        }
    }
    return fillReport(report_data);
}


uint8_t fillReport(USB_KeyboardReport_Data_t *report_data)
{
    if(activeKeys.keycnt==0) {
        // empty report
        memset(&report_data->KeyCode[0], 0, 6 );
        report_data->Modifier=0;
        return sizeof(USB_KeyboardReport_Data_t);
    }

    uint8_t idx=0;

    for(uint8_t i=0; i < activeKeys.keycnt; ++i) {
        struct Key k=activeKeys.keys[i];
        if(k.normalKey && idx < 6) {
            report_data->KeyCode[idx]=getKeyCode(k.row, k.col, getActiveLayer());
            idx++;
        }
        if(idx>6) {
            printf("\nError: more than 6 keys! ");
            for( uint8_t k=0; k<6; ++k)
                printf(" %d ", report_data->KeyCode[k]);
            break;
        }
    }

    report_data->Modifier=getActiveModifiers()|getActiveKeyCodeModifier();

    return sizeof(USB_KeyboardReport_Data_t);
}



uint8_t get_kb_release( uint8_t key_mask, uint8_t col)
{
    ATOMIC_BLOCK(ATOMIC_FORCEON) {
        key_mask &= kb_release[col];                      // read key(s)
        kb_release[col] ^= key_mask;                      // clear key(s)
    }
    return key_mask;
}

uint8_t get_kb_press( uint8_t key_mask, uint8_t col )
{
    ATOMIC_BLOCK(ATOMIC_FORCEON) {
        key_mask &= kb_press[col];                      // read key(s)
        kb_press[col] ^= key_mask;                      // clear key(s)
    }
    return key_mask;
}
uint8_t get_kb_rpt( uint8_t key_mask, uint8_t col )
{
    ATOMIC_BLOCK(ATOMIC_FORCEON) {
        key_mask &= kb_rpt[col];                        // read key(s)
        kb_rpt[col] ^= key_mask;                        // clear key(s)
    }
    return key_mask;
}

/** The real hardware access take place here.
 *  Each of the rows is individually activated and the resulting column value read.
 *  Should more than 8 channels be needed, this can easily be extended to 16/32bit.
 *  By means of a neat routine found on
 *
 */
void scan_matrix(void)
{
    TRACE("\ns_m ");
    uint8_t i, data;

    for (uint8_t row = 0; row < ROWS; ++row) {
        activate(row);

        // Insert NOPs for synchronization
        _delay_us(20);

        // Place data on all column pins for active row
        // into a single 32 bit value.
        data = read_col();
        /// @see top comment for source of debounce magic
        // Needs to be adjusted for more than 8 columns
        i = kb_state[row] ^ (~data);                    // key changed ?
        ct0[row] = ~( ct0[row] & i );                   // reset or count ct0
        ct1[row] = ct0[row] ^ (ct1[row] & i);           // reset or count ct1
        i &= ct0[row] & ct1[row];                       // count until roll over ?
        kb_state[row] ^= i;                             // then toggle debounced state

        kb_press  [row] |=  kb_state[row] & i;          // 0->1: key press detect
        kb_release[row] |= ~kb_state[row] & i;          // 1->0: key press detect

        if( (kb_state[row] & REPEAT_MASK) == 0 ) {      // check repeat function
            rpt[row] = idle_count + REPEAT_START;       // start delay
        }
        if(  rpt[row] <= idle_count ) {
            rpt[row] = idle_count + REPEAT_NEXT;        // repeat delay
            kb_rpt[row] |= kb_state[row] & REPEAT_MASK;
        }

        // Now evaluate results
        uint8_t p,r,h;
        p=get_kb_press  (ALL_COLS_MASK, row);
        h=get_kb_rpt    (ALL_COLS_MASK, row);
        r=get_kb_release(ALL_COLS_MASK, row);

        rowData[row] = ((rowData[row]|(p|h)) & ~r);
    }
}

void printCurrentKeys(void)
{
    for(uint8_t r=0; r<ROWS/2; ++r) {
        printf("\n");
        for(uint8_t c=0; c< COLS; ++c) {
            if( rowData[r] & (1<<c))
                printf("X");
            else
                printf(".");
        }
        printf("|  |");
        for(uint8_t c=0; c< COLS; ++c) {
            if( rowData[r+ROWS/2] & (1<<c))
                printf("X");
            else
                printf(".");
        }
    }
    printf("\n");
}


void clearActiveKeys()
{
    activeKeys.keycnt=0;
}

/// @todo Switch back from mouse layer!
uint8_t getActiveLayer()
{
    TRACE("gAL ");

    if( g_mouse_keys_enabled)
        return 4; /// @todo  hardcoded layer

    uint8_t layer=0;
    for(uint8_t i=0; i < activeKeys.keycnt; ++i) {
        struct Key k = activeKeys.keys[i];
        if( isLayerKey(k.row, k.col) ) {
            if(layer!=0) {
                // printf("\nWARN: More than one layer key pressed!");
            }
            layer += getModifier(k.row, k.col,0)-MOD_LAYER_0;
        }
    }
    return layer;
}


uint8_t getActiveKeyCodeModifier()
{
    TRACE("gAKCM ");
    for(uint8_t i=0; i < activeKeys.keycnt; ++i) {
        struct Key k=activeKeys.keys[i];
        if(k.normalKey) {
            return getModifier(k.row, k.col, getActiveLayer());
        }
    }
    return 0;
}

// Assuming there is only one active key
uint8_t getActiveModifiers()
{
    TRACE("gAM ");
    uint8_t modifiers=0;
    for(uint8_t i=0; i < activeKeys.keycnt; ++i) {
        struct Key k = activeKeys.keys[i];
        if( isModifierKey(k.row, k.col) ) {
            modifiers |= (1 << (getModifier(k.row, k.col,0)-MOD_L_CTRL));
        }
    }
#ifdef ANALOGSTICK
    modifiers |= analogData.mods;
#endif

    return modifiers;
}


/**
  * check whether layer 0 key is a modifier
  */
bool isModifierKey(uint8_t row, uint8_t col)
{
    TRACE("iMK ");
    if( getModifier(row,col,0) >= MOD_L_CTRL  && getModifier(row,col,0) < MOD_END)
    //if( getKeyCode(row,col,0) >= HID_L_CONTROL  && getKeyCode(row,col,0) <= HID_R_GUI)
        return true;
    return false;
}

/** check whether layer 0 key is a modifier )
  */
bool isLayerKey(uint8_t row, uint8_t col)
{
    TRACE("iLK ");
    if( getModifier(row,col,0) > MOD_LAYER_0 && getModifier(row,col,0) < MOD_LAYER_LAST) {
        return true;
    }
    return false;
}

bool isNormalKey(uint8_t row, uint8_t col)
{
    TRACE("iNK ");
    return !(isLayerKey(row,col) || isModifierKey(row,col));
}


uint8_t getMouseKey(uint8_t row, uint8_t col)
{
    uint16_t hid = SecondaryUsage[row][col];
    if ( hid >= MS_BTN_1 && hid <= MS_SCROLL )
        return (1<<(hid-MS_BTN_1));
    return 0;
}

/**
 * Appends a row/column pair to the currently active key list unless
 * g_mouse_keys are enabled (for a short while after mouse movement)
 * MKT is either stopped or initialyzed, depending on the situation.
 */
void ActiveKeys_Add(uint8_t row, uint8_t col)
{
    if(activeKeys.keycnt>= MAX_ACTIVE_KEYS) {
        //printf("ERR: Too many active keys!");
        return;
    }
    struct Key key;
    key.row=row;
    key.col=col;
    key.normalKey = isNormalKey(row,col);

    // quit early if mouse key is pressed in mouse mode, or end it on other keypress.
    if( g_mouse_keys_enabled ) {
        uint8_t btns = getMouseKey(row, col);
        if(btns) {
            g_mouse_keys |= btns;
            return;
        } else if(isNormalKey(row,col)) { // quicker exit from mousekey mode on other key
            g_mouse_keys_enabled = 0;
        }
    }

    activeKeys.keys[activeKeys.keycnt]=key;
    activeKeys.keycnt++;

    // MKT can only be active for a single pressed key, more than that and we use it as modifier
    if(activeKeys.keycnt>1)
        mkt_state = MKT_TOOMANYKEYS;
    // a possible MKT was found, save timer for timeout
    else if(mkt_state == MKT_INIT && !key.normalKey /*&& activeKeys.keycnt==1*/ ) {
        mkt_state = MKT_YES;
        mkt_key = key;
        mkt_timer=idle_count;
    }
}

/** key matrix is read into rowData[] at this point,
  * now map all active keys
  */
void init_active_keys()
{
    if( rowData[3] & (1<<0) ) {
        rowData[3] &= ~(1<<0);
        setMacroMode(true);
        return;
    }
    // all four corners pressed: switch to command mode
    else if( (rowData[0] & (1<<0)) &&
             (rowData[2] & (1<<0)) &&
             (rowData[4] & (1<<5)) &&
             (rowData[6] & (1<<5)) ) {
        setCommandMode(true);
    }

    // process row/column data to find the active keys
    for (uint8_t row = 0; row < ROWS; ++row) {
        for (uint8_t col = 0; col < COLS; ++col) {
            if (rowData[row] & (1UL << col)) {
                // Check macro and inhibit any keys if valid macro is selected.
                if(macroMode() && activateMacro(row*ROWS+col)) {
                    rowData[row] &= ~(1UL << col);
                    return;
                } else {
                    ActiveKeys_Add(row,col);
                }
            }
        }
    }
}


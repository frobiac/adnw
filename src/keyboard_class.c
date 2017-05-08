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


    Debounce code based on code from Peter Dannegger [danni/At/specs/d0t/de]
        described in German at bottom of page
            http://www.mikrocontroller.net/articles/Entprellung
        and discussed at
            http://www.mikrocontroller.net/topic/48465
    Array extensions inspired by Pult.zip
            http://www.mikrocontroller.net/topic/48465#1489399

*/

#include <limits.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <util/atomic.h>

#include "Keyboard.h"
#include "keyboard_class.h"

#include "keymap.h"
#include "mousekey.h"
#include "macro.h"
#include "matrix.h"
#include "command.h"
#include "jump_bootloader.h"

#include "global_config.h"
#ifdef PS2MOUSE
    #include "trackpoint.h"
#endif

bool    isModifierKey(uint8_t row, uint8_t col);
bool    isLayerKey   (uint8_t row, uint8_t col);
bool    isNormalKey  (uint8_t row, uint8_t col);
uint8_t getMouseKey  (uint8_t row, uint8_t col);
void    getSecondaryUsage(uint8_t r, uint8_t c, uint8_t *hid);

uint8_t getModifier(uint8_t row, uint8_t col, uint8_t layer);

uint8_t getActiveModifiers(void);
uint8_t getActiveLayer(void);
uint8_t getActiveKeyCodeModifier(void);

// #define DBG_2ND_USE

/**
 * SecondUseToggle is a simple state machine that enables keys to:
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
typedef enum {
    SECOND_USE_OFF,
    SECOND_USE_ACTIVE,
    SECOND_USE_REPEAT,
    SECOND_USE_PASSIVE
} SecondUse_State;
typedef enum {
    MOD_TRANS_ON,
    DELAY_MOD_TRANS
} ModifierTransmission_State;

#define SECOND_USE_TIMEOUT 30 // = x/61 [s]: a modekey will remain modifier key if pressed this long; if released earlier the second use normal key is signaled
uint32_t    secondUse_timer;
SecondUse_State secondUse_state=SECOND_USE_OFF;
SecondUse_State secondUse_state_prev=SECOND_USE_OFF;
ModifierTransmission_State prev_modTrans_state = DELAY_MOD_TRANS;
uint8_t modTrans_prev_Mods = 0;
struct Key  secondUse_key;

#define REPEAT_GESTURE_TIMEOUT 10 // = x/61 [s]: a modekey will remain modifier key if pressed this long; if released earlier the second use normal key is signaled
uint32_t    repeatGesture_timer;
bool g_send_now;

inline void zeroReport(USB_KeyboardReport_Data_t *report_data)
{
    // todo: is this neccessary?
    clearActiveKeys();
    memset(&report_data->KeyCode[0], 0, 6 );
    report_data->Modifier=0;
}

/**
  * ISR that should get called 61 times a second.
  * Allows exact timers
  */
ISR(TIMER0_OVF_vect)
{
    ++idle_count;
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

    set_led_color(16,0,0);

    secondUse_timer=idle_count + SECOND_USE_TIMEOUT;

    init_cols();

    set_led_color(0,16,0);

    // @TODO bootloader if CMD_MODE combo is pressed during startup?
    scan_matrix();  // don't remove, required for successful mouse init on boot below.
    _delay_ms(150);

    init_config();

#ifdef PS2MOUSE
    // default to false as not to hang the firmware when something goes wrong with init.
    // Note that scan_matrix() should be called once before this works?!
    g_ps2_connected = 0;
    if(g_cfg.fw.mouse_enabled)
        ps2_init_mouse();
#endif
    set_led_color(0,2,0);
}


// Save all currently pressed keys for later evaluation
void fill_secondUse_Prev_activeKeys(void)
{
    secondUse_Prev_activeKeys.keycnt = activeKeys.keycnt;
    for (uint8_t i = 0; i < activeKeys.keycnt; ++i) {
        secondUse_Prev_activeKeys.keys[i] = activeKeys.keys[i];
    }
}

// Updates new and previous state and prints a debug message
void changeSecondUseState(SecondUse_State currentState, SecondUse_State newState)
{
    if (currentState != newState) {
        secondUse_state = newState;
        secondUse_state_prev = currentState;

#ifdef DBG_2ND_USE
        char stateStr = '-';
        if      (secondUse_state == SECOND_USE_ACTIVE)  stateStr = 'A';
        else if (secondUse_state == SECOND_USE_PASSIVE) stateStr = 'P';
        else if (secondUse_state == SECOND_USE_REPEAT)  stateStr = 'R';
        else if (secondUse_state == SECOND_USE_OFF)     stateStr = 'O';
        xprintf("\n2nd: -> %c (%d->%d active)", stateStr, secondUse_Prev_activeKeys.keycnt, activeKeys.keycnt);
#endif
    }
}

// Send out necessary modifiers
void handleModifierTransmission(USB_KeyboardReport_Data_t* report_data, ModifierTransmission_State newState)
{
    switch( newState ) {
        case MOD_TRANS_ON:
            // remember sent mods to be able to repeat them later
            modTrans_prev_Mods = report_data->Modifier;
            break;

        case DELAY_MOD_TRANS:
            report_data->Modifier = modTrans_prev_Mods;
            break;

        default:    // invalid state, should not happen!
            break;
    }

    if (prev_modTrans_state != newState) {
        prev_modTrans_state = newState;
#ifdef DBG_2ND_USE
        char stateStr = '-' ;
        if (newState == MOD_TRANS_ON) stateStr = 'T';
        else if (newState == DELAY_MOD_TRANS) stateStr = 'D';
        xprintf("; Mod %d->%d %c\n",prev_modTrans_state, newState, stateStr);
#endif
    }
}

/**
 * State machine to handle secondary usage keys.
 *
 * Secondary keys are modifier keys (layer or control) which
 * when pressed and released by themselves emit a normal keycode.
 *
 * This code is quite complex since timeouts, repeats and multiple
 * key combinations all have to work.
 *
 */
void handleSecondaryKeyUsage(USB_KeyboardReport_Data_t* report_data)
{
    switch( secondUse_state ) {
        case SECOND_USE_REPEAT: {
            if( activeKeys.keycnt==1 ) { // only one => previously determined key to repeat still pressed
                zeroReport(report_data);
                getSecondaryUsage(activeKeys.keys[0].row,activeKeys.keys[0].col, &(report_data->KeyCode[0]));
                changeSecondUseState(SECOND_USE_REPEAT, SECOND_USE_REPEAT);
                break;
            } else {  // repeated key was released
                repeatGesture_timer = idle_count;
                changeSecondUseState(SECOND_USE_REPEAT, SECOND_USE_OFF);
            }
        }
        case SECOND_USE_OFF: {
            uint8_t hid;
            getSecondaryUsage(activeKeys.keys[0].row, activeKeys.keys[0].col, &hid);

            if( activeKeys.keycnt==1 && ! activeKeys.keys[0].normalKey &&
                hid!=0 &&
                activeKeys.keys[0].row == secondUse_Prev_activeKeys.keys[0].row &&
                activeKeys.keys[0].col == secondUse_Prev_activeKeys.keys[0].col &&
                idle_count-repeatGesture_timer < REPEAT_GESTURE_TIMEOUT ) {
                changeSecondUseState(SECOND_USE_OFF, SECOND_USE_REPEAT);
                break;
            }
            uint8_t modifierOrLayerKeysPresent = 0;
            // Modifier among pressed keys?
            for (uint8_t i = 0; i < activeKeys.keycnt; ++i) {
                struct Key current_key = activeKeys.keys[i];
                if (!current_key.normalKey) {
                    modifierOrLayerKeysPresent++;
                }
            }

            // activate 2nd-use when modifier is pressed
            if(modifierOrLayerKeysPresent>0) {
                // @TODO should drop extra global g_send_now, maybe by checking in Passive transition instead?
                //       also work with N normal keys prior to modifier...
                // previously pressed key was normal, now a modifier that should probably
                // be interpreted as normal once the first has been released, e.g
                // "t " will only send space if it is pressed after t has been released due to secondary usage of space
                if(secondUse_Prev_activeKeys.keycnt == 1 &&
                   secondUse_Prev_activeKeys.keys[0].normalKey &&
                   activeKeys.keycnt == 2 &&
                   modifierOrLayerKeysPresent==1) {

                    uint8_t key_regular=0, key_second=0;
                    for (uint8_t i = 0; i < activeKeys.keycnt; ++i) {
                        struct Key k = activeKeys.keys[i];
                        if (k.normalKey)
                            key_regular=getKeyCode(k.row, k.col, getActiveLayer());
                        else
                            getSecondaryUsage(k.row, k.col, &key_second);
                    }
                    zeroReport(report_data);
                    report_data->KeyCode[0]=key_regular;
                    report_data->KeyCode[1]=key_second;
                    g_send_now=true;

                } else {
                    changeSecondUseState(SECOND_USE_OFF, SECOND_USE_ACTIVE);
                    secondUse_timer=idle_count;
                    // fix for repeat of last released modifier if another one is held directly afterwards.
                    modTrans_prev_Mods=0;
                    fillReport(report_data);    // prepare to send, but modifiers not just yet
                    handleModifierTransmission(report_data, DELAY_MOD_TRANS);
                }
            } else { // normal key / no modifier
                changeSecondUseState(SECOND_USE_OFF, SECOND_USE_OFF);
            }

            fill_secondUse_Prev_activeKeys();
            break;
        }

        case SECOND_USE_ACTIVE: {
            bool normalKeyPresent = false;
            // Normal key among pressed ?
            for (uint8_t i = 0; i < activeKeys.keycnt; ++i) {
                struct Key current_key = activeKeys.keys[i];
                if (current_key.normalKey) {
                    normalKeyPresent = true;
                    break;
                }
            }
            // Which key was just released ?
            if (activeKeys.keycnt < secondUse_Prev_activeKeys.keycnt) {
                bool secondUseNecessityFound = false;
                for (uint8_t i = 0; i < activeKeys.keycnt; ++i) {
                    struct Key current_key = activeKeys.keys[i];
                    struct Key previous_key = secondUse_Prev_activeKeys.keys[i];
                    if (!(current_key.col == previous_key.col
                          && current_key.row == previous_key.row)) {
                        secondUseNecessityFound = true;
                        secondUse_key = previous_key;
                        break;
                    }
                }
                // the last previously saved key was released
                if (!secondUseNecessityFound) {
                    secondUse_key = secondUse_Prev_activeKeys.keys[secondUse_Prev_activeKeys.keycnt - 1];
                }
                // report without the released key
                fillReport(report_data);
                // and add keycode as only keycode
                getSecondaryUsage(secondUse_key.row,secondUse_key.col, &(report_data->KeyCode[0])  );
                changeSecondUseState(SECOND_USE_ACTIVE, SECOND_USE_PASSIVE);
                // send remaining modifiers and stop delaying
                handleModifierTransmission(report_data, MOD_TRANS_ON);
                // store pressed keys for next 2nd-Use run
                fill_secondUse_Prev_activeKeys();

                repeatGesture_timer = idle_count;
                break; // end switch
            } // less keys than previously

            // normal key or timeout of secondUse timer ends further second use
            if ( normalKeyPresent || (secondUse_timer + SECOND_USE_TIMEOUT < idle_count) ) {
                changeSecondUseState(SECOND_USE_ACTIVE, SECOND_USE_PASSIVE);
                fillReport(report_data);
                handleModifierTransmission(report_data, MOD_TRANS_ON);
                fill_secondUse_Prev_activeKeys();
                break; // end switch
            }

            // Only MKTs were and still are pressed, do not yet emit them though
            if (!normalKeyPresent && activeKeys.keycnt >= secondUse_Prev_activeKeys.keycnt) {
                if(activeKeys.keycnt > secondUse_Prev_activeKeys.keycnt)
                    secondUse_timer=idle_count;
                changeSecondUseState(SECOND_USE_ACTIVE, SECOND_USE_ACTIVE);
                fillReport(report_data);
                handleModifierTransmission(report_data, DELAY_MOD_TRANS);
                fill_secondUse_Prev_activeKeys();
                break; // end switch
            }
            xprintf("2U: unh. ACT");
            break;
        }

        case SECOND_USE_PASSIVE: {
            // number of modifiers now ...
            uint8_t actModNo=0, prevModNo = 0;
            for (uint8_t i = 0; i < activeKeys.keycnt; ++i) {
                struct Key current_key = activeKeys.keys[i];
                if (!current_key.normalKey) {
                    actModNo++;
                }
            }
            // ... and previously
            for (uint8_t i = 0; i < secondUse_Prev_activeKeys.keycnt; ++i) {
                struct Key current_key = secondUse_Prev_activeKeys.keys[i];
                if (!current_key.normalKey) {
                    prevModNo++;
                }
            }
            if (activeKeys.keycnt == 0) {  // nothing is pressed
                changeSecondUseState(SECOND_USE_PASSIVE, SECOND_USE_OFF);
                fill_secondUse_Prev_activeKeys();
                break; // end switch
            }
            // was a modifier released ?
            if(actModNo <= prevModNo) {
                changeSecondUseState(SECOND_USE_PASSIVE, SECOND_USE_PASSIVE);
                fillReport(report_data);
                // send modifiers now
                handleModifierTransmission(report_data, MOD_TRANS_ON);
                fill_secondUse_Prev_activeKeys();
                break; // end switch
            }
            // another modifier pressed?
            if(actModNo > prevModNo) {
                changeSecondUseState(SECOND_USE_PASSIVE, SECOND_USE_ACTIVE);
                secondUse_timer=idle_count;
                fillReport(report_data);
                handleModifierTransmission(report_data, DELAY_MOD_TRANS);
                fill_secondUse_Prev_activeKeys();
                break; // end switch
            }
            xprintf("2U: unh. PASS");
            break; // end switch
        }
        default:
            xprintf("2U: ill sta");
            break;
    }
}



/**
 * This function is periodically called from host.
 *
 * Last chance to modify the data being sent.
 *
 * Logical flow:
 *   - scan matrix -> keyChange() which in turn updates dual usage state
 *   - check overrides like macro or passhash playback
 *   - fill report with keyboard data.
 *
 *
 */
uint8_t getKeyboardReport(USB_KeyboardReport_Data_t *report_data)
{
#ifdef HAS_LED
    set_led();
#endif

    scan_matrix();
    init_active_keys();


#ifdef ANALOGSTICK
    analogDataAcquire();
#endif

    /// @todo return early!

    handleSecondaryKeyUsage(report_data);
    if( secondUse_state != SECOND_USE_OFF || g_send_now ) {
        g_send_now=false;
        // buffer already filled by 2nd-use
        return sizeof(USB_KeyboardReport_Data_t);
    }

    // send pending data to output like a macro or passhash
    if(printOutstr(report_data)) {
        return sizeof(USB_KeyboardReport_Data_t);
    }

    // no other function inhibts sending, so fill report with pending data.
    return fillReport(report_data);
}


/**
 * @brief fillReport
 * @param report_data
 * @return Currently fixed, could be used to signal something to layers above.
 */
uint8_t fillReport(USB_KeyboardReport_Data_t *report_data)
{
    if(activeKeys.keycnt==0) {
        zeroReport(report_data);
        return sizeof(USB_KeyboardReport_Data_t);
    }

    // if layer is set to MOUSEKEY LAYER
    if(getActiveLayer()==(MOD_MOUSEKEY-MOD_LAYER_0)) {
        uint16_t mk_mask=0;
        for(uint8_t i=0; i < activeKeys.keycnt; ++i) {
            struct Key k=activeKeys.keys[i];
            mk_mask |= (1<<(getKeyCode(k.row, k.col, (MOD_MOUSEKEY-MOD_LAYER_0))-1-MS_BEGIN));
        }
        mousekey_activate(mk_mask);

        // @todo: There could technically be keys on mouse layer, but why?
        zeroReport(report_data);
        return sizeof(USB_KeyboardReport_Data_t);
    } else {
        mousekey_activate(0);
    }

    uint8_t idx=0;

    for(uint8_t i=0; i < activeKeys.keycnt; ++i) {
        struct Key k=activeKeys.keys[i];
        uint8_t kc = getKeyCode(k.row, k.col, getActiveLayer());
        if(k.normalKey && idx < 6) {
            report_data->KeyCode[idx]= kc;
            idx++;
        } else if ( kc == HID_R_GUI && idx<6) {
            report_data->KeyCode[idx]= HID_INT1;
            idx++;
        }
        if( HID_CMDMODE == getKeyCode(k.row, k.col, getActiveLayer() ) )
            goto cmdmode;

        if(idx>6) {
            xprintf("\nError: more than 6 keys! ");
            for( uint8_t k=0; k<6; ++k)
                xprintf(" %d ", report_data->KeyCode[k]);
            break;
        }
    }

    report_data->Modifier=getActiveModifiers()|getActiveKeyCodeModifier();
    report_data->Modifier &= ~(1<<(MOD_R_GUI-MOD_FIRST));

    return sizeof(USB_KeyboardReport_Data_t);

cmdmode:
    for (uint8_t row = 0; row < ROWS; ++row)
        rowData[row]=0;
    clearActiveKeys();
    setCommandMode(true);
    zeroReport(report_data);
    return sizeof(USB_KeyboardReport_Data_t);
}


/**
 * Set led blink mode to current configured state in g_cfg.led.
 */

uint8_t rgb[3];

void set_led()
{
#ifdef HAS_LED
    if(g_cfg.led.on==0)
        return;

#ifdef __AVR_ATmega32U4__

    rgb[0]=0; rgb[1]=0; rgb[2]=0;
    if(g_mouse_keys_enabled) {
        rgb[1] = 10;
        set_led_color(rgb[0], rgb[1], rgb[2]);
        return;
    }
    if(commandMode()) {
        if(commandModeSub() == SUB_MACRO_REC) {
            rgb[0] = 50;
        } else if(commandModeSub() == SUB_PASSHASH) {
            rgb[2] = 25;
        } else {
            rgb[0]=15;
        }
        set_led_color(rgb[0], rgb[1], rgb[2]);
        return;
    }

    // global blinking:
    if(idle_count % (g_cfg.led.on+g_cfg.led.off+1)  < g_cfg.led.on) // +1 needed for mod 0 ?
        set_led_color(g_cfg.led.r, g_cfg.led.g, g_cfg.led.b);
    else
        set_led_color(0,0,0);
#endif
#endif
}

void set_led_color(uint8_t r, uint8_t g, uint8_t b)
{
// # ifdef __AVR_ATmega32U4__
#ifdef BLACKFLAT
    OCR4D = g;
#elif defined(BLACKBOWL)
    OCR1A = b;
    OCR1B = r;
    OCR1C = g;
#endif
}


void clearActiveKeys()
{
    activeKeys.keycnt=0;
}

/// @todo Switch back from mouse layer!
uint8_t getActiveLayer()
{

    uint8_t layer=0;
    for(uint8_t i=0; i < activeKeys.keycnt; ++i) {
        struct Key k = activeKeys.keys[i];
        if( isLayerKey(k.row, k.col) ) {
            layer += getModifier(k.row, k.col,0)-MOD_LAYER_0;
        }
    }
    return layer;
}

/**
 *  Returns the modifier needed to send first normal key in active keys.
 *  Example: For "@" on a german PC layout "q"+"AltGr" needs to be send to host.
 *  @todo : Where are the other keys parsed? Would need to split a report in case
 *          another modifier is pressed or needed for later characters.
 */
uint8_t getActiveKeyCodeModifier()
{
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
 * Appends a row/column pair to the currently active key list unless
 * g_mouse_keys are enabled (for a short while after mouse movement)
 * MKT is either stopped or initialized, depending on the situation.
 */
void ActiveKeys_Add(uint8_t row, uint8_t col)
{
    if(activeKeys.keycnt>= MAX_ACTIVE_KEYS) {
        return;
    }

    struct Key key;
    key.row=row;
    key.col=col;
    key.normalKey = isNormalKey(row,col);

    // quit early if mouse key is pressed in mouse mode, or end it on other keypress.
    if( g_mouse_keys_enabled ) {
        uint8_t btns = getMouseKey(row, col);
        if(btns != 0) {
            g_mouse_keys |= btns;
            return;
        } else { /*if(isNormalKey(row,col))*/  // quicker exit from mousekey mode on other key
            enable_mouse_keys(0);
        }
    }

    activeKeys.keys[activeKeys.keycnt]=key;
    activeKeys.keycnt++;
}

/** key matrix is read into rowData[] at this point,
  * now map all active keys
  */
void init_active_keys()
{
    clearActiveKeys();

    // process row/column data to find the active keys
    for (uint8_t row = 0; row < ROWS; ++row) {
        for (uint8_t col = 0; col < COLS; ++col) {
            if (rowData[row] & (1UL << col)) {
                uint8_t offset=0;
#ifdef PINKYDROP
                if(g_cfg.fw.pinkydrop) {
                    if(col == 1 && row < 4)
                        offset = (row == 0 ? 3 : -1); //+(4+row-1)%4;
                    else if( col==5 && row>3)
                        offset = (row == 4 ? 3 : -1); //+(4+row-1)%4;
                }
#endif
                ActiveKeys_Add(row+offset,col);
            }
        }
    }
    return;
}



void hostLEDChange(uint8_t leds)
{
}


void led(uint8_t n)
{
    DDRD |= (1 << 6);
    if(n==0)
        PORTD &= ~(1 << 6);
    else if(n==1)
        PORTD |= (1 << 6);
    return;
}

/**
 * Wake up host on 3 or more pressed keys.
 * @todo: Power saving, idle, sleep...
 */
bool suspend_wakeup_condition()
{
    scan_matrix();
    return (activeKeys.keycnt>2);
}

/**
 * BF: One on PD7 is OC4D, can set OCR4D to 0-255
 * BB: RGB on PB5,6,7 = OC1A, OC1B, 0C1C and Timer 1
 *
 * http://www.mikrocontroller.net/articles/AVR-GCC-Tutorial/Die_Timer_und_Z%C3%A4hler_des_AVR
 * http://blog.saikoled.com/post/43165849837/secret-konami-cheat-code-to-high-resolution-pwm-on
 */
void initPWM()
{
    // timer 4 not available on teensy++2.0 with at90usb1286
#if defined(HAS_LED) && defined(__AVR_ATmega32U4__)
#   ifdef BLACKFLAT
    DDRD = (1<<7);
    //@TODO what is really required to get PWM on PD7?
    TCCR4A = 1 << COM4A1 | 1 << COM4B1 | 1 << PWM4A | 1 << PWM4B; // Enable outputs A and B in PWM mode
    TCCR4B = 1 << CS43 | 1 << CS40;    // Clock divided by 256 for 244kHz PWM
    TCCR4C = 1 << COM4D1 | 1 << PWM4D; // Enable output D in PWM mode
    // OC4C=0xFF; // Set top to FFFF

#   elif defined BLACKBOWL
    //Set 8-bit fast PWM mode for RGB on B5,6,7
    DDRB |= (1<<7)|(1<<6)|(1<<5);

    //Mode and Prescaler (Phase Correct PWM und Prescaler 1)
    TCCR1A |= (1<<COM1A1)|(1<<COM1B1)|(1<<COM1C1)|(1<<WGM11); // 0xAA
    TCCR1B |= (1<<WGM13)|(1<<CS10); // 0x19
    // 16-Bit Resolution ICR1 |= 0xFFFF;
    // 8-Bit Resolution
    ICR1 |= 0x00FF;

#   endif
#endif
}


/**
 * This enables interpretation of key presses as mouse buttons or movements.
 * It is temporarily activated on PS/2 data detection, or
 * when the separate mouse key layer is active.
 *
 */
void enable_mouse_keys(uint8_t on)
{
    if(on!=g_mouse_keys_enabled) {
#if defined (PS2MOUSE)
        tp_sensitivity(on? g_cfg.tp_config.sens : g_cfg.tp_config.sensL);
#endif
        // @TODO must use some way of activating different modes, keeping track of changes:
        // e.g: Start command mode, change some values there, move mouse and while it
        // is still enabled leave command mode...
        //set_led_mode(LED_MOUSE, on); // -> OCR4D= (on ? 20 : 0);
    }
    g_mouse_keys_enabled = on;
}


/** Configures the board hardware and chip peripherals for keyboard and mouse functionality. */
void SetupHardware()
{
#if (ARCH == ARCH_AVR8)
    /* Disable watchdog if enabled by bootloader/fuses */
    MCUSR &= ~(1 << WDRF);
    wdt_disable();

    /* Disable clock division */
    clock_prescale_set(clock_div_1);
#endif

    /* Hardware Initialization */
#ifdef ANALOGSTICK
    Analog_Init();
#endif

    LEDs_Init();
    USB_Init();

    // set up timer
    TCCR0A = 0x00;
    TCCR0B = 0x05;

#ifdef HAS_LED
    // @TODO tensy++2.0 at90usb1286 does not have 4th timer?
#  ifdef BLACKFLAT
    PORTC &= ~(1<<7);
    DDRC |= (1<<7);  // D7 is external LED on BF -> output
#  elif defined(BLACKBOWL)
    // BB: RGB on B5,67=OC1A, OC1B, OC1C
    PORTB &= ~((1<<7)|(1<<6)|(1<<5));
    DDRB |= (1<<7)|(1<<6)|(1<<5);  // B5,6,7 is external RGB LED on BF -> output
#  endif
    initPWM();
#endif

    TIMSK0 = (1<<TOIE0);

    /* Task init */
    initKeyboard();
}


/**
  * check whether layer 0 key is a modifier.
  * @todo : keys that are not modifiers in layer 0 cannot be in other layers (but no sensible use case)
  */
bool isModifierKey(uint8_t row, uint8_t col)
{
    if( getModifier(row,col,0) >= MOD_FIRST  && getModifier(row,col,0) <= MOD_LAST)
        return true;
    return false;
}

/**
 * check whether layer 0 key is a modifier )
 * @todo : keys that are not modifiers in layer 0 cannot be in other layers (but no sensible use case)
 */
bool isLayerKey(uint8_t row, uint8_t col)
{
    if( getModifier(row,col,0) > MOD_LAYER_0 && getModifier(row,col,0) < MOD_LAYER_LAST) {
        return true;
    }
    return false;
}

bool isNormalKey(uint8_t row, uint8_t col)
{
    return !(isLayerKey(row,col) || isModifierKey(row,col));
}

void getSecondaryUsage(uint8_t r, uint8_t c, uint8_t *hid)
{
    memcpy_P(hid, &SecondaryUsage[r][c], sizeof(uint8_t));
}

/**
 * Returns the corresponding mouse hid code, or 0 if none is defined for this matrix position.
 *
 * This enables overiding normal keys when in mouse mode.
 */
uint8_t getMouseKey(uint8_t r, uint8_t c)
{
    uint8_t hid;
    memcpy_P(&hid, &MouseUsage[r][c], sizeof(uint8_t));
    if ( hid >= HID_BTN_L && hid <= HID_BTN_S )
        return (hid);
    return 0;
}


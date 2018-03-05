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
bool    isTappingKey (uint8_t row, uint8_t col);
bool    isLayerKey   (uint8_t row, uint8_t col);
bool    isNormalKey  (uint8_t row, uint8_t col);

uint8_t getMouseKey  (uint8_t row, uint8_t col);
uint8_t getMouseKeyButtonMask(void);

uint8_t getModifier(uint8_t row, uint8_t col, uint8_t layer);

uint8_t getActiveModifiers(void);
uint8_t getActiveLayer(void);
uint8_t getActiveKeyCodeModifier(void);

void reportPrint (USB_KeyboardReport_Data_t * report, char * str);
void printKeys(void);

/**
 * Timeouts in idle_count ticks = x/61 [s]
 *
 * If pressed _and_ released within the timeout (and without any other key presses or releases in between)
 * a dual use key will be interpreted with its tapped meaning, usually a regular printable character.
 * Otherwise, its regular meaning (typically as a modifier) is assumed.
 */
#define TAP_TIMEOUT        30
/**
 * If a released dual use key was tapped and pressed again within the repeat timeout, it is continuously
 * interpreted as tapped key until released.
 */
#define TAP_TIMEOUT_REPEAT 10

#define MAX_ACTIVE_KEYS 10

typedef struct {
    union {
        struct {
            uint8_t col:4;
            uint8_t row:4;
        };
        uint8_t id;
    };

    union {
        struct {
            bool    dual:1;
            uint8_t state:3;
            bool    layer:1;
            bool    mod:1;
            bool    mouse:1;
            bool    unused:1;
        };
        uint8_t mode;
    };

} keypress_t;


keypress_t lastDualUseKey;
keypress_t activeKeys[MAX_ACTIVE_KEYS];

uint8_t activeKeyCount;


enum DualUsageMode {
    DUAL_NONE,              // init, or released as mod
    DUAL_UNDECIDED,         // tap not yet determined
    DUAL_NO_TAP,            // no tap, timed out
    DUAL_TAPPED,            // was tapped
    DUAL_TAPPED_NOW_HELD,   // was tapped and now held
};


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

#ifdef ANALOGSTICK
    analogDataAcquire();
#endif

    // send pending data to output like a macro or passhash
    if(printOutstr(report_data)) {
        return sizeof(USB_KeyboardReport_Data_t);
    }

    if(useAsMouseReport())
        return 0;

    fillKeyboardReport(report_data);

    // clear report in command mode to disable echoing of selected commands.
    if( handleCommand(report_data->KeyCode[0], report_data->Modifier) )
        zeroReport(report_data);

    return sizeof(USB_KeyboardReport_Data_t);
}


void fillKeyboardReport(USB_KeyboardReport_Data_t *report_data)
{
    uint8_t activeLayer = getActiveLayer();

    uint8_t idx=0;
    uint8_t kc;

    //@TODO modified characters like "@" or "%" ? Unlikely, but currently not supported...
    if(lastDualUseKey.state == DUAL_TAPPED || lastDualUseKey.state == DUAL_TAPPED_NOW_HELD) {
        kc = getKeyCode(lastDualUseKey.row, lastDualUseKey.col, 0);
        report_data->KeyCode[idx]= kc;
        idx++;
        if(lastDualUseKey.state == DUAL_TAPPED)
            lastDualUseKey.state = DUAL_NONE; // remove after handling

    }

    for(uint8_t i=0; i < activeKeyCount; ++i) {
        uint8_t row = activeKeys[i].row;
        uint8_t col = activeKeys[i].col;


        kc = getKeyCode(row, col, activeLayer);
        if(isNormalKey(row,col) && idx < 6) {
            report_data->KeyCode[idx]= kc;
            idx++;
        }

        // @TODO Hack to be able to use Int1 as Hyper modifier
        // It should be handled as a modifier in all regards up until sending
        // Thus HID_R_GUI is used until here, and now replaced here and at modifiers below...
        if ( kc == HID_R_GUI && idx<6) {
            report_data->KeyCode[idx]= HID_INT1;
            idx++;
        }

        if( HID_CMDMODE == getKeyCode(row, col, activeLayer) )
            goto cmdmode;

        if(idx>=6)
            break;
    }

    report_data->Modifier=getActiveModifiers()|getActiveKeyCodeModifier();
    report_data->Modifier &= ~(1<<(MOD_R_GUI-MOD_FIRST));

#ifdef REORDER_REPORT_LIBINPUT_BUG
    // Debian unstable bug 20171202 with x.org only parsing Modifier on second consecutive press
    // [Bug 104030](https://bugs.freedesktop.org/show_bug.cgi?id=104030)
    // Sending only modifier first solves this, but modifier + dualuse (like Alt-Enter) breaks
    // so store delayed keycode for now (dualuse ?)
    {
        static USB_KeyboardReport_Data_t prev;
        static uint8_t delayed = 0;


        if(delayed != 0) {
            if(report_data->KeyCode[0] == delayed)
                delayed=0;
            else if(report_data->KeyCode[0] == 0) {
                report_data->KeyCode[0] = delayed;
                delayed=0;
            }
        }
        if(0!=memcmp(&prev,report_data,sizeof(USB_KeyboardReport_Data_t))) {
            if(prev.KeyCode[0] == 0 && report_data->KeyCode[0] !=0 &&
               report_data->Modifier != 0 ) {
                delayed = report_data->KeyCode[0];
                memcpy(&prev,report_data,sizeof(USB_KeyboardReport_Data_t));
                report_data->KeyCode[0]=0;
            } else {
                memcpy(&prev,report_data,sizeof(USB_KeyboardReport_Data_t));
            }
        }
    }
#endif

    // Activate for trace
    // reportPrint(report_data, "");
    return;

cmdmode:
    setCommandMode(true);
    zeroReport(report_data);
}


void reportPrint(USB_KeyboardReport_Data_t * report, char * str)
{
    static uint8_t bu[8];

    if(0!=memcmp(bu,report,8)) {
        xprintf("\n%s %04X|", str, report->Modifier);
        for( uint8_t k=0; k<6; ++k)
            xprintf("%02X ", report->KeyCode[k]);
    }
    memcpy(bu,report,8);
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
            //} else if(commandModeSub() == SUB_PASSHASH) {
            //    rgb[2] = 25;
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


/// @todo Switch back from mouse layer!
uint8_t getActiveLayer()
{
    uint8_t layer=0;
    for(uint8_t i=0; i < activeKeyCount; ++i) {
        uint8_t row = activeKeys[i].row;
        uint8_t col = activeKeys[i].col;
        if(activeKeys[i].id == lastDualUseKey.id && lastDualUseKey.state != DUAL_NO_TAP)
            continue;
        if( isLayerKey(row, col)) {
            layer += (getModifier(row, col,0)&0x0F);
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
    for(uint8_t i=0; i < activeKeyCount; ++i) {
        if(activeKeys[i].id == lastDualUseKey.id && lastDualUseKey.state != DUAL_NO_TAP)
            continue;
        if( isNormalKey(activeKeys[i].row, activeKeys[i].col)) {
            uint8_t mod = getModifier(activeKeys[i].row, activeKeys[i].col, getActiveLayer());
            if(mod & MODHID_MASK)
                return (1<<(mod&0x0F));
            return 0;
        }
    }
    return 0;
}

// Assuming there is only one active key
uint8_t getActiveModifiers()
{
    uint8_t modifiers=0;
    for(uint8_t i=0; i < activeKeyCount; ++i) {
        if(activeKeys[i].id == lastDualUseKey.id && lastDualUseKey.state != DUAL_NO_TAP)
            continue;
        if( isModifierKey(activeKeys[i].row, activeKeys[i].col)) {
            modifiers += (1<< (getModifier(activeKeys[i].row, activeKeys[i].col,0)&0x0F));
        }
    }
    return modifiers;
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
    return (activeKeyCount>2);
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


bool useAsMouseReport(void)
{
    // Intercept here if we are in mousekey layer...
    // @TODO check for mousemode should go directly into descriptor polling
    if(getActiveLayer() == (MOD_MOUSEKEY-MOD_LAYER_0) ) {
        uint16_t mk_mask=0;
        for(uint8_t i=0; i < activeKeyCount; ++i) {
            uint8_t mk_key = getKeyCode(activeKeys[i].row, activeKeys[i].col, (MOD_MOUSEKEY-MOD_LAYER_0));
            if(mk_key != 0)
                mk_mask |= (1<<(mk_key-MS_BEGIN));
        }
        mousekey_activate(mk_mask);
        return true;
    } else {
        mousekey_activate(0);
    }

    if(g_mouse_keys_enabled) {
        g_mouse_keys = getMouseKeyButtonMask();
        return true;
    }
    return false;
}

uint8_t getMouseKeyButtonMask(void)
{
    uint8_t btns=0;
    for(uint8_t i=0; i<activeKeyCount; ++i) {
        uint8_t mk = getMouseKey(activeKeys[i].row, activeKeys[i].col);
        if(mk>=MS_BTN1 && mk<=MS_BTN5)
            btns |= (1<<(mk-MS_BTN1));
    }
    return btns;
}


void addKey(uint8_t row, uint8_t col)
{
    if(activeKeyCount>=MAX_ACTIVE_KEYS-1) {
        xprintf("\nErr: +");
        printKeys();
        activeKeyCount=0;
        return;
    }
    activeKeys[activeKeyCount].row=row;
    activeKeys[activeKeyCount].col=col;
    //activeKeys[activeKeyCount].hid=getModifier(row, col);
    //activeKeys[activeKeyCount].mod=getModifier(row, col);
    ++activeKeyCount;

    // immediately exit mouse mode on non-mousekey press.
    if( g_mouse_keys_enabled ) {
        if(! getMouseKey(row, col))
            enable_mouse_keys(0);
    }
}


void delKey(uint8_t row, uint8_t col)
{
    if(activeKeyCount<1) {
        xprintf("\nErr: -");
        return;
    }
    for(uint8_t i=activeKeyCount-1; i>=0; --i) {
        if(activeKeys[i].row == row && activeKeys[i].col == col) {
            activeKeys[i] = activeKeys[activeKeyCount-1];
            --activeKeyCount;
            return;
        }
    }
}


void printKeys(void)
{
    xprintf("%d: ", activeKeyCount);
    if(lastDualUseKey.state != DUAL_NONE) {
        xprintf("DU %1X%1X=%1d|", lastDualUseKey.row, lastDualUseKey.col, lastDualUseKey.state);
    } else {
        xprintf("DU   = |");
    }

    for(uint8_t i=0; i<activeKeyCount; ++i) {
        if(activeKeys[i].id != lastDualUseKey.id)
            xprintf("%02X ", activeKeys[i].id );
    }
}


uint8_t activeKeysCount(void)
{
    return activeKeyCount;
}

/**
 *  React to any key change event emitted from scan_matrix().
 *
 *  Specified row/col key was either pressed, released or is still held.
 *  When holding occurs is determined by repeat timeout in scan_matrix().
 *
 *  To be able to fill a keyboard report when requested by host, we need to keep track of
 *  all keys somehow and map them to actual keycodes.
 *
 *  Regular keys, modifiers and layers are rather easy to parse at that time as each
 *  combination of any of them will be assembled in one report by adding all modifiers
 *  and taking the symbol(s) from the selected layer.
 *
 *  For dual use keys a special handling is required since they
 *  - change their meaning depending on timeouts and other keys pressed
 *  - may need to emit after having been released
 *
 *  Luckily, this handling only affects the last pressed dual use key:
 *
 *  State of the saved dual use key:
 *
 *  undecided : a new dual use key is pressed. Update save.
 *  no_tap    : if another key is pressed, or the saved key was not in tap_rep and is now held
 *  tap       : saved key in init and released within tap timeout
 *  tap_rep   : saved key in tap  and pressed again within tap_repeat timeout.
 *
 *  If the last key pressed was
 *
 */
void keyIdChange(uint8_t row, uint8_t col, uint8_t change)
{
    static uint8_t lastDualUse __attribute__((unused));
    static uint8_t lastPressRelease;
    static uint32_t tapStartTime;
    static uint32_t tapRepeatTime;

    uint8_t id = (row<<4|col);

    if(isTappingKey(row, col)) {    // change is a dual use key
        if(change!=2) {
            // state is set below
            lastDualUseKey =  (keypress_t) { .row=row, .col=col, .dual=true };
        }
        if(change==2) { // held -> dual use is mod
            if(lastDualUseKey.state != DUAL_TAPPED_NOW_HELD)
                lastDualUseKey.state = DUAL_NO_TAP;
        } else if(change==1) { // pressed again after release, check time for repeat, maybe normal
            if(id==lastPressRelease && idle_count - tapRepeatTime < TAP_TIMEOUT_REPEAT) {
                lastDualUseKey.state = DUAL_TAPPED_NOW_HELD;
            } else {
                lastDualUseKey.state = DUAL_UNDECIDED;
            }
            tapStartTime = idle_count; // no additional press on fast release
        } else if(change==0) { // released again, check time for tap
            lastDualUseKey.state = DUAL_NONE;
            if(id==lastPressRelease && idle_count - tapStartTime < TAP_TIMEOUT) {
                lastDualUseKey.state = DUAL_TAPPED;
            }
            tapRepeatTime = idle_count;
        }
    } else {
        // non-dual use key changed.
        lastDualUseKey.state = DUAL_NO_TAP;
    };

    // any key is added or removed from list - latest dual use is handled during
    // modifier, layer or tap analysis
    if(change==1)
        addKey(row, col);
    else if (change==0)
        delKey(row, col);

    // last press or release id must also be updated
    if(change!=2) {
        lastPressRelease = id;
    }

    //printKeys();
}

void keyChange(uint8_t row, column_size_t p, column_size_t h, column_size_t r)
{
    for(uint8_t col=0; col<COLS; ++col) {
        uint8_t mask=1<<col;
        if(p&mask)
            keyIdChange(row, col, 1);
        if(h&mask)
            keyIdChange(row, col, 2);
        if(r&mask)
            keyIdChange(row, col, 0);
    }
}

/**
  * check whether layer 0 key is a modifier.
  * @todo : keys that are not modifiers in layer 0 cannot be in other layers (but no sensible use case)
  */
bool isModifierKey(uint8_t row, uint8_t col)
{
    return ( MODKEY_MASK & getModifier(row,col,0));
}

bool isTappingKey(uint8_t row, uint8_t col)
{
    return ( TAPPING_MASK & getModifier(row,col,0));
}

/**
 * check whether layer 0 key is a modifier )
 * @todo : keys that are not modifiers in layer 0 cannot be in other layers (but no sensible use case)
 */
bool isLayerKey(uint8_t row, uint8_t col)
{
    return ( LAYER_MASK & getModifier(row,col,0));
}

bool isNormalKey(uint8_t row, uint8_t col)
{
    return !(isLayerKey(row,col) || isModifierKey(row,col));
}


/**
 * Returns the corresponding mouse hid code, or 0 if none is defined for this matrix position.
 *
 * This enables overiding normal keys when in mouse mode.
 */
uint8_t getMouseKey(uint8_t r, uint8_t c)
{
    return getKeyCode(r, c, MOD_MOUSEKEY-MOD_LAYER_0);
}


/*
void hostLEDChange(uint8_t leds)
{
//    printf("\nLED 0x%02x, C%d N%d S%d", leds, (leds & HID_KEYBOARD_LED_CAPSLOCK)>0,
//                  (leds & HID_KEYBOARD_LED_NUMLOCK)>0, (leds & HID_KEYBOARD_LED_SCROLLLOCK)>0);
}
*/

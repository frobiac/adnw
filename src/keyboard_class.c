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
#include "macro.h"
#include "matrix.h"
#include "command.h"
#include "jump_bootloader.h"

uint8_t lastKeyCode;
uint8_t rowData[ROWS];
uint8_t prevRowData[ROWS];

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
    SECOND_USE_OFF ,
    SECOND_USE_ACTIVE ,
    SECOND_USE_PASSIVE
} SecondUse_State;
typedef enum {
    MOD_TRANS_ON ,
    RETARD_MOD_TRANS
} ModifierTransmission_State;

#define SECOND_USE_TIMEOUT 30 // = x/61 [s]: a modekey will remain modifier key if pressed this long; if released earlier the second use normal key is signaled
uint32_t    secondUse_timer;
SecondUse_State secondUse_state=SECOND_USE_OFF;
SecondUse_State secondUse_state_prev=SECOND_USE_OFF;
ModifierTransmission_State prev_modTrans_state = RETARD_MOD_TRANS;
uint8_t modTrans_prev_Mods = 0;
struct Key  secondUse_key;

#define REPEAT_GESTURE_TIMEOUT 30 // = x/61 [s]: a modekey will remain modifier key if pressed this long; if released earlier the second use normal key is signaled
uint32_t    repeatGesture_timer;


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
    secondUse_timer=idle_count + SECOND_USE_TIMEOUT;

#ifdef DEBUG_OUTPUT
    stdio_init();
#endif
    init_cols();

    initMacros();
}

void clearRowData() {
    for (uint8_t row = 0; row < ROWS; ++row) {
        rowData[row]=0;
        ct0[row]=0xFF;
        ct1[row]=0xFF;
    }
}

//void send_extra(void){
//  USB_ExtraReport_Data_t report_data = {
//      .report_id = REPORT_ID_CONSUMER,
//      .usage = 0
//  };
//  static uint16_t prevRowData[ROWS];
//  bool changeInMatrixDetected=false;
//
//  uint16_t consumer_code = 0;
//  uint16_t system_code = 0;
//  uint8_t code = 0;
//  uint8_t idx=0;
//
//  // scan for changes:
//  for (uint8_t row = 0; row < ROWS; ++row) {
//    if (rowData[row] != prevRowData[row]){
//      changeInMatrixDetected = true;
//    }
//    prevRowData[row] = rowData[row];
//  }
//
//  // scan keys
//  for(uint8_t i=0; i < activeKeys.keycnt; ++i) {
//      struct Key k=activeKeys.keys[i];
//      if(k.normalKey && idx < 6) {
//          code=getKeyCode(k.row, k.col, getActiveLayer());
//          idx++;
//      }
//      if(idx>6) {
//          printf("\nError: more than 6 keys! ");
//          break;
//      }
//  }
//
//  // System Control
//   if (code == KB_SYSTEM_POWER) {
//      system_code = SYSTEM_POWER_DOWN;
//  } else if (code == KB_SYSTEM_SLEEP) {
//      system_code = SYSTEM_SLEEP;
//  } else if (code == KB_SYSTEM_WAKE) {
//      system_code = SYSTEM_WAKE_UP;
//  }
//  // Consumer Page
//  else if (code == KB_AUDIO_MUTE) {
//      consumer_code = AUDIO_MUTE;
//  } else if (code == KB_AUDIO_VOL_UP) {
//      consumer_code = AUDIO_VOL_UP;
//  } else if (code == KB_AUDIO_VOL_DOWN) {
//      consumer_code = AUDIO_VOL_DOWN;
//  }
//  else if (code == KB_MEDIA_NEXT_TRACK) {
//      consumer_code = TRANSPORT_NEXT_TRACK;
//  } else if (code == KB_MEDIA_PREV_TRACK) {
//      consumer_code = TRANSPORT_PREV_TRACK;
//  } else if (code == KB_MEDIA_STOP) {
//      consumer_code = TRANSPORT_STOP;
//  } else if (code == KB_MEDIA_PLAY_PAUSE) {
//      consumer_code = TRANSPORT_PLAY_PAUSE;
//  } else if (code == KB_MEDIA_SELECT) {
//      consumer_code = AL_CC_CONFIG;
//  }
//  else if (code == KB_MAIL) {
//      consumer_code = AL_EMAIL;
//  } else if (code == KB_CALCULATOR) {
//      consumer_code = AL_CALCULATOR;
//  } else if (code == KB_MY_COMPUTER) {
//      consumer_code = AL_LOCAL_BROWSER;
//  }
//  else if (code == KB_WWW_SEARCH) {
//      consumer_code = AC_SEARCH;
//  } else if (code == KB_WWW_HOME) {
//      consumer_code = AC_HOME;
//  } else if (code == KB_WWW_BACK) {
//      consumer_code = AC_BACK;
//  } else if (code == KB_WWW_FORWARD) {
//      consumer_code = AC_FORWARD;
//  } else if (code == KB_WWW_STOP) {
//      consumer_code = AC_STOP;
//  } else if (code == KB_WWW_REFRESH) {
//      consumer_code = AC_REFRESH;
//  } else if (code == KB_WWW_FAVORITES) {
//      consumer_code = AC_BOOKMARKS;
//  }
//
//  // fill report
//   if (system_code != 0){
//     report_data.report_id=REPORT_ID_SYSTEM;
//     report_data.usage=system_code;
//   }
//   else { // fill in consumer code or empty report (consumer_code=0)
//     report_data.report_id=REPORT_ID_CONSUMER;
//     report_data.usage=consumer_code;
//   }
//
//   // send
//   if ((system_code + consumer_code > 0) &&
//       changeInMatrixDetected) {
//     Endpoint_SelectEndpoint(EXTRA_IN_EPADDR);
//     if (Endpoint_IsReadWriteAllowed()) {
//       Endpoint_Write_Stream_LE(&report_data, sizeof(USB_ExtraReport_Data_t), NULL);
//       Endpoint_ClearIN();
//       printf("\n extra send %x",system_code + consumer_code);
//     }
//   }
//}


uint8_t getExtraReport(USB_ExtraReport_Data_t *report_data)
{
    uint16_t consumer_code = 0;
    uint16_t system_code = 0;
    uint16_t code = 0;

    // scan keys
    clearActiveKeys();
    scan_matrix();
    init_active_keys();

    // no keys or macromode or command mode
    // then send empty report
    if(activeKeys.keycnt==0 ||
            macroMode() ||
            commandMode()) {
        // empty report
        report_data->report_id=REPORT_ID_CONSUMER;
        report_data->usage=0;
        return sizeof(USB_ExtraReport_Data_t);
    }

    for(uint8_t i=0; i < activeKeys.keycnt; ++i) {
        struct Key k=activeKeys.keys[i];
        if(k.normalKey &&
                (USAGE_PAGE(code) != page_keyboard)) {
            code=getKeyCode(k.row, k.col, getActiveLayer());
            break;
        }
    }


    // System Control
    if (USAGE_PAGE(code) == page_system) {
        system_code = USAGE_ID(code);
        report_data->report_id=REPORT_ID_SYSTEM;
        report_data->usage=system_code;
    }
    // Consumer Page
    else if (USAGE_PAGE(code) == page_consumer) {
        consumer_code = USAGE_ID(code);
        report_data->report_id=REPORT_ID_CONSUMER;
        report_data->usage=consumer_code;
    }
    else { // fill in empty report (consumer_code=0)
        report_data->report_id=REPORT_ID_CONSUMER;
        report_data->usage=0;
    }

    return sizeof(USB_ExtraReport_Data_t);
}

// merke alle gedrŸckten Tasten fŸr spŠter
void fill_secondUse_Prev_activeKeys(void) {
    secondUse_Prev_activeKeys.keycnt = activeKeys.keycnt;
    for (uint8_t i = 0; i < activeKeys.keycnt; ++i) {
        secondUse_Prev_activeKeys.keys[i] = activeKeys.keys[i];
    }
}

void changeSecondUseState(SecondUse_State currentState, SecondUse_State newState) {
    char * stateStr;
    if (currentState != newState) {
        secondUse_state = newState;
        secondUse_state_prev = currentState;
        if (secondUse_state == SECOND_USE_ACTIVE) stateStr = "SECOND_USE_ACTIVE";
        if (secondUse_state == SECOND_USE_PASSIVE) stateStr = "SECOND_USE_PASSIVE";
        if (secondUse_state == SECOND_USE_OFF) stateStr = "SECOND_USE_OFF";
        printf("\nSecondUse: next state: %s",stateStr);
    }
}

void handleModifierTransmission(USB_KeyboardReport_Data_t* report_data, ModifierTransmission_State newState) {
    // Jeweils die richtigen Modifier nach drau§en weitergeben
    switch( newState ) {
        case MOD_TRANS_ON:
            // †bertragene Modifier merken, um sie spŠter wiederholen zu kšnnen
            modTrans_prev_Mods = report_data->Modifier;
            break; // switch beenden

        case RETARD_MOD_TRANS:
            report_data->Modifier = modTrans_prev_Mods;
            break; // switch beenden

        default:
            // ungŸltiger Zustand! Sollte eigentlich nie auftreten
            printf("ModifierTransmission: illegal state");
            break;
    }
    // Loggen, wenn sich der Zustand Šndert
    if (prev_modTrans_state != newState) {
        char * stateStr;
        prev_modTrans_state = newState;
        if (newState == MOD_TRANS_ON) stateStr = "MOD_TRANS_ON";
        if (newState == RETARD_MOD_TRANS) stateStr = "RETARD_MOD_TRANS";
        printf("; ModifierTransmission: next state: %s\n",stateStr);
    }
}

void handleSecondaryKeyUsage(USB_KeyboardReport_Data_t* report_data) {

    switch( secondUse_state ) {
        case SECOND_USE_OFF:
        {
            bool modifierOrLayerKeyPresent = false;
            // Gibt es eine Modifier-Taste in den bisher gedrŸckten Tasten?
            for (uint8_t i = 0; i < activeKeys.keycnt; ++i) {
                struct Key current_key = activeKeys.keys[i];
                if (!current_key.normalKey) {
                    modifierOrLayerKeyPresent = true;
                    break;
                }
            }

            // Wenn ein Modifier gedrŸckt wurde, schalte den SecondUse-Modus scharf
            if(modifierOrLayerKeyPresent) {
                changeSecondUseState(SECOND_USE_OFF, SECOND_USE_ACTIVE);
                secondUse_timer=idle_count;
                // senden vorbereiten
                fillReport(report_data);
                // Modifier noch nicht Ÿbermitteln
                handleModifierTransmission(report_data, RETARD_MOD_TRANS);
                // merke die gedrŸckten Tasten fŸr die nŠchste SecondUse Beurteilung.
                fill_secondUse_Prev_activeKeys();
                break; // switch beenden
            }
            // normale Tasten gedrŸckt, aber kein Modifier
            else {
                changeSecondUseState(SECOND_USE_OFF, SECOND_USE_OFF);
                // merke die gedrŸckten Tasten fŸr die nŠchste SecondUse Beurteilung.
                fill_secondUse_Prev_activeKeys();
                break; // switch beenden
            }
            printf("SecondUse: unhandled event in SECOND_USE_OFF");
            break; // switch beenden
        }
        case SECOND_USE_ACTIVE:
        {
            bool normalKeyPresent = false;
            // Gibt es normale Taste in den bisher gedrŸckten Tasten?
            for (uint8_t i = 0; i < activeKeys.keycnt; ++i) {
                struct Key current_key = activeKeys.keys[i];
                if (current_key.normalKey) {
                    normalKeyPresent = true;
                    break;
                }
            }
            // Wenn eine Taste weniger vorliegt, dann prŸfe, welche fehlt und ermittle deren SecondaryUsage-Code
            if (activeKeys.keycnt < secondUse_Prev_activeKeys.keycnt) {
                // ermittle den MKT-key (den Weggefallenen)
                bool secondUseNecessityFound = false;
                // entweder findet sich die unterschiedliche Taste unter den ersten Tasten
                for (uint8_t i = 0; i < activeKeys.keycnt; ++i) {
                    struct Key current_key = activeKeys.keys[i];
                    struct Key previous_key = secondUse_Prev_activeKeys.keys[i];
                    if (!(current_key.col == previous_key.col
                            && current_key.row == previous_key.row)) {
                        secondUseNecessityFound = true;
                        secondUse_key = previous_key;
                        break; // for beenden
                    }
                }
                // oder die unterschiedliche Taste ist die Letzte im Feld der zwischengespeicherten Tasten
                if (!secondUseNecessityFound) {
                    secondUse_key = secondUse_Prev_activeKeys.keys[secondUse_Prev_activeKeys.keycnt - 1];
                }
                // Report ohne den MKT-key befŸllen
                fillReport(report_data);
                // secondaryModifierUsageMatrix-Code ermitteln
                uint8_t currentLayoutNr = eeprom_read_byte(&alternateLayoutNr);
                // den MK-key als (einzigen) Nichtmodifier hinzufŸgen:
                report_data->KeyCode[0] = USAGE_ID(secondaryModifierUsageMatrix[currentLayoutNr][secondUse_key.row][secondUse_key.col]);;
                changeSecondUseState(SECOND_USE_ACTIVE, SECOND_USE_PASSIVE);
                // Restliche Modifier zusammen mit secondUse_key Ÿbermitteln und nicht weiter verzšgern
                handleModifierTransmission(report_data, MOD_TRANS_ON);
                // merke die gedrŸckten Tasten fŸr die nŠchste SecondUse Beurteilung.
                fill_secondUse_Prev_activeKeys();
                // ziehe Timer fŸr Tasten-Repeat fŸr SecondUse auf:
                repeatGesture_timer = idle_count;
                break; // switch beenden
            }
            // timeout of secondUse timer prevents further second use
            if (secondUse_timer + SECOND_USE_TIMEOUT < idle_count) {
                changeSecondUseState(SECOND_USE_ACTIVE, SECOND_USE_PASSIVE);
                // senden vorbereiten
                fillReport(report_data);
                // Modifier nun Ÿbermitteln und nicht weiter verzšgern
                handleModifierTransmission(report_data, MOD_TRANS_ON);
                // merke die gedrŸckten Tasten fŸr die nŠchste SecondUse Beurteilung.
                fill_secondUse_Prev_activeKeys();
                break; // switch beenden
            }
            // Wenn eine normale Taste in den bisher gedrŸckten Tasten befindet
            if (normalKeyPresent) {
                changeSecondUseState(SECOND_USE_ACTIVE, SECOND_USE_PASSIVE);
                // senden vorbereiten
                fillReport(report_data);
                // Modifier nun Ÿbermitteln und nicht weiter verzšgern
                handleModifierTransmission(report_data, MOD_TRANS_ON);
                // merke die gedrŸckten Tasten fŸr die nŠchste SecondUse Beurteilung.
                fill_secondUse_Prev_activeKeys();
                break; // switch beenden
            }
            // Wenn lauter MKTs bisher gedrŸckt waren und weiter gehalten werden
            // Microsoft mag nicht ALT signalisiert und spŠter ein Buchstabe (z.B. Word: Alt->MenŸshortcutmode: ->BKSP gibt nicht im MenŸ, also 'beep')
            // also alle allein gedrŸckten Modifier unterdrŸcken, weil wir wollen eh SecondUse ausgeben in diesem Zustand
            if (!normalKeyPresent &&
                    activeKeys.keycnt == secondUse_Prev_activeKeys.keycnt) {
                changeSecondUseState(SECOND_USE_ACTIVE, SECOND_USE_ACTIVE);
                // senden vorbereiten
                fillReport(report_data);
                // Modifier noch nicht Ÿbermitteln
                handleModifierTransmission(report_data, RETARD_MOD_TRANS);
                // merke die gedrŸckten Tasten fŸr die nŠchste SecondUse Beurteilung.
                fill_secondUse_Prev_activeKeys();
                break; // switch beenden
            }
            // Wenn lauter MKTs bisher gedrŸckt waren und nun einer dazukam
            if (!normalKeyPresent &&
                    activeKeys.keycnt > secondUse_Prev_activeKeys.keycnt) {
                // timer neu aufziehen
                changeSecondUseState(SECOND_USE_ACTIVE, SECOND_USE_ACTIVE);
                secondUse_timer=idle_count;
                // senden vorbereiten
                fillReport(report_data);
                // Modifier noch nicht Ÿbermitteln
                handleModifierTransmission(report_data, RETARD_MOD_TRANS);
                // merke die gedrŸckten Tasten fŸr die nŠchste SecondUse Beurteilung.
                fill_secondUse_Prev_activeKeys();
                break; // switch beenden
            }
            printf("SecondUse: unhandled event in SECOND_USE_ACTIVE");
            break; // switch beenden
        }

        case SECOND_USE_PASSIVE:
        {
            // aktuelle Zahl der Modifier ermitteln
            uint8_t actModNo=0,prevModNo = 0;
            for (uint8_t i = 0; i < activeKeys.keycnt; ++i) {
                struct Key current_key = activeKeys.keys[i];
                if (!current_key.normalKey) {
                    actModNo++;
                }
            }
            // vorherige Zahl der Modifier ermitteln
            for (uint8_t i = 0; i < secondUse_Prev_activeKeys.keycnt; ++i) {
                struct Key current_key = secondUse_Prev_activeKeys.keys[i];
                if (!current_key.normalKey) {
                    prevModNo++;
                }
            }
            // keine Tasten gedrŸckt?
            if (activeKeys.keycnt == 0) {
                changeSecondUseState(SECOND_USE_PASSIVE, SECOND_USE_OFF);
                // merke die gedrŸckten Tasten fŸr die nŠchste SecondUse Beurteilung.
                fill_secondUse_Prev_activeKeys();
                break; // switch beenden
            }
            // ein Modifier weniger? (evtl. zusŠtzlich zu normalen Tasten)
            if(actModNo <= prevModNo) {
                changeSecondUseState(SECOND_USE_PASSIVE, SECOND_USE_PASSIVE);
                // senden vorbereiten
                fillReport(report_data);
                // Modifier nun Ÿbermitteln und nicht weiter verzšgern
                handleModifierTransmission(report_data, MOD_TRANS_ON);
                // merke die gedrŸckten Tasten fŸr die nŠchste SecondUse Beurteilung.
                fill_secondUse_Prev_activeKeys();
                break; // switch beenden
            }
            // ein Modifier mehr? (evtl. zusŠtzlich zu normalen Tasten)
            if(actModNo > prevModNo) {
                // timer neu aufziehen
                changeSecondUseState(SECOND_USE_PASSIVE, SECOND_USE_ACTIVE);
                secondUse_timer=idle_count;
                // senden vorbereiten
                fillReport(report_data);
                // Modifier noch nicht Ÿbermitteln
                handleModifierTransmission(report_data, RETARD_MOD_TRANS);
                // merke die gedrŸckten Tasten fŸr die nŠchste SecondUse Beurteilung.
                fill_secondUse_Prev_activeKeys();
                break; // switch beenden
            }
            printf("SecondUse: unhandled event in SECOND_USE_PASSIVE");
            break; // switch beenden
        }
        default:
            // ungŸltiger Zustand! Sollte eigentlich nie auftreten
            printf("SecondUse: illegal state");
            break;
    }
}


uint8_t getKeyboardReport(USB_KeyboardReport_Data_t *report_data)
{

    clearActiveKeys();
    scan_matrix();

    init_active_keys();


#ifdef ANALOGSTICK
    analogDataAcquire();
#endif

    // send empty report in command mode
    if(commandMode()) {
        handleCommand();
        report_data->Modifier=0;
        memset(&report_data->KeyCode[0], 0, 6);
        return sizeof(USB_KeyboardReport_Data_t);
    }

    handleSecondaryKeyUsage(report_data);
    if( secondUse_state == SECOND_USE_ACTIVE || secondUse_state == SECOND_USE_PASSIVE) {
        // Sendepuffer bereits gefŸllt und los gehts...
        return sizeof(USB_KeyboardReport_Data_t);
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
 *  By means of a neat routine found on http://hackaday.com/2010/11/09/debounce-code-one-post-to-rule-them-all/
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
 * MKT is either stopped or initialized, depending on the situation.
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
        if(btns != 0) {
            g_mouse_keys |= btns;
            return;
        } else if(isNormalKey(row,col)) { // quicker exit from mousekey mode on other key
            g_mouse_keys_enabled = 0;
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
    if( rowData[3] & (1<<0) ) {
        rowData[3] &= ~(1<<0);
        setMacroMode(true);
        return;
    }
    // all four corners pressed: switch to command mode
    else if(// (rowData[0] & (1<<0)) &&
        (rowData[2] & (1<<0)) &&
        // (rowData[4] & (1<<5)) &&
        (rowData[6] & (1<<5)) ) {
        setCommandMode(true);
        return;
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


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

#include "macro.h"
#include "Keyboard.h"

uint8_t curMacro  = MACROCOUNT;
uint8_t sendEmpty = 0;    // empty report needed to send the same character twice in a row
uint8_t idx=0;


bool macromode=false;

char macrosC[MACROCOUNT][MACROLEN];

bool initMacros()
{
    // clear array
    for(int i=0; i<MACROCOUNT; ++i)
        for(int j=0; j<MACROLEN; ++j)
            macrosC[i][j]='0';

    // This should eventually be done in command mode or through external flashing
    // of appropriate eep file.
    for(int i=0; i<MACROCOUNT; ++i) {
        updateEEMacro(EEmacrosC[i],i);
    }

    uint8_t str[MACROLEN];
    for(int i=0; i<MACROCOUNT; ++i) {
        readEEMacro(str,i);
        uint8_t len = strlen((char*)str);
        // printf("\n  %d = len(%s)", len,  str);
        for(int j=0; j<len; ++j) {
            macrosC[i][j]=str[j];
        }
        //strncpy(&macrosC[i][0], str, strlen(str));
        macrosC[i][len]='\0';
    }

    return true;
}

void printMacros(void)
{
    uint8_t str[MACROLEN];
    for(int i=0; i<MACROCOUNT; ++i) {
        readEEMacro(str,i);
    }
}

bool macroMode(void)
{
    return(macromode != 0);
}

void setMacroMode( bool on )
{
    macromode=on;
}

void endMacro()
{
    macromode = 0;
    idx  = 0;
    curMacro=MACROCOUNT;
}

/**
 * set current active macro
 */
bool activateMacro(uint8_t id)
{
    if(!macromode)
        return false;
    // de-bounce not working in current macro detection
    if(id==curMacro)
        return true;
    if(id<MACROCOUNT) {
        idx=0;
        curMacro=id;
        return true;
    } else {
        setMacroMode(false);
        return false;
    }
}

bool getMacroReport(USB_KeyboardReport_Data_t *report)
{
    char c;
    if(!macromode)
        return false;
    if(curMacro>=MACROCOUNT)
        return false;

    sendEmpty = sendEmpty ? 0 : 1;
    if( sendEmpty) {
        c='\0';
        memset(&report->KeyCode[0], 0, 6);
        //sendEmpty = 0;
        return true;
    }

    if( idx < sizeof(macrosC[curMacro])/sizeof(char) ) {
        uint8_t mod = 0;

        c=macrosC[curMacro][idx];
        if( c == '\0' ) {
            memset(&report->KeyCode[0], 0, 6);
            endMacro();
            return false;
        }
        if(c == MACRO_PAUSE)
            _delay_ms(200);

        // if > 127, it is a modifier
        if(c & 0x80 ) {
            mod=(c & 0x7F);
            c=0;
            sendEmpty = sendEmpty ? 0 : 1;
            idx++;
            if( idx < sizeof(macrosC[curMacro])/sizeof(char) ) {
                c=macrosC[curMacro][idx];
            }
        }
        report->KeyCode[0] = ascii2hid[(uint8_t)c][0];
        report->Modifier = ascii2hid[(uint8_t)c][1];
        report->Modifier |= mod;
        memset(&report->KeyCode[1], 0, 5);

        idx++;
        return true;
    } else {
        endMacro();
        return false;
    }

    return false;
}

/**
 * Reads the macro at given index from eeprom into macro and returns its length.
 * Caller needs to make sure there in enough space allocated!
 */
uint8_t readEEMacro(uint8_t * macro, uint8_t idx)
{
    eeprom_busy_wait();
    uint8_t len=eeprom_read_byte (( const void *) EE_ADDR_MACRO(idx) );

    if(len>MACROLEN)
        len=MACROLEN;

    eeprom_busy_wait();
    eeprom_read_block (( void *) macro , ( const void *) (EE_ADDR_MACRO(idx)+1) , len);

    macro[len]='\0';
    //printf("\nEE readEEMacro #%d @%d len=%d \"%s\"", idx, EE_ADDR_MACRO(idx), len, macro );

    return len;
}


/**
 * Writes the macro to eeprom at given index and returns length of written string.
 * As eeprom update functions are used, no unnecessary writes are performed.
 */
uint8_t updateEEMacro(const char * macro, uint8_t idx)
{
    uint8_t len=strlen(macro);
    if(len>MACROLEN)
        len=MACROLEN;

    eeprom_busy_wait();
    eeprom_update_byte ((void *) EE_ADDR_MACRO(idx) , len );
    eeprom_busy_wait();
    eeprom_update_block (( const void *) macro , (void *) (EE_ADDR_MACRO(idx)+1) , len );

    return len;
}

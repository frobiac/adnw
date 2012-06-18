#include "macro.h"
#include "Keyboard.h"

#ifdef _private_macros.h_NOT_FOUND
/** 
 * _private_macros.h
 * Put private macro declaration in this header file and define LEN and COUNT:
 * - use ascii characters/strings, or the corresponding codes from ascii_table directly
 * - any modifier(s) for the following character should be ORed with 0x80
 */
#define MACROCOUNT   3
#define MACROLEN    20 

static const char EEmacrosC[MACROCOUNT][MACROLEN]  =  {
    "SampleString!",
    // CTRL+ALT+DEL
    { (L_CTL|L_ALT)+0x80, 127, '\0' },
    // ^c ^t ^v return for firefox
    { (L_CTL)+0x80, c , (L_CTL)+0x80, t, (L_CTL)+0x80, v, '\0' },
};
#endif

#include "_private_macros.h"


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

    // This has to be externally
    for(int i=0; i<MACROCOUNT; ++i){
        writeMacro(EEmacrosC[i],i);
    }

    uint8_t str[MACROLEN];
    for(int i=0; i<MACROCOUNT; ++i){
        readMacro(str,i);
        uint8_t len = strlen(str);
        // printf("\n  %d = len(%s)", len,  str);
        for(int j=0; j<len; ++j){
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
    for(int i=0; i<MACROCOUNT; ++i){
        readMacro(str,i);
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

        // if > 127, it is a modifier
        if(c & 0x80 ){
            mod=(c & 0x7F);
            c=0;
            sendEmpty = sendEmpty ? 0 : 1;
            idx++;
            if( idx < sizeof(macrosC[curMacro])/sizeof(char) ) {
                c=macrosC[curMacro][idx];
            }

        }

        memcpy_P(report, &ascii_table[(uint8_t)c], sizeof(USB_KeyboardReport_Data_t));
        report->Modifier |= mod;
        // set remaining 5 bytes to 0
        memset(&report->KeyCode[1], 0, 5);

        idx++;
        return true;
    } else {
        endMacro();
        return false;
    }

    return false;
}


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
#include <avr/pgmspace.h>
#include "keymap.h"
#include "config.h"
/**
 *  Mappings of row and columns of the switch matrix to the selected layout matrix.
 *  Currently, the two halves of the keyboard are stacked so that
 *  rows 0-3 correspond to the left half and
 *  rows 4-7 to the right hand side.
 *
 *  All indices are zero-based.
 */
static keycode getKeyStruct(uint8_t row, uint8_t col, uint8_t layer);

uint8_t getModifier(uint8_t row, uint8_t col, uint8_t layer)
{
    return getKeyStruct(row, col, layer).mods;
}

uint16_t getKeyCode(uint8_t row, uint8_t col, uint8_t layer)
{
    return getKeyStruct(row, col, layer).hid;
}

uint8_t getKeyChar(uint8_t row, uint8_t col, uint8_t layer)
{
    uint8_t ch = 'S'; // 'S'pecial
    InterimsKeycode ikc; // Wert aus Key-Matrix des jeweils ausgewŠhlten Layouts
    Layout current_layout = eeprom_read_byte (&alternateLayoutNr);

    // now map IntermediateKeycode to ASCII code.
    memcpy_P(&ikc,&KeyMatrix[current_layout][layer][row][col],sizeof(ikc));
    if (ikc == _ik_no) {
        ch = ' ';
    }
    // ikc in Tabelle ascii2hid[] suchen
    else {
        for (uint8_t i=33; i<127; i++) {
            if (ascii2hid[i] == ikc) {
                ch = i;
                break;
            }
        }
    }
    return ch;
}

EEMEM Layout   alternateLayoutNr = QWERTZ_DE;
EEMEM MacOrPC  altMacOrPC        = PC;
EEMEM GeoArea  alternateGeoArea  = DE;

keycode getKeyStruct(uint8_t row, uint8_t col, uint8_t layer)
{
    InterimsKeycode ikc; // Wert aus Key-Matrix des jeweils ausgewŠhlten Layouts
    keycode kc;          // tatsŠchlichen auszugebenden Keycode
    Layout current_layout = eeprom_read_byte (&alternateLayoutNr);
    // ermittle den Interims-Keycode aus dem aktuellen Layout
    memcpy_P(&ikc,&KeyMatrix[current_layout][layer][row][col],sizeof(ikc));
    kc = getKeyStruct_Ic(ikc);
    return kc;
}

keycode getKeyStruct_Ic(InterimsKeycode ikc) {
    local_keycode lkc;  // Keycode aus Localization-Matrix mit den Keycode-Werten aller geografischen Bereiche
    GeoArea current_GeoArea = eeprom_read_byte (&alternateGeoArea);
    MacOrPC current_MacOrPC = eeprom_read_byte (&altMacOrPC);
    keycode kc_error = {HID_E, L_SHF};   // Fehler-Keycode
    keycode kc_null = {0, 0};   // Fehler-Keycode
    keycode kc = kc_null;   // tatsŠchlichen auszugebenden Keycode

    // Mac SonderfŠlle lokalisiert abhandeln
    if (current_MacOrPC==Mac) {
        // kucken, ob aktueller Interimscode in der Liste steckt
        for (int i=0; i<MAC_DIFFS_COUNT; i++) {
            memcpy_P(&lkc, &localizedMacDifferences[i], sizeof(lkc));
            if (lkc.ikc == ikc) {
                kc = lkc.kc[current_GeoArea];
                break;
            }
        }
    }
    // wenn kc bei Mac nicht gesetzt wurde, ermittle den local_keycode Eintrag aus der Standard (PC)-Tabelle
    if (USAGE_ID(kc.hid) == 0 /*kc_null*/) {
        // ermittle den local_keycode Eintrag
        memcpy_P(&lkc, &localizationMatrix[ikc], sizeof(lkc));
        // ermittle keycode je nach aktuellem geografischen Bereich
        kc = lkc.kc[current_GeoArea];
        // Falls Fehlzugriff gib "E", wie 'E'rror aus
        if (lkc.ikc != ikc) {
            // verrutscht: Reihenfolge in den beiden Strukturen ist anzugleichen
            kc = kc_error;
            printf("\nlocalizationMatrix (code:%d) und Enum fŸr InterimsKeycode (code:%d) nicht synchron!",lkc.ikc,ikc);
        }
    }
    return kc;
}

void printLayout(uint8_t layer)
{
    uint8_t ch;
    printf("\n--- Layer:%u",layer);
    for(uint8_t row=1; row<ROWS-2; ++row) {
        printf("\n");
        for(uint8_t col=0; col<COLS; ++col) {
            if(col==6) printf("  ");
            ch =getKeyChar(row,col,layer);
            if(ch == 92) 
                printf("\\\\");
            else
                printf("%c", ch);
        }
    }
}



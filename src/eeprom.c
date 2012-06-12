#include "eeprom.h"


/* Byte */
uint8_t EEMEM  eeFooByte = 0x12;

/* Wort */
uint16_t EEMEM eeFooWord = 0x345;

/* float */
float eeFooFloat EEMEM;

/* Byte-Array */
uint8_t eeFooByteArray1[] EEMEM = { 0x11, 0x22, 0x33 };
uint8_t eeFooByteArray2[] EEMEM = { 0x44, 0x55, 0x66 };

/* 16-bit unsigned short feld */
uint16_t eeFooWordArray1[4] EEMEM;

static uint8_t EEMEM SFR = 0xFF;

void eeprom_example (void)
{
    eeprom_update_byte(&SFR, 128);
    uint8_t myByte;
 
    // myByte lesen (Wert = 123)
    myByte = eeprom_read_byte (&eeFooByte);
 
    // der Wert 99 wird im EEPROM an die Adresse der
    // Variablen eeFooByte geschrieben
    myByte = 99;
    eeprom_write_byte(&eeFooByte, myByte); // schreiben
 
    myByte = eeprom_read_byte (&eeFooByteArray1[1]); 
    // myByte hat nun den Wert 3
 
    // Beispiel zur "Sicherung" gegen leeres EEPROM nach "Chip Erase"
    // (z. B. wenn die .eep-Datei nach Programmierung einer neuen Version
    // des Programms nicht in den EEPROM uebertragen wurde und EESAVE
    // deaktiviert ist (unprogrammed/1)
    // 
    // Vorsicht: wenn EESAVE "programmed" ist, hilft diese Sicherung nicht
    // weiter, da die Speicheraddressen in einem neuen/erweiterten Programm
    // moeglicherweise verschoben wurden. An der Stelle &eeFooByte steht
    // dann u.U. der Wert einer anderen Variable aus einer "alten" Version.
 
    uint8_t fooByteDefault = 222;
    if ((myByte = eeprom_read_byte (&eeFooByte)) == EEPROM_DEF)
    {
        myByte = fooByteDefault;
    }
}

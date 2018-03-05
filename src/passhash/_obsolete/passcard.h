/*
    Key for HMAC should be long. If longer than  hash output it will be hashed first internally.
        Randomly generated during initial compile and defined in private file / homedir chmod 600
        Can be used to "encrypt" eeprom

    Msg will contain 27 char b64 encoded result after call.

    Resulting Tabula Recta is build by rows of
        hmac-sha1( msg|counter, key)
        preferably 13*26 characters

    Select a password by ABX, with A & B letters and X a number%rows
        different algos: read n chars at (A,B) in a direction
*/

#pragma once

#include <stdint.h>

typedef union {
    struct {
        uint8_t h:2; // 2=-1 3=free
        uint8_t v:2; //
        uint8_t h_skip:2;
        uint8_t v_skip:2;
    };
    uint8_t raw;
} read_pattern_t;

#define TAG "mytag"

#ifdef __AVR__
#else // if !defined(__AVR__)
    char g_pw[20];  // at most 20 chars
    char g_tag[27]; // contains only 26 chars after stripping last char (often '=')
    char g_b64[27]; // contains only 26 chars after stripping last char (often '=')
    void verifyPWs(char r, char c, uint8_t n, uint8_t dir_raw, char * expect);
    void runTests(void);
#endif


void genPWs(char r, char c, char * res, uint8_t n, uint8_t  dir);
void tabula_recta(void);
void hmac_line(uint8_t idx);


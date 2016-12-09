#include <avr/io.h>  // RAMEND
#include "mem-check.h"

// Mask to init SRAM and check against
#define MASK 0xaa

// From linker script
extern unsigned char __heap_start;

// !!! This doesn't work together with malloc et.al. (whose use is
// !!! discouraged on AVR, anyway). alloca, however, is no problem
// !!! because it allocates on stack.

uint16_t get_free_mem(void)
{
    return SP - (uint16_t) &__heap_start;
}

// Get minimum of free memory (in bytes) up to now.
unsigned short
get_mem_unused (void)
{
    unsigned short unused = 0;
    unsigned char *p = &__heap_start;

    do {
        if (*p++ != MASK)
            break;

        unused++;
    } while (p <= (unsigned char*) RAMEND);

    return unused;
}

// !!! Never call this function, it is part of .init-Code
static void __attribute__ ((naked, used, section(".init3")))
init_mem (void)
{
    // Use inline assembler so it works even with optimization turned off.
    // Apart from that, according to GCC docs, the only code that's safe
    // in a naked function is inline assembly.
    __asm volatile (
        "ldi r30, lo8 (__heap_start)"  "\n\t"
        "ldi r31, hi8 (__heap_start)"  "\n\t"
        "ldi r24, %0"                  "\n\t"
        "ldi r25, hi8 (%1)"            "\n"
        "0:"                           "\n\t"
        "st  Z+,  r24"                 "\n\t"
        "cpi r30, lo8 (%1)"            "\n\t"
        "cpc r31, r25"                 "\n\t"
        "brne 0b"
        :
        : "i" (MASK), "i" (RAMEND)
    );
}


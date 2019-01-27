/* Copyright 2012 Jun Wako <wakojun@gmail.com> */
/* Very basic print functions, intended to be used with usb_debug_only.c
 * http://www.pjrc.com/teensy/
 * Copyright (c) 2008 PJRC.COM, LLC
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#pragma once

#include <stdint.h>
#include <stdbool.h>
// #include "util.h"

/// @TODO printf hangs (after buffer overflow?)
#define printf err_use_xprintf_instead

#ifdef DEBUG_OUTPUT
#include "Descriptors.h"
#include "mem-check.h"

typedef struct {
    char data[DBG_EPSIZE];
} USB_DBGReport_Data_t;

uint8_t DBG__get_report(USB_DBGReport_Data_t* report);


#include <src/external/xprintf.h>
#define print(s)    xputs(PSTR(s))
#define println(s)  xputs(PSTR(s "\r\n"))

#ifdef __cplusplus
    extern "C"
#endif

void Console_Task(void);
void InitDebugPrint(void);


/* decimal */
#define print_dec(i)                xprintf("%u", i)
#define print_decs(i)               xprintf("%d", i)
/* hex */
#define print_hex4(i)               xprintf("%X", i)
#define print_hex8(i)               xprintf("%02X", i)
#define print_hex16(i)              xprintf("%04X", i)
#define print_hex32(i)              xprintf("%08lX", i)
/* binary */
#define print_bin4(i)               xprintf("%04b", i)
#define print_bin8(i)               xprintf("%08b", i)
#define print_bin16(i)              xprintf("%016b", i)
#define print_bin32(i)              xprintf("%032lb", i)
#define print_bin_reverse8(i)       xprintf("%08b", bitrev(i))
#define print_bin_reverse16(i)      xprintf("%016b", bitrev16(i))
#define print_bin_reverse32(i)      xprintf("%032lb", bitrev32(i))
/* print value utility */
#define print_val_dec(v)            xprintf(#v ": %u\n", v)
#define print_val_decs(v)           xprintf(#v ": %d\n", v)
#define print_val_hex8(v)           xprintf(#v ": %X\n", v)
#define print_val_hex16(v)          xprintf(#v ": %02X\n", v)
#define print_val_hex32(v)          xprintf(#v ": %04lX\n", v)
#define print_val_bin8(v)           xprintf(#v ": %08b\n", v)
#define print_val_bin16(v)          xprintf(#v ": %016b\n", v)
#define print_val_bin32(v)          xprintf(#v ": %032lb\n", v)
#define print_val_bin_reverse8(v)   xprintf(#v ": %08b\n", bitrev(v))
#define print_val_bin_reverse16(v)  xprintf(#v ": %016b\n", bitrev16(v))
#define print_val_bin_reverse32(v)  xprintf(#v ": %032lb\n", bitrev32(v))

#else   /* NO DEBUG_OUTPUT */

typedef struct {
    char data[0];
} USB_DBGReport_Data_t;

uint8_t DBG__get_report(USB_DBGReport_Data_t *report_data); //  { return 0; }

void Console_Task(void); // {};

#define xprintf(s,...)              ((void)0)
#define print(s)                    ((void)0)
#define println(s)                  ((void)0)
#define print_set_sendchar(func)    ((void)0)
#define print_dec(data)             ((void)0)
#define print_decs(data)            ((void)0)
#define print_hex4(data)            ((void)0)
#define print_hex8(data)            ((void)0)
#define print_hex16(data)           ((void)0)
#define print_hex32(data)           ((void)0)
#define print_bin4(data)            ((void)0)
#define print_bin8(data)            ((void)0)
#define print_bin16(data)           ((void)0)
#define print_bin32(data)           ((void)0)
#define print_bin_reverse8(data)    ((void)0)
#define print_bin_reverse16(data)   ((void)0)
#define print_bin_reverse32(data)   ((void)0)
#define print_val_dec(v)            ((void)0)
#define print_val_decs(v)           ((void)0)
#define print_val_hex8(v)           ((void)0)
#define print_val_hex16(v)          ((void)0)
#define print_val_hex32(v)          ((void)0)
#define print_val_bin8(v)           ((void)0)
#define print_val_bin16(v)          ((void)0)
#define print_val_bin32(v)          ((void)0)
#define print_val_bin_reverse8(v)   ((void)0)
#define print_val_bin_reverse16(v)  ((void)0)
#define print_val_bin_reverse32(v)  ((void)0)
#define init_printf(s,ss)           ((void)0)

#endif  /* NO_PRINT */



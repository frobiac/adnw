/*
                   The HumbleHacker Keyboard Project
                 Copyright © 2008-2010, David Whetstone
               david DOT whetstone AT humblehacker DOT com

  This file is a part of The HumbleHacker Keyboard Project.

  The HumbleHacker Keyboard Project is free software: you can redistribute
  it and/or modify it under the terms of the GNU General Public License as
  published by the Free Software Foundation, either version 3 of the
  License, or (at your option) any later version.

  The HumbleHacker Keyboard Project is distributed in the hope that it will
  be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General
  Public License for more details.

  You should have received a copy of the GNU General Public License along
  with The HumbleHacker Keyboard Project.  If not, see
  <http://www.gnu.org/licenses/>.

*/

#include "hhstdio.h"
#include "keymap.h"

static int hid_putc(char, FILE*);
static FILE buf_stdout = FDEV_SETUP_STREAM(hid_putc, NULL, _FDEV_SETUP_WRITE);

/// @todo .data usage: this adds 256 bytes and can probably be reduced
enum {OUTPUT_BUFSIZE = 256};
static char output_buffer[OUTPUT_BUFSIZE];
static uint8_t front;
static uint8_t count;

void
stdio_init(void)
{
    stdout = &buf_stdout;
    front  = 0;
    count  = 0;
}

void
stdio_fill_report(char ch, USB_KeyboardReport_Data_t *report)
{
    // test: memcpy_P(report, &ascii_table[(uint8_t)ch], sizeof(USB_KeyboardReport_Data_t));
    // report->KeyCode[0]=ascii2hid[(uint8_t)ch][0];
    // report->Modifier  =ascii2hid[(uint8_t)ch][1];
    InterimsKeycode ikc=ascii2hid[(uint8_t)ch];
    keycode kc = getKeyStruct_Ic(ikc);
    report->KeyCode[0]=USAGE_ID(kc.hid);
    report->Modifier  =kc.mods;
    // set remaining 5 bytes to 0
    memset(&report->KeyCode[1], 0, 5);
}

int
hid_putc(char c, FILE *stream)
{
    if (stdout_is_full())
        return -1;

    uint8_t index = (front + count) % OUTPUT_BUFSIZE;
    count++;
    output_buffer[index] = c;
    return 0;
}

bool
stdout_is_empty(void)
{
    return count == 0;
}

bool
stdout_is_full()
{
    return count == OUTPUT_BUFSIZE;
}

char
stdout_popchar(void)
{
    if (stdout_is_empty()) {
        return '\0';
    }

    char ch = output_buffer[front];
    front = (front + 1) % OUTPUT_BUFSIZE;
    if (ch == '\\') {
        switch ((ch = output_buffer[front])) {
            case 'n':
                ch = (char)10;
                break;
            case 't':
                ch = (char)9;
                break;
            case 'b':
                ch = (char)8;
                break;
            case '\\':
                break;
        }
        front = (front + 1) % OUTPUT_BUFSIZE;
    }
    count--;
    return ch;
}



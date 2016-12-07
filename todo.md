**UNSORTED TODO LIST**
==================

BUGS
----
* Compiled with TP support, firmware hangs on init if TP error / not connected
* Find leaner printf replacement

ISSUES
------
* Modifier mismatch: Press (and hold) "/" and additional character will be capitalized:
    may be explained as "/" implies shift on german layout
* REDTILT: Left thumb rests on center key (space), while right one on second from middle (Shift)
        Win/Ctrl/Spc not like other boards...

LUFA
----
* inquire about upstream horizontal scroll support

Documentation
-------------
* license check on older files (hhkb)
* code
* pinout for different controllers
* case files and pictures


Trackpoint
----------
* initialization not really thought out
* document most important registers ( see trackpoint.[ch] from kernel)
* configuration editable at runtime + eeprom store
* startup even if TP is disabled
* TP_ROTATE in hw via register

HW
--
* PS/2 pull-ups needed

Refactor
--------
* english / german host layout
* check licenses

Layout
------
* Alt on redtilt, Ctrl-Alt usage
* BU-TECK better?
* usage page for system keys
* copy/paste


Git
---
* hooks (beautify, sanitize)
* document workflow


Macro
-----
* dedicated macro key instead ob subcommands?

Commandmode
-----------
* cleanup layer switching between different layouts
* read data like tp settings
* hierarchical command structure

BUGS
----
Test sizes:
* avr-nm --size-sort --print-size -td *.elf
* avr-size --mcu=atmega32u4 --format=avr *.elf

- mousekey accel not working: normal, slow, fast

Coding style
------------
* astyle --style=linux --convert-tabs --lineend=linux --indent=spaces=4  -o -O -S src/\*.[ch]
* astyle -o -O -S src/\*.[ch]
* astyle --options=$(pwd)/.astyle.rc -R "src/*.h" "src/*.c"

Ideas
-----
- copy paste on mouse middle btn on Win possible?
- prntscreen...
- openmoko touchpad?
- analog input for 2-wire matrix-scan


Thumbkey double usage
---------------------
- outmost column only on hypernano
- make lower center keys mousebuttons for TP
                               _____         _____
                              |     |       | M3  |
                              |     |       |     |
  |- HN:only                  |     |       |     |                         |- HN only, uses pinky key
 _V___________________________|_____|       |_____|_________________________V____
|     |     | ALT |CTRL |     | WIN |       | M2  | M0  | M1  |AltGr|     |MOUSE |
|     |     |     |     |     |     |       |     |     |     |     |     |      |
|     |     |     |     |SPACE| TAB |       | ESC | <-| |ENTER|     |     |      |
 -----------------------------------         ------------------------------------

Unsorted
--------
Testcases/mode
Pinkydrop again ?!


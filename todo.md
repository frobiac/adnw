**UNSORTED TODO LIST**
==================

BUGS
----
* Compiled with TP support, firmware hangs on init if TP error / not connected
* Find leaner printf replacement
Test sizes:
* avr-nm --size-sort --print-size -td *.elf
* avr-size --mcu=atmega32u4 --format=avr *.elf

- mousekey accel not working: normal, slow, fast
* mousemode not exited on other keys fast enough with thumb keys
    also maybe only activate after threshold? clearance of TP could be ignored more easily..

* hangs on 10 keys pressed

FEATURES
--------
* password card easier?
* bootmode for BIOS
* encrypt secret, simple password card support
* basic LED signaling: command mode...
* EEPROM loader LUFA/$(DMBS_MODULE_PATH)/HID_EEPROM_Loader/

ISSUES
------
* Modifier mismatch: Press (and hold) "/" and additional character will be capitalized:
    may be explained as "/" implies shift on german layout
* REDTILT: Left thumb rests on center key (space), while right one on second from middle (Shift)
        Win/Ctrl/Spc not like other boards...
* Test both qwerty/z

CONFIG
----
* usage help for command mode?

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
* startup even if TP is disabled
* PS/2 -> 3.3V on Raspberry Pi:
    - http://www.deater.net/weave/vmwprod/hardware/pi-ps2/
      https://github.com/deater/vmw-meter/tree/master/pi-ps2/hardware
    - Diode https://sites.google.com/site/thepihacker/ps2pi
            https://sites.google.com/site/thepirow/home/ps2pi
    - Voltage divider: https://www.seanet.com/~karllunt/bareRasPi.html
* PtS for scrolling, button mask?

I2C
---
* mcp23018
    - https://github.com/jackhumbert/qmk_firmware/blob/master/keyboards/ergodox/
    - https://github.com/benblazak/ergodox-firmware/blob/master/src/keyboard/ergodox/controller/mcp23018.c
    - https://github.com/ecerulm/tmk_ergodox

* Pull-ups:
    - http://www.varesano.net/blog/fabio/how-disable-internal-arduino-atmega-pullups-sda-and-scl-i2c-bus

* Error checking if any part fails.


SPI
---
* http://maxembedded.com/2013/11/the-spi-of-the-avr/


Teensy 2.0
----------
* Doc of ergodox: PWM ...
    - https://github.com/benblazak/ergodox-firmware/blob/master/src/keyboard/ergodox/controller/teensy-2-0.md


Refactor
--------
* english / german host layout
* check licenses
* Code refactor: http://www.avrfreaks.net/forum/tut-modularizing-c-code-managing-large-projects?name=PNphpBB2&file=viewtopic&t=48535
    - global vars
    - static functions
    - volatile only for variables shared with interrupt context, variables in delay loops that would be optimized away
    * EXTERN global.h http://www.mikrocontroller.net/topic/72934



Layout
------
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
* Provide help and feedback
* General config variable handling: name, value, increment mapped to command lower/uppercase, eeprom, ..

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
`
                                 _____         _____
                                |     |       | M3  |
                                |     |       |     |
    |- HN:only                  |     |       |     |                         |- HN only, uses pinky key
   _V___________________________|_____|       |_____|_________________________V____
  |     |     | ALT |CTRL |     | WIN |       | M2  | M0  | M1  |AltGr|     |MOUSE |
  |     |     |     |     |     |     |       |     |     |     |     |     |      |
  |     |     |     |     |SPACE| TAB |       | ESC | <-| |ENTER|     |     |      |
   -----------------------------------         ------------------------------------
`

Unsorted
--------
Testcases/mode
suspend/wakeup:
    win: powercfg -devicequery wake_armed
    implement EVENT_USB_Device_Suspend() ?

Passhash
--------
naming of key secret
allowed length of entries
error checks on all input...


URLs
----
github.com/NicoHood/HID  Hidcodes
PaulStoffregen/cores/teensy/keylayouts.c
tmk issue #332
tmk wiki HID-Report-Descriptor Dump descriptor linux
eleccelerator.com tutorial about usb hid report descriptors


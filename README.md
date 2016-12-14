About
-----
This is a firmware for AVR microcontrollers with USB device support to simulate a keyboard and a mouse. It was initially created for a custom split keyboard design with 4x6 keys on each side but can be adapted easily for other hardware.

By default, it implements a more ergonomic layout without any further drivers on any host operating system. In its current state, the well evolved [AdNW layout][adnw] is implemented with modified layers 3&4 (special characters, navigational keys and numpad) from the [neo layout][neo]. In addition, an integrated IBM trackpoint is also supported, but any PS/2 compatible device can also be hooked-up to the controller.

![BlueCube](http://frobiac.github.com/adnw/img/bluecube/BlueCube-4-tilted.jpg)
![HyperNano](http://frobiac.github.com/adnw/img/hypernano/2-Final_top.JPG)

This is my daily driver since about 2 years now:
![RedTilt](http://frobiac.github.com/adnw/img/RT_both.jpg)


A presentation of my current keyboard prototype can be found [here][DT_hw], where you'll also find more detailed information on the hardware and the layout used as well as discussions related to these project.

Features
--------
- Full macro support with all configuration done on the keyboard itself, no external tools needed.
- 2nd-Use modifier keys acting as normal keys when toggled by themselves.
- PS/2 protocol for pointing devices
- Mouse device with 3 buttons and 2 scrollwheels
- IBM trackpoint configuration
- Supports multiple layouts in switchable layers
- Native support for different host keyboard language settings, qwertz and qwerty implemented
- HMAC-SHA1 based string generation for safer passwords

Credits
-------
USB device support depends on the [LUFA][lufa] framework by Dean Camera.

The driver was originally based on humblehackers [hhkb firmware][hhkb].

Implementation of mousekey layer, KEYMAP macro and other neat ideas borrowed from the very advanced [tmk_firmware][tmk].

PS/2 support is copied from [TMK sources][tmk].

A dedicated USB ID was generously supplied by Openmoko, Inc.

Password hash generation is implemented after the algorithm from the Android apps [Twik][twik_android] ([python implementation here][twik_python]) and [HashIt!][hashit_android], which in turn are based on the chrome extension Password Hasher Plus.

It uses hmac-sha1 and sha1 implementations from [avr-crypto-lib][avr_crypto_lib], licensed under GPL3.

xprintf implementation from http://elm-chan.org/fsw/strf/xprintf.html

I2C Master library by Peter Fleury : http://homepage.hispeed.ch/peterfleury/i2cmaster.zip
MCP23018 code borrowing from Ruben Laguna : http://rubenlaguna.com/blog/2014/01/16/mcp23018-i2c-communication-advice


License
-------
Unless otherwise mentioned all code is licensed under GPL3.
External code may use a different license which should be declared in the relevant files.

**If you create a firmware from this code you _must_ change the vendor and product id!**

Install
-------
You will need a recent version of the [LUFA][lufa] usb stack to compile this software.

    $ git submodule init
    $ git submodule update


To build, please adjust controller type and build switches in makefile.
The hardware wiring needs to be specified in matrix.h .
Further configuration can be achieved by editing src/config.h .

Valid ids can be retrieved from AVR demoes, or the keyboard examples of the LUFA lib.

Then simply call

    $ make

To build for a different hardware, either edit KB_HW in makefile or export via environment:

    $ make KB_HW=REDTILT


Command Mode Keys
-----------------
For a full list of commands refer to handleCommand() in [command.c](/src/command.c) :

- v Version info
- q Quit command mode where possible
- b Jump to bootloader
- t TrackPoint info output
- l Alternate layer toggle
- m Mouse toggle

The following commands need further input:
- x Execute macro
    * Press key to playback its macro

- r Record macro (see number/length limits in [macro.h](/src/macro.h)
    * Press key to record macro for
    * Type in macro slowly
    * <Ctrl>+<Esc> aborts, <Ctrl>+<Return> saves, <Alt>+<Return> insert pause

- c Config
    * @see SUB_CONFIG in src/command.c for details
    * Must "save" for persistent storage of changes in EEPROM
    * Default LED mode
    * Full TrackPoint configuration of axes orientations and speed/sensitivity/threshold can be performed.

- h PassHash
    * Enter tag [length [mode]] for passhash creation


Password Hash Support
---------------------
Support for passwords compatible with PassHash implementations, for example the android app Twik,
must be configured in a separate header:
Twik private key of a profile and master password can be set in src/_private_data.h,
which is created on first make call and ignored in git.


Current Layout
==============
Current default layout as implemented in [keymap.h](/src/keymap.h) :
( discussion at [deskthority][DT_layout] and as a [graphical layout][RT_AdNW] )

![RedTiltAdNW](http://frobiac.github.com/adnw/img/AdNW-RedTilt-clean-8ca683a4aac4f76e226c78d2b9f55bf8.png)

##### Thumb keys & modifiers
                            Macro    TP
                            -        -
    LShft LAlt LCtrl Space                  Mod1 Mod3 AltGr Fx
     -     Esc  Tab  Space                  BkSp Ret  -     -
                            LWin     Mod2
                            -        -

##### Layer 0 & Mod1 ( letters shifted)
    K  U  Q  .: J     P  C  L  M  F
    H  I  E  A  O     D  T  R  N  S
    X  Y  -_ ,; /     B  G  W  V  Z

##### M2 / special
    @  °  [  ]  #     !  <  >  =  &
    \  /  {  }  *     ?  (  )  +  :
       $  |  ~  `     ^  %  "  '

##### M3 / MoveNum
    PgUp BkSp  Up  Del  PgDo   ß 7 8 9 ä Ä
    Pos1 Left Down Righ End    . 4 5 6 ö Ö
    Esc  Tab  Ins  Ret  Undo   0 1 2 3 ü Ü

##### Mousekeys
    tbd, see keymap.h


[DT_hw]:     http://deskthority.net/workshop-f7/my-diy-keyboard-collection-or-how-i-became-a-kb-geek-t2534.html
[DT_layout]: http://deskthority.net/keyboards-f2/the-decent-keyboard-layout-discussion-thread-matrix-t2898-30.html
[adnw]:      http://www.adnw.de
[neo]:       http://www.neo-layout.org/
[tmk]:       http://github.com/tmk/tmk_keyboard
[ergodox]:   http://github.com/benblazak/ergodox-firmware
[hhkb]:      http://github.com/humblehacker/keyboard
[RT_AdNW]:   http://www.keyboard-layout-editor.com/#/layouts/aff6811ce65a019e45942ed73f99ddd6
[twik_python]: http://github.com/coxande/Twik
[twik_android]: https://github.com/gustavomondron/twik/blob/master/app/src/main/java/com/reddyetwo/hashmypass/app/hash
[hashit_android]: https://github.com/ginkel/hashit/blob/master/src/main/java/com/ginkel/hashit
[avr_crypto_lib]: https://github.com/cantora/avr-crypto-lib
[lufa]:           http://www.lufa-lib.org


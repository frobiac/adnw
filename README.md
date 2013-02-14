**WARNING**
=================
The code is by no means in a state I would normally want to present it, but merely a snapshot of my current work in progress.

Due to the numerous requests I nonetheless chose to open it to the public, in the hopes that someone may find valuable information or hints even though it is documented poorly and hackish at best - maybe this encourages me enough to clean it up a little, too. 


About
-----
This is a firmware for avr controllers like the teensy to simulate a keyboard and a mouse. It was created for a custom split keyboard design with 4x6 keys on each side in a matrix layout but could be adapted easily for other hardware. A presentation of my current keyboard prototype can be found [here][DT_hw].

It enables a different, more ergonomic layout without any further drivers on any host operating system. In its current state, the well evolved [AdNW layout][adnw] is implemented with layers 3&4 (special characters, navigational keys and numpad) from the [neo layout][neo]. In addition, an integrated IBM trackpoint is also supported, but any PS/2 compatible device can also be hooked-up to the controller.


Credits
-------
The driver was originally based on humblehackers [hhkb firmware][hhkb], but I found some things (while pretty neat and cleanly implemented) a little too difficult to adjust, so I revamped everything but the most basic USB code.

Implementation of mousekey layer, KEYMAP macro and other neat ideas borrowed from the very advanced [tmk_firmware][tmk].

Klaus Weber contributed greatly to an enhanced secondary use state machine and provided several other modifications, including alternate layouts as well as PC/Mac and host language differentiations.

PS/2 support is credited in large part to Akshay Srinivasan <akshaysrinivasan@nitk.ac.in>
 

Install
-------
You will need a recent version of the LUFA usb stack to compile this software.
If you want scrollwheel support for the mouse, the included [patch](/LUFA-scrollwheel.patch) needs to be applied.
From this root directory, execute

    $ git submodule init
    $ git submodule update


To build, please adjust controller type and build switches in makefile.
The hardware wiring needs to be specified in matrix.h .
Further configuration can be achieved by editing src/config.h .
If you change the code, you are at least required to change the vendor and product id! Valid ids can be retrieved from AVR demoes, or the keyboard examples of the LUFA lib.

Then simply call
    $ make

Current Layout
==============
Current default layout as implemented in [keymap.h](/src/keymap.h) : 
( discussion at [deskthority][DT_layout] )

##### Thumb keys & modifiers 
                            Macro    TP
                            -        -
    LShft LAlt LCtrl Space                  Mod1 Mod3 AltGr Fx
     -     Esc  Tab  Space                  BkSp Ret  -     -
                            LWin     Mod2
                            -        -

##### Layer 0 & Mod1 ( letters shifted)
    K  U  Q  .: J     V  G  C  L  F
    H  I  E  A  O     D  T  R  N  S
    X  Y  -  ,; /\    B  P  W  M  Z

##### M2 / special
    @  _  [  ]  #     !  <  >  =  &
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

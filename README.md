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
The driver was originally based on humblehackers hhkb firmware, but I found some things (while pretty neat and cleanly implemented) a little too difficult to adjust, so I revamped everything but the most basic USB code.

PS/2 support is credited in large part to Akshay Srinivasan <akshaysrinivasan@nitk.ac.in>


Install
-------
You will need a recent version of the LUFA usb stack to compile this software.
If you want scrollwheel support for the mouse, the included [patch](/frobiac/adnw/LUFA-scrollwheel.patch) needs to be applied.
From this root directory, execute

    $ git clone https://github.com/abcminiuser/lufa-lib.git ../lufa-lib
    $ ln -sf ../lufa-lib/trunk LUFA
    $ cd LUFA
    $ patch -Np1 -i ../LUFA-scrollwheel.patch


To build, please adjust controller type and build switches in makefile and call make from root directory.


Current Layout
==============
Current default layout as implemented in [keymap.h](/frobiac/adnw/src/keymap.h) : 
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
    X  Y  -  ,; /\\    B  P  W  M  Z

##### M2 / special
    @  _  [  ]  #     !  <  >  =  &
    \\  /  {  }  *     ?  (  )  +  :
       $  |  ~  `     ^  %  "  '

##### M3 / MoveNum
    PgUp BkSp  Up  Del  PgDo   ß 7 8 9 ä Ä
    Pos1 Left Down Righ End    . 4 5 6 ö Ö
    Esc  Tab  Ins  Ret  Undo   0 1 2 3 ü Ü


[DT_hw]:     http://deskthority.net/workshop-f7/my-diy-keyboard-collection-or-how-i-became-a-kb-geek-t2534.html 
[DT_layout]: http://deskthority.net/keyboards-f2/the-decent-keyboard-layout-discussion-thread-matrix-t2898-30.html
[adnw]:      http://www.adnw.de
[neo]:       http://www.neo-layout.org/

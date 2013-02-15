#!/bin/bash

MCU=$( grep "^MCU\s*=" makefile | sed "s/.*=\s*//")
HEX=$( grep "^TARGET\s*=" makefile | sed "s/.*=\s*//")
#MCU=atmega32u4
#MCU=at90usb1286

mv $HEX.hex $HEX.hex.old 2>/dev/null

rm log log.err 2>/dev/null

if [ ! -z $1 ]; then 
	echo "*** make clean" 
	make clean >> log
fi
echo "*** make " &&
make >> log 2>log.err &&

if [ -f $HEX.hex ]; then 
  echo "*** Now flashing $MCU with $HEX.hex ..." &
  teensy_loader_cli -mmcu=$MCU -w -v $HEX.hex &&
  sudo -S  /usr/sbin/hid_listen
else
  echo "$HEX not found, cannot flash."
fi

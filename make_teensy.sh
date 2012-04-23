#!/bin/bash

MCU=atmega32u4
#MCU=at90usb1286

if [ ! -z $1 ]; then 
	echo "*** make clean" 
	make clean > log 
fi
echo "*** make " &&
make >> log &&
echo "*** Now flashing $MCU..." &&
#teensy_loader_cli -mmcu=at90usb1286 -w -v  build/firmware.hex &&
teensy_loader_cli -mmcu=$MCU -w -v  firmware.hex &&

sudo -S  /usr/sbin/hid_listen

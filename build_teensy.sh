#!/bin/bash

MCU=$( grep "^MCU *=" makefile | sed "s/.*= *//")
#MCU=atmega32u4
#MCU=at90usb1286

if [ ! -z $1 ]; then 
	echo "*** make clean" 
	make clean > log 
fi
echo "*** make " &&
make >> log &&
echo "*** Now flashing $MCU..." &&

teensy_loader_cli -mmcu=$MCU -w -v adnw.hex &&

sudo -S  /usr/sbin/hid_listen

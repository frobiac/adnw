#!/bin/bash
#
# Build xor test app from AVR code
#

base=$(git rev-parse --show-toplevel)
cd ${base}/


# same as in makefile
FW_VERSION=$(git describe --tags --always --long --dirty="-D")-$(git log --pretty=format:%cd --date=short -n1)

# using -O0 fails, need to check
FLAGS="-g -O -W -std=c99 -DFW_VERSION=\"${FW_VERSION}\""
CC=gcc

XORINIT=$(tr -dc 'a-f0-9' </dev/urandom | head -c 8 )
FLAGS="$FLAGS -DXOR_RND_INIT=0x${XORINIT}"

mkdir .build 2>/dev/null

cd .build &&
$CC $FLAGS -c ../src/helpers.c &&
$CC $FLAGS -c ../src/xor.c  &&
$CC $FLAGS -c ../src/b64.c  &&
$CC $FLAGS helpers.o xor.o b64.o -o xor &&
cd .. && 
echo ".build/xor ready."


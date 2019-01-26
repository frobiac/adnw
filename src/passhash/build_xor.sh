#!/bin/bash
#
# call with extra arg to test ph verification
#   '-DPH_EXPECTED_4_DIGITS_FOR_A=4616'
#

rm ./xor

# same as in makefile
FW_VERSION=$(git describe --tags --always --long --dirty="-D")-$(git log --pretty=format:%cd --date=short -n1)

# using -O0 fails, need to check
FLAGS="-g -O -W -std=c99 -DFW_VERSION=\"${FW_VERSION}\""
CC=gcc

XORINIT=$(tr -dc 'a-f0-9' </dev/urandom | head -c 8 )
FLAGS="$FLAGS -DXOR_RND_INIT=0x${XORINIT}"

mkdir .build 2>/dev/null
(
  rm xor
  cd .build

  $CC $FLAGS -c ../../helpers.c &&
  $CC $FLAGS -c ../xor.c  &&
  $CC $FLAGS -c ../b64.c  &&
  $CC $FLAGS helpers.o xor.o b64.o -o ../xor
)

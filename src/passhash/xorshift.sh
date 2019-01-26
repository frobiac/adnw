#!/bin/bash
set -o nounset

UNLOCK=${1}
XORSHIFT=1   # random on firmware compile
TR_COLS=26

xorshift32()
{
    XORSHIFT=$(( $XORSHIFT ^ $(( $XORSHIFT << 13 )) ))
    XORSHIFT=$(( $XORSHIFT & 0xFFFFFFFF ))
    XORSHIFT=$(( $XORSHIFT ^ $(( $XORSHIFT >> 17 )) ))
    XORSHIFT=$(( $XORSHIFT & 0xFFFFFFFF ))
    XORSHIFT=$(( $XORSHIFT ^ $(( $XORSHIFT <<  5 )) ))
    XORSHIFT=$(( $XORSHIFT & 0xFFFFFFFF ))
}

xor_init()
{
    c=0
    XORSHIFT=0

    while read -n1 c; do
        XORSHIFT=$(( $XORSHIFT + $(printf "%d" \"$c) ))
        xorshift32
    done < <(echo -n "$UNLOCK")

    SEED=$XORSHIFT
    printf "seed(%s) = %08X\n" $1 $SEED
}

b64()
{
    b64map="ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789-."
    echo -n "${b64map:${1}:1}"
}


# param len row col
tr_code()
{
    len=$1; row=$2; col=$3;
    for (( i=0; i< $row * TR_COLS + $col; i++ )); do
        xorshift32
    done
    for i in $(seq 1 $len); do
        xorshift32
        b64 $((XORSHIFT & 0x3F))
    done
    echo
}

##############
# MAIN
##############

xor_init $UNLOCK

for row in $(seq 0 12); do
    code=$(tr_code 26 $row 0)
    printf "\n%2d: %s" $row $code
done

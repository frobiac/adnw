#!/bin/bash

# PassHash:
# - Master Password + Secret

# Crypted content:
# - eeprom macros: Problem: Not random -> Invalid chars leak info about
# - PassHash init

# INIT: read unlock password -> hash for 20Byte unlock string
# XOR all

# Twik:
# genHash privkey tag 24 1 -> mhash with hmac_sha1(key=tag, value=privkey)
# genHash mhash master_key tag 24 1 with hmac_sha1(key=master_key, value=mhash)

# echo -n 'value' | openssl dgst -sha1 -mac HMAC -macopt key:key -binary | xxd -p |wc -c

# $1 = val
# $2 = key
hmac_sha1()
{
    val=$1
    key=$2
    hmac=$(echo -n "${val}" | openssl dgst -sha1 -hmac "${key}" | sed 's/^.*= //')
    echo "# key=$2 val=$1 -> $hmac $(echo $hmac|base64)" >&2
    echo $hmac
}

hmac_sha1_binary()
{
    echo "# key=$2 val=$1 -> $hmac" >&2
    echo -n "$1" | xxd -r -p | openssl dgst -sha1 -mac HMAC -macopt key:$2 | sed 's/^.*= //'
}
hmac_sha1_hex()
{
    echo -n "$1" | openssl dgst -sha1 -mac HMAC -macopt hexkey:$2 | sed 's/^.*= //'
}


# twik requires special characters and numbers to be present in first hmac, and
# also in final result depending on output type requested.
twik_check_special()
{
    NUM=$(echo "$1" |tr -d -c "[0-9]" |wc -c)
    SPE=$(echo "$1" |tr -d -c "[/+]" |wc -c)
    echo $NUM/$SPE number/special in $1
}

twik()
{
    # privkey from config
    # mpw from dialog
    # tag from dialog
    tag=$1
    mpw=$2
    key=$3

    # round 1: key=tag, message=privkey
    hmac1=$(echo -n "$key" | openssl dgst -sha1 -hmac "${tag}" --binary | base64 | head -c 24)
    # round 2: key=master pw, message=hmac1
    hmac2=$(echo -n "${hmac1}" | openssl dgst -sha1 -hmac "${mpw}" --binary | base64 | head -c 26)

    echo $hmac1
    echo $hmac2

    twik_check_special $hmac1
    twik_check_special $hmac2
}


twik $ {1:-tag23} key privkey

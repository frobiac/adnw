#!/bin/bash

# tag_base read from header (or provide 40 char hex string)
# tag = tag_base, first byte replaced by counter = row character in hex
# unlock read from user -> sha1sum(unlock)
# hmacsha1(tag, sha1sum(unlock))

HEADER="src/_private_data.h"

if [ -f ${HEADER} ]; then
    echo "Found header ${HEADER}"
    BASE=$(grep -m 1 "HMAC_TAG_BASE\s*= " ${HEADER} | sed 's/.*\"\(.*\)".*/\1/' | tr -d 'x\\')
    test ${#BASE} -eq 40 || { echo "didn't find HMAC_TAG_BASE[20] definition: Expected 40 hexchars, got ${#BASE}!"; exit 1; }
    [[ ${BASE} =~ ^[a-fA-F0-9]*$ ]] || { echo "Found HMAC_TAG_BASE[20], but contains non-hex-characters."; exit 2; }
else
    echo "Header not found, enter tag manually:"
    read -p "base_tag(txt): " TAG
    BASE=$(echo -n "$TAG" | openssl dgst -sha1 | sed 's/^.*= //')
fi

# use -s for silent operation
read -n 27 -p "unlock(txt): " UNLOCK
key=$(echo -n "$UNLOCK" | openssl dgst -sha1 | sed 's/^.*= //')
UNLOCK=""

while true; do
    read -p "shortcode: " CODE

    # parse shortcode: row=[a..z], col=[0-25], len=[0-9]
    r=${CODE:0:1}
    c=${CODE:1:1}
    l=${CODE:2:1}

    row=$(printf "%d" "'${r}") # row character in decimal
    row=$((($row-97)%13))      # ... subtract 'a', wrap at 'm'=12
    row=$(printf "%02x" $row)  # ... to hex
    col=$(printf "%d" "'${c}") # col character in decimal
    col=$((($col-97)%26))      # subtract 'a'
    len=8
    [ $l -gt 0 -a $len -le 9 ] && len=$l

    # tag is sha1sum(unlock) in hex, with first Byte replaced by row character in Hex
    tag=${row}${HMAC_TAG_BASE:2:38}

    # echo key=$key
    # echo tag="${tag:0:40}|${tag:40:14}"
    # HMAC_HEX=$(echo -n "${tag:0:40}" | xxd -r -p | openssl dgst -sha1 -mac HMAC -macopt hexkey:$key | sed 's/^.*= //')

    HMAC_B64=$(echo -n "${tag:0:40}" | xxd -r -p | openssl dgst -sha1 -mac HMAC -macopt hexkey:$key -binary| base64 | head -c 26)

    echo "HMAC($r=$row  ) = ${HMAC_B64}"
    # wrap around
    HMAC_B64=${HMAC_B64}${HMAC_B64}
    echo " res($c=$col,$len) = ${HMAC_B64:${col}:${len}}"



done


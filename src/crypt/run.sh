#!/bin/bash

gcc -Wno-write-strings xxtea.c -o xxtea || { echo "ERROR building."; exit 1; }

./xxtea key_012301230123 "999999999"
./xxtea

exit


./xxtea key_012301230123 "1"
./xxtea key_012301230123 "22"
./xxtea key_012301230123 "333"
./xxtea key_012301230123 "4444"
./xxtea key_012301230123 "55555"
./xxtea key_012301230123 "666666"
./xxtea key_012301230123 "7777777"
./xxtea key_012301230123 "88888888"
./xxtea key_012301230123 "999999999"
./xxtea key_012301230123 "abcdefgh012345670123"
./xxtea key_012301230123 "abcdefgh012345670124"
./xxtea key_012301230123 "abcdefgh012345670123456"

./xxtea
exit

echo "TR with cli args:"
for i in $(seq -w 0 30); do
    ./xxtea key1123412341235 testtesttesttesttesttest__${i} 2>/dev/null | xxd -r -p |base64
done

echo "Test mode"
./xxtea

./xxtea "Fine. And you?  " "How do you do?"

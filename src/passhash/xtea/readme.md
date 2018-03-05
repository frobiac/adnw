http://www.efton.sk/crypt/

TEA
Data block and key length are 64 bits.
TEA for '51 - encryption/decryption
XTEA for '51 - encryption/decryption
TEA for AVR - encryption
XTEA for AVR - encryption/decryption
XTEA for SDCC
The XTEA cipher only slightly modified as an SDCC function, together with an appropriate header and an example test "application".

Both encryption and decryption is implemented; if only one of these is needed, comment the corresponding #define in xtea.h. The original implementation in C provided for reference.

Usage:

include xtea.h into your application
define a secret key as an "initialized" array of 16 bytes (or 4 longints, or any similar scheme) in code memory
fill in 8 bytes of plaintext (ciphertext, if decription is to be used rather than encryption) into the global xtea_data.b array (alternatively, into the xtea_data.l.x and xtea_data.l.y long int variables)
call xtea(&key) (xtea_i(&key) for decryption)
ciphertext (plaintext in case of decryption) is now in the xtea_data.b array
Usage as pseudorandom number generator (pRNG):
it is enough to have only encryption selected using the #defines in xtea.h
define a secret key as above
before first usage, fill in the xtea_data.b array with "random" seed (e.g. based on timer, keyboard input, or similar "random" input), or store/restore the current value of seed in nonvolatile memory
to generate one pseudorandom number, call xtea(&key)
use (part of) the ciphertext as the pseudorandom number (observe proper "handling" (e.g. scaling) of pseudorandom numbers)
re-seed xtea_data.b xoring with "real randomness" regularly.



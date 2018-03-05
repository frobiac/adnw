// test for xtea for sdcc

#include <stdint.h>

#include "xtea.h"

uint32_t code xtea_k[4] = {0x00010203, 0x04050607, 0x08090a0b, 0x0c0d0e0f};

uint32_t code test_vector_key1[4] = {0x00000000, 0x00000000, 0x00000000, 0x00000000};
uint32_t code test_vector_key2[4] = {0x00000001, 0x00000002, 0x00000003, 0x00000004};
uint32_t code test_vector_key3[4] = {0x00000003, 0x00000004, 0x00000005, 0x00000006};
uint32_t code test_vector_key4[4] = {0x78695a4b, 0x3c2d1e0f, 0xf0e1d2c3, 0xb4a59687};



void main(void)
{

    xtea_data.l.y = 0x41424344;
    xtea_data.l.z = 0x45464748;

    xtea(xtea_k);

    if ((xtea_data.l.y != 0x497df3d0) || (xtea_data.l.z != 0x72612cb5)) goto bad;

    xtea_i(xtea_k);

    if ((xtea_data.l.y != 0x41424344) || (xtea_data.l.z != 0x45464748)) goto bad;

// now the test vector we have from the net:
    /*
    TEA-N code

    Iteration:  32
    Plaintext:  0x00000000 0x00000000
    Key:        0x00000000 0x00000000 0x00000000 0x00000000
    Ciphertext: 0xdee9d4d8 0xf7131ed9
    ----------------------
    Iteration:  32
    Plaintext:  0x00000000 0x00000000
    Key:        0x00000001 0x00000002 0x00000003 0x00000004
    Ciphertext: 0xa597ab41 0x76014d72
    ----------------------
    Iteration:  32
    Plaintext:  0x00000001 0x00000002
    Key:        0x00000003 0x00000004 0x00000005 0x00000006
    Ciphertext: 0xb1fd5da9 0xcc6dc9dc
    ----------------------
    Iteration:  32
    Plaintext:  0xf0e1d2c3 0xb4a59687
    Key:        0x78695a4b 0x3c2d1e0f 0xf0e1d2c3 0xb4a59687
    Ciphertext: 0x704b3134 0x4744dfab
    */

    xtea_data.l.y = 0x00000000;
    xtea_data.l.z = 0x00000000;

    xtea(test_vector_key1);

    if ((xtea_data.l.y != 0xdee9d4d8) || (xtea_data.l.z != 0xf7131ed9)) goto bad;

    xtea_i(test_vector_key1);

    if ((xtea_data.l.y != 0x00000000) || (xtea_data.l.z != 0x00000000)) goto bad;



    xtea_data.l.y = 0x00000000;
    xtea_data.l.z = 0x00000000;

    xtea(test_vector_key2);

    if ((xtea_data.l.y != 0xa597ab41) || (xtea_data.l.z != 0x76014d72)) goto bad;

    xtea_i(test_vector_key2);

    if ((xtea_data.l.y != 0x00000000) || (xtea_data.l.z != 0x00000000)) goto bad;



    xtea_data.l.y = 0x00000001;
    xtea_data.l.z = 0x00000002;

    xtea(test_vector_key3);

    if ((xtea_data.l.y != 0xb1fd5da9) || (xtea_data.l.z != 0xcc6dc9dc)) goto bad;

    xtea_i(test_vector_key3);

    if ((xtea_data.l.y != 0x00000001) || (xtea_data.l.z != 0x00000002)) goto bad;



    xtea_data.l.y = 0xf0e1d2c3;
    xtea_data.l.z = 0xb4a59687;

    xtea(test_vector_key4);

    if ((xtea_data.l.y != 0x704b3134) || (xtea_data.l.z != 0x4744dfab)) goto bad;

    xtea_i(test_vector_key4);

    if ((xtea_data.l.y != 0xf0e1d2c3) || (xtea_data.l.z != 0xb4a59687)) goto bad;

good: return;

bad: while(1);



}

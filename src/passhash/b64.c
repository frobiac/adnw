/*
    Base64 encoding based on code from base64.sf.net/b64.c

    Copyright (c) 2001 Bob Trower, Trantor Standard Systems Inc.
    MIT license

MODULE NAME:    b64.c

AUTHOR:         Bob Trower 2001/08/04

PROJECT:        Crypt Data Packaging

COPYRIGHT:      Copyright (c) Trantor Standard Systems Inc., 2001

NOTES:          This source code may be used as you wish, subject to
                the MIT license.  See the LICENCE section below.

                Canonical source should be at:
                    http://base64.sourceforge.net

LICENCE:        Copyright (c) 2001 Bob Trower, Trantor Standard Systems Inc.

            Permission is hereby granted, free of charge, to any person
            obtaining a copy of this software and associated
            documentation files (the "Software"), to deal in the
            Software without restriction, including without limitation
            the rights to use, copy, modify, merge, publish, distribute,
            sublicense, and/or sell copies of the Software, and to
            permit persons to whom the Software is furnished to do so,
            subject to the following conditions:

            The above copyright notice and this permission notice shall
            be included in all copies or substantial portions of the
            Software.

            THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY
            KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
            WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR
            PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS
            OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR
            OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR
            OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
            SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/


#include "b64.h"
/**
 * Evaluated options with bitfields, but no size-advantage.
 * Maybe just drop b64-encoding altogether, or at least simplify?
 * - only 1 to 1 mapping
 */

char b64( uint8_t i )
{
    i &= 0x3f; // only lowest 6 Bits are mapped
    if(i<26)
        return 'A'+ i;
    if(i<52)
        return 'a'+ i-26;
    if(i<62)
        return '0'+ i-52;
    if(i==62)
        return '+';

    //if(i==63)
    return '/';
}

/// encode 3 8-bit binary bytes as 4 '6-bit' characters
void encodeblock( unsigned char *in, unsigned char *out, int len )
{
    out[0] = (unsigned char) b64( (uint8_t)(in[0] >> 2) );
    out[1] = (unsigned char) b64( (uint8_t)(((in[0] & 0x03) << 4) | ((in[1] & 0xf0) >> 4)) );
    out[2] = (unsigned char) (len > 1 ? b64( (uint8_t)(((in[1] & 0x0f) << 2) | (((len>2?in[2]:0) & 0xc0) >> 6)) ) : '=');
    out[3] = (unsigned char) (len > 2 ? b64( (uint8_t)(in[2] & 0x3f) ) : '=');
}

//
/** Encode data of size len to base64 result
 *  Result must be at least len*4.0/3 (rounded up!)
 *
 *  A SHA1 hash of 20 Bytes will thus occupy 27 Bytes (20*4/3=26.666)
 */
void b64enc(const unsigned char* data, size_t len, char* result, size_t resultSize)
{
    for(size_t x=0; x*3<len; ++x)
        encodeblock( (unsigned char *)(&data[x*3]), (unsigned char *)(&result[x*4]), (len-x*3));
}


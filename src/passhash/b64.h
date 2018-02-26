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


#pragma once

#include <stddef.h>
#include <stdint.h>

void b64enc(const unsigned char* data, size_t len, char* result, size_t resultSize);


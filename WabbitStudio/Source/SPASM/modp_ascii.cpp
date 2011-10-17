/* -*- mode: c; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set expandtab shiftwidth=4 tabstop=4: */

/**
 * \file modp_ascii.c
 * <PRE>
 * MODP_ASCII - Ascii transformations (upper/lower, etc)
 * http://code.google.com/p/stringencoders/
 *
 * Copyright &copy; 2007  Nick Galbreath -- nickg [at] modp [dot] com
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *
 *   Redistributions of source code must retain the above copyright
 *   notice, this list of conditions and the following disclaimer.
 *
 *   Redistributions in binary form must reproduce the above copyright
 *   notice, this list of conditions and the following disclaimer in the
 *   documentation and/or other materials provided with the distribution.
 *
 *   Neither the name of the modp.com nor the names of its
 *   contributors may be used to endorse or promote products derived from
 *   this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * This is the standard "new" BSD license:
 * http://www.opensource.org/licenses/bsd-license.php
 * </PRE>
 */
#include "stdafx.h"
typedef char uint8_t;
typedef int uint32_t;
#include "modp_ascii.h"

static const unsigned char gsToUpperMap[256] = {
'\0', 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, '\t',
'\n', 0x0b, 0x0c, '\r', 0x0e, 0x0f, 0x10, 0x11, 0x12, 0x13,
0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d,
0x1e, 0x1f,  ' ',  '!',  '"',  '#',  '$',  '%',  '&', '\'',
 '(',  ')',  '*',  '+',  ',',  '-',  '.',  '/',  '0',  '1',
 '2',  '3',  '4',  '5',  '6',  '7',  '8',  '9',  ':',  ';',
 '<',  '=',  '>',  '?',  '@',  'A',  'B',  'C',  'D',  'E',
 'F',  'G',  'H',  'I',  'J',  'K',  'L',  'M',  'N',  'O',
 'P',  'Q',  'R',  'S',  'T',  'U',  'V',  'W',  'X',  'Y',
 'Z',  '[', '\\',  ']',  '^',  '_',  '`',  'A',  'B',  'C',
 'D',  'E',  'F',  'G',  'H',  'I',  'J',  'K',  'L',  'M',
 'N',  'O',  'P',  'Q',  'R',  'S',  'T',  'U',  'V',  'W',
 'X',  'Y',  'Z',  '{',  '|',  '}',  '~', 0x7f, 0x80, 0x81,
0x82, 0x83, 0x84, 0x85, 0x86, 0x87, 0x88, 0x89, 0x8a, 0x8b,
0x8c, 0x8d, 0x8e, 0x8f, 0x90, 0x91, 0x92, 0x93, 0x94, 0x95,
0x96, 0x97, 0x98, 0x99, 0x9a, 0x9b, 0x9c, 0x9d, 0x9e, 0x9f,
0xa0, 0xa1, 0xa2, 0xa3, 0xa4, 0xa5, 0xa6, 0xa7, 0xa8, 0xa9,
0xaa, 0xab, 0xac, 0xad, 0xae, 0xaf, 0xb0, 0xb1, 0xb2, 0xb3,
0xb4, 0xb5, 0xb6, 0xb7, 0xb8, 0xb9, 0xba, 0xbb, 0xbc, 0xbd,
0xbe, 0xbf, 0xc0, 0xc1, 0xc2, 0xc3, 0xc4, 0xc5, 0xc6, 0xc7,
0xc8, 0xc9, 0xca, 0xcb, 0xcc, 0xcd, 0xce, 0xcf, 0xd0, 0xd1,
0xd2, 0xd3, 0xd4, 0xd5, 0xd6, 0xd7, 0xd8, 0xd9, 0xda, 0xdb,
0xdc, 0xdd, 0xde, 0xdf, 0xe0, 0xe1, 0xe2, 0xe3, 0xe4, 0xe5,
0xe6, 0xe7, 0xe8, 0xe9, 0xea, 0xeb, 0xec, 0xed, 0xee, 0xef,
0xf0, 0xf1, 0xf2, 0xf3, 0xf4, 0xf5, 0xf6, 0xf7, 0xf8, 0xf9,
0xfa, 0xfb, 0xfc, 0xfd, 0xfe, 0xff
};

static const unsigned char gsToLowerMap[256] = {
'\0', 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, '\t',
'\n', 0x0b, 0x0c, '\r', 0x0e, 0x0f, 0x10, 0x11, 0x12, 0x13,
0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d,
0x1e, 0x1f,  ' ',  '!',  '"',  '#',  '$',  '%',  '&', '\'',
 '(',  ')',  '*',  '+',  ',',  '-',  '.',  '/',  '0',  '1',
 '2',  '3',  '4',  '5',  '6',  '7',  '8',  '9',  ':',  ';',
 '<',  '=',  '>',  '?',  '@',  'a',  'b',  'c',  'd',  'e',
 'f',  'g',  'h',  'i',  'j',  'k',  'l',  'm',  'n',  'o',
 'p',  'q',  'r',  's',  't',  'u',  'v',  'w',  'x',  'y',
 'z',  '[', '\\',  ']',  '^',  '_',  '`',  'a',  'b',  'c',
 'd',  'e',  'f',  'g',  'h',  'i',  'j',  'k',  'l',  'm',
 'n',  'o',  'p',  'q',  'r',  's',  't',  'u',  'v',  'w',
 'x',  'y',  'z',  '{',  '|',  '}',  '~', 0x7f, 0x80, 0x81,
0x82, 0x83, 0x84, 0x85, 0x86, 0x87, 0x88, 0x89, 0x8a, 0x8b,
0x8c, 0x8d, 0x8e, 0x8f, 0x90, 0x91, 0x92, 0x93, 0x94, 0x95,
0x96, 0x97, 0x98, 0x99, 0x9a, 0x9b, 0x9c, 0x9d, 0x9e, 0x9f,
0xa0, 0xa1, 0xa2, 0xa3, 0xa4, 0xa5, 0xa6, 0xa7, 0xa8, 0xa9,
0xaa, 0xab, 0xac, 0xad, 0xae, 0xaf, 0xb0, 0xb1, 0xb2, 0xb3,
0xb4, 0xb5, 0xb6, 0xb7, 0xb8, 0xb9, 0xba, 0xbb, 0xbc, 0xbd,
0xbe, 0xbf, 0xc0, 0xc1, 0xc2, 0xc3, 0xc4, 0xc5, 0xc6, 0xc7,
0xc8, 0xc9, 0xca, 0xcb, 0xcc, 0xcd, 0xce, 0xcf, 0xd0, 0xd1,
0xd2, 0xd3, 0xd4, 0xd5, 0xd6, 0xd7, 0xd8, 0xd9, 0xda, 0xdb,
0xdc, 0xdd, 0xde, 0xdf, 0xe0, 0xe1, 0xe2, 0xe3, 0xe4, 0xe5,
0xe6, 0xe7, 0xe8, 0xe9, 0xea, 0xeb, 0xec, 0xed, 0xee, 0xef,
0xf0, 0xf1, 0xf2, 0xf3, 0xf4, 0xf5, 0xf6, 0xf7, 0xf8, 0xf9,
0xfa, 0xfb, 0xfc, 0xfd, 0xfe, 0xff
};

static const unsigned char gsToPrintMap[256] = {
 '?',  '?',  '?',  '?',  '?',  '?',  '?',  '?',  '?',  '?',
 '?',  '?',  '?',  '?',  '?',  '?',  '?',  '?',  '?',  '?',
 '?',  '?',  '?',  '?',  '?',  '?',  '?',  '?',  '?',  '?',
 '?',  '?',  ' ',  '!',  '"',  '#',  '$',  '%',  '&', '\'',
 '(',  ')',  '*',  '+',  ',',  '-',  '.',  '/',  '0',  '1',
 '2',  '3',  '4',  '5',  '6',  '7',  '8',  '9',  ':',  ';',
 '<',  '=',  '>',  '?',  '@',  'A',  'B',  'C',  'D',  'E',
 'F',  'G',  'H',  'I',  'J',  'K',  'L',  'M',  'N',  'O',
 'P',  'Q',  'R',  'S',  'T',  'U',  'V',  'W',  'X',  'Y',
 'Z',  '[', '\\',  ']',  '^',  '_',  '`',  'a',  'b',  'c',
 'd',  'e',  'f',  'g',  'h',  'i',  'j',  'k',  'l',  'm',
 'n',  'o',  'p',  'q',  'r',  's',  't',  'u',  'v',  'w',
 'x',  'y',  'z',  '{',  '|',  '}',  '~',  '?',  '?',  '?',
 '?',  '?',  '?',  '?',  '?',  '?',  '?',  '?',  '?',  '?',
 '?',  '?',  '?',  '?',  '?',  '?',  '?',  '?',  '?',  '?',
 '?',  '?',  '?',  '?',  '?',  '?',  '?',  '?',  '?',  '?',
 '?',  '?',  '?',  '?',  '?',  '?',  '?',  '?',  '?',  '?',
 '?',  '?',  '?',  '?',  '?',  '?',  '?',  '?',  '?',  '?',
 '?',  '?',  '?',  '?',  '?',  '?',  '?',  '?',  '?',  '?',
 '?',  '?',  '?',  '?',  '?',  '?',  '?',  '?',  '?',  '?',
 '?',  '?',  '?',  '?',  '?',  '?',  '?',  '?',  '?',  '?',
 '?',  '?',  '?',  '?',  '?',  '?',  '?',  '?',  '?',  '?',
 '?',  '?',  '?',  '?',  '?',  '?',  '?',  '?',  '?',  '?',
 '?',  '?',  '?',  '?',  '?',  '?',  '?',  '?',  '?',  '?',
 '?',  '?',  '?',  '?',  '?',  '?',  '?',  '?',  '?',  '?',
 '?',  '?',  '?',  '?',  '?',  '?'
};

void modp_toupper_copy(char* dest, const char* str, size_t len)
{
    int i;
    uint32_t eax, ebx;
    const uint8_t* ustr = (const uint8_t*) str;
    const int leftover = len % 4;
    const int imax = len / 4;
    const uint32_t* s = (const uint32_t*) str;
    uint32_t* d = (uint32_t*) dest;
    for (i = 0; i != imax; ++i) {
        eax = s[i];
        /*
         * This is based on the algorithm by Paul Hsieh
         * http://www.azillionmonkeys.com/qed/asmexample.html
         */
        ebx = (0x7f7f7f7fu & eax) + 0x05050505u;
        ebx = (0x7f7f7f7fu & ebx) + 0x1a1a1a1au;
        ebx = ((ebx & ~eax) >> 2)  & 0x20202020u;
        *d++ = eax - ebx;
    }

    i = imax*4;
    dest = (char*) d;
    switch (leftover) {
    case 3: *dest++ = (char) gsToUpperMap[ustr[i++]];
    case 2: *dest++ = (char) gsToUpperMap[ustr[i++]];
    case 1: *dest++ = (char) gsToUpperMap[ustr[i]];
    case 0: *dest = '\0';
    }
}

void modp_tolower_copy(char* dest, const char* str, size_t len)
{
    int i;
    uint32_t eax, ebx;
    const uint8_t* ustr = (const uint8_t*) str;
    const int leftover = len % 4;
    const int imax = len / 4;
    const uint32_t* s = (const uint32_t*) str;
    uint32_t* d = (uint32_t*) dest;
    for (i = 0; i != imax; ++i) {
        eax = s[i];
        /*
         * This is based on the algorithm by Paul Hsieh
         * http://www.azillionmonkeys.com/qed/asmexample.html
         */
        ebx = (0x7f7f7f7fu & eax) + 0x25252525u;
        ebx = (0x7f7f7f7fu & ebx) + 0x1a1a1a1au;
        ebx = ((ebx & ~eax) >> 2)  & 0x20202020u;
        *d++ = eax + ebx;
    }

    i = imax*4;
    dest = (char*) d;
    switch (leftover) {
    case 3: *dest++ = (char) gsToLowerMap[ustr[i++]];
    case 2: *dest++ = (char) gsToLowerMap[ustr[i++]];
    case 1: *dest++ = (char) gsToLowerMap[ustr[i]];
    case 0: *dest = '\0';
    }
}

void modp_toupper(char* str, size_t len)
{
    modp_toupper_copy(str, str, len);
}

void modp_tolower(char* str, size_t len)
{
    modp_tolower_copy(str, str, len);
}

void modp_toprint_copy(char* dest, const char* str, size_t len)
{
    int i;
    uint8_t c1,c2,c3,c4;

    const int leftover = len % 4;
    const int imax = len - leftover;
    const uint8_t* s = (const uint8_t*) str;
    for (i = 0; i != imax ; i+=4) {
        /*
         * it's important to make these variables
         * it helps the optimizer to figure out what to do
         */
        c1 = s[i]; c2=s[i+1]; c3=s[i+2]; c4=s[i+3];
        dest[0] = (char) gsToPrintMap[c1];
        dest[1] = (char) gsToPrintMap[c2];
        dest[2] = (char) gsToPrintMap[c3];
        dest[3] = (char) gsToPrintMap[c4];
        dest += 4;
    }

    switch (leftover) {
    case 3: *dest++ = (char) gsToPrintMap[s[i++]];
    case 2: *dest++ = (char) gsToPrintMap[s[i++]];
    case 1: *dest++ = (char) gsToPrintMap[s[i]];
    case 0: *dest = '\0';
    }
}

void modp_toprint(char* str, size_t len)
{
    modp_toprint_copy(str,str,len);
}

/*
 * Copyright (C) 2003-2007 Funambol, Inc.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY, TITLE, NONINFRINGEMENT or FITNESS FOR A PARTICULAR
 * PURPOSE.  See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA
 * 02111-1307  USA
 */
#include <string.h>

#include "base/fscapi.h"
#include "base/util/StringBuffer.h"

static const char b64_tbl[] =
        "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
static const char b64_pad = '=';

/* base64 encode a group of between 1 and 3 input chars into a group of
 * 4 output chars */
static void encode_group(char output[], const unsigned char
input[], int n)
{
        unsigned char ingrp[3];

        ingrp[0] = n > 0 ? input[0] : 0;
        ingrp[1] = n > 1 ? input[1] : 0;
        ingrp[2] = n > 2 ? input[2] : 0;

        /* upper 6 bits of ingrp[0] */
        output[0] = n > 0 ? b64_tbl[ingrp[0] >> 2] : b64_pad;
        /* lower 2 bits of ingrp[0] | upper 4 bits of ingrp[1] */
        output[1] = n > 0 ? b64_tbl[((ingrp[0] & 0x3) << 4) | (ingrp[1] >> 4)]
                : b64_pad;
        /* lower 4 bits of ingrp[1] | upper 2 bits of ingrp[2] */
        output[2] = n > 1 ? b64_tbl[((ingrp[1] & 0xf) << 2) | (ingrp[2] >> 6)]
                : b64_pad;
        /* lower 6 bits of ingrp[2] */
        output[3] = n > 2 ? b64_tbl[ingrp[2] & 0x3f] : b64_pad;

}

/**
 * Encodes the given src string in Base64. It returns the encoded string size.
 *
 * Warning: don't use this function unless you know exactly how large the output
 * is going to be.
 *
 * @param dest where the encoded string will be stored
 * @param src the string to encode
 * @param len src length
 *
 */
int __cdecl b64_encode(char *dest, void *src, int len)
{
        int outsz = 0;

        while (len > 0) {
                encode_group(dest + outsz,
                    (const unsigned char*)src,
                    len > 3 ? 3 : len);
                len -= 3;
                src = (char*)src + 3 * sizeof(char);
                outsz += 4;
        }

        return outsz;
}

void b64_encode(StringBuffer &dest, void *src, int len)
{
    // "magic" formular copied from B64Encoder::transform()
    char *buffer = new char[((len/3+1)<<2) + 32];
    int destlen = b64_encode(buffer, src, len);
    buffer[destlen] = 0;

    // it would be nice to use the StringBuffer's buffer directly,
    // but StringBuffer API does not allow that
    dest = buffer;
    delete [] buffer;
}

/* base64 decode a group of 4 input chars into a group of between 0 and
 * 3
 * output chars */
static void decode_group(unsigned char output[], const char
input[], int *n)
{
        const char *t1,*t2;
        *n = 0;

        if (input[0] == b64_pad)
                return;

        if (input[1] == b64_pad) {
            lastErrorCode = ERR_UNSPECIFIED;
            sprintf(lastErrorMsg, ERRMSG_B64_ORPHANED_BITS);
            return;
        }

        t1 = strchr(b64_tbl, input[0]);
        t2 = strchr(b64_tbl, input[1]);

        if ((t1 == NULL) || (t2 == NULL)) {
            lastErrorCode = ERR_UNSPECIFIED;
            sprintf(lastErrorMsg, ERRMSG_B64_GARBAGE);
        }

        output[(*n)++] = ((t1 - b64_tbl) << 2) | ((t2 - b64_tbl) >> 4);

        if (input[2] == b64_pad)
                return;

        t1 = strchr(b64_tbl, input[2]);

        if (t1 == NULL) {
            lastErrorCode = ERR_UNSPECIFIED;
            sprintf(lastErrorMsg, ERRMSG_B64_GARBAGE);
            return;
        }

        output[(*n)++] = ((t2 - b64_tbl) << 4) | ((t1 - b64_tbl) >> 2);

        if (input[3] == b64_pad)
                return;

        t2 = strchr((const char *)b64_tbl, input[3]);

        if (t2 == NULL) {
            lastErrorCode = ERR_UNSPECIFIED;
            sprintf(lastErrorMsg, ERRMSG_B64_GARBAGE);
            return;
        }

        output[(*n)++] = ((t1 - b64_tbl) << 6) | (t2 - b64_tbl);

        return;
}

int b64_decode(void *dest, const char *src)
{
        int len = 0;
        int outsz = 0;

        while (*src) {
                decode_group((unsigned char*)dest + outsz,
                    src,
                    &len);
                src += 4;
                outsz += len;
        }

        return outsz;
}

void * b64_decode(int & len, const char *src)
{
    char * dest = new char[strlen(src) + 1];
    len = b64_decode((void *)dest, src);
    dest[len] = 0;
    return (void *)dest;
}

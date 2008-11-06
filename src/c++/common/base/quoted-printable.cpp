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

#include "base/fscapi.h"
#include "base/quoted-printable.h"

static int hex2int( char x )
{
    return (x >= '0' && x <= '9') ? x - '0' :
        (x >= 'A' && x <= 'F') ? x - 'A' + 10 :
        (x >= 'a' && x <= 'f') ? x - 'a' + 10 :
        0;
}

char *qp_decode(const char *qp)
{
    const char *in;
    char *ret = new char[strlen(qp)+1];
    char *out = ret;

    for (in = qp; *in; in++ ) {
        // Handle encoded chars
        if ( *in == '=' ) {
            if (in[1] && in[2] ) {
                // The sequence is complete, check it
                in++;   // skip the '='
                if (in[0] == '\r' && in[1] == '\n') {
                    // soft line break, ignore it
                    in ++;
                    continue;
                }
                else if ( isxdigit(in[0]) && isxdigit(in[1]) ) {
                    // Ok, we have two hex digit: decode them
                    *out = (hex2int(in[0]) << 4) | hex2int(in[1]);
                    out++;
                    in ++;
                    continue;
                }
            }
            // In all wrong cases leave the original bytes
            // (see RFC 2045). They can be incomplete sequence,
            // or a '=' followed by non hex digit.
        }
        // TODO:
        // RFC 2045 says to exclude control characters mistakenly
        // present (unencoded) in the encoded stream.

        // Copy other characters
        *out = *in;
        out++;
    }
    *out = 0;

    return ret;
}

// A simple version of qp_encoding
char *qp_encode(const char *qp) {
	char QP_DIGITS[] = "0123456789ABCDEF";
	char* ret = new char[strlen(qp)*3+1];
	int i = 0;

	const char *in;
	for (in = qp; *in; in++ ) {
		if ( (0x21 <= in[0]) & (in[0] <= 0x7e) && in[0] != '=' ) {
            ret[i] = *in;
			i++;
        }
        else {
            ret[i] = '=';
			i++;
            ret[i] = QP_DIGITS[in[0] >> 4 & 0xf];
			i++;
            ret[i] = QP_DIGITS[in[0] & 0xf];
			i++;
        }
	}

	ret[i] = '\0';

	return ret;
}

bool qp_isNeed(const char *in) {
	for(int i = 0; i < int(strlen(in)); i++)
		if ( (0x21 > in[i]) || (in[i] > 0x7e) || in[i] == '=' )
			return true;

	return false;
}


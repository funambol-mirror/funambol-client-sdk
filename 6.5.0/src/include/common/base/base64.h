/*
 * Copyright (C) 2003-2007 Funambol, Inc
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

#ifndef _BASE64_H
#define _BASE64_H
/** @cond DEV */

#include "base/fscapi.h"

int b64_encode(char *dest, void *src, int len);
int b64_decode(void *dest, const char *src);

class StringBuffer;

/**
 * Encode arbitrary data into b64 encoded, nul-terminated string.
 *
 * @retval dest    string buffer for encoded string
 * @param src      binary data
 * @len            number of bytes
 */
void b64_encode(StringBuffer &dest, void *src, int len);

/**
 * Decode b64 encoded, nul-terminated string into the original
 * binary data.
 *
 * @retval len       number of valid bytes in dest, not counting the extra nul-byte
 * @param src        nul-terminated input string
 * @return address of the dynamically allocated buffer,
 *         has to be freed by caller with delete [];
 *         always contains nul-byte after original data
 */
void * b64_decode(int & len, const char *src);

/** @endcond */
#endif /* BASE64_H */


/*
 * Funambol is a mobile platform developed by Funambol, Inc. 
 * Copyright (C) 2003 - 2007 Funambol, Inc.
 * 
 * This program is free software; you can redistribute it and/or modify it under
 * the terms of the GNU Affero General Public License version 3 as published by
 * the Free Software Foundation with the addition of the following permission 
 * added to Section 15 as permitted in Section 7(a): FOR ANY PART OF THE COVERED
 * WORK IN WHICH THE COPYRIGHT IS OWNED BY FUNAMBOL, FUNAMBOL DISCLAIMS THE 
 * WARRANTY OF NON INFRINGEMENT  OF THIRD PARTY RIGHTS.
 * 
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
 * details.
 * 
 * You should have received a copy of the GNU Affero General Public License 
 * along with this program; if not, see http://www.gnu.org/licenses or write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 * MA 02110-1301 USA.
 * 
 * You can contact Funambol, Inc. headquarters at 643 Bair Island Road, Suite 
 * 305, Redwood City, CA 94063, USA, or at email address info@funambol.com.
 * 
 * The interactive user interfaces in modified source and object code versions
 * of this program must display Appropriate Legal Notices, as required under
 * Section 5 of the GNU Affero General Public License version 3.
 * 
 * In accordance with Section 7(b) of the GNU Affero General Public License
 * version 3, these Appropriate Legal Notices must retain the display of the
 * "Powered by Funambol" logo. If the display of the logo is not reasonably 
 * feasible for technical reasons, the Appropriate Legal Notices must display
 * the words "Powered by Funambol".
 */

#ifndef _BASE64_H
#define _BASE64_H
/** @cond DEV */

#include "base/fscapi.h"
#include "base/globalsdef.h"

BEGIN_NAMESPACE

int b64_encode(char *dest, const void *src, int len);
int b64_decode(void *dest, const char *src);

class StringBuffer;

/**
 * Encode arbitrary data into b64 encoded, nul-terminated string.
 *
 * @retval dest    string buffer for encoded string
 * @param src      binary data
 * @len            number of bytes
 */
void b64_encode(StringBuffer &dest, const void *src, int len);

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


END_NAMESPACE

/** @endcond */
#endif /* BASE64_H */


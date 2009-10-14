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


#ifndef INCL_STRING_UTILS
#define INCL_STRING_UTILS


#include "base/util/StringBuffer.h"

BEGIN_NAMESPACE

//
// ------------------ With buf16 descriptors  ------------------
//
/**
 * Converter: TDesC16 descriptor -> char*.
 * The returned buffer is new allocated, must be released by the caller.
 * @note    a final '0' is appended to the returned buffer, as Symbian
 *          descriptors don't have the trailing null character.
 * @param   aInput the input descriptor to convert
 * @return  a new allocated char* buffer
 */ 
const char*    bufToNewChar (const TDesC&   aInput);
const wchar_t* bufToNewWchar(const TDesC&   aInput);

/**
 * Converter: char* -> HBufC16* descriptor.
 * The HBufC16* descriptor is new allocated, must be released by the caller.
 * @note  A good way to use this function is to assign the ownership of
 *       the returned descriptor to a local RBuf, so that there's no need to 
 *       care about the release of the buffer. For example:
 *            RBuf myBuf;
 *            myBuf.Assign(charToNewBuf("test"));
 * 
 * @param   aInput the input char* to convert
 * @return  a new allocated HBufC16* buffer
 */ 
HBufC*         charToNewBuf (const char*    aInput);
HBufC*         wcharToNewBuf(const wchar_t* aInput);


/// Converter: TDesC16 descriptor -> StringBuffer.
StringBuffer bufToStringBuffer   (const TDesC&        aInput);

/// Converter: StringBuffer -> HBufC16* descriptor.
HBufC*       stringBufferToNewBuf(const StringBuffer& aInput);



//
// ------------------ With buf8 descriptors  ------------------
//
const char*   buf8ToNewChar(const TDesC8& aInput);
HBufC8*       charToNewBuf8(const char*   aInput);

StringBuffer buf8ToStringBuffer(const TDesC8&       aInput);
HBufC8* stringBufferToNewBuf8  (const StringBuffer& aInput);


/**
 * Contextes are defined as: (string/)* and on Symbian
 * we map them to file. But Symbian uses backslashes as
 * directory separator.
 */
StringBuffer contextToPath(const char* cont);

/**
 * Concatenate two directory names (of file names)
 * If src1 is terminated by dir separator then src2 is simply appended
 * otherwise a dir separator is inserted first.
 */
void concatDirs(StringBuffer& src1, const char* src2);

END_NAMESPACE

#endif /*INCL_STRING_UTILS*/

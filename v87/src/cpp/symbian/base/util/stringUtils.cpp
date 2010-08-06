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

#include <e32base.h>
#include <aknnotewrappers.h>
#include <apgtask.h>
#include <eikenv.h>
#include <apgcli.h> 
#include <utf.h>             // Unicode <-> utf-8

#include "base/fscapi.h"
#include "base/util/utils.h"
#include "base/util/stringUtils.h"
#include "base/globalsdef.h"

BEGIN_NAMESPACE



//
// ------------------ With buf16 descriptors  ------------------
//
HBufC* charToNewBuf(const char* aInput) 
{
    int len = strlen(aInput);
    TUint8* chars = (TUint8*)aInput;

    RBuf8 buf8;
    buf8.CreateL(len);
    buf8.Copy(chars);
    
    HBufC* ret = CnvUtfConverter::ConvertToUnicodeFromUtf8L(buf8);
    buf8.Close();
    return ret;
}

const char* bufToNewChar(const TDesC& aInput) 
{
    HBufC8* buf8 = CnvUtfConverter::ConvertFromUnicodeToUtf8L(aInput);

    // This allocates a NEW char* buffer
    char* ret = stringdup((const char*)buf8->Ptr(), buf8->Length());
    delete buf8;
    return (const char*)ret;
}


const wchar_t* bufToNewWchar(const TDesC& aInput) 
{
    // This allocates a NEW wchar_t* buffer
    wchar_t* ret = wstrdup((const wchar_t*)aInput.Ptr(), aInput.Length());
    return (const wchar_t*)ret;
}


HBufC* wcharToNewBuf(const wchar_t* aInput) 
{
    int len = wcslen(aInput);
    TUint16* wchars = (TUint16*)aInput;

    RBuf16 buf16;
    buf16.CreateL(len);
    buf16.Copy(wchars);
    
    return buf16.Alloc();
}


StringBuffer bufToStringBuffer(const TDesC& aInput) 
{
    const char* chars = bufToNewChar(aInput);
    StringBuffer ret(chars);
    delete [] chars;
    return ret;
}

HBufC* stringBufferToNewBuf(const StringBuffer& aInput)
{
    return charToNewBuf(aInput.c_str());
}


//
// ------------------ With buf8 descriptors  ------------------
//
HBufC8* charToNewBuf8(const char* aInput) 
{
    int len = strlen(aInput);
    TUint8* chars = (TUint8*)aInput;

    RBuf8 buf8;
    buf8.CreateL(len);
    buf8.Copy(chars);
    HBufC8* res = buf8.Alloc();
    buf8.Close();
    return res;
}

const char* buf8ToNewChar(const TDesC8& aInput) 
{
    char* ret = stringdup((const char*)aInput.Ptr(), aInput.Length());
    return (const char*)ret;
}

StringBuffer buf8ToStringBuffer(const TDesC8& aInput) 
{
    const char* chars = buf8ToNewChar(aInput);
    StringBuffer ret(chars);
    delete [] chars;
    return ret;
}

HBufC8* stringBufferToNewBuf8(const StringBuffer& aInput)
{
    return charToNewBuf8(aInput.c_str());
}



StringBuffer contextToPath(const char* cont) 
{
    StringBuffer sb(cont);
    sb.replaceAll("/", "\\", 0);
    return sb;
}

void concatDirs(StringBuffer& src1, const char* src2) {
 
    // If the src path terminates with \\ then there is no
    // need to add it again (this would be an illegal symbian path)
    // Unfortunately we cannot use StringBuffer directly for this check
    // there is something weird with \\ in StringBuffer (at least on symbian)
    const char* chars = src1.c_str();
    if (chars[strlen(chars)-1] != '\\') {
        src1.append("\\");
    }
    src1.append(src2);
}


END_NAMESPACE


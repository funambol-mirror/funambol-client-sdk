 /*
 * Copyright (C) 2007 Funambol, Inc.
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

#ifndef INCL_STRINGUTILS_WIN
#define INCL_STRINGUTILS_WIN

/** @cond API */
/** @addtogroup win_adapter */
/** @{ */

#include "base/fscapi.h"
#include <string>


#define ERR_ENCRYPT_DATA                    "Error occurred encrypting private data"
#define ERR_DECRYPT_DATA                    "Error occurred decrypting private data"


void  toLowerCase      (std::string& s);
void  toLowerCase      (std::wstring& s);
void  replaceAll       (const std::wstring& source, const std::wstring& dest, std::wstring& dataString, const int startPos = 0);
int   getElementContent(const std::wstring& xml, const std::wstring& tag, std::wstring& content, unsigned int pos = 0);
int   getElementContent(const std::wstring& xml, const std::wstring& tag, std::wstring& content, const std::wstring::size_type pos, std::wstring::size_type& start, std::wstring::size_type& end);
char* encryptData      (const char* data);
char* decryptData      (const char* b64Data);


/** @} */
/** @endcond */
#endif
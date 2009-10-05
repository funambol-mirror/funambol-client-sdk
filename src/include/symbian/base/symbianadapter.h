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

#ifndef SYMBIANADAPTER_H_
#define SYMBIANADAPTER_H_

#include "base/globalsdef.h"

#include <e32def.h>
#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <stdarg.h>
#include <string.h>
#include <utf.h>
#include <ctype.h>

#define PLATFORM_VA_LIST    VA_LIST
#define PLATFORM_VA_START   VA_START
#define PLATFORM_VA_END     VA_END
#define PLATFORM_VA_ARG     VA_ARG

#ifdef __WINSCW__
    #define PLATFORM_VA_COPY(a,b) (a[0] = b[0])
    #if !defined(va_copy)
    #define va_copy
    #endif
#endif

#define WCHAR       wchar_t
#define BOOL        int
#define TRUE        1
#define FALSE       0
#define __cdecl
#define USE_WCHAR   1
#define TEXT(_x)    L##_x

#define _wcsicmp wcscasecmp

BEGIN_NAMESPACE

/* These functions miss in the Symbian libc and are implemented here */
wchar_t * wcsncpy (wchar_t *dst, const wchar_t *src, size_t count);
int towlower(int c);
int towupper(int c);
size_t vsnprintf(char* s, size_t size, const char* format, PLATFORM_VA_LIST aq);
size_t snwprintf(WCHAR *v, size_t size, const WCHAR* format, unsigned long value);
WCHAR *wcschr(const WCHAR *ws, WCHAR wc);
WCHAR *wcsstr(const WCHAR *ws1, const WCHAR *ws2);
WCHAR *wcstok(WCHAR *ws1, const WCHAR *ws2);
WCHAR *wcsncat(WCHAR *ws1, const WCHAR *ws2, size_t n);
double wcstod(const WCHAR *nptr, WCHAR ** endptr);	
int _wtoi(const WCHAR *str);
int wcscasecmp(const WCHAR* s1, const WCHAR* s2);
bool readFile(const char* path, char **message, size_t *len, bool binary);
bool saveFile(const char *filename, const char *buffer, size_t len, bool binary);
bool fileExists(const char *aFileName);
char** readDir(const char* name, int *count, bool onlyCount);
bool removeDir(const char *aDirPath);
END_NAMESPACE

/* Symbian does not ship a stdint.h interface (unless you use PIPS or OpenC)
 * We define the types used in the library, but at the moment we are not
 * standard complaint as we do not define all the necessary macros. This needs
 * to be done
 **/
typedef TInt8   int8_t;
typedef TUint8  uint8_t;
typedef TInt16  int16_t;
typedef TUint16 uint16_t;
typedef TInt32  int32_t;
typedef TUint32 uint32_t;
typedef TInt64  int64_t;
typedef TUint64 uint64_t;

#endif /*SYMBIANADAPTER_H_*/


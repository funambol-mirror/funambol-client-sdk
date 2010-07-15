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


#ifndef INCL_POSIX_BUILD_ADAPTER
#define INCL_POSIX_BUILD_ADAPTER
/** @cond DEV */

#include "base/globalsdef.h"

// Enable the fix for encoding when building for posix
#define VOCL_ENCODING_FIX

#define min(x,y) ( (x) < (y) ? (x) : (y) )
#define max(x,y) ( (x) > (y) ? (x) : (y) )

#ifdef USE_WCHAR

BEGIN_NAMESPACE

// FIXME: remove this and adapt VOCL.
WCHAR *wcstok(WCHAR *s, const WCHAR *delim);

inline int _wtoi(const WCHAR *s) { return (int)wcstol(s, NULL, 10); }

END_NAMESPACE

#define _wcsicmp wcscasecmp
#define wcsicmp wcscasecmp
#define snwprintf swprintf

#if defined(MAC) || defined(FUN_IPHONE)

#   undef  _wcsicmp
#   undef  wcsicmp



inline int wcsicmp(const WCHAR * s1, const WCHAR * s2)
{
    const WCHAR * i1 = s1;
    const WCHAR * i2 = s2;
    
    while (i1 != NULL && i2 != NULL)
    {
        if (*i1 > *i2)
            return 1;
        if (*i1 < *i2)
            return -1;
            
        i1++;
        i2++;
    }
    if (i1 == NULL && i2 == NULL)
        return 0;
    if (i1 == NULL)
        return -1;
    if (i2 == NULL)
        return 1;
    // Should never happen
    return 0;
}

#   define _wcsicmp wcsicmp
#endif

#else

/* map WCHAR and its functions back to standard functions */

#       define SYNC4J_LINEBREAK "\n"

#       define wsprintf sprintf
#       define _wfopen fopen
#       define wprintf printf
#       define fwprintf fprintf
#       define wsprintf sprintf
#       define swprintf snprintf
#       define snwprintf snprintf
#       define wcscpy strcpy
#       define wcsncpy strncpy
#       define wcsncmp strncmp
#       define wcslen strlen
#       define wcstol strtol
#       define wcstoul strtoul
#       define wcsstr strstr
#       define wcscmp strcmp
#       define wcstok strtok
        inline char towlower(char x) { return tolower(x); }
        inline char towupper(char x) { return toupper(x); }
#       define wmemmove memmove
#       define wmemcpy memcpy
#       define wmemcmp memcmp
#       define wmemset memset
#       define wcschr strchr
#       define wcsrchr strrchr
#       define wcscat strcat
#       define wcsncat strncat
#       define _wtoi atoi
#       define wcstod strtod
#       define wcsicmp strcasecmp
#       define _wcsicmp strcasecmp
#       define _stricmp strcasecmp

#endif

#endif


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


#ifndef INCL_AUTOTOOLS_ADAPTER
#define INCL_AUTOTOOLS_ADAPTER
/** @cond DEV */


/*
 * POSIX environment, configured and compiled with automake/autoconf
 */

#include <config.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <stdio.h>
#include <time.h>
#include <stdarg.h>
#include <unistd.h>

// For ntoh functions
#include <netinet/in.h>

// Workaround for wchar defines below: unit testing may depend
// on standard header files. Include its header file first.
#include <base/test.h>

// Cygwin version of gcc does have these builtin
#ifndef __CYGWIN__
# define __declspec(x)
# define __cdecl
#endif

#define BOOL int
#define TRUE 1
#define FALSE 0

// Enable the fix for encoding when building for posix
#define VOCL_ENCODING_FIX


#ifdef USE_WCHAR

#undef WCHAR
#include <wchar.h>
#include <wctype.h>

#define WCHAR wchar_t
#define WCHAR_PRINTF "s"
#define TEXT(_x) L##_x

// FIXME: remove this and adapt VOCL.
WCHAR *wcstok(WCHAR *s, const WCHAR *delim);

inline int _wtoi(const WCHAR *s) { return (int)wcstol(s, NULL, 10); }

#define _wcsicmp wcscasecmp
#define wcsicmp wcscasecmp
#define snwprintf swprintf

#else

/* map WCHAR and its functions back to standard functions */
#       undef WCHAR
#       define WCHAR char
#       define WCHAR_PRINTF "s"
#       define TEXT(_x) _x

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

/* some of the code compares NULL against integers, which
   fails if NULL is defined as (void *)0 */
#undef NULL
#define NULL 0

#define min(x,y) ( (x) < (y) ? (x) : (y) )
#define max(x,y) ( (x) > (y) ? (x) : (y) )

/** @endcond */
#endif


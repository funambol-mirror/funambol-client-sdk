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

/**
 * IMPORTANT: make sure your compiler includes both the include
 *            paths:
 *              <...>/src/include/<platform>
 *              <...>/src/include/common
 *            (in the given sequence)
 */

#ifndef INCL_FSCAPI
    #define INCL_FSCAPI
/** @cond DEV */

    #ifdef AUTOTOOLS
        #include "base/posixadapter.h"
    #endif
    #ifdef HAVE_STDARG_H
        #include <stdarg.h>
    #endif

    #include "base/errors.h"

    #if defined(_WIN32_WCE) || defined(WIN32)
        // Windows common stuff
        #define WIN32_LEAN_AND_MEAN     // Exclude rarely-used stuff from Windows headers

        #include <windows.h>
        #include "base/winadapter.h"
    #endif

    #if defined(WIN32)
        #include "wininet.h"
    #endif

    #if defined(WIN32) && !defined(_WIN32_WCE)
        #include <wchar.h>
        #include <time.h>
        #include <stdlib.h>
    #endif

    #ifdef _WIN32_WCE
        #include "base/time.h"
    #endif

    #if defined(__PALMOS__)
      #include "base/palmadapter.h"
    #endif

    #ifdef MALLOC_DEBUG
      #pragma warning(disable:4291)
      extern size_t dbgcounter;
      void *operator new(size_t s);
      void *operator new[](size_t s);
      void *operator new(size_t s, char* file, int line);
      void *operator new[](size_t s, char* file, int line);
      void operator delete(void* p);
      void operator delete[] (void* p);

      #define new new(__FILE__, __LINE__)
    #endif

    #ifndef SYNC4J_LINEBREAK
    // default encoding of line break in native strings,
    // may be overridden by adapter header files above
    # define SYNC4J_LINEBREAK TEXT("\r\n")
    #endif

    #ifndef WCHAR_PRINTF
    /** use in format string like this: printf( "str '%" WCHAR_PRINTF "'", (WCHAR *)foo) */
    # define WCHAR_PRINTF "ls"
    #endif

    /**
     * All platforms are expected to have assert.h and provide
     * assert() in it. However, controlling whether assertions are
     * enabled or not depends on the specific platform.
     *
     * On Windows, the Visual Studio project file enables assertions
     * in debug builds and disables them in release builds.
     *
     * On systems using the autotools build, the --enable-assert
     * option must be used to enable assertions. To be compatible with
     * previous revisions they are disabled by default.
     *
     * @warning Source files should always include assert.h via
     * fscapi.h so that the platform specific code above has a chance
     * to control assertions.
     */
    #include <assert.h>

/** @endcond */
#endif



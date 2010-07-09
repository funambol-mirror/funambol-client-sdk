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

    /* This is the list of possible transport agents (some are available on
     * different platforms, while other ones are available on a single platform)
     **/
    #define FUN_CURL_TRANSPORT_AGENT 1
    #define FUN_MAC_TRANSPORT_AGENT  2
    #define FUN_WIN_TRANSPORT_AGENT  3
    #define FUN_SYM_TRANSPORT_AGENT  4
    #define FUN_MOZ_TRANSPORT_AGENT  5

/* Autodetect target system based on compiler automatic macros */
    #if defined(__unix__) || defined(__linux__) || defined(ANDROID)
    /* This is a generic unix system */
    #define POSIX 1

    #if !defined(FUN_TRANSPORT_AGENT)
    #define FUN_TRANSPORT_AGENT FUN_CURL_TRANSPORT_AGENT
    #endif

    #elif defined(__ENVIRONMENT_IPHONE_OS_VERSION_MIN_REQUIRED__)
    /* This is an iPhone */
    #define FUN_IPHONE   1
    #define POSIX 1

    #if !defined(FUN_TRANSPORT_AGENT)
    #define FUN_TRANSPORT_AGENT FUN_MAC_TRANSPORT_AGENT
    #endif

    #elif defined(__APPLE__) && defined(__MACH__)
    /* This is a MacOSx. If needed we could use __ENVIRONMENT_MAC_OS_X_VERSION_MIN_REQUIRED__
     * to discriminate on the OS version (e.g. 10.5 vs 10.4)
     */
    #define POSIX 1
    #define MAC   1  

    #if !defined(FUN_TRANSPORT_AGENT)
    #define FUN_TRANSPORT_AGENT FUN_MAC_TRANSPORT_AGENT
    #endif

    #elif defined(_WIN32_WCE) || defined(WIN32)
    /* This is windows or windows mobile, in this case we don't have our
     * own macro.
     */

    #if !defined(FUN_TRANSPORT_AGENT)
    #define FUN_TRANSPORT_AGENT FUN_WIN_TRANSPORT_AGENT
    #endif


    #elif defined(__SYMBIAN32__)
    /* This is Symbian. Other macros allow to discriminate the version, but at
     * the moment we don't need it
     */
    #define SYMBIAN 1

    #if !defined(FUN_TRANSPORT_AGENT)
    #define FUN_TRANSPORT_AGENT FUN_SYM_TRANSPORT_AGENT
    #endif

    #else
    #error "Unrecognized platform, cannot proceed"
    #endif

    /* Include the proper files */
    #ifdef MAC
        #ifndef MSG_NOSIGNAL
            #define MSG_NOSIGNAL SO_NOSIGPIPE
        #endif
    #endif
    
    #ifdef POSIX
        #ifdef ANDROID
		    #include "base/androidadapter.h"
	    #else	
		    #include "base/posixadapter.h"
        #endif
        #include <sys/stat.h>
        #include <errno.h>
        #include <sys/types.h>
        
        #ifndef S_ISDIR
        #define S_ISDIR(x) (((x) & S_IFMT) == S_IFDIR)
        #endif
    #elif SYMBIAN
        #include "base/symbianadapter.h"
        #include <sys/stat.h>
        #include <errno.h>
        #include <sys/types.h>
        
        #ifndef S_ISDIR
        #define S_ISDIR(x) (((x) & S_IFMT) == S_IFDIR)
        #endif
    #elif defined(_WIN32_WCE) || defined(WIN32)
        // Windows common stuff
        #define WIN32_LEAN_AND_MEAN     // Exclude rarely-used stuff from Windows headers

        #include <windows.h>
        #include "base/winadapter.h"

        #if defined(WIN32)
            #include "wininet.h"
        #endif

        #if defined(WIN32) && !defined(_WIN32_WCE)
            #include <sys/stat.h>
            #include <errno.h>
            #include <sys/types.h>
            #include "shlobj.h"        
            #include <wchar.h>
            #include <time.h>
            #include <stdlib.h>
            
            #ifndef S_ISDIR
            #define S_ISDIR(x) (((x) & S_IFMT) == S_IFDIR)
            #endif
        #endif

        #ifdef _WIN32_WCE
            #include "base/time.h"
            #include "base/stat.h" // for file handling on wm
            const int errno = -1;
        #endif
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

    #if !defined(PLATFORM_VA_LIST)
    #define PLATFORM_VA_LIST va_list
    #endif

    #if !defined(PLATFORM_VA_START)
    #define PLATFORM_VA_START va_start
    #endif

    #if !defined(PLATFORM_VA_END)
    #define PLATFORM_VA_END  va_end
    #endif

    #if !defined(PLATFORM_VA_COPY)
    #define PLATFORM_VA_COPY va_copy
    #endif

    #if !defined(PLATFORM_VA_ARG)
    #define PLATFORM_VA_ARG va_arg
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

    #ifdef HAVE_STDARG_H
        #include <stdarg.h>
    #endif
 
    #include "base/errors.h"

/** @endcond */
#endif



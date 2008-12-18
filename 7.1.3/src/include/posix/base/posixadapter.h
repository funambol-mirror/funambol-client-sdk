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


#ifndef INCL_POSIX_ADAPTER
#define INCL_POSIX_ADAPTER
/** @cond DEV */


/*
 * POSIX environment, configured and compiled with automake/autoconf
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif
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

#ifdef USE_WCHAR

# undef WCHAR
# include <wchar.h>
# include <wctype.h>

# define WCHAR wchar_t
# define WCHAR_PRINTF "ls"
# define TEXT(_x) L##_x

#else

/* map WCHAR and its functions back to standard functions */
#       undef WCHAR
#       define WCHAR char
#       define WCHAR_PRINTF "s"
#       define TEXT(_x) _x

#endif

#ifdef FUNAMBOL_BUILD_API
# include "base/posix_build_adapter.h"
#endif

/** @endcond */
#endif


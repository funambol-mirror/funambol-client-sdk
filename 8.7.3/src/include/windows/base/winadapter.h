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

#ifndef INCL_WIN_ADAPTER
#define INCL_WIN_ADAPTER
/** @cond DEV */

#include <stdarg.h>
#include <stdio.h>


#ifdef _WIN32_WCE
#include <winsock.h>
#define __declspec(ignore)

// Map snprintf and vsnprintf to winmobile functions
#define snprintf _snprintf
#define vsnprintf _vsnprintf

// some of the code compares NULL against integers, which
// fails if NULL is defined as (void *)0
#undef NULL
#define NULL 0
#endif  // #ifdef _WIN32_WCE

typedef wchar_t WCHAR;

typedef __int8             int8_t;
typedef __int16            int16_t;
typedef __int32            int32_t;
typedef __int64            int64_t;
typedef unsigned __int8    uint8_t;
typedef unsigned __int16   uint16_t;
typedef unsigned __int32   uint32_t;
typedef unsigned __int64   uint64_t;

// Define this to the implementation of the sprintf variant with
// size check for wide char.
#define snwprintf _snwprintf

#include "base/globalsdef.h"

BEGIN_NAMESPACE

/**
 * Returns a rounded integer value from double.
 */
int round(double val);

/**
 * create and signal a sync cancelled event handle
 */
bool setSyncAborted();

/**
 * check if a the sync cancelled event has been signalled
 */
bool checkSyncAborted();

/**
 * reset the handle
 */
bool resetSyncAborted();

#if defined(WIN32) && !defined(_WIN32_WCE)
void printMemLeaks();
#endif

END_NAMESPACE

// Enable warning: class has virtual functions, but destructor is not virtual
// When a class has virtual functions but a nonvirtual destructor, 
// objects of the type might not be destroyed properly when the class 
// is destroyed through a base class pointer.
#pragma warning(default : 4265)


/** @endcond */
#endif

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

#ifndef INCL_TIMEUTILS_WIN
#define INCL_TIMEUTILS_WIN

/** @cond API */
/** @addtogroup win_adapter */
/** @{ */


typedef double DATE;

// Date/time definitions
#define REFERRED_MAX_DATE                   949998.000000                   /**< this is "4501-01-01" in double format: the error date of Outlook   */
#define LIMIT_MAX_DATE                      767011.000000                   /**< this is "4000-01-01" in double format: the max date accepted       */



#include "base/fscapi.h"
#include <string>


void   doubleToStringTime(std::wstring& stringDate, const DATE doubleDate, bool onlyDate = false);
void   stringTimeToDouble(const std::wstring& dataString, DATE* date);

bool   isAllDayFormat    (const std::wstring& dataString);
bool   isAllDayInterval  (const DATE startdate, const DATE enddate);

WCHAR* daysOfWeekToString(int l);
int    stringToDaysOfWeek(WCHAR* in);
int    getWeekDayFromDate(DATE date);
bool   isWeekDay         (WCHAR* data);

/** @} */
/** @endcond */
#endif
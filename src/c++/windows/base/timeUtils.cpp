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

#include "base/Log.h"
#include "base/timeUtils.h"
#include "vocl/constants.h"
#include <oleauto.h>

using namespace std;


//
// ------------------------------ DATE/TIME CONVERSIONS FUNCTIONS ------------------------------
//
/**
 * Variant time (double) -> System time ("YYYYMMDDThhmmssZ" or "YYYYMMDD")
 * The output value is a string. 
 *
 * @param stringDate   [OUT] the date returned in SystemTime format
 * @param doubleDate   the input date in variant time format
 * @param onlyDate     the input date in variant time format
 */
void doubleToStringTime(wstring& stringDate, const DATE doubleDate, bool onlyDate) {

    if (!doubleDate || doubleDate > LIMIT_MAX_DATE) {
        stringDate = L"";
        return;
    }

    SYSTEMTIME t;
    VariantTimeToSystemTime(doubleDate, &t);

    WCHAR date[20];
    wsprintf(date, TEXT("%i%02i%02i"), t.wYear, t.wMonth, t.wDay);
    if (!onlyDate) {
        wsprintf(&date[8], TEXT("T%02i%02i%02iZ"), t.wHour, t.wMinute, t.wSecond);
    }

    stringDate = date;
}



/**
 * String time ("YYYYMMDDThhmmssZ" or "YYYYMMDD") -> Variant time (double).
 *
 * @param dataString : the input string in System time format
 * @param date       : [OUT] the returned value into VariantTime format
 */
void stringTimeToDouble(const wstring& dataString, DATE* date) {

    WCHAR inputTime[20];
    SYSTEMTIME t;

    if (dataString.size() < 8) {
        *date = NULL;  // Error!
        return;
    }

    wsprintf(inputTime, dataString.c_str());

    wstring::size_type pos = dataString.find(L"-", 0);
    if (pos == wstring::npos) {
        // "yyyyMMdd"
        swscanf(inputTime, L"%4d%2d%2d", &t.wYear, &t.wMonth, &t.wDay);

        if (dataString.size() > 9 && dataString.size() < 17) {
            // "hhmmss"
            swscanf(&inputTime[9], L"%2d%2d%2d", &t.wHour, &t.wMinute, &t.wSecond);
        }
        else {
            t.wHour   = 0;
            t.wMinute = 0;
            t.wSecond = 0;
        }
    }
    else {
        // old format: "yyyy-MM-dd"
        swscanf(inputTime, L"%4d-%2d-%2d", &t.wYear, &t.wMonth, &t.wDay);
        t.wHour   = 0;
        t.wMinute = 0;
        t.wSecond = 0;
    }

    t.wMilliseconds = 0;
    t.wDayOfWeek    = 0;
    SystemTimeToVariantTime(&t, date);
}




/*
 * Return true if date passed is in format "yyyyMMdd" (or old format "yyyy-MM-dd").
 */
bool isAllDayFormat(const wstring& dataString) {

    if (dataString.size() == 8) {
        return true;
    }

    // Also support "yyyy-MM-dd" old format...
    wstring::size_type pos = dataString.find(L"-", 0);
    if (pos != wstring::npos) {
        return true;
    }
    else {
        return false;
    }
}



/**
 * Returns true if startdate = 00:00 and enddate = 23:59.
 * This is an all-day-event.
 */
bool isAllDayInterval(const DATE startdate, const DATE enddate) {

    SYSTEMTIME ststart, stend;
    VariantTimeToSystemTime(startdate, &ststart);
    VariantTimeToSystemTime(enddate,   &stend);
    
    bool ret = false;
    if (ststart.wHour == 0 && ststart.wMinute == 0 &&
        stend.wHour == 23  && stend.wMinute  == 59) {
        ret = true;
    }
    return ret;
}




/**
 * daysOfWeekMask -> string.
 * @return  a string with days of week formatted (like "SU MO TU FR"),
 *          based on the daysOfWeek mask passed.
 * @note    returns a new allocated string, must be freed by the caller.
 */
WCHAR* daysOfWeekToString(int l) {

    if (l<0 || l>128)
        return NULL;

    //SU MO TU WE TH FR SA
    WCHAR* ret = new WCHAR[22];
    wcscpy(ret, TEXT(""));

    if(l & winSunday)    wcscat(ret, TEXT("SU "));
    if(l & winMonday)    wcscat(ret, TEXT("MO "));
    if(l & winTuesday)   wcscat(ret, TEXT("TU "));
    if(l & winWednesday) wcscat(ret, TEXT("WE "));
    if(l & winThursday)  wcscat(ret, TEXT("TH "));
    if(l & winFriday)    wcscat(ret, TEXT("FR "));
    if(l & winSaturday)  wcscat(ret, TEXT("SA "));

    return ret;
}

/**
 * string -> dayOfWeekMask
 * Calculate the dayOfWeekMask based on the input string
 * of days (like "SU MO TU FR").
 */
int stringToDaysOfWeek(WCHAR* in) {
    int ret = 0;

    WCHAR* index;
    index = NULL;
    index = wcsstr(in, TEXT("SU"));
    if(index)
        ret += winSunday;

    index = NULL;
    index = wcsstr(in, TEXT("MO"));
    if(index)
        ret += winMonday;

    index = NULL;
    index = wcsstr(in, TEXT("TU"));
    if(index)
        ret += winTuesday;

    index = NULL;
    index = wcsstr(in, TEXT("WE"));
    if(index)
        ret += winWednesday;

    index = NULL;
    index = wcsstr(in, TEXT("TH"));
    if(index)
        ret += winThursday;

    index = NULL;
    index = wcsstr(in, TEXT("FR"));
    if(index)
        ret += winFriday;

    index = NULL;
    index = wcsstr(in, TEXT("SA"));
    if(index)
        ret += winSaturday;

    return ret;
}


int getWeekDayFromDate(DATE date) {

    if (!date || date > LIMIT_MAX_DATE) {  
        return 0;  // Error
    }

    SYSTEMTIME st;
    VariantTimeToSystemTime(date, &st);
    return (st.wDayOfWeek)*(st.wDayOfWeek);
}


/**
 * Returns true if input string is a day of week.
 */
bool isWeekDay(WCHAR* data) {

    bool ret = false;
    WCHAR* weekDay[] = {L"SU", L"MO", L"TU", L"WE", L"TH", L"FR", L"SA"};

    for(int i=0; i<7 ; i++) {
        if(!wcscmp(data, weekDay[i]))
            return true;
    }
    return ret;
}

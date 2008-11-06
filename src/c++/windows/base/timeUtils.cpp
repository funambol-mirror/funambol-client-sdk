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

#include "base/Log.h"
#include "base/timeUtils.h"
#include "vocl/constants.h"
#include <oleauto.h>
#include "base/globalsdef.h"

USE_NAMESPACE

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
        *date = REFERRED_MAX_DATE;
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


wstring getDateFromTzRule(const int year, SYSTEMTIME tzRule) {

    wstring out;
    SYSTEMTIME stDate;

    stDate.wYear         = year;
    stDate.wMonth        = tzRule.wMonth;
    stDate.wHour         = tzRule.wHour;
    stDate.wMinute       = tzRule.wMinute;
    stDate.wSecond       = tzRule.wSecond;
    stDate.wMilliseconds = tzRule.wMilliseconds;
    stDate.wDayOfWeek    = tzRule.wDayOfWeek;

    // Get the correct wDayOfWeek of the first day of month.
    stDate.wDay = 1;
    DATE firstDay = 0;
    SYSTEMTIME stFirstDay;
    SystemTimeToVariantTime(&stDate, &firstDay);
    VariantTimeToSystemTime(firstDay, &stFirstDay);

    // Offset > 0 between the first desired day, and the first day.
    // (e.g. if looking for wednesday and first day is monday, offset = +2)
    int offset = tzRule.wDayOfWeek - stFirstDay.wDayOfWeek;
    if (offset < 0) {
        offset += 7;
    }

    stDate.wDay = ((tzRule.wDay - 1) * 7) + 1;      // day is: [1, 8, 15, 22, 29]
    stDate.wDay += offset;

    // tzRule.wDay = 5 means 'the last'. Need safe checks.
    if (tzRule.wDay == 5) {
        if (stDate.wDay > 31) {
            // overflow
            stDate.wDay -= 7;
        }
        // If still after the last month, SystemTimeToVariantTime() will adjust to next month, the 1st
        DATE tmpDay = 0;
        SYSTEMTIME stTmpDay;
        SystemTimeToVariantTime(&stDate, &tmpDay);
        VariantTimeToSystemTime(tmpDay, &stTmpDay);
        if (stTmpDay.wMonth != stDate.wMonth) {
            // overflow
            stDate.wDay -= 7;
        }
    }

    // not necessary
    //stDate.wDayOfWeek = tzRule.wDayOfWeek;

    WCHAR date[20];
    wsprintf(date, TEXT("%i%02i%02iT%02i%02i%02i"), stDate.wYear, stDate.wMonth, stDate.wDay, 
                                                    stDate.wHour, stDate.wMinute, stDate.wSecond);
    out = date;
    return out;
}


SYSTEMTIME getTzRuleFromDates(list<wstring>& dates, bool *found) {

    SYSTEMTIME tzRule;
    SYSTEMTIME stDate;
    DATE date = NULL;
    list<wstring>::iterator it;

    if (dates.size() == 0) {
        goto error;
    }

    //
    // Get the SYSTEMTIME for the first date. 
    // All properties are retrieved from the first date.
    //
    it = dates.begin();
    stringTimeToDouble(*it, &date);
    VariantTimeToSystemTime(date, &stDate);

    // Fill tzRule structure.
    tzRule.wYear         = 0;                       // every year
    tzRule.wMonth        = stDate.wMonth;
    tzRule.wDayOfWeek    = stDate.wDayOfWeek;
    tzRule.wHour         = stDate.wHour;
    tzRule.wMinute       = stDate.wMinute;
    tzRule.wSecond       = stDate.wSecond;
    tzRule.wMilliseconds = stDate.wMilliseconds;

    tzRule.wDay = ((stDate.wDay - 1) / 7) + 1;      // # of week from day: [1-31] -> [1-5]

    if (tzRule.wDay != 4) {
        // we're sure it is correct, the first date is enough.
        *found = true;
        goto exit;
    }
    
    // modification: to handle properly the timezone structure on Windows desktop
    // if the tzRule.wDay is 4, we set the default to 5, that means the last week.
    // By the way, we try to see the other rules to understand if it really is the 5th
    *found = false;
    tzRule.wDay = 5;    

    
    //
    // If it's "the 4th week" (wDay=4), we have to check the other dates
    // because it could be "the last week" (wDay=5).
    //
    it++;
    while (it != dates.end()) {

        // Get the SYSTEMTIME for the current date.
        date = NULL;
        stringTimeToDouble(*it, &date);
        VariantTimeToSystemTime(date, &stDate);

        int week = ((stDate.wDay - 1) / 7) + 1;     // # of week from day: [1-31] -> [1-5]

        if (week == 5) {
            // This year it's the 5th week, so the rule is "the last week"!
            // we really found this is the 5th week
            *found = true;
            goto exit;
        }
        it++;
    }
    goto exit;
    

error:
    LOG.error("Error creating Timezone rule from list of DAYLIGHT dates.");

exit:
    return tzRule;
}



wstring formatBias(const int bias) {

    wstring out;
    WCHAR tmp[6];

    int hours   = bias / 60;
    int minutes = bias % 60;
    if (bias >= 0) { wsprintf(tmp, TEXT("-%02d%02d"), hours,   minutes); }
    else           { wsprintf(tmp, TEXT("+%02d%02d"), -hours, -minutes); }

    out = tmp;
    return out;
}


int parseBias(const WCHAR* data) {

    int bias    = 0;
    int hours   = 0;
    int minutes = 0;
    int len = wcslen(data);

    if (data && len>1) {
        int sign  = 1;              // The sign of the bias (default "+1" if not found).
        int start = 1;              // Starting position of the first digit.

        if (iswdigit(data[0])) { 
            start = 0;
            sign = 1;
        }
        else if (data[0] == '-') {
            start = 1;
            sign = -1;
        }

        if (len-start < 3) {
            // "+08"
            swscanf(&data[start], L"%2d", &hours);
        }
        else {
            // "+0800"
            swscanf(&data[start], L"%2d%2d", &hours, &minutes);
        }

        bias = ((hours*60) + minutes) * (-sign);    // e.g. "+0130" -> -90
    }

    return bias;
}


bool isSameTimezone(const TIME_ZONE_INFORMATION* tz1, const TIME_ZONE_INFORMATION* tz2) {

    if ( tz1->Bias         == tz2->Bias         &&
         tz1->DaylightBias == tz2->DaylightBias &&
         tz1->StandardBias == tz2->StandardBias &&
         isSameSystemtime(&tz1->DaylightDate, &tz2->DaylightDate) && 
         isSameSystemtime(&tz1->StandardDate, &tz2->StandardDate) ) {
        return true;
    }
    else {
        return false;
    }
}

bool isSameSystemtime(const SYSTEMTIME* st1, const SYSTEMTIME* st2) {

    if ( st1->wDay       == st2->wDay       &&
         st1->wDayOfWeek == st2->wDayOfWeek &&
         st1->wHour      == st2->wHour      &&
         st1->wMinute    == st2->wMinute    &&
         st1->wMonth     == st2->wMonth     &&
         st1->wYear      == st2->wYear ) {
        return true;
    }
    else {
        return false;
    }
}

bool hasDayLightSaving(const TIME_ZONE_INFORMATION* tz) {
    
    if (tz->DaylightDate.wYear   == 0  &&
        tz->DaylightDate.wMonth  == 0  &&
        tz->DaylightDate.wDay    == 0  && 
        tz->DaylightDate.wHour   == 0  &&
        tz->DaylightDate.wMinute == 0  &&        
        tz->StandardDate.wYear   == 0  &&
        tz->StandardDate.wMonth  == 0  &&
        tz->StandardDate.wDay    == 0  && 
        tz->StandardDate.wHour   == 0  &&
        tz->StandardDate.wMinute == 0) {
        
            return false;
    } else {
        return true;
    }

}
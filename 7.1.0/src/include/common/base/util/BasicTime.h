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
#ifndef INCL_BASIC_TIME
    #define INCL_BASIC_TIME
/** @cond DEV */

#include "base/fscapi.h"
#include "base/util/ArrayElement.h"
#include "base/globalsdef.h"

BEGIN_NAMESPACE

class BasicTime : public ArrayElement {

    // ------------------------------------------------------- Private data
    private:

        int year;
        int month;      // 1 - 12
        int day;        // 1 - 31
        int weekday;    // 0 - 7  (Sunday is 0 or 7)
        int hour;       // 0 - 23
        int min;        // 0 - 59
        int sec;        // 0 - 59

        int tzHour;
        int tzMin;

    public:

    // ------------------------------------------------------- Constructors
    BasicTime();

    // ---------------------------------------------------------- Accessors
    int getYear() const { return year; }
    void setYear(int v) { year=v; }

    int getMonth() const { return month; }
    void setMonth(int v) { month=v; }

    int getDay() const { return day; }
    void setDay(int v) { day=v; }

    int getWeekday() const { return weekday; }
    void setWeekday(int v) { weekday=v; }

    int getHour() const { return hour; }
    void setHour(int v) { hour=v; }

    int getMin() const { return min; }
    void setMin(int v) { min=v; }

    int getSec() const { return sec; }
    void setSec(int v) { sec=v; }

    int getTzHour() const { return tzHour; }
    void setTzHour(int v) { tzHour=v; }

    int getTzMin() const { return tzMin; }
    void setTzMin(int v) { tzMin=v; }

    // ----------------------------------------------------- Public Methods

    int set(int yy, int mon, int dd, int wd,
            int hh, int mm, int ss, int tzh, int tzm);

    int parseRfc822(const char *date);
    char *formatRfc822() const ;
    bool isADate(const char* date);
    ArrayElement *clone();

    BasicTime& operator=(const BasicTime& d);
    bool operator==(const BasicTime& d) const;
    bool operator!=(const BasicTime& d) const { return !(*this == d); }


};

END_NAMESPACE

/** @endcond */
#endif

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
#ifndef INCL_BASIC_TIME
    #define INCL_BASIC_TIME
/** @cond DEV */

#include "base/fscapi.h"
#include "base/util/ArrayElement.h"

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
/** @endcond */
#endif

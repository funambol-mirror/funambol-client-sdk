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

#ifndef INCL_WINRECURRENCE
#define INCL_WINRECURRENCE

/** @cond API */
/** @addtogroup win_adapter */
/** @{ */

#include "base/timeUtils.h"
#include "vocl/WinItem.h"
#include "base/globalsdef.h"

BEGIN_NAMESPACE


/**
 * Rapresents a recurrence pattern object for Windows Clients.
 * The object can be filled passing a vCalendar RRULE string, or filling
 * directly the map. Calling 'toString()' a vCalendar RRULE is formatted and returned.
 */
class WinRecurrence : public WinItem {

private:

    /// Internal string formatted (RRULE).
    wstring rrule;


protected:

    /// The start date (double format) of the correspondent event/task
    /// It's necessary to parse correctly the RRULE string
    DATE startDate;

    /// true if the parent calendar has a timezone information, and uses it.
    /// Recurring props are in local time if this flag is true.
    bool useTimezone;


public:

    /// Default Constructor
    WinRecurrence();

    /**
     * Constructor: fills propertyMap parsing the vCalendar RRULE string
     * @param dataString  input RRULE string to parse
     * @param date        start date (double format) of the correspondent event/task
     */
    WinRecurrence(const wstring & dataString, const DATE date = 0);

    /// Destructor
    virtual ~WinRecurrence();


    /**
     * Parse a vCalendar RRULE string and fills the propertyMap.
     * @param dataString   input RRULE string to parse
     * @return             0 if no errors
     */
    virtual int parse(const wstring & dataString);

    /// Format and return a vCalendar RRULE string from the propertyMap.
    virtual wstring toString();


    /// Returns the startDate (double format)
    const DATE getStartDate()          { return startDate;   }

    /// Sets the startDate (double format)
    void setStartDate(const DATE date) { startDate = date;   }

    /// Returns the 'useTimezone' flag value.
    const bool hasTimezone()           { return useTimezone; }

    /// Sets the 'useTimezone' flag to the given value.
    void setUseTimezone(bool val)      { useTimezone = val;  } 

    void trim(wstring& str);

};


END_NAMESPACE

/** @} */
/** @endcond */
#endif

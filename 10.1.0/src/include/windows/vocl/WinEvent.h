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

#ifndef INCL_WINEVENT
#define INCL_WINEVENT

/** @cond API */
/** @addtogroup win_adapter */
/** @{ */

#include "vocl/WinItem.h"
#include "vocl/WinRecurrence.h"
#include "vocl/WinRecipient.h"
#include "vocl/constants.h"
#include "vocl/VObject.h"

BEGIN_NAMESPACE

typedef list<wstring>               exceptionList;
typedef exceptionList::iterator     exceptionsIterator;
typedef list<WinRecipient>          recipientList;

#define MAX_DAYLIGHT_PROPS          6      // Max 6 "DAYLIGHT" properties for infinite recurrences.
#include "base/globalsdef.h"


/**
 * Rapresents an event object for Windows Clients.
 * The object can be filled passing a vCalendar, or filling
 * directly the map. Calling 'toString()' a vCalendar is formatted and returned.
 */
class WinEvent : public WinItem {

private:

    /// The recurrence pattern object, containing recurring properties.
    WinRecurrence recPattern;

    /**
     * Checks the productID and version of VObject passed for vCalendar.
     * - 'productID' MUST be "VCALENDAR"
     * - 'version' is only checked to be the one supported (log info if wrong)
     *
     * @param  vo         the VObject to check
     * @return            true if productID is correct
     */
    bool checkVCalendarTypeAndVersion(VObject* vo);

    /**
     * Adds the timezone properties (TZ and DAYLIGHT) into the passed VObject. 
     * Used from Client to Server. Example of formatted vProperties added:
     *     TZ:-0800
     *     DAYLIGHT:TRUE;-0900;20080406T020000;20081026T020000;Pacific Standard Time;Pacific Daylight Time
     *     DAYLIGHT:TRUE;-0900;20090405T020000;20091025T020000;Pacific Standard Time;Pacific Daylight Time
     * When using timezone properties, recurrence data is in local time.
     */
    void addTimezone(VObject* vo);

    /**
     * Parse the timezone properties (TZ and DAYLIGHT) from the passed VObject
     * and fills the 'tzInfo' timezone structure. Used from Server to Client.
     * When using timezone properties, recurrence data is expected in local time.
     * @return  true if timezone properties found
     */
    bool parseTimezone(VObject* vo);

protected:

    /// List of occurrences dates to exclude (recurring exceptions to delete).
    exceptionList excludeDate;
    /// List of occurrences dates to include (recurring exceptions to add).
    exceptionList includeDate;

    /// List of recipients (attendees) for this event.
    recipientList recipients;


    /**
     * The timezone structure for this event.
     * It can be set/get using getTimezone() and setTimezone() methods.
     * If the timezone is set, it will be formatted following vCalendar 1.0 specs
     * when calling toString() method ("TZ" and "DAYLIGHT" properties). 
     * @note Recurring properties will be in local time if timezone is used.
     */
    TIME_ZONE_INFORMATION tzInfo;

    /// true if this event has a timezone information, and uses it.
    bool useTimezone;

    //bool isRecurring;
    //bool isAllday;


    /**
     * Retrieves the interval when the recurrence occurrs, in years.
     * It checks the "Start" property and the "PatternEndDate" property.
     * @param yearBegin  [IN-OUT] the year when recurrence begins
     * @param yearEnd    [IN-OUT] the year when recurrence ends
     */
    void getIntervalOfRecurrence(int* yearBegin, int*yearEnd);


public:

    /// Default Constructor
    WinEvent();

    /// Constructor: fills propertyMap parsing the passed vCalendar string
    WinEvent(const wstring & dataString);

    /// Destructor
    ~WinEvent();


    /**
     * Parse a vCalendar string and fills the propertyMap.
     * The map is cleared and will contain only found properties
     * at the end of the parsing.
     * @param dataString  input vCalendar string to be parsed
     * @return            0 if no errors
     */
    virtual int parse(const wstring & dataString);

    /**
     * Format and return a vCalendar string from the propertyMap.
     * Not supported properties are ignored and so not formatted 
     * as they don't have a correspondence in propertyMap.
     * @return  the vCalendar string formatted
     */
    virtual wstring toString();

    /// Returns a pointer to the (internally owned) WinRecurrence.
    virtual WinRecurrence* getRecPattern();

    /// Returns a pointer to the list (internally owned) of exclude dates.
    exceptionList* getExcludeDates();

    /// Returns a pointer to the list (internally owned) of include dates.
    exceptionList* getIncludeDates();

    /// Returns a pointer to the list (internally owned) of recipients.
    recipientList* getRecipients();


    /**
     * Returns a pointer to the timezone information for this event, NULL if
     * timezone is not used (not set).
     */
    const TIME_ZONE_INFORMATION* getTimezone();

    /// Copies the given timezone for this event. 'useTimezone' is set to true.
    void setTimezone(const TIME_ZONE_INFORMATION* tz);

    /// Returns true if this event has a timezone information, and uses it.
    bool hasTimezone();


    /**
     * Return the crc value of the internal map with all values.
     * It uses only the values of the map not the key.
     * Overrides method of WinItem, to include recurring properties 
     * and event exceptions in the crc.
     */
    long getCRC();
};


END_NAMESPACE

/** @} */
/** @endcond */
#endif

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


/**
 * Rapresents an event object for Windows Clients.
 * The object can be filled passing a vCalendar, or filling
 * directly the map. Calling 'toString()' a vCalendar is formatted and returned.
 */
class WinEvent : public WinItem {

private:

    /// Internal string formatted (VCALENDAR).
    wstring vCalendar;

    /// The recurrence pattern object, containing recurring properties.
    WinRecurrence recPattern;

    /// List of occurrences dates to exclude (recurring exceptions to delete).
    list<wstring> excludeDate;
    /// List of occurrences dates to include (recurring exceptions to add).
    list<wstring> includeDate;

    /// List of recipients (attendees) for this event.
    list<WinRecipient> recipients;

    //bool isRecurring;
    //bool isAllday;


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
     * Utility to safe-retrieve the property value inside VObject 'vo'.
     * @param vo           : VObject to read from
     * @param propertyName : the property name requested
     * @return               the property value (NULL if not found)
     */
    WCHAR* getVObjectPropertyValue(VObject* vo, const WCHAR* propertyName);


public:

    /// Default Constructor
    WinEvent();

    /// Constructor: fills propertyMap parsing the passed vCalendar string
    WinEvent(const wstring dataString);

    /// Destructor
    ~WinEvent();


    /**
     * Parse a vCalendar string and fills the propertyMap.
     * The map is cleared and will contain only found properties
     * at the end of the parsing.
     * @param dataString  input vCalendar string to be parsed
     * @return            0 if no errors
     */
    int parse(const wstring dataString);

    /**
     * Format and return a vCalendar string from the propertyMap.
     * Not supported properties are ignored and so not formatted 
     * as they don't have a correspondence in propertyMap.
     * @return  the vCalendar string formatted, reference to internal wstring
     */
    wstring& toString();



    /// Returns a pointer to the (internally owned) WinRecurrence.
    WinRecurrence* getRecPattern();

    /// Returns a pointer to the list (internally owned) of exclude dates.
    list<wstring>* getExcludeDates();

    /// Returns a pointer to the list (internally owned) of include dates.
    list<wstring>* getIncludeDates();

    /// Returns a pointer to the list (internally owned) of recipients.
    list<WinRecipient>* getRecipients();
};

/** @} */
/** @endcond */
#endif

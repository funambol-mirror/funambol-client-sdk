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

#ifndef INCL_WINRECIPIENT
#define INCL_WINRECIPIENT

/** @cond API */
/** @addtogroup win_adapter */
/** @{ */

#include "vocl/WinItem.h"


/**
 * Rapresents a recipient object (attendee) for Windows Clients.
 * The object can be filled passing a vCalendar ATTENDEE string, or filling
 * directly the map. Calling 'toString()' a vCalendar ATTENDEE is formatted and returned.
 */
class WinRecipient : public WinItem {

public:

    /// Default Constructor
    WinRecipient();

    /// Constructor: fills propertyMap parsing the vCalendar ATTENDEE string
    WinRecipient(const wstring rrule);

    /// Destructor
    ~WinRecipient();


    /// Parse a vCalendar ATTENDEE string and fills the propertyMap.
    int parse(const wstring attendee);

    /// Format and return a vCalendar ATTENDEE string from the propertyMap.
    int toString(wstring& attendee);
};

/** @} */
/** @endcond */
#endif

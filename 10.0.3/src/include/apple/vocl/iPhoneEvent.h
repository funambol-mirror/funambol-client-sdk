/*
 * Funambol is a mobile platform developed by Funambol, Inc. 
 * Copyright (C) 2010 Funambol, Inc.
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

#ifndef INCL_IPHONE_EVENT
#define INCL_IPHONE_EVENT

/** @cond API */
/** @addtogroup apple_adapter */
/** @{ */

#include "vocl/AppleEvent.h"
#include "vocl/VObject.h"

BEGIN_FUNAMBOL_NAMESPACE

#define MAX_DAYLIGHT_PROPS          6      // Max 6 "DAYLIGHT" properties for infinite recurrences.
#include "base/globalsdef.h"


/**
 * Rapresents an event object for Apple Clients.
 */
class iPhoneEvent : public AppleEvent {
    
private:
    
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
    
    
    
public:
    
    /// Default Constructor
    iPhoneEvent();
    
    /// Constructor: fills propertyMap parsing the passed vCalendar string
    iPhoneEvent(const StringBuffer& dataString);
    
    /// Destructor
    ~iPhoneEvent();
    
    
    /**
     * Parse a vCalendar string and fills the propertyMap.
     * The map is cleared and will contain only found properties
     * at the end of the parsing.
     * @param dataString  input vCalendar string to be parsed
     * @return            0 if no errors
     */
    virtual int parse(const StringBuffer & dataString);
    
    /**
     * Format and return a vCalendar string from the propertyMap.
     * Not supported properties are ignored and so not formatted 
     * as they don't have a correspondence in propertyMap.
     * @return  the vCalendar string formatted
     */
    virtual StringBuffer toString();
    
    /**
     * Return the crc value of the internal map with all values.
     * It uses only the values of the map not the key.
     * Overrides method of WinItem, to include recurring properties 
     * and event exceptions in the crc.
     */
    long getCRC();
};

END_FUNAMBOL_NAMESPACE

/** @} */
/** @endcond */
#endif

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

#ifndef INCL_WINEVENT_SIF
#define INCL_WINEVENT_SIF

/** @cond API */
/** @addtogroup win_adapter */
/** @{ */

#include "vocl/VObject.h"
#include "vocl/WinItem.h"
#include "vocl/WinEvent.h"
#include "vocl/WinRecurrenceSIF.h"
#include "base/globalsdef.h"

BEGIN_NAMESPACE

using namespace std;


/**
 * Rapresents an event object for Windows Clients, for SIF format.
 * The object can be filled passing a SIF string, or filling
 * directly the map. Calling 'toString()' a SIF is formatted and returned.
 */
class WinEventSIF : public WinEvent {

private:

    /// NULL terminated array of SIF fields names.
    const wchar_t** sifFields;

    /// The recurrence pattern object for SIF data, containing recurring properties.
    WinRecurrenceSIF recPatternSIF;

    /// Parse appointment exceptions from SIF string, and fill exception lists.
    void parseExceptions(const wstring& sifString);

    /**
     * Adds the Timezone tag into the passed SIF string. 
     * Used from Client to Server. Example of formatted tags added:
     *   <Timezone>
     *      <BasicOffset>-0800</DSTOffset>
     *      <DayLight>
     *         <DSTOffset>-0900</DSTOffset>
     *         <DSTStart>20080406T020000</DSTStart>
     *         <DSTEnd>20081026T020000</DSTEnd>
     *         <StandardName>Pacific Standard Time</StandardName>
     *         <DSTName>Pacific Daylight Time</DSTName>
     *      </DayLight>
     *      [... more <DayLight> tags may follow ]
     *  </Timezone>
     *
     * When using timezone properties, recurrence data is in local time.
     */
    void addTimezone(wstring& sif);

    /**
     * Parse the timezone properties from the passed SIF data string
     * and fills the 'tzInfo' timezone structure. Used from Server to Client.
     * When using timezone properties, recurrence data is expected in local time.
     * @return  true if timezone properties found
     */
    bool parseTimezone(const wstring& data);

public:

    /// Default Constructor
    WinEventSIF();

    /**
     * Constructor: fills propertyMap parsing the passed SIF
     * @param dataString  the input SIF string
     * @param fields      the NULL terminated array of SIF fields
     * @param fields      the NULL terminated array of SIF fields for recurrence pattern
     */
    WinEventSIF(const wstring dataString, const wchar_t** fields, const wchar_t** recFields);

    /// Destructor
    ~WinEventSIF();  


    /// Returns a pointer to the (internally owned) WinRecurrenceSIF.
    /// @note: overrides the method of WinEvent, to return a SIF formatted string.
    WinRecurrenceSIF* getRecPattern();

    
     /**
     * Parse a SIF string and fills the propertyMap.
     * The map is cleared and will contain only found properties
     * at the end of the parsing.
     * @param dataString  input SIF string to be parsed
     * @return            0 if no errors
     */
    int parse(const wstring & dataString);
   
    /**
     * Format and return a SIF string from the propertyMap.
     * Not supported properties are ignored and so not formatted 
     * as they don't have a correspondence in propertyMap.
     * @return  the SIF string formatted
     */
    wstring toString();


    /**
    * Transform the value of the specified property according to the SIF specifications.
    * The values are formatted following the vcard and icalendar specs that in some cases
    * they are different from the SIF expectations. If there are no differences, propValue 
    * is returned.
    *
    * @param  propName   [IN] the property name
    * @param  propValue  [IN] the property value    
    * @return            the converted value if necessary
    */
    wstring adaptToSIFSpecs(const wstring& propName, const wstring& propValue);


    /**
     * Transform the value of the specified property found in SIF structure to the 
     * one expected by Outlook.
     * The values are formatted following the vcard and icalendar specs that in some cases
     * they are different from the SIF expectations. If there are no differences the same
     * value is returned.
     *
     * @param  propName   the property name
     * @param  propValue  the property value
     * @return            the converted value if necessary
     */
    wstring adaptFromSIFSpecs(const wstring& propName, const wstring& propValue);
    
};

END_NAMESPACE

/** @} */
/** @endcond */
#endif

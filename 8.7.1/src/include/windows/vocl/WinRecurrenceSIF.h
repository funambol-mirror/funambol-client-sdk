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

#ifndef INCL_WINRECURRENCE_SIF
#define INCL_WINRECURRENCE_SIF

/** @cond API */
/** @addtogroup win_adapter */
/** @{ */

#include "base/timeUtils.h"
#include "vocl/WinRecurrence.h"
#include "base/globalsdef.h"

BEGIN_NAMESPACE


/**
 * Rapresents a recurrence pattern object for Windows Clients, for SIF format.
 * The object can be filled passing a SIF rec pattern string, or filling
 * directly the map. Calling 'toString()' a SIF rec pattern string is formatted and returned.
 */
class WinRecurrenceSIF : public WinRecurrence {

private:

    /// NULL terminated array of SIF fields names.
    const wchar_t** sifFields;


public:

    /// Default Constructor
    WinRecurrenceSIF();

    /**
     * Constructor: fills propertyMap parsing the SIF rec pattern string
     * @param dataString   input SIF string to parse
     * @param fields      the NULL terminated array of SIF fields
     */
    WinRecurrenceSIF(const wstring & dataString, const wchar_t **fields);

    /// Destructor
    ~WinRecurrenceSIF();


    /// Sets the internal pointer sifFields with the passed array.
    void setSifFields(const wchar_t** fields);


    /**
     * Parse a SIF string and fills the propertyMap.
     * @param dataString   input SIF string to parse
     * @return             0 if no errors
     */
    int parse(const wstring & dataString);

    /// Format and return a SIF rec pattern string from the propertyMap.
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

};



END_NAMESPACE

/** @} */
/** @endcond */
#endif

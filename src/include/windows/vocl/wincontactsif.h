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

#ifndef INCL_WINCONTACT_SIF
#define INCL_WINCONTACT_SIF

/** @cond API */
/** @addtogroup win_adapter */
/** @{ */

#include "vocl/VObject.h"
#include "vocl/WinItem.h"
#include "vocl/WinContact.h"

using namespace std;


/**
 * Rapresents a contact object for Windows Clients.
 * The object can be filled passing a vCard, or filling
 * directly the map. Calling 'toString()' a vCard is formatted and returned.
 */
class WinContactSIF : public WinContact {

private:

    /// Internal string formatted (SIF).
    wstring sif;
    const wchar_t** sifFields;
   
public:

    /// Default Constructor
    WinContactSIF();
    /// Constructor: fills propertyMap parsing the passed vCard
    WinContactSIF(const wstring dataString, const wchar_t **fields);

    /// Destructor
    ~WinContactSIF();   
    
     /**
     * Parse a vCard string and fills the propertyMap.
     * The map is cleared and will contain only found properties
     * at the end of the parsing.
     * @param dataString  input vCard string to be parsed
     * @return            0 if no errors
     */
    int parse(const wstring dataString);
   
    /**
     * Format and return a vCard string from the propertyMap.
     * Not supported properties are ignored and so not formatted 
     * as they don't have a correspondence in propertyMap.
     * @return  the vCard string formatted, reference to internal wstring
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
    * @param  type       [IN] the type of the data (contact, calendar...)
    * @return            the converted value if necessary
    */
    wstring adaptToSpecsSIF(const wstring& propName, const wstring& propValue, const wstring& type);


    /**
    * Adds a tag <PropertyName>PropertyValue</PropertyName> into sifString.
    */
    void addPropertyToSIF(const wstring propertyName, wstring propertyValue, wstring& sif);

    /*
    * Trim the string
    */
    wstring trim(const wstring& str);

    /*
    * Format a date like yyyyMMdd in yyyy-MM-dd
    */
    wstring formatDateWithMinus(wstring stringDate);

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
/** @} */
/** @endcond */
#endif

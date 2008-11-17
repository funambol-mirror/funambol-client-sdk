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

#ifndef INCL_WINITEM
#define INCL_WINITEM

/** @cond API */
/** @addtogroup win_adapter */
/** @{ */


#include "base/fscapi.h"
#include "base/Log.h"
#include <string>
#include <list>
#include <map>

using namespace std;


// Versions supported:
#define VCARD_VERSION                       L"2.1"
#define VCALENDAR_VERSION                   L"1.0"

// Error messages:
#define ERR_ITEM_VOBJ_PARSE                 "VConverter: error occurred parsing the item data."
#define ERR_ITEM_VOBJ_WRONG_TYPE            "Error: wrong vobject type \"%ls\" (\"%ls\" expected)"
#define INFO_ITEM_VOBJ_WRONG_VERSION        "Warning! Wrong vobject version \"%ls\" (\"%ls\" expected)"


/**
 ****************************************************************************
 * Rapresents an item object for Windows Clients.
 * Contains a map of <propertyName,propertyValue> for all properties 
 * exchanged and methods to get/set them.
 ****************************************************************************
 */
class WinItem {

private:
    static wstring badString;


public:
    
    /**
     * Map <propertyName, propertyValue> of props exchanged.
     * - Client to Server: contains props supported by Client, should be filled
     *                     by Client calling setProperty() for each property.
     * - Server to Client: contains props parsed from vCard/vCalendar, it's automatically
     *                     filled by parsers of derived classes. Client should call getProperty()
     *                     for each property he wants to retrieve.
    */
    map<wstring,wstring> propertyMap;

    /// Default Constructor
    WinItem();

    /// Destructor
    ~WinItem();


    /// Returns the size of propertyMap;
    int getPropertyMapSize();


    /**
     * Sets a property value of name 'propertyName'.
     * Stores the value into the propertyMap, adds a new row <name, value> if the
     * property is not found, otherwise existing value is overwritten.
     * @param  propertyName   the name of property to set
     * @param  propertyValue  the value of property to set
     */
    void setProperty(const wstring propertyName, const wstring propertyValue);

    /**
     * Gets a property value from its name.
     * Retrieves the value from the propertyMap. If property is not
     * found, returns false.
     * @param  propertyName   the name of property to retrieve
     * @param  propertyValue  [IN-OUT] the value of property, it's set to empty string
     *                        if the property is not found
     * @return                true if property found, false if not found
     */
    bool getProperty(const wstring propertyName, wstring& propertyValue);

    /**
     * Gets a property value from its name.
     * Retrieves the value from the propertyMap. Returns a reference to the internal
     * value of property inside the map (value not copied).
     * @note  If property not found, returns a reference to the 
     *        'badString' static member of this class.
     * 
     * @param  propertyName   the name of property to retrieve
     * @param  propertyValue  [IN-OUT] true if property found, false if not found
     * @return                the value of property found, by reference
     */
    wstring& getPropertyRef(const wstring propertyName, bool* found);

    void removeElement(wstring key);

    /// Reset the propertyMap (clear all rows).
    void resetPropertyMap();

    /// Reset all fields values of the propertyMap (only values).
    void resetAllValues();

    /**
    * Return the crc value of the internal map with all values.
    * It uses only the values of the map not the key
    */
    long getCRC();
};

/** @} */
/** @endcond */
#endif

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

#ifndef INCL_WINITEM
#define INCL_WINITEM

/** @cond API */
/** @addtogroup win_adapter */
/** @{ */


#include "base/fscapi.h"
#include "base/Log.h"
#include "vocl/VObject.h"
#include <string>
#include <list>
#include <map>
#include <vector>

using namespace std;


// Versions supported:
#define VCARD_VERSION                       TEXT("2.1")
#define VCALENDAR_VERSION                   TEXT("1.0")
#define VNOTE_VERSION                       TEXT("1.1")
#define SIF_VERSION                         TEXT("1.1")


// Error messages:
#define ERR_ITEM_VOBJ_PARSE                 "VConverter: error occurred parsing the item data."
#define ERR_ITEM_VOBJ_WRONG_TYPE            "Error: wrong vobject type \"%ls\" (\"%ls\" expected)"
#define ERR_ITEM_VOBJ_TYPE_NOTFOUND         "Error: vobject type not specified (\"%ls\" expected)"
#define ERR_SIFFIELDS_NULL                  "Parsing error: sifFields must be initialized before parsing data."
#define INFO_ITEM_VOBJ_WRONG_VERSION        "Warning! Wrong vobject version \"%ls\" (\"%ls\" expected)"
#define INFO_ITEM_VOBJ_VERSION_NOTFOUND     "Warning! VObject version not specified (\"%ls\" expected)"
#include "base/globalsdef.h"

BEGIN_NAMESPACE


typedef map<wstring,wstring>::iterator      mapIterator;


/**
 ****************************************************************************
 * Rapresents an item object for Windows Clients.
 * Contains a map of <propertyName,propertyValue> for all properties 
 * exchanged and methods to get/set them.
 ****************************************************************************
 */
class WinItem {

private:

    /// A dummy bad string "<NULL>", used by methods that return a reference
    /// to an internal property value, when the property is missing.
    static wstring badString;

    static bool (*defaultValidateExtraProperty) (const std::wstring &);

    bool (*validateExtraProperty) (const std::wstring &);

protected:

    /// The table used to calculate the crc.
    static unsigned long crc32Table[256];


public:
    
    /**
     * Map <propertyName, propertyValue> of props exchanged.
     * - Client to Server: contains props supported by Client, MUST be filled by Client 
     *                     calling setProperty() for each property, before calling toString().
     * - Server to Client: contains props parsed from vCard/vCalendar, it's automatically
     *                     filled by parsers of derived classes. Client SHOULD call getProperty()
     *                     for each property he wants to retrieve, after calling parse().
    */
    map<wstring,wstring> propertyMap;

    map<wstring,wstring> extraPropertyMap;

    /// Default Constructor
    WinItem();

    /// Destructor
    virtual ~WinItem();

    /// Returns the size of propertyMap;
    int getPropertyMapSize();

    /// Returns a reference to the internal propertyMap.
    map<wstring,wstring>& getPropertyMap();


    /**
     * Sets a property value of name 'propertyName'.
     * Stores the value into the propertyMap, adds a new row <name, value> if the
     * property is not found, otherwise existing value is overwritten.
     * @param  propertyName   the name of property to set
     * @param  propertyValue  the value of property to set
     */
    void setProperty(const wstring propertyName, const wstring propertyValue);

    /**
     * Sets a integer property value of name 'propertyName'.
     * Like 'setProperty', but the input value is integer and it's formatted
     * into a string.
     * @param  propertyName   the name of property to set
     * @param  propertyValue  the integer value of property to set
     */
    void setIntProperty(const wstring propertyName, const int intValue);


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

    /**
     * Gets a property value from its name, and convert into a integer value.
     * Retrieves the value from the propertyMap. If property is not
     * found or not correctly converted into int, returns false.
     * @param  propertyName   the name of property to retrieve
     * @param  propertyValue  [IN-OUT] the value of property, integer value
     * @return                true if property found, false if not found or wrong format
     */
    bool getIntProperty(const wstring propertyName, int* intValue);


    void removeElement(wstring key);

    /// Reset the propertyMap (clear all rows).
    void resetPropertyMap();

    /// Reset all fields values of the propertyMap (only values).
    void resetAllValues();


    /**
     * Format and return a string from the propertyMap.
     * Not supported properties are ignored and so not formatted 
     * as they don't have a correspondence in propertyMap.
     * @return  the string formatted, reference to internal wstring
     */
    virtual wstring toString() = 0;

    /**
     * Parse a string and fills the propertyMap.
     * The map is cleared and will contain only found properties
     * at the end of the parsing.
     * @param dataString  input vCard string to be parsed
     * @return            0 if no errors
     */
    virtual int parse(const wstring & dataString) = 0;


    /**
    * Return the crc value of the internal map with all values.
    * It uses only the values of the map not the key.
    * Can be overrided by derived classes if other properties are involved
    * (e.g. Events have recucurrence props and exceptions)
    */
    virtual long getCRC();

    /**
     * Utility to safe-retrieve the property value inside VObject 'vo'.
     * @param vo           : VObject to read from
     * @param propertyName : the property name requested
     * @return               the property value (NULL if not found)
     */
    WCHAR* getVObjectPropertyValue(VObject* vo, const WCHAR* propertyName);


    /**
     * Util function that returns the name of the item, usually the "Subject" property.
     * It should be implemented by derived classes to return a desired value.
     * If property doesn't exist, returns "<NULL>"
     */
    virtual wstring& getName();

    void setExtraProperty(const wstring propertyName, const wstring propertyValue);
    bool getExtraProperty(const wstring propertyName, wstring& propertyValue);
    vector<wstring> getExtraPropertyNames();
    void addExtraProperties(VObject * vo);

    bool validExtraProperty(const std::wstring & name);

    void setValidateFunction(bool (*validate)(const std::wstring &));
    static void setDefaultValidateFunction(bool (*default) (const std::wstring &));
};


END_NAMESPACE

/** @} */
/** @endcond */
#endif

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

#ifndef INCL_WINCONTACT
#define INCL_WINCONTACT

/** @cond API */
/** @addtogroup win_adapter */
/** @{ */

#include "vocl/VObject.h"
#include "vocl/WinItem.h"

using namespace std;


/**
 * Rapresents a contact object for Windows Clients.
 * The object can be filled passing a vCard, or filling
 * directly the map. Calling 'toString()' a vCard is formatted and returned.
 */
class WinContact : public WinItem {

private:

    /// Internal string formatted (VCARD).
    wstring vCard;

    /**
     * Checks the productID and version of VObject passed for vCard.
     * - 'productID' MUST be "VCARD"
     * - 'version' is only checked to be the one supported (log info if wrong)
     *
     * @param  vo         the VObject to check
     * @return            true if productID is correct
     */
    bool checkVCardTypeAndVersion(VObject* vo);


public:

    /// Default Constructor
    WinContact();
    /// Constructor: fills propertyMap parsing the passed vCard
    WinContact(const wstring dataString);

    /// Destructor
    ~WinContact();


    /**
     * Parse a vCard string and fills the propertyMap.
     * The map is cleared and will contain only found properties
     * at the end of the parsing.
     * @param dataString  input vCard string to be parsed
     * @return            0 if no errors
     */
    virtual int parse(const wstring dataString);
   
    /**
     * Format and return a vCard string from the propertyMap.
     * Not supported properties are ignored and so not formatted 
     * as they don't have a correspondence in propertyMap.
     * @return  the vCard string formatted, reference to internal wstring
     */
    virtual wstring toString();     
    
};
/** @} */
/** @endcond */
#endif

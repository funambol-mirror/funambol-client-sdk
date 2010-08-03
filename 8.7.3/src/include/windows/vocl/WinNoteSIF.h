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

#ifndef INCL_WINNOTE_SIF
#define INCL_WINNOTE_SIF

/** @cond API */
/** @addtogroup win_adapter */
/** @{ */

#include "vocl/VObject.h"
#include "vocl/WinItem.h"
#include "vocl/WinNote.h"
#include "base/globalsdef.h"

BEGIN_NAMESPACE

using namespace std;


/**
 * Rapresents a note object for Windows Clients, for SIF format.
 * The object can be filled passing a SIF string, or filling
 * directly the map. Calling 'toString()' a SIF is formatted and returned.
 */
class WinNoteSIF : public WinNote {

private:

    /// NULL terminated array of SIF fields names.
    const wchar_t** sifFields;


public:

    /// Default Constructor
    WinNoteSIF();

    /**
     * Constructor: fills propertyMap parsing the passed SIF
     * @param dataString  the input SIF string
     * @param fields      the NULL terminated array of SIF fields
     * @param fields      the NULL terminated array of SIF fields for recurrence pattern
     */
    WinNoteSIF(const wstring dataString, const wchar_t** fields);

    /// Destructor
    ~WinNoteSIF();

    
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
};


END_NAMESPACE

/** @} */
/** @endcond */
#endif

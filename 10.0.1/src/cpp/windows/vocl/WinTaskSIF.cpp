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

#include "vocl/WinTaskSIF.h"
#include "vocl/VConverter.h"
#include "vocl/constants.h"
#include "base/stringUtils.h"
#include "vocl/sifUtils.h"
#include "base/globalsdef.h"

USE_NAMESPACE

using namespace std;


// Constructor
WinTaskSIF::WinTaskSIF() {
    sifFields = NULL;
}

// Constructor: fills propertyMap parsing the passed SIF string
WinTaskSIF::WinTaskSIF(const wstring& dataString, const wchar_t** fields, const wchar_t** recFields) {
    sifFields = fields;
    recPatternSIF.setSifFields(recFields);

    parse(dataString);
}

// Destructor
WinTaskSIF::~WinTaskSIF() {
}

wstring WinTaskSIF::toString() {
    
    wstring propertyValue;
    wstring sif;
    sif = L"<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n";   
    sif += L"<task>\n";

    sif += L"<SIFVersion>";
    sif += SIF_VERSION;
    sif += L"</SIFVersion>\n";

    //
    // Add all task properties
    //
    map<wstring,wstring>::iterator it = propertyMap.begin();       
    while (it != propertyMap.end()) {        
        propertyValue = it->second;      
        propertyValue = adaptToSIFSpecs(it->first, propertyValue);
        addPropertyToSIF(sif, it->first, propertyValue);
        it ++;
    }

    //
    // Append recurrence properties
    //
    wstring element;
    if (getProperty(L"IsRecurring", element)) {
        wstring recurrenceSIF = recPatternSIF.toString();
        if(recurrenceSIF != L"") {
            sif += recurrenceSIF;
        }
    }

    sif += L"</task>";

    return sif;
}



int WinTaskSIF::parse(const wstring &data) {
    
    if (!sifFields) {
        LOG.error("%s", ERR_SIFFIELDS_NULL);
        return 1;
    }
    propertyMap.clear();
    
    // Check if <task> tag is present...
    wstring::size_type pos = 0;
    wstring itemTypeTag = L"<task>";    
    pos = data.find(itemTypeTag, 0);
    if (pos == wstring::npos) {
        LOG.error("Tag '%ls' not found", itemTypeTag.c_str());
        return 1;
    }
    wstring propertyValue;

    //
    // Set task properties
    //
    for (int i=0; sifFields[i]; i++) {
        // Set only properties found!
        if (!getElementContent(data, sifFields[i], propertyValue, 0)) {

            replaceAll(L"&lt;",  L"<", propertyValue);
            replaceAll(L"&gt;",  L">", propertyValue);
            replaceAll(L"&amp;", L"&", propertyValue);
            
            setProperty(sifFields[i], propertyValue);
        }
    } 

    //
    // Set recurrence properties
    //
    wstring element;
    if (getProperty(L"IsRecurring", element)) {
        bool isRec = (element != TEXT("0"));
        if(isRec) {
            // Fill recPatternSIF propertyMap.
            recPatternSIF.parse(data);
        }
    }

    return 0;
}


WinRecurrenceSIF* WinTaskSIF::getRecPattern() {
    return &recPatternSIF;
}



wstring WinTaskSIF::adaptToSIFSpecs(const wstring& propName, const wstring& propValue) {
    
    wstring propertyValue = L"";

    if (propValue.length() == 8) {
        if ( propName == L"DateCompleted" || 
             propName == L"DueDate"       ||
             propName == L"StartDate" ) {
            propertyValue = formatDateWithMinus(propValue);    
        }
    }

    if (propertyValue != L"") {
        return propertyValue;
    } 
    return propValue;
}


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

#include "vocl/WinContactSIF.h"
#include "vocl/VConverter.h"
#include "vocl/constants.h"
#include "base/stringUtils.h"
#include "vocl/sifUtils.h"
#include "base/globalsdef.h"

USE_NAMESPACE

using namespace std;


// Constructor
WinContactSIF::WinContactSIF() {
    sifFields = NULL;
    sif = L"";
}

// Constructor: fills propertyMap parsing the passed SIF string
WinContactSIF::WinContactSIF(const wstring dataString, const wchar_t **fields) {
    sif = L"";
    sifFields = fields;
    parse(dataString);
}

// Destructor
WinContactSIF::~WinContactSIF() {
}

wstring& WinContactSIF::toString() {
    
    wstring propertyValue, propertyKey;
    sif = L"<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n";   
    sif += L"<contact>\n";

    sif += L"<SIFVersion>";
    sif += SIF_VERSION;
    sif += L"</SIFVersion>\n";

    map<wstring,wstring>::iterator it = propertyMap.begin();       
    while (it != propertyMap.end()) {        
        propertyValue = it->second;      
        propertyValue = adaptToSIFSpecs(it->first, propertyValue);
        if ((it->first) == L"Photo") {
            addPhotoToSIF(propertyValue);       // To manage the TYPE attribute
        }
        else {
            addPropertyToSIF(sif, it->first, propertyValue);
        }
        it ++;
    }
    sif += L"</contact>";

    return sif;
}



void WinContactSIF::addPhotoToSIF(wstring propertyValue) {

    if (photoType.length() == 0) {
        // Type not specified
        addPropertyToSIF(sif, L"Photo", propertyValue);
        return;
    }

    if (propertyValue != L"") {
        sif += L"<Photo TYPE=\"";
        sif += photoType;
        sif += L"\">";
        sif += propertyValue;
        sif += L"</Photo>\n";
    }
    else {
        sif += L"<Photo/>\n";
    }
}



wstring WinContactSIF::adaptToSIFSpecs(const wstring& propName, const wstring& propValue) {
    
    wstring propertyValue = L"";

    if ((propName == L"Anniversary" || propName == L"Birthday") && propValue != L"") {
       propertyValue = formatDateWithMinus(propValue);    
    } 
    else if (propName == L"Photo" && propValue != L"") {
        // the picture is right for vcard: for windows we have to format better the sif
        // even if it should work anyway.
        //<Photo> /9j/4AAQSkZJRgABAQEAcwBzAAD/2wBDAAgGBgcGBQgHBwcJCQgKDBQNDAsLDBkSEw8UHRof
        //        Hh0aHBwgJC4nICIsIxwcKDcpLDAxNDQ0Hyc5PTgyPC4zNDL/2wBDAQkJCQwLDBgNDRgyIRwh   
        //        MDyN8gIU9aKyV+o9D//Z
        //</Photo>
        propertyValue = trim(propValue);
    } 

    if (propertyValue != L"") {
        return propertyValue;
    } 
    return propValue;

}

int WinContactSIF::parse(const wstring data) {

    if (!sifFields) {
        LOG.error("%s", ERR_SIFFIELDS_NULL);
        return 1;
    }
    propertyMap.clear();

    // Check if <contact> tag is present...
    wstring::size_type pos = 0;
    wstring itemTypeTag = L"<contact>";    
    pos = data.find(itemTypeTag, 0);
    if (pos == wstring::npos) {
        LOG.error("Tag '%ls' not found", itemTypeTag.c_str());
        return 1;
    }
    wstring propertyValue;
       
    for (int i=0; sifFields[i]; i++) {
        // Set only properties found!
        if (!getElementContent(data, sifFields[i], propertyValue, 0)) {

            replaceAll(L"&lt;",  L"<", propertyValue);
            replaceAll(L"&gt;",  L">", propertyValue);
            replaceAll(L"&amp;", L"&", propertyValue);
            
            setProperty(sifFields[i], propertyValue);
        }
    }   
    return 0;
}


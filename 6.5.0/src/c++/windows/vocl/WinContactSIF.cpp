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

#include "vocl/WinContactSIF.h"
#include "vocl/VConverter.h"
#include "vocl/constants.h"
#include "base/stringUtils.h"
using namespace std;


// Constructor
WinContactSIF::WinContactSIF() {
    sif = L"";
}

// Constructor: fills propertyMap parsing the passed vCard string
WinContactSIF::WinContactSIF(const wstring dataString, const wchar_t **fields) {
    sif = L"";
    sifFields = fields;
    parse(dataString);
}

// Destructor
WinContactSIF::~WinContactSIF() {
}

wstring WinContactSIF::toString() {
    
    wstring propertyValue, propertyKey;
    sif = L"<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n";   
    sif += L"<contact>\n";

    map<wstring,wstring>::iterator it = propertyMap.begin();       
    while (it != propertyMap.end()) {        
        propertyValue = it->second;      
        propertyValue = adaptToSIFSpecs(it->first, propertyValue); 
        addPropertyToSIF(it->first, propertyValue, sif);                
        it ++;
    }
    sif += L"</contact>";

    return sif;
}




void WinContactSIF::addPropertyToSIF(const wstring propertyName, wstring propertyValue, wstring& sif) {

    if (propertyValue != L"") {

        replaceAll(L"&", L"&amp;", propertyValue);
        replaceAll(L"<", L"&lt;",  propertyValue);
        replaceAll(L">", L"&gt;",  propertyValue);

        sif += L"<";
        sif += propertyName;
        sif += L">";
        sif += propertyValue;
        sif += L"</";
        sif += propertyName;
        sif += L">\n";
    }
    else {
        sif += L"<";
        sif += propertyName;
        sif += L"/>\n";
    }
}


wstring WinContactSIF::trim(const wstring& str) {
    wstring ret = str;
    int idx = 0;
    while((idx=ret.find_first_of(' ')) == 0 ) {
        ret.replace( idx, 1, L"" );            
    }
    while((idx=ret.find_last_of(' ')) == ret.size()-1 ) {
        ret.replace( idx, 1, L"" );            
    }
    return ret;
}

wstring WinContactSIF::formatDateWithMinus(wstring stringDate) {
    
    wstring ret;
    ret = stringDate.substr(0, 4);
    ret += L"-";
    ret += stringDate.substr(4, 2);
    ret += L"-";
    ret += stringDate.substr(6, 2);    
    return ret;
}

wstring WinContactSIF::adaptToSIFSpecs(const wstring& propName, const wstring& propValue) {
    
    wstring propertyValue = L"";

    if ((propName == L"Anniversary" || propName == L"Birthday") && propValue != L"") {
       propertyValue = formatDateWithMinus(propValue);    
    } else if (propName == L"Picture" && propValue != L"") {
        // the picture is right for vcard: for windows we have to format better the sif
        // even if it should work anyway.
        //<Picture>    /9j/4AAQSkZJRgABAQEAcwBzAAD/2wBDAAgGBgcGBQgHBwcJCQgKDBQNDAsLDBkSEw8UHRof
        //        Hh0aHBwgJC4nICIsIxwcKDcpLDAxNDQ0Hyc5PTgyPC4zNDL/2wBDAQkJCQwLDBgNDRgyIRwh   
        //        MDyN8gIU9aKyV+o9D//Z
        //        </Picture>
        propertyValue = trim(propValue);
        
    } 

    if (propertyValue != L"") {
        return propertyValue;
    } 
    return propValue;

}

int WinContactSIF::parse(const wstring data) {
    
    propertyMap.clear();
    // Check if <itemType> tag is present...

    wstring::size_type pos = 0;
    wstring itemTypeTag = L"<contact>";    
    pos = data.find(itemTypeTag, 0);
    if (pos == wstring::npos) {
        LOG.error("Property not found", itemTypeTag.c_str());
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


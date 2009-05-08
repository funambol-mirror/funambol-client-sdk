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

#include "base/fscapi.h"
#include "vocl/vCard/vCardConverter.h"
#include "base/util/WString.h"
#include "base/globalsdef.h"

USE_NAMESPACE

vCardConverter::vCardConverter() {
    vCard = NULL;
    contact = NULL;
}

vCardConverter::~vCardConverter() {
    if (vCard) {
        delete [] vCard; vCard = NULL;
    }
    if (contact) {
        delete contact; contact = NULL;
    }
}

void vCardConverter::setSource(WCHAR* inputCard) {
    if(vCard) {
        delete[] vCard;
        vCard = NULL;
    }
    if(inputCard) {
        vCard = new WCHAR[wcslen(inputCard) + 1];
        wcscpy(vCard, inputCard);
    }
    if(contact) {
        delete contact; contact = NULL;
    }
}

void vCardConverter::setSource(Contact& inputContact) {
    if(contact)
        delete contact;

    contact = inputContact.clone();

    if(vCard) {
        delete [] vCard; vCard = NULL;
    }
}

void vCardConverter::getvCard(WCHAR* outputCard) {
    if(vCard) {
        if(!outputCard)
            return;
        wcscpy(outputCard, vCard);
    }
    else
        outputCard = NULL;
}

void vCardConverter::getContact(Contact** outputContact) {
    if(contact) {
        *outputContact = contact->clone();
    }
    else
        *outputContact = NULL;
}

bool vCardConverter::convert(WString& errorDescription, long* errorCode) {
    errorDescription = TEXT("");
    *errorCode = ERROR_SUCCESS;

    if(!contact && !vCard)
        return false;
    if(contact && vCard)
        return true;
    if(contact) {
        vCard = contact->toString();
        if (!validate((VObject*) contact, errorDescription, errorCode)) {
            delete [] vCard; vCard = NULL;
        }
    }
    if (vCard) {
        WCHAR* vCardCopy = new WCHAR[wcslen(vCard) + 1];
        vCardCopy =  vCard;
        Contact* invalidatedContact = (Contact*)VConverter::parse(vCardCopy);
        if (invalidatedContact && validate((VObject*)invalidatedContact, errorDescription, errorCode))
            contact = invalidatedContact->clone();
        else
            return false;
        delete invalidatedContact; invalidatedContact = NULL;
    }

   return true;
}

bool vCardConverter::validate(VObject* vObject, WString& errorDescription, long* errorCode) {

    //validate begin, end, N and FN properties
    if(wcscmp(vObject->getProperty(0)->getName(), TEXT("BEGIN")) ||
        wcscmp(vObject->getProperty(0)->getValue(), TEXT("VCARD"))) {
        *errorCode = ERROR_KEY_PROPERTY_MISSING;
        errorDescription =  TEXT("'BEGIN' property is missing");
        return false;
        }
    if(wcscmp(vObject->getProperty(vObject->propertiesCount()-1)->getName(), TEXT("END")) ||
        wcscmp(vObject->getProperty(vObject->propertiesCount()-1)->getValue(), TEXT("VCARD"))) {
        *errorCode = ERROR_KEY_PROPERTY_MISSING;
        errorDescription =  TEXT("'END' property is missing");
        return false;
        }
    if(!vObject->containsProperty(TEXT("N"))) {
        *errorCode = ERROR_KEY_PROPERTY_MISSING;
        errorDescription =  TEXT("'N' property is missing");
        return false;
    }
    if(!vObject->containsProperty(TEXT("FN"))) {
        *errorCode = ERROR_KEY_PROPERTY_MISSING;
        errorDescription =  TEXT("'FN' property is missing");
        return false;
    }

    //validate the version and then each property according with version
    if(vObject->containsProperty(TEXT("VERSION"))) {
        if(!vObject->getProperty(TEXT("VERSION"))->getValue()) {
            *errorCode = ERROR_ILLEGAL_VERSION_NUMBER;
            errorDescription =  TEXT("Version number is missing");
            return false;
        }
        if(!wcscmp(vObject->getProperty(TEXT("VERSION"))->getValue(), TEXT("2.1")))
            for(int i = 0; i < vObject->propertiesCount(); i++) {
                if(!validateProperty21(vObject->getProperty(i), errorDescription, errorCode))
                    return false;
            }
        else if(!wcscmp(vObject->getProperty(TEXT("VERSION"))->getValue(), TEXT("3.0"))) {
            for(int i = 0; i < vObject->propertiesCount(); i++) {
                if(!validateProperty30(vObject->getProperty(i), errorDescription, errorCode))
                    return false;
            }
        }
        //VERSION property is neither 2.1 nor 3.0
        else {
            *errorCode = ERROR_ILLEGAL_VERSION_NUMBER;
            errorDescription = TEXT("Illegal version number : %s");
            errorDescription += vObject->getProperty(TEXT("VERSION"))->getValue();
            return false;
        }
    }
    //Version property is missing
    else {
        *errorCode = ERROR_KEY_PROPERTY_MISSING;
        errorDescription =  TEXT("'VERSION' property is missing");
        return false;
    }

    return true;
}

bool vCardConverter::validateProperty21(VProperty* prop, WString& errorDescription, long* errorCode) {

    if(!wcsstr(VCARD21_PROPERTIES_LIST, prop->getName()) &&
        wcsstr(prop->getName(),TEXT("X-")) != prop->getName()) {
        *errorCode = ERROR_ILLEGAL_PROPERTY_NAME;
        errorDescription = TEXT("Illegal property name : %s");
        errorDescription += prop->getName();
        return false;
    }

    //The default encoding for the vCard object is 7-Bit.
    //It can be overridden for an individual property value by using the “ENCODING” property parameter.
    //This parameter value can be either “BASE64”, “QUOTED-PRINTABLE”, or “8BIT”.

    if(prop->containsParameter(TEXT("ENCODING")) && prop->getParameterValue(TEXT("ENCODING")) &&
        wcscmp(prop->getParameterValue(TEXT("ENCODING")), TEXT("BASE64")) &&
        wcscmp(prop->getParameterValue(TEXT("ENCODING")), TEXT("QUOTED-PRINTABLE")) &&
        wcscmp(prop->getParameterValue(TEXT("ENCODING")), TEXT("8BIT"))) {
        *errorCode = ERROR_UNSUPPORTED_ENCODING;
        errorDescription = TEXT("Unsupported property encoding : %s");
        errorDescription += prop->getParameterValue(TEXT("ENCODING"));
        return false;
    }

    //photo property type
    if(!wcscmp(prop->getName(), TEXT("PHOTO")) && prop->containsParameter(TEXT("TYPE")))
        if(prop->getParameterValue(TEXT("TYPE")) && !wcsstr(PHOTO21_TYPES, prop->getParameterValue(TEXT("TYPE")))) {
            *errorCode = ERROR_ILLEGAL_TYPE_PARAMETER;
            errorDescription = TEXT("Illegal PHOTO type parameter : %s");
            errorDescription += prop->getParameterValue(TEXT("TYPE"));
            return false;
        }

    //logo property type
    if(!wcscmp(prop->getName(), TEXT("LOGO")) && prop->containsParameter(TEXT("TYPE")))
        if(prop->getParameterValue(TEXT("TYPE")) && !wcsstr(PHOTO21_TYPES, prop->getParameterValue(TEXT("TYPE")))) {
            *errorCode = ERROR_ILLEGAL_TYPE_PARAMETER;
            errorDescription = TEXT("Illegal LOGO type parameter : %s");
            errorDescription += prop->getParameterValue(TEXT("TYPE"));
            return false;
        }

    //address and label properties
    if(!wcscmp(prop->getName(), TEXT("ADR")) || !wcscmp(prop->getName(), TEXT("LABEL"))) {
        WCHAR *addressParameterList = new WCHAR[wcslen(PARAMTER21_LIST) + wcslen(ADDRESS21_TYPES) + 1];
        wcscpy(addressParameterList, PARAMTER21_LIST);
        wcscat(addressParameterList, ADDRESS21_TYPES);
        for(int i = 0; i < prop->parameterCount(); i++)
            if(!wcsstr(addressParameterList,prop->getParameter(i)) &&
                wcsstr(prop->getParameter(i),TEXT("X-")) != prop->getParameter(i)) {
                delete [] addressParameterList; addressParameterList = NULL;
                *errorCode = ERROR_ILLEGAL_PARAMETER;
                errorDescription = TEXT("Illegal ADDRESS parameter : %s");
                errorDescription += prop->getParameter(i);
                return false;
            }
        delete [] addressParameterList; addressParameterList = NULL;
    }

    //telephone number
    if(!wcscmp(prop->getName(), TEXT("TEL"))) {
        WCHAR *telParameterList = new WCHAR[wcslen(PARAMTER21_LIST) + wcslen(TEL21_TYPES) + 1];
        wcscpy(telParameterList, PARAMTER21_LIST);
        wcscat(telParameterList, TEL21_TYPES);
        for(int i = 0; i < prop->parameterCount(); i++)
            if(!wcsstr(telParameterList,prop->getParameter(i)) &&
                wcsstr(prop->getParameter(i),TEXT("X-")) != prop->getParameter(i)) {
                delete [] telParameterList; telParameterList = NULL;
                *errorCode = ERROR_ILLEGAL_PARAMETER;
                errorDescription = TEXT("Illegal TEL parameter : %s");
                errorDescription += prop->getParameter(i);
                return false;
            }
        delete [] telParameterList; telParameterList = NULL;
    }

    //email property
    if(!wcscmp(prop->getName(), TEXT("EMAIL")) && prop->containsParameter(TEXT("TYPE")) &&
        !wcsstr(EMAIL21_TYPES, prop->getParameterValue(TEXT("TYPE")))) {
        *errorCode = ERROR_ILLEGAL_TYPE_PARAMETER;
        errorDescription = TEXT("Illegal EMAIL type parameter : %s");
        errorDescription += prop->getParameterValue(TEXT("TYPE"));
        return false;
    }

    //sound property
    if(!wcscmp(prop->getName(), TEXT("SOUND")) && prop->containsParameter(TEXT("TYPE")) &&
        !wcsstr(SOUND21_TYPES, prop->getParameterValue(TEXT("TYPE")))) {
        *errorCode = ERROR_ILLEGAL_TYPE_PARAMETER;
        errorDescription = TEXT("Illegal SOUND type parameter : %s");
        errorDescription += prop->getParameterValue(TEXT("TYPE"));
        return false;
    }

    //key property
    if(!wcscmp(prop->getName(), TEXT("KEY")) && prop->containsParameter(TEXT("TYPE")) &&
        !wcsstr(KEY_TYPES, prop->getParameterValue(TEXT("TYPE")))) {
        *errorCode = ERROR_ILLEGAL_TYPE_PARAMETER;
        errorDescription = TEXT("Illegal KEY type parameter : %s");
        errorDescription += prop->getParameterValue(TEXT("TYPE"));
        return false;
    }

    //GEO property (must be in the format LAT;LON)
    if(!wcscmp(prop->getName(), TEXT("GEO")) && prop->getValue())
         if(!validateGeo(prop->getValue())) {
            *errorCode = ERROR_INVALID_PROPERTY_VALUE;
            errorDescription = TEXT("Property GEO, Invalid value format : %s");
            errorDescription += prop->getValue();
            return false;
        }

    //TZ property
    if(!wcscmp(prop->getName(), TEXT("TZ")) && prop->getValue())
        if(!validateTZ(prop->getValue())) {
            *errorCode = ERROR_INVALID_PROPERTY_VALUE;
            errorDescription = TEXT("Property TZ, Invalid value format : %s");
            errorDescription += prop->getValue();
            return false;
        }
    return true;
}

bool vCardConverter::validateProperty30(VProperty* prop, WString& errorDescription, long* errorCode) {

    if(!wcsstr(VCARD30_PROPERTIES_LIST, prop->getName()) &&
        wcsstr(prop->getName(),TEXT("X-")) != prop->getName()) {
        *errorCode = ERROR_ILLEGAL_PROPERTY_NAME;
        errorDescription = TEXT("Illegal property name : %s");
        errorDescription += prop->getName();
        return false;
    }

    if(prop->containsParameter(TEXT("ENCODING")) && wcscmp(prop->getParameterValue(TEXT("ENCODING")), TEXT("b")))
        return false;

    if(!wcscmp(prop->getName(), TEXT("ADR")))
        for(int i = 0; i < prop->parameterCount(); i++) {
            if(!wcsstr(PARAMTER30_LIST,prop->getParameter(i)) &&
                wcsstr(prop->getParameter(i),TEXT("X-")) != prop->getParameter(i)) {
                *errorCode = ERROR_ILLEGAL_PARAMETER;
                errorDescription = TEXT("Illegal ADDRESS parameter : %s");
                errorDescription += prop->getParameter(i);
                return false;
            }
            if(!wcscmp(prop->getParameter(i),TEXT("TYPE")) && prop->getParameterValue(TEXT("TYPE")) &&
                !checkType(prop->getParameterValue(TEXT("TYPE")), ADDRESS30_TYPES)) {
                *errorCode = ERROR_ILLEGAL_TYPE_PARAMETER;
                errorDescription = TEXT("Illegal ADDRESSS type parameter : %s");
                errorDescription += prop->getParameterValue(TEXT("TYPE"));
                return false;
            }

        }

    if(!wcscmp(prop->getName(), TEXT("LABEL")))
        for(int i = 0; i < prop->parameterCount(); i++) {
            if(!wcsstr(PARAMTER30_LIST,prop->getParameter(i)) &&
                wcsstr(prop->getParameter(i),TEXT("X-")) != prop->getParameter(i)) {
                    *errorCode = ERROR_ILLEGAL_PARAMETER;
                    errorDescription = TEXT("Illegal LABEL parameter : %s");
                    errorDescription += prop->getParameter(i);
                    return false;
                }
                if(!wcscmp(prop->getParameter(i),TEXT("TYPE")) && prop->getParameterValue(TEXT("TYPE")) &&
                    !checkType(prop->getParameterValue(TEXT("TYPE")), ADDRESS30_TYPES)) {
                    *errorCode = ERROR_ILLEGAL_TYPE_PARAMETER;
                    errorDescription = TEXT("Illegal LABEL type parameter : %s");
                    errorDescription += prop->getParameterValue(TEXT("TYPE"));
                    return false;
               }

        }

    if(!wcscmp(prop->getName(), TEXT("TEL")))
        for(int i = 0; i < prop->parameterCount(); i++) {
            if(!wcsstr(PARAMTER30_LIST,prop->getParameter(i)) &&
                wcsstr(prop->getParameter(i),TEXT("X-")) != prop->getParameter(i)) {
                *errorCode = ERROR_ILLEGAL_PARAMETER;
                errorDescription = TEXT("Illegal TEL parameter : %s");
                errorDescription += prop->getParameter(i);
                return false;
            }
            if(!wcscmp(prop->getParameter(i),TEXT("TYPE")) && prop->getParameterValue(TEXT("TYPE")) &&
                !checkType(prop->getParameterValue(TEXT("TYPE")), TEL30_TYPES)) {
                *errorCode = ERROR_ILLEGAL_TYPE_PARAMETER;
                errorDescription = TEXT("Illegal TEL type parameter : %s");
                errorDescription += prop->getParameterValue(TEXT("TYPE"));
                return false;
            }
        }

    if(!wcscmp(prop->getName(), TEXT("EMAIL")) && prop->containsParameter(TEXT("TYPE")) &&
        prop->getParameterValue(TEXT("TYPE")) && !checkType(prop->getParameterValue(TEXT("TYPE")), EMAIL30_TYPES)) {
        *errorCode = ERROR_ILLEGAL_TYPE_PARAMETER;
        errorDescription = TEXT("Illegal EMAIL type parameter : %s");
        errorDescription += prop->getParameterValue(TEXT("TYPE"));
        return false;
    }

    if(!wcscmp(prop->getName(), TEXT("KEY")) && prop->containsParameter(TEXT("TYPE")) &&
        prop->getParameterValue(TEXT("TYPE")) && !wcsstr(KEY_TYPES, prop->getParameterValue(TEXT("TYPE")))) {
        *errorCode = ERROR_ILLEGAL_TYPE_PARAMETER;
        errorDescription = TEXT("Illegal KEY type parameter : %s");
        errorDescription += prop->getParameterValue(TEXT("TYPE"));
        return false;
     }

    if(!wcscmp(prop->getName(), TEXT("GEO")) && prop->getValue())
         if(!validateGeo(prop->getValue())) {
            *errorCode = ERROR_INVALID_PROPERTY_VALUE;
            errorDescription = TEXT("Property GEO, Invalid value format : %s");
            errorDescription += prop->getValue();
            return false;
        }

    if(!wcscmp(prop->getName(), TEXT("TZ")) && prop->getValue())
        if(!validateTZ(prop->getValue())) {
            *errorCode = ERROR_INVALID_PROPERTY_VALUE;
            errorDescription = TEXT("Property GEO, Invalid value format : %s");
            errorDescription += prop->getValue();
            return false;
        }

    return true;
}

bool vCardConverter::checkType(WCHAR* types, WCHAR* typesList) {

    if (!types)
        return true;


    WCHAR seps[] = TEXT(",");
    WCHAR* token;
    WCHAR* copyTypes = new WCHAR[wcslen(types)+1];
    wcscpy(copyTypes, types);

    token = wcstok( copyTypes, seps );

    while( token != NULL )
    {
        if(!wcsstr(typesList, token) && wcsstr(token,TEXT("X-")) != token)
            return false;
        token = wcstok( NULL, seps );
    }

    delete [] copyTypes; copyTypes  = NULL;

    return true;
}

bool vCardConverter::validateTZ(WCHAR* timeZone) {
    //("+" / "-") time-hour ":" time-minute
    //time-hour    = 2DIGIT;00-23
    //time-minute  = 2DIGIT;00-59

    WCHAR tzHour[3] = TEXT("");
    WCHAR tzMin[3] = TEXT("");

    if(!wcschr(timeZone,':'))
        return false;
    if(wcslen(timeZone) == 6) {
        if(timeZone[0] != '+' && timeZone[0] != '-')
            return false;
        tzHour[0] = timeZone[1];
        tzHour[1] = timeZone[2];
        tzMin[0] = timeZone[4];
        tzMin[1] = timeZone[5];

    }
    else if(wcslen(timeZone) == 5) {
        tzHour[0] = timeZone[0];
        tzHour[1] = timeZone[1];
        tzMin[0] = timeZone[3];
        tzMin[1] = timeZone[4];
    }

    if(!tzHour || !tzMin)
        return false;
    else {
        int hour, min;
        hour = _wtoi(tzHour);
        min = _wtoi(tzMin);
        if((hour < 0 || hour > 23) || (min < 0 || min >59))
            return false;
    }

    return true;
}

bool vCardConverter::validateGeo(WCHAR* geo) {
    if(!geo)
        return false;

    //expected format: longitude;latitude
    WCHAR* pDest = NULL;
    pDest = wcschr(geo, ';');

    if(!pDest)
        return false;

    WCHAR* longitude = new WCHAR[wcslen(geo)+1];
    wcsncpy(longitude, geo, pDest-geo);

    WCHAR* latitude = new WCHAR[wcslen(geo)+1];
    if(++pDest)
        wcscpy(latitude, pDest);
    else
        return false;

    WCHAR* stopstring;
   // double lon = _wtof(longitude);
   // double lat = _wtof(latitude);

    double lon = wcstod(longitude, &stopstring);
    double lat = wcstod(latitude, &stopstring);

    if(lon == 0.0 || lat == 0.0)
        return false;

    return true;
}

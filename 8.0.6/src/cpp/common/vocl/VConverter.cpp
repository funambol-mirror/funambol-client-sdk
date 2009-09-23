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


#include "base/util/utils.h"
#include "vocl/VConverter.h"
#include "vocl/VObjectFactory.h"
#include "base/util/WString.h"
#include "base/quoted-printable.h"
#include "base/globalsdef.h"

USE_NAMESPACE


VObject* VConverter::parse(const WCHAR* buffer) {

    WCHAR *objType = extractObjectType(buffer);
    WCHAR *objVersion = extractObjectVersion(buffer);
    if(!objType)
        return NULL;

    VObject* vo = VObjectFactory::createInstance(objType, objVersion);
    VProperty *prop;

    // Unfolding
    WCHAR* buffCopy = unfolding(buffer);

    while ( true ) {
        prop = readFieldHeader(buffCopy);
        if (!prop) {
            break;
        }
        if ( readFieldBody(buffCopy, prop )) {
            vo->addProperty(prop);
        }
        delete prop;
    }

    if(objType)    { delete [] objType;    objType    = NULL; }
    if(objVersion) { delete [] objVersion; objVersion = NULL; }
    if (buffCopy)  { delete [] buffCopy;   buffCopy   = NULL; }

    return vo;
}

VProperty* VConverter::readFieldHeader(WCHAR* buffer) {

    WCHAR* headerIndex = NULL;
    WCHAR* quotaIndex = NULL;
    quotaIndex = wcschr(buffer, '"');
    headerIndex = wcschr(buffer, ':');


    if(!headerIndex)
        return NULL;
    bool quota = false;
    // If the header contains a quotation mark,
    // then rescan it starting directly after the _quotation mark_
    // (not after the end of the header, as in the original code)
    // to find the real end of the header.
    //
    // The reason for this code apparently is that the simple search above
    // might have found a headerIndex which points into the middle of
    // the quoted string.
    //
    // A better solution would be to always scan the header properly.
    if(quotaIndex && quotaIndex < headerIndex) {
        quota = true;
        int len = int(wcslen(buffer));
        for(int i = int(quotaIndex - buffer) + 1; i < len; i++) {
            if(buffer[i] == '"')
                quota = !quota;
            if(buffer[i] == ':' && !quota) {
                headerIndex = &buffer[i];
                break;
            }
        }
    }

    if(quota)
        return NULL;

    VProperty* prop = new VProperty(NULL);

    WCHAR* header = new WCHAR[wcslen(buffer) + 1];
    buffer[headerIndex - buffer] = '\0';
    wcscpy(header, buffer);
    // Shift the remaing string to the front of the buffer.
    // Using wcscpy() for that is incorrect because the standard
    // does not guarantee in which order bytes are moved!
    // wcscpy(buffer, ++headerIndex);
    ++headerIndex;
    memmove(buffer, headerIndex, (wcslen(headerIndex) + 1) * sizeof(*headerIndex));

    //if the header is folded (in .ics files)
    //we need to remove the folding
    WCHAR* headerFolding = NULL;
    if ((headerFolding = wcsstr(header, TEXT("\n ")))) {
        header[headerFolding - header] = '\0';
    }

    WCHAR seps[] = TEXT(";");
    WCHAR *token;
    bool first = true;

    token = wcstok( header, seps );
    while( token != NULL ) {
        if (first) {

            WCHAR* group = new WCHAR[wcslen(token) + 1];
            if(extractGroup(token, group))
                prop->addParameter(TEXT("GROUP"), group);
            else
                delete [] group; group= NULL;
            prop->setName(token);
            first = false;
        }
        else {
            WCHAR* paramIndex;
            paramIndex = wcschr(token, '=');

            if(paramIndex) {
                WCHAR* paramName = new WCHAR[wcslen(token) + 1];
                token[paramIndex - token] = '\0';
                wcscpy(paramName, token);
                ++paramIndex;
                memmove(token, paramIndex, (wcslen(paramIndex) + 1) * sizeof(*paramIndex));

                WCHAR* paramVal = new WCHAR[wcslen(token) + 1];
                wcscpy(paramVal, token);
                prop->addParameter(paramName, paramVal);

                delete [] paramName; paramName = NULL;
                delete [] paramVal; paramVal = NULL;
            }
            else {
                prop->addParameter(token,NULL);
            }
        }
        token = wcstok( NULL, seps );
    }

    delete [] header; header = NULL;
    delete token; token = NULL;

    return prop;
}

bool VConverter::readFieldBody(WCHAR* buffer, VProperty* vprop) {

    int i      = 0;
    int j      = 0;
    int len    = 0;
    int offset = 0;
    bool ret   = false;
    WCHAR* value     = NULL;
    WCHAR* allValues = NULL;
    WCHAR* c         = NULL;

    // Get length of all values
    while (buffer[i] != '\0') {
        if ((buffer[i] == '\r') || buffer[i] == '\n') {

            // Get offset of next property
            for (j=i+1; buffer[j] != '\0'; j++) {
                if((buffer[j] != '\r') && (buffer[j] != '\n'))
                    break;
            }
            offset = j;
            break;
        }
        i++;
    }
    len = i;


    if (!len) {
        // This field is empty, we MUST consider it adding an empty value
        // so any value on client will be deleted.
        vprop->addValue(TEXT(""));
        ret = true;
        goto finally;
    }

    // This is a string with all values for this property (to parse)
    allValues = new WCHAR[len + 1];
    wcsncpy(allValues, buffer, len);
    allValues[len] = 0;


    //
    // If needed, decode QP string and copy to 'allValues'.
    //
    if(vprop->equalsEncoding(TEXT("QUOTED-PRINTABLE"))) {

        char* buf = toMultibyte(allValues);
        char* dec = qp_decode(buf);
        len = strlen(dec);
        delete [] buf;

        if (dec) {
            char* t = toMultibyte(vprop->getParameterValue(TEXT("CHARSET")));
            WCHAR* wdecoded = toWideChar(dec, t);
            delete [] dec;
            delete [] t;

            if (wdecoded) {
                wcsncpy(allValues, wdecoded, len);
                allValues[len] = 0;
                delete [] wdecoded;
            }
        }
        if (!len) {
            goto finally;
        }
    }

    /*
    --- base64 is not decoded ----
    IT IS NOT POSSIBLE TO DECODE BASE64 PARAMETERS IN A WCHAR
    AND TAKE THE LENGHT OF A BINARY!!
    */

    // This is a buffer for each single value
    value = new WCHAR[len + 1];
    wcscpy(value, TEXT(""));

    //
    // Extract values and add to Vproperty
    //
    j=0;
    c = allValues;
    for (i=0; i<len; i++) {

        // End of value
        if (c[i] == ';') {
            vprop->addValue(value);
            j = 0;
            wcscpy(value, TEXT(""));
        }

        else {
            // Manage escaped chars: jump back-slash
            if (c[i] == '\\') {
                if (c[i+1]=='n') {
                    // none: this is "\n" sequence (formatted line ending for 3.0)
                }
                else {
                    i++;
                    if (c[i] == '\0')
                        break;
                }
            }
            value[j] = c[i];
            j++;
            value[j] = '\0';
        }
    }

    vprop->addValue(value);
    ret = true;

finally:

    // Shift buffer for next property to parse
    //wcscpy(buffer, buffer+offset);
    memmove(buffer, buffer+offset, (wcslen(buffer+offset) + 1)*sizeof(*buffer));

    if (value) {
        delete [] value;     value = NULL;
    }
    if (allValues) {
        delete [] allValues; allValues = NULL;
    }

    return ret;
}



WCHAR* VConverter::extractObjectProperty(const WCHAR* buffer, const WCHAR *property,
                                           WCHAR * &buffCopy, size_t &buffCopyLen) {

    // Memory handling in extractObjectType() and
    // extractObjectVersion() was broken:
    // they allocated a buffer, then returned a pointer into
    // parts of this buffer as result. The caller cannot
    // free the result in this case. The functions were also
    // duplicating the same code.
    //
    // This partial fix reuses previously allocated
    // memory if the function is called a second time.

    size_t len = wcslen(buffer) + 1;
    if (buffCopyLen < len) {
        if (buffCopy) {
            delete [] buffCopy;
        }
        buffCopy = new WCHAR[len];
        buffCopyLen = len;
    }
    wcscpy(buffCopy, buffer);

    WCHAR seps[] = TEXT(":\n");
    WCHAR *token;

    token = wcstok( buffCopy, seps );
    while (token != NULL) {
        if(!wcscmp(token, property)) {
            token = wcstok( NULL, seps );
            WCHAR* index = wcschr(token,'\r');
            if(index)
                token[index-token] = '\0';
            return token;
        }
        token = wcstok( NULL, seps );
    }

    return NULL;
}

WCHAR* VConverter::extractObjectType(const WCHAR* buffer) {
    WCHAR* buffCopy = NULL;
    size_t buffCopyLen = 0;

    WCHAR* tmp = extractObjectProperty(buffer, TEXT("BEGIN"),
                                 buffCopy, buffCopyLen);

    WCHAR* ret = wstrdup(tmp);

    if (buffCopy) { delete [] buffCopy; }
    buffCopyLen = 0;

    return ret;
}


WCHAR* VConverter::extractObjectVersion(const WCHAR* buffer) {
    WCHAR* buffCopy = NULL;
    size_t buffCopyLen = 0;

    WCHAR* tmp = extractObjectProperty(buffer, TEXT("VERSION"),
                                 buffCopy, buffCopyLen);
    
    WCHAR* ret = wstrdup(tmp);
    if (buffCopy) { delete [] buffCopy; }
    buffCopyLen = 0;

    return ret;
}

bool VConverter::extractGroup(WCHAR* propertyName, WCHAR* propertyGroup) {

    WCHAR* groupIndex;
    groupIndex = wcschr(propertyName, '.');

    if(!groupIndex)
        return false;

    propertyName[groupIndex - propertyName] = '\0';
    wcscpy(propertyGroup, propertyName);
    wcscpy(propertyName, ++groupIndex);

    return true;
}

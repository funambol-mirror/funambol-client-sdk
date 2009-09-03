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
#include "base/util/WString.h"
#include "vocl/VProperty.h"
#include "base/quoted-printable.h"
#include "base/globalsdef.h"

USE_NAMESPACE

VProperty::VProperty(const WCHAR* propname, const WCHAR* propvalue) {

    name = (propname) ? wstrdup(propname) : NULL;

    parameters = new ArrayList();
    values     = new ArrayList();
    valueBuf   = NULL;

    if (propvalue) {
        WString wsVal = propvalue;
        values->add((ArrayElement&)wsVal);
    }
}

VProperty::~VProperty() {

    if (name) {
        delete [] name; name = NULL;
    }
    if (parameters) {
        delete parameters; parameters = NULL;
    }
    if (values) {
        delete values; values = NULL;
    }
    if (valueBuf) {
        delete [] valueBuf; valueBuf = NULL;
    }
}

void VProperty::setName (const WCHAR* s) {

    set(&name, s);
}


WCHAR* VProperty::getName(WCHAR* buf, int size) {

    if (buf == NULL) {
        return name;
    }

    if (size >= 0) {
        wcsncpy(buf, name, size);
    }
    else {
        wcscpy(buf, name);
    }

    return buf;
}

void VProperty::addValue(const WCHAR* value) {

    // Empty strings are accepted
    if(value) {
        WString wsVal = value;
        values->add((ArrayElement &)wsVal);
    }
}

bool VProperty::removeValue(const int index) {

    bool ret = false;
    if (values != NULL) {
        values->removeElementAt(index);
                ret = true;
    }
    return ret;
}

// Returned value is a pointer to internal buffer,
// copy it if must have a different life cycle.
WCHAR* VProperty::getValue(int index) {

    if (valueBuf) {
        delete [] valueBuf; valueBuf = NULL;
    }

    WString* wsValue = (WString*)values->get(index);
    if (wsValue) {
        valueBuf = new WCHAR[wsValue->length() + 1];
        wcscpy(valueBuf, wsValue->c_str());
    }

    return valueBuf;
}

int VProperty::valueCount() {

    return values->size();
}


////////// Only for compatibility
WCHAR* VProperty::getValue(WCHAR* buf) {

    if (!buf) {
        return getValue(0);
    }

    WString* wsValue = (WString*)values->get(0);
    if (wsValue) {
        wcscpy(buf, wsValue->c_str());
    }
    return buf;
}


void VProperty::setValue(const WCHAR* value) {

    addValue(value);

}

WCHAR* VProperty::getPropComponent(int i) {

    return getValue(i-1);
}
//////////////////////////////////


void VProperty::addParameter (const WCHAR* paramName, const WCHAR* paramValue) {

    if(paramName) {
    WKeyValuePair *parameter = new WKeyValuePair(paramName, paramValue);
    parameters->add((ArrayElement &)*parameter);

    delete parameter; parameter = NULL;
    }
}

void VProperty::removeParameter(WCHAR* paramName) {

    if (parameters != NULL) {
        for (int i=0; i<parameters->size(); i++){
            WKeyValuePair *parameter;
            parameter = (WKeyValuePair* )parameters->get(i);
            if(!wcscmp(parameter->getKey(), paramName)) {
                parameters->removeElementAt(i);
                break;
            }
        }
    }
}
bool VProperty::containsParameter(WCHAR* paramName) {

    if (parameters != NULL) {
        for (int i=0; i<parameters->size(); i++){
            WKeyValuePair *parameter;
            parameter = (WKeyValuePair* )parameters->get(i);
            if(!wcscmp(parameter->getKey(), paramName)){
                return true;
            }
        }
    }

    return false;
}
WCHAR* VProperty::getParameterValue(WCHAR* paramName) {

    if (parameters != NULL) {
        for (int i=0; i<parameters->size(); i++) {
            WKeyValuePair *parameter;
            parameter = (WKeyValuePair* )parameters->get(i);
            if(!wcscmp(parameter->getKey(), paramName))
                return ((WCHAR *)parameter->getValue());
        }
    }

    return NULL;
}
WCHAR* VProperty::getParameterValue(int index) {

    if (parameters != NULL) {
        WKeyValuePair *parameter;
        parameter = (WKeyValuePair*)parameters->get(index);
        return parameter ? (WCHAR *)parameter->getValue() : NULL;
    }

    return NULL;
}

void VProperty::set(WCHAR** p, const WCHAR* v) {

    if (*p) {
        delete [] *p;
    }
    *p = (v) ? wstrdup(v) : NULL;
}

ArrayElement* VProperty::clone() {

    if(name) {

        VProperty *cloneProperty = new VProperty(name);

        if(values != NULL) {
            for (int i=0; i<valueCount(); i++) {
                WString* valueCopy = (WString*)values->get(i)->clone();
                cloneProperty->addValue(valueCopy->c_str());
                delete valueCopy;
            }
        }

        if (parameters != NULL) {
            for (int i=0; i<parameters->size(); i++) {
                WKeyValuePair* parameterCopy;
                parameterCopy = (WKeyValuePair*)parameters->get(i)->clone();
                cloneProperty->addParameter(parameterCopy->getKey(), parameterCopy->getValue());
                delete parameterCopy;
            }
        }
        return cloneProperty;
    }
    return NULL;
}

int VProperty::parameterCount() {

    return parameters->size();
}


/*
 * Returns a WCHAR* string of this VProperty, based on vCard-vCal specifications.
 * Here values of the property are encoded / special chars are escaped according to
 * vCard-vCal 2.1/3.0 specifications.
 * @param version: vCard version "2.1" or "3.0" - we have different specs
 *                 (if not defined, default will be 2.1)
 *
 * Note:
 * The returned WCHAR* is new allocated, must be freed by the caller.
 */
WCHAR* VProperty::toString(WCHAR* version) {

    bool is_30 = false;
    if (version) {
        is_30 = !wcscmp(version, TEXT("3.0"));
    }

    WString propertyString = TEXT("");
    bool isToFormatValue = true;

    if (!name){
        goto finally;
    }
    
    if (parameterCount()>0 && containsParameter(TEXT("CONTENT-VALUE"))) {
        WCHAR* parVal = getParameterValue(TEXT("CONTENT-VALUE"));
        if (parVal != NULL && wcscmp(parVal, TEXT("UNCHANGED")) == 0) 
            isToFormatValue = false;
        
    }

    // Set encoding (QP/B64) parameter if necessary
    // QP encoding not allowed for vCard 3.0 (RFC 2426)
    if (is_30) {
        if (isToFormatValue) {
            if(!equalsEncoding(TEXT("BASE64")) &&
               !equalsEncoding(TEXT("B")) &&
               !equalsEncoding(TEXT("b")) ) {
                for (int i=0; i<valueCount(); i++) {
                    char* charValue = toMultibyte(getValue(i));
                    if (encodingIsNeed(charValue)) {
                        addParameter(TEXT("ENCODING"), TEXT("b"));
                        delete [] charValue;
                        break;
                    }
                }
            }
        }
    }
    else {               
        if (isToFormatValue) {
            if (!equalsEncoding(TEXT("QUOTED-PRINTABLE")) ) {
                for (int i=0; i<valueCount(); i++) {
                    char* charValue = toMultibyte(getValue(i));
                    if (encodingIsNeed(charValue)) {
                        addParameter(TEXT("ENCODING"), TEXT("QUOTED-PRINTABLE"));
                        addParameter(TEXT("CHARSET"), TEXT("UTF-8"));
                        delete [] charValue;
                        break;
                    }
                    delete [] charValue;
                }
            }
        }
    }


    //
    // Write Group:
    //
    if (parameterCount()>0 && containsParameter(TEXT("GROUP"))) {
        propertyString.append(getParameterValue(TEXT("GROUP")));
        propertyString.append(TEXT("."));
    }

    //
    // Write name:
    //
    propertyString.append(name);

    //
    // Write parameters:
    //
    if(parameterCount()>0) {
        for (int i=0; i<parameterCount(); i++) {
            WKeyValuePair *parameter;
            parameter = (WKeyValuePair*)parameters->get(i);
            if (parameter->getKey()) {
                if (!wcscmp(parameter->getKey(), TEXT("GROUP"))) {
                    continue;
                }
                // for the custom value 
                if (!wcscmp(parameter->getKey(), TEXT("CONTENT-VALUE"))) {
                    continue;
                }
                propertyString.append(TEXT(";"));
                propertyString.append(parameter->getKey());
            }
            if (parameter->getValue()) {
                propertyString.append(TEXT("="));
                propertyString.append(parameter->getValue());
            }
        }
    }

    //
    // Write values:
    //
    propertyString.append(TEXT(":"));
    if(valueCount()>0) {
        WString valueString = TEXT("");
        
        if (isToFormatValue) {
            // Get all values in one single string
            WCHAR *value, *valueConv;
            for (int i=0; i<valueCount(); i++) {
                if (i>0) {
                    valueString.append(TEXT(";"));
                }
                value = getValue(i);

                // Escape special chars - based on version (";"  "\", ",")
                valueConv = escapeSpecialChars(value, version);

                valueString.append(valueConv);
                delete [] valueConv;
            }
        } else { 
            
            WCHAR *value;
            for (int i=0; i<valueCount(); i++) {
                if (i>0) {
                    valueString.append(TEXT(";"));
                }
                value = getValue(i);         
                if (i == 0 && (value != NULL) && wcslen(value) > 0 && (wcscmp(name, TEXT("PHOTO")) == 0)) {
                    valueString.append(TEXT("\r\n"));
                }   
                valueString.append(value);
            }

        }

        if (isToFormatValue) {
            // QUOTED-PRINTABLE encoding (of all values)
            if (equalsEncoding(TEXT("QUOTED-PRINTABLE"))) {

                char* s  = toMultibyte(valueString.c_str());
                char* qp = convertToQP(s, 0);
                WCHAR* qpValueString = toWideChar(qp);
                if(qpValueString)
                    propertyString.append(qpValueString);
                else
                    propertyString.append(valueString);

                delete [] qpValueString;
                delete [] s;
                delete [] qp;
            }

            // BASE64 encoding (of all values)
            else if(equalsEncoding(TEXT("BASE64")) ||
               equalsEncoding(TEXT("B")) ||
               equalsEncoding(TEXT("b")) ) {

                char* s  = toMultibyte(valueString.c_str());
                int len = strlen(s);
                char* base64 = new char[2*len + 1];
                b64_encode(base64, s, len);
                WCHAR* b64ValueString = toWideChar(base64);

                propertyString.append(b64ValueString);
                // Extra line break: required for v.2.1 / optional for v.3.0
                //propertyString.append(RFC822_LINE_BREAK);

                delete [] b64ValueString;
                delete [] base64;
                delete [] s;
            }


            // Default encoding (7bit)
            else {
                propertyString.append(valueString);
            }
        } else { // not is to apply any transformation
            propertyString.append(valueString);
        }
    }


finally:
    // memory must be free by caller with delete []
    WCHAR *str = wstrdup(propertyString);
    return str;
}




WCHAR* VProperty::getParameter(int index){

    WKeyValuePair *parameter;
    parameter = (WKeyValuePair*)parameters->get(index);
    return (WCHAR *)parameter->getKey();
}

bool VProperty::equalsEncoding(WCHAR* encoding) {

    if ((encoding != NULL) && ((containsParameter(TEXT("ENCODING")) &&
        !wcscmp(getParameterValue(TEXT("ENCODING")),encoding)) ||
        containsParameter(encoding)))
        return true;
    return false;
}



bool VProperty::isType(WCHAR* type) {
    if(containsParameter(type))
        return true;
    if(containsParameter(TEXT("TYPE")) && getParameterValue(TEXT("TYPE"))) {

        WCHAR seps[] = TEXT(",");
        WCHAR* token;

        token = wcstok(getParameterValue(TEXT("TYPE")), seps );

        while( token != NULL )
        {
            if(!wcscmp(type, token))
                return true;
            token = wcstok( NULL, seps );
        }
    }

    if(containsParameter(TEXT("type")) && getParameterValue(TEXT("type"))) {

        WCHAR seps[] = TEXT(",");
        WCHAR* token;

        token = wcstok(getParameterValue(TEXT("type")), seps );

        while( token != NULL )
        {
            if(!wcscmp(type, token))
                return true;
            token = wcstok( NULL, seps );
        }
    }
    return false;
}


BEGIN_NAMESPACE

// ------------------ Public functions --------------------

/*
 * Convert a char* string into QUOTED-PRINTABLE format.
 * @param psz     : the input char* string
 * @param start   : the start point of the line
 * @return        : the char* string in QP format (new - must be freed by the caller!)
 *                  (NULL if conversion failed)
 */
char* convertToQP(const char* input, int start) {

    int   count   = start;
    int   maxLen  = 3*strlen(input);         // This is the max length for output string
    char *sAppend = NULL;
    char  szTemp[10];
    const char *p;

    // new - must be freed by the caller
    char* qpString = new char[maxLen + 1];
    strcpy(qpString, "");

    if (maxLen>0) {
        sAppend = new char[maxLen + 1];
        strncpy(sAppend, input, maxLen);
        sAppend[maxLen]=0;

        if(!sAppend)
            return NULL;

        for (p = sAppend; *p; p++) {
            //if (count > QP_MAX_LINE_LEN) {
            //    strcat(qpString, "=\r\n");
            //    count = 0;
            //}
            //else
            if (*p == '\t' || *p == ' ') {
                const char *pScan = p;
                while (*pScan && (*pScan == '\t' || *pScan == ' ')) {
                    pScan++;
                }
                if (*pScan == '\0') {
                    while (*p) {
                        unsigned char ind = *p;
                        sprintf(szTemp, "=%02X", ind);
                        strcat(qpString, szTemp);
                        count += 3;
                        p++;

                        //if (count > QP_MAX_LINE_LEN) {
                        //    strcat(qpString, "=\r\n");
                        //    count = 0;
                        //}
                    }
                    break;
                }
                else {
                    sprintf(szTemp, "%c", *p);
                    strcat(qpString, szTemp);
                    count++;
                }
                continue;
            }
            else if (('!' <= *p) && ('~' >= *p) && ('=' != *p)) {
                sprintf(szTemp, "%c", *p);
                strcat(qpString, szTemp);
                count++;
            }
            else {
                unsigned char ind = *p;
                sprintf(szTemp, "=%02X", ind);
                strcat(qpString, szTemp);
                count += 3;
            }
        }

        delete [] sAppend;
    }
    return qpString;
}



// Returns true if special encoding is needed for the string 'in'.
bool encodingIsNeed(const char *in) {
    for(int i = 0; i < int(strlen(in)); i++)
        if ( (in[i] < 0x20) || ((unsigned char)in[i] > 0x7f))
            return true;

    return false;
}





/*
* Escape special characters adding a back-slash (i.e. ";" -> "\;")
* @param inputString  : the input string to parse
* @param version      : vCard version "2.1" or "3.0" - we have different chars to escape
 *                     (if not defined, default will be 2.1)
* @return             : the new allocated string with escaped chars
* Note:
*      returns new allocated WCHAR*, must be freed by the caller.
*/
WCHAR* escapeSpecialChars(const WCHAR* inputString, WCHAR* version) {

    int i, j, inputLen, outputLen;
    inputLen  = wcslen(inputString);
    WCHAR* wc = new WCHAR[2];
    WCHAR charsToEscape[4];

    bool is_30 = false;
    if (version) {
        is_30 = !wcscmp(version, TEXT("3.0"));
    }
    if (is_30) {
        wcscpy(charsToEscape, VCARD30_SPECIAL_CHARS);
    }
    else {
        wcscpy(charsToEscape, VCARD21_SPECIAL_CHARS);
    }


    // First find the length of output value
    outputLen = inputLen;
    for (i=0; i<inputLen; i++) {
        wcsncpy(wc, &inputString[i], 1);
        wc[1]=0;
        if (wcsstr(charsToEscape, wc))
            outputLen ++;
    }
    WCHAR* outputString = new WCHAR[outputLen+1];


    // Now escape special characters (add back-slash)
    j=0;
    for (i=0; i<inputLen; i++) {
        wcsncpy(wc, &inputString[i], 1);
        wc[1]=0;
        if (wcsstr(charsToEscape, wc)) {
            if (is_30 && inputString[i]=='\\' && inputString[i+1]=='n') {
                // none: this is "\n" sequence, MUST NOT be escaped in 3.0
            }
            else {
                outputString[j]   = '\\';
                j++;
            }
        }
        outputString[j] = inputString[i];
        j++;
    }

    outputString[outputLen] = 0;
    delete [] wc;
    return outputString;
}




/*
 * Folding of long lines. Output string is splitted into multiple
 * lines, delimited by the RFC-822 line break ("\r\n").
 * @param inputString : input WCHAR string of text
 * @param maxLine     : the length of lines in the output string
 * @return            : output WCHAR string with folded lines (new allocated)
 *
 * Note:
 *      returns new allocated WCHAR*, must be freed by the caller.
 */
WCHAR* folding(const WCHAR* inputString, const int maxLine) {

    // "\r\n" followed by a white space as line ending (RFC 2425)
    WCHAR newLine[4];
    wcscpy(newLine, RFC822_LINE_BREAK);
    wcscat(newLine, TEXT(" \0"));
    int outputLen = 0, i = 0;
    int inputLen  = wcslen(inputString);
    WCHAR* outputString;

    // No folding needed
    if (inputLen <= maxLine) {
        outputString = new WCHAR[inputLen + 1];
        wcscpy(outputString, inputString);
        goto finally;
    }

    outputLen = inputLen + (int)(inputLen/maxLine + 1)*wcslen(newLine);
    outputString = new WCHAR[outputLen + 1];
    outputString[0] = 0;

    for (i=0; i<inputLen; i += maxLine) {
        wcsncat(outputString, inputString+i, maxLine);
        wcscat(outputString, newLine);
    }
    outputString[outputLen] = 0;

finally:
    return outputString;
}



/*
 * Unfolding a logical line. Input string is splitted into multiple
 * lines, delimited by the RFC-822 line break ("\r\n") followed by one space.
 * @param inputString : input  WCHAR string with folded lines
 * @return            : output WCHAR string unfolded (new allocated)
 *
 * Note:
 *      returns new allocated WCHAR*, must be freed by the caller.
 */
WCHAR* unfolding(const WCHAR* inputString) {

    int inputLen  = wcslen(inputString);
    WCHAR* outputString = new WCHAR[inputLen + 1];
    outputString[0] = 0;

    int j=0;
    for (int i=0; i<inputLen-2; i++) {
        if (inputString[i]   == '\r' &&
            inputString[i+1] == '\n' &&
            inputString[i+2] == ' ') {
            i += 2;
            continue;
        }
        outputString[j] = inputString[i];
        j++;
    }
    outputString[j] = 0;

    return outputString;
}

END_NAMESPACE



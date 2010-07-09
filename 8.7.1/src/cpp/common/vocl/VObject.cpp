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
#include "base/Log.h"
#include "vocl/VObject.h"
#include "string.h"
#include "base/globalsdef.h"

USE_NAMESPACE

VObject::VObject() {
    productID = NULL;
    version = NULL;
    properties = new ArrayList();
}

VObject::VObject(const WCHAR* prodID, const WCHAR* ver) {

    productID = NULL;
    version = NULL;

    if (prodID) {
        setProdID(prodID);
    }
    if (ver) {
        setVersion(ver);
    }
    properties = new ArrayList();
}

VObject::~VObject() {
    if (productID) {
        delete [] productID; productID = NULL;
    }
    if (version) {
        delete [] version; version = NULL;
    }
    if (properties) {
        delete properties; properties = NULL;
    }
}

void VObject::set(WCHAR** p, const WCHAR* v) {
    if (*p) {
        delete [] *p;
    }
    *p = (v) ? wstrdup(v) : NULL;
}

void VObject::setVersion(const WCHAR* ver) {
    set(&version, ver);
}

void VObject::setProdID(const WCHAR* prodID) {
    set(&productID, prodID);
}

WCHAR* VObject::getVersion() {
    return version;
}

WCHAR* VObject::getProdID() {
    return productID;
}

void VObject::addProperty(VProperty* vProp) {
    properties->add(*vProp);
}

int VObject::propertiesCount() {
    return properties->size();
}

bool VObject::removeProperty(int index) {
    if(index < 0 || index >= propertiesCount())
        return false;
    properties->removeElementAt(index);
    return true;
}

void VObject::removeProperty(const WCHAR* propName) {
    for (int i=0; i<properties->size(); i++) {
        VProperty *property;
        property = (VProperty* )properties->get(i);
        if(!wcscmp(property->getName(), propName)) {
            properties->removeElementAt(i);
            break;
        }
    }
}

bool VObject::containsProperty(const WCHAR* propName) {
    for (int i=0; i<properties->size(); i++) {
        VProperty *property;
        property = (VProperty* )properties->get(i);
        if(!wcscmp(property->getName(), propName)) {
            return true;
        }
    }
    return false;
}

VProperty* VObject::getProperty(int index) {
    return (VProperty*)properties->get(index);
}

VProperty* VObject::getProperty(const WCHAR* propName) {
    for (int i=0; i<properties->size(); i++) {

        VProperty *property;
        property = (VProperty* )properties->get(i);

        if(!wcscmp(property->getName(), propName)) {
            return property;
        }
    }
    return NULL;
}

/*
 * Returns the VCard - iCal string fro this VObject.
 * Note:
 * The returned WCHAR* is new allocated, must be freed by the caller.
 */
WCHAR* VObject::toString() {

    WString strVObject;

    bool is_30 = false;
    if (version) {
        is_30 = !wcscmp(getVersion(), TEXT("3.0"));
    }

    // *** FIXME ***
    // By now folding feature not supported on server...
    bool useFolding = false;

    // let's reserve some space to avoid reallocation in most cases
    strVObject.reserve(5000);

    for (int i=0; i<properties->size(); i++) {
        VProperty *prop = getProperty(i);
        WCHAR* propString = prop->toString(version);
        WCHAR* valueConv = NULL;

        // Folding
        if (useFolding && wcslen(propString) > VCARD_MAX_LINE_LEN) {
            valueConv = folding(propString, VCARD_MAX_LINE_LEN);
            strVObject.append(valueConv);
        }
        else {
            strVObject.append(propString);
        }
        strVObject.append(RFC822_LINE_BREAK);

        if (propString) {
            delete [] propString;  propString = NULL;
        }
        if (valueConv) {
            delete [] valueConv;   valueConv = NULL;
        }
    }

    // memory must be free by caller with delete []
    WCHAR *str = wstrdup(strVObject);
    return str;
}


void VObject::insertProperty(VProperty* property) {

    if (propertiesCount() == 0 || wcscmp(getProperty(propertiesCount()-1)->getName(),TEXT("END")))
        addProperty(property);
    else {
        VProperty* lastProperty = getProperty(TEXT("END"));
        removeProperty(TEXT("END"));
        addProperty(property);
        addProperty(lastProperty);
    }
}

void VObject::addFirstProperty(VProperty* property) {
    properties->add(0,(ArrayElement&)*property);
}

void VObject::removeAllProperies(const WCHAR* propName) {
    for(int i = 0, m = propertiesCount(); i < m ; i++)
        if(!wcscmp(getProperty(i)->getName(), propName)) {
            removeProperty(i);
            --i;
            --m;
        }
}

#ifdef VOCL_ENCODING_FIX

// Patrick Ohly: hack below, see header file

static int hex2int( WCHAR x )
{
    return (x >= '0' && x <= '9') ? x - '0' :
        (x >= 'A' && x <= 'F') ? x - 'A' + 10 :
        (x >= 'a' && x <= 'f') ? x - 'a' + 10 :
        0;
}

#define SEMICOLON_REPLACEMENT '\a'

void VObject::toNativeEncoding()
{
    bool is_30 = !wcscmp(getVersion(), TEXT("3.0"));
    // line break is encoded with either one or two
    // characters on different platforms
    const int linebreaklen = wcslen(SYNC4J_LINEBREAK);

    for (int index = propertiesCount() - 1; index >= 0; index--) {
        VProperty *vprop = getProperty(index);
        WCHAR *foreign = vprop->getValue();
        // the native encoding is always shorter than the foreign one
        WCHAR *native = new WCHAR[wcslen(foreign) + 1];

        if (vprop->equalsEncoding(TEXT("QUOTED-PRINTABLE"))) {
            int in = 0, out = 0;
            WCHAR curr;

            // this is a very crude quoted-printable decoder,
            // based on Wikipedia's explanation of quoted-printable
            while ((curr = foreign[in]) != 0) {
                in++;
                if (curr == '=') {
                    WCHAR values[2];
                    values[0] = foreign[in];
                    in++;
                    if (!values[0]) {
                        // incomplete?!
                        break;
                    }
                    values[1] = foreign[in];
                    in++;
                    if (values[0] == '\r' && values[1] == '\n') {
                        // soft line break, ignore it
                    } else {
                        native[out] = (hex2int(values[0]) << 4) |
                            hex2int(values[1]);
                        out++;

                        // replace \r\n with \n?
                        if ( linebreaklen == 1 &&
                             out >= 2 &&
                             native[out - 2] == '\r' &&
                             native[out - 1] == '\n' ) {
                            native[out - 2] = SYNC4J_LINEBREAK[0];
                            out--;
                        }

                        // the conversion to wchar on Windows is
                        // probably missing here
                    }
                } else {
                    native[out] = curr;
                    out++;
                }
            }
            native[out] = 0;
            out++;
        } else {
            wcscpy(native, foreign);
        }

        // decode escaped characters after backslash:
        // \n is line break only in 3.0
        WCHAR curr;
        int in = 0, out = 0;
        while ((curr = native[in]) != 0) {
            in++;
            switch (curr) {
             case '\\':
                curr = native[in];
                in++;
                switch (curr) {
                 case 'n':
                    if (is_30) {
                        // replace with line break
                        wcsncpy(native + out, SYNC4J_LINEBREAK, linebreaklen);
                        out += linebreaklen;
                    } else {
                        // normal escaped character
                        native[out] = curr;
                        out++;
                    }
                    break;
                 case 0:
                    // unexpected end of string
                    break;
                 default:
                    // just copy next character
                    native[out] = curr;
                    out++;
                    break;
                }
                break;
             case ';':
                // field separator - must replace with something special
                // so that we can encode it again in fromNativeEncoding()
                native[out] = SEMICOLON_REPLACEMENT;
                out++;
                break;
             default:
                native[out] = curr;
                out++;
            }
        }
        native[out] = 0;
        out++;

        // charset handling:
        // - doesn't exist at the moment, vCards have to be in ASCII or UTF-8
        // - an explicit CHARSET parameter is removed because its parameter
        //   value might differ between 2.1 and 3.0 (quotation marks allowed in
        //   3.0 but not 2.1) and thus would require extra code to convert it;
        //   when charsets really get supported this needs to be addressed
        WCHAR *charset = vprop->getParameterValue(TEXT("CHARSET"));
        if (charset) {
            // proper decoding of the value and the property value text
            // would go here, for the time being we just remove the
            // value
            if (_wcsicmp(charset, TEXT("UTF-8")) &&
                _wcsicmp(charset, TEXT("\"UTF-8\""))) {
                LOG.error("ignoring unsupported charset");
            }
            vprop->removeParameter(TEXT("CHARSET"));
        }

        vprop->setValue(native);
        delete [] native;
    }
}

void VObject::fromNativeEncoding()
{
    bool is_30 = !wcscmp(getVersion(), TEXT("3.0"));

    for (int index = propertiesCount() - 1; index >= 0; index--) {
        VProperty *vprop = getProperty(index);

        if (vprop->equalsEncoding(TEXT("QUOTED-PRINTABLE"))) {
            // remove this, we cannot recreate it
            vprop->removeParameter(TEXT("ENCODING"));
        }

        WCHAR *native = vprop->getValue();
        // in the worst case every comma/linebreak is replaced with
        // two characters and each \n with =0D=0A
        WCHAR *foreign = new WCHAR[6 * wcslen(native) + 1];
        WCHAR curr;
        int in = 0, out = 0;
        // line break is encoded with either one or two
        // characters on different platforms
        const int linebreaklen = wcslen(SYNC4J_LINEBREAK);

        // use backslash for special characters,
        // if necessary do quoted-printable encoding
        bool doquoted = !is_30 &&
            wcsstr(native, SYNC4J_LINEBREAK) != NULL;
        while ((curr = native[in]) != 0) {
            in++;
            switch (curr) {
             case ',':
                if (!is_30) {
                    // normal character
                    foreign[out] = curr;
                    out++;
                    break;
                }
                // no break!
             case ';':
             case '\\':
                foreign[out] = '\\';
                out++;
                foreign[out] = curr;
                out++;
                break;
             case SEMICOLON_REPLACEMENT:
                foreign[out] = ';';
                out++;
                break;
             default:
                if (doquoted &&
                    (curr == '=' || (unsigned char)curr >= 128)) {
                    // escape = and non-ASCII characters
                    swprintf(foreign + out, 4, TEXT("=%02X"), (unsigned int)(unsigned char)curr);
                    out += 3;
                } else if (!wcsncmp(native + in - 1,
                                    SYNC4J_LINEBREAK,
                                    linebreaklen)) {
                    // line break
                    if (is_30) {
                        foreign[out] = '\\';
                        out++;
                        foreign[out] = 'n';
                        out++;
                    } else {
                        wcscpy(foreign + out, TEXT("=0D=0A"));
                        out += 6;
                    }
                    in += linebreaklen - 1;
                } else {
                    foreign[out] = curr;
                    out++;
                }
                break;
            }
        }
        foreign[out] = 0;
        vprop->setValue(foreign);
        delete [] foreign;
        if (doquoted) {
            // we have used quoted-printable encoding
            vprop->addParameter(TEXT("ENCODING"), TEXT("QUOTED-PRINTABLE"));
        }
    }
}

#endif


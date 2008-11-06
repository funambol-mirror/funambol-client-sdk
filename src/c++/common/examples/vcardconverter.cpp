/*
 * Copyright (C) 2003-2007 Funambol, Inc.
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

/**
 * This program reads one vcard from the file
 * given as first parameter and applies certain
 * conversions to it.
 *
 * The content of the file has to be ASCII or
 * UTF-8 encoded.
 */

#include <stdio.h>
#include <memory>

#include "vocl/VConverter.h"
#include "vocl/vCard/vCardConverter.h"
#include "base/util/utils.h"
#include "base/util/WString.h"

// very simply auto_ptr for arrays
template <class T> class auto_array {
    T *m_array;
  public:
    auto_array(T *array = 0) : m_array(array) {}
    ~auto_array() { if (m_array) delete [] m_array; }

    void operator = (T *array) {
        if (m_array) delete [] m_array;
        m_array = array;
    }
    operator T * () { return m_array; }
    T *get() { return m_array; }
    T &operator [] (int index) { return m_array[index]; }
};

int main( int argc, char **argv )
{
    WCHAR *sep = TEXT("--------------- %s -----------------------\n");
    WCHAR *sep2 = TEXT("-----------------------------------------------------------\n");

    if (argc != 2) {
        fprintf(stdout, "usage: %s <vcard file>\n", argv[0]);
        return 1;
    }

    // read as char *
    char *buffer;
    size_t len;
    if (!readFile(argv[1], &buffer, &len, true)) {
        fprintf(stdout, "%s: reading failed", argv[1]);
    }
    auto_array<char> vcard(buffer);

    // convert to WCHAR
    auto_array<WCHAR> wvcard(toWideChar(vcard));
    fwprintf(stdout, sep, TEXT("original vcard"));
    fwprintf(stdout, TEXT("%s\n"), wvcard.get());
    fwprintf(stdout, sep2);
    fwprintf(stdout, TEXT("\n"));

    // parse it
    std::auto_ptr<VObject> vobj(VConverter::parse(wvcard));
    if (vobj.get() == 0) {
        fprintf(stdout, "VConverter::parse()failed\n");
        return 1;
    }
    vobj->toNativeEncoding();

    VProperty *fileas = vobj->getProperty(TEXT("X-EVOLUTION-FILE-AS"));
    VProperty *n = vobj->getProperty(TEXT("FN"));
    fwprintf(stdout,
             TEXT("version: %s\nprodid: %s\nfull name: %s\nfile-as: %s\n\n"),
             vobj->getVersion(),
             vobj->getProdID(),
             n ? n->getValue() : TEXT("<not set>"),
             fileas ? fileas->getValue() : TEXT("<not set>"));

    // convert into the other version, then back again
    WCHAR *versions[2];
    if (!wcscmp(vobj->getVersion(), TEXT("3.0"))) {
        versions[0] = TEXT("2.1");
        versions[1] = TEXT("3.0");
    } else {
        versions[0] = TEXT("3.0");
        versions[1] = TEXT("2.1");
    }
    for (int index = 0; index < 2; index++) {
        vobj->setVersion(versions[index]);
        VProperty *vprop = vobj->getProperty(TEXT("VERSION"));

        for (int property = vobj->propertiesCount() - 1;
             property >= 0;
             property--) {
            VProperty *vprop = vobj->getProperty(property);

            // replace 3.0 ENCODING=B with 2.1 ENCODING=BASE64 and vice versa
            WCHAR *encoding = vprop->getParameterValue(TEXT("ENCODING"));
            if (encoding &&
                (!wcsicmp(TEXT("B"), encoding) || !wcsicmp(TEXT("BASE64"), encoding))) {
                vprop->removeParameter(TEXT("ENCODING"));
                vprop->addParameter(TEXT("ENCODING"),
                                    !wcscmp(versions[index], TEXT("2.1")) ?
                                    TEXT("BASE64") : TEXT("b"));
            }
        }

        vprop->setValue(versions[index]);
        vobj->fromNativeEncoding();
        wvcard = vobj->toString();
        vobj->toNativeEncoding();
        fwprintf(stdout, sep, versions[index]);
        fwprintf(stdout, TEXT("%s\n"), wvcard.get());
        fwprintf(stdout, sep2);
        fwprintf(stdout, TEXT("\n"));
    }

    // convert into validated contact
    vCardConverter converter;
    converter.setSource(wvcard);
    Contact *contactPtr;
    long errorCode;
    WString error;
    if (!converter.convert(error, &errorCode)) {
        fwprintf(stdout, TEXT("converter failed: %s (%ld)\n"),
                 error.c_str(), errorCode);
        return 1;
    }
    converter.getContact(&contactPtr);
    std::auto_ptr<Contact> contact(contactPtr);
    wvcard = contact->toString();

    fwprintf(stdout, sep, TEXT("after parsing"));
    fwprintf(stdout, TEXT("%s\n"), wvcard.get());
    fwprintf(stdout, sep2);
    fwprintf(stdout, TEXT("\n"));

    // let's see how the Contact class interprets the properties
    Name *name = contact->getName();
    vCardProperty *displayname = name->getDisplayName();
    fwprintf(stdout,
             TEXT("display name\nencoding: %s\ncharset: %s\nlanguage: %s\nvalue: %s\n\n"),
             displayname->getEncoding(),
             displayname->getCharset(),
             displayname->getLanguage(),
             displayname->getValue());

    return 0;
}



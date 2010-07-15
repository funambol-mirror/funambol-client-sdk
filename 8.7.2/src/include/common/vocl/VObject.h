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


#ifndef INCL_VIRTUAL_OBJECT
#define INCL_VIRTUAL_OBJECT
/** @cond DEV */

#include "vocl/VProperty.h"
#include "base/globalsdef.h"

BEGIN_NAMESPACE



/*
 * ************************************* Class VObject *********************************************
 * *************************************************************************************************
 * A VObject object rapresents an item that can be a vCard, a vCalendar or vTodo.
 * A VObject contains an array of VProperty, each one rapresents a property.
 * A property is the definition of an individual attribute describing the vCard/vCal/vTodo.
 * Each property has a name, an array of parameters and an array of values,
 * for example, the property:
 *
 *    TEL;HOME:+1-919-555-1234
 *
 * has name 'TEL', one parameter 'HOME' and one value '+1-919-555-1234'
 *
 * Some properties have more than one parameter, each parameter is a pair: (name, value).
 * Use VProperty::add/getParameter() methods to access them.
 *
 * Some properties have more than one value, each value is a string of text.
 * Values MUST be inserted in the right order (as vCard specific).
 * Use VProperty::add/getValue() methods to access them.
 * Note:
 *       If one of these value is not present, it must be inserted however as
 *       an empty string, to make sure the right order is respected.
 *
 *
 * Properties with more than one value are:
 *
 * Property         name     1^value         2^value          3^value         4^value          5^value    6^value       7^value
 * ----------------------------------------------------------------------------------------------------------------------------------
 * Name         ->  N:      <LastName>      <FirstName>      <MiddleName>    <Title>          <Suffix>
 * Address      ->  ADR:    <Post Office>   <Extended Addr>  <Street>        <City>           <State>    <PostalCode>  <Country>
 * Audio Alarm  ->  AALARM: <RunTime>       <Snooze Time>    <Repeat Count>  <Audio Content>
 *
 *
 * Property values should be added as simple text, Quoted-Printable and BASE64 encodings are
 * automatically managed inside VObject. Also the escaping of special chars is
 * automatically managed inside VObject.
 *  Escaped chars for v.2.1:  ";" "\"
 *  Escaped chars for v.3.0:  ";" "\" ","
 * Use VObject::toString() method to obtain the correspondent vCard/vCal/vTodo.
 *
 *
 * To set Quoted-Printable / Base64 encoding for a property:
 * =========================================================
 * QP and B64 encodings are automatically managed inside VObject. This means that
 * data conversion is performed inside VObject, to force data encoding
 * just add the encoding parameter to the VProperty, calling:
 *
 *   for QP:    VProperty->addParameter(TEXT("ENCODING"), TEXT("QUOTED-PRINTABLE"))
 *   for B64:   VProperty->addParameter(TEXT("ENCODING"), TEXT("b"))
 *
 * Note that QP encoding is used only for vCard v.2.1 (not allowed in v.3.0).
 * If a property contains characters that cannot be managed with the default
 * 7bit encoding, it is encoded automatically with QP (v.2.1) or B64 (v.3.0).
 *
 */


class VObject {

private:

    WCHAR* version;
    WCHAR* productID;
    ArrayList* properties;
    void set(WCHAR**, const WCHAR*);

public:

    VObject();
    VObject(const WCHAR* prodID, const WCHAR* ver);
    virtual ~VObject();
    void setVersion (const WCHAR* ver);
    void setProdID (const WCHAR* prodID);
    WCHAR* getVersion();
    WCHAR* getProdID();
    /** Add a new property to the VObject */
    void addProperty(VProperty* property);
    /** Add a new property to the VObject, using key and value for it. */
    void addProperty(const WCHAR* key, const WCHAR* value) { 
        VProperty vprop(key, value); addProperty(&vprop);
    }
    void addFirstProperty(VProperty* property);
    void insertProperty(VProperty* property);
    bool removeProperty(int index);
    void removeProperty(const WCHAR* propName);
    void removeAllProperies(const WCHAR* propName);
    //removes all properties having name - propName;
    bool containsProperty(const WCHAR* propName);
    int propertiesCount();
    VProperty* getProperty(int index);
    VProperty* getProperty(const WCHAR* propName);
    WCHAR* toString();

#ifdef VOCL_ENCODING_FIX

    // Patrick Ohly:
    //
    // Normally the class does not change the encoding
    // of properties. That means that users of this class
    // have to be prepared to handle e.g. quoted-printable
    // encoding of non-ASCII characters or \n as line break
    // in vCard 3.0.
    //
    // This function decodes all property strings into the
    // native format. It supports:
    // - decoding quoted-printable characters if
    //   ENCODING=QUOTED-PRINTABLE
    // - replacement of \n with a line break and \, with
    //   single comma if version is 3.0
    //
    // It does not support charset conversions, so everything
    // has to be UTF-8 to work correctly.
    //
    // This function does not modify the property parameters,
    // so one could convert back into the original format.
    //
    // Consider this function a hack: at this time it is not
    // clear how the class is supposed to handle different
    // encodings, but I needed a solution, so here it is.
    //
    void toNativeEncoding();

    //
    // Converts properties in native format according to
    // their parameters.
    //
    // It only supports:
    // - replacement of line breaks and commas (if 3.0)
    //
    // It does not support any charsets or encoding.
    // ENCODING=QUOTED-PRINTABLE will be removed because
    // it no longer applies when toNativeEncoding() was
    // used before, but the other parameters are preserved:
    // this might be good enough to recreate the original
    // object.
    //
    void fromNativeEncoding();

/** @endcond */
#endif

};



END_NAMESPACE

/** @endcond */
#endif

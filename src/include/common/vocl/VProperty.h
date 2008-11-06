/*
 * Copyright (C) 2003-2007 Funambol, Inc
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


#ifndef INCL_VIRTUAL_PROPERTY
#define INCL_VIRTUAL_PROPERTY
/** @cond DEV */

#include "base/fscapi.h"
#include "base/util/WKeyValuePair.h"
#include "base/util/ArrayList.h"


// Quoted-Printable formatted lines should be max 76 chars long.
#define QP_MAX_LINE_LEN             70
#define VCARD_MAX_LINE_LEN          76


// These are special chars to escape in vCard/vCal/vTodo (version 2.1 - 3.0)
#define VCARD21_SPECIAL_CHARS       TEXT(";\\")
#define VCARD30_SPECIAL_CHARS       TEXT(";\\,")
#define RFC822_LINE_BREAK           TEXT("\r\n")


// ------------ Public functions --------------
WCHAR* escapeSpecialChars(const WCHAR* inputString, WCHAR* version);
char*    convertToQP(const char* input, int start);
bool     encodingIsNeed(const char *in);
WCHAR* folding(const WCHAR* inputString, const int maxLine);
WCHAR* unfolding(const WCHAR* inputString);



// ------------ Class VProperty ---------------

class VProperty : public ArrayElement {

private:

    WCHAR* name;
    void set(WCHAR** p, const WCHAR* v);

    ArrayList* parameters;
    ArrayList* values;

    // This is only used as a buffer for 'getValue()'
    WCHAR* valueBuf;

 public:

    VProperty(const WCHAR* propName , const WCHAR* propValue  = NULL);
    ~VProperty();
    ArrayElement* clone();
    void setName (const WCHAR* name);
    WCHAR* getName(WCHAR* buf = NULL, int size = -1);

    void addValue(const WCHAR* value);
    bool removeValue(const int index);
    WCHAR* getValue(int index);
    int valueCount();

    // For back-compatibility (to remove)
    WCHAR* getValue(WCHAR* buf = NULL);
    void setValue (const WCHAR* value);
    WCHAR* getPropComponent(int i);

    void addParameter(const WCHAR* paramName, const WCHAR* paramValue);
    void removeParameter(WCHAR* paramName);
    bool containsParameter(WCHAR* paramName);
    // Warning: the name does not always uniquely identify
    // the parameter, some of them may occur multiple times.
    // Use getParameterValue(int index) to get the value which
    // corresponds to a specific parameter.
    WCHAR* getParameterValue(WCHAR* paramName);
    WCHAR* getParameterValue(int index);
    WCHAR* getParameter(int index);
    int parameterCount();
    bool equalsEncoding(WCHAR* encoding);
    //WCHAR* getPropComponent(int i);
    bool isType(WCHAR* type);
    WCHAR* toString(WCHAR* version = NULL);

 };

/** @endcond */
#endif

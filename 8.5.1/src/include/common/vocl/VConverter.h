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


#ifndef INCL_VIRTUAL_CONVERTER
#define INCL_VIRTUAL_CONVERTER
/** @cond DEV */

#include "vocl/VObject.h"
#include "base/globalsdef.h"

BEGIN_NAMESPACE

class VConverter{

public:

    /**
     * Parses the passed string 'buffer', returns a NEW ALLOCATED VObject*.
     */
    static VObject* parse(const WCHAR* buffer);

private:
    static VProperty* readFieldHeader(WCHAR* buffer);
    static bool readFieldBody(WCHAR* buffer, VProperty* property);

    // Extract the parameter of certain properties, e.g. "BEGIN:" or "VERSION:".
    // The result is a pointer into buffCopy, which is expected to have
    // buffCopyLen wchars and will be reallocated if necessary.
    static WCHAR* extractObjectProperty(const WCHAR* buffer, const WCHAR *property,
                                          WCHAR * &buffCopy, size_t &buffCopyLen);

    // extractObjectType() and extractObjectVersion() contain static buffers,
    // copy the result before calling these functions again!
    static WCHAR* extractObjectType(const WCHAR* buffer);
    static WCHAR* extractObjectVersion(const WCHAR* buffer);
    static bool extractGroup(WCHAR* propertyName, WCHAR* propertyGroup);

};

END_NAMESPACE

/** @endcond */
#endif

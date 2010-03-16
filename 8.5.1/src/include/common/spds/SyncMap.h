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

 #ifndef INCL_SYNC_MAP
    #define INCL_SYNC_MAP
/** @cond DEV */

    #include "base/fscapi.h"
    #include "base/util/ArrayElement.h"
#include "base/globalsdef.h"

BEGIN_NAMESPACE


class SyncMap : public ArrayElement {

    private:
        char*  luid;
        char*  guid;

    public:
        SyncMap(const char*  guid = NULL, const char*  luid = NULL);
        ~SyncMap();

        /*
         * Returns the guid of this mapping.
         */
        const char*  getGUID();


        /*
         * Returns the luid of this mapping.
         */
        const char*  getLUID();

        /**
         * Sets a new value for the LUID property. The value is copied in the
         * class internal memory.
         *
         * @param luid the new value
         */
        void setLUID(const char*  luid);

        /**
         * Sets a new value for the LUID property (as unsigned int). It internally
         * calls setLUID(const char* )
         *
         * @param luid the new value
         */
        void setLUID(unsigned long luid);


        /**
         * Sets a new value for the GUID property. The value is copied in the
         * class internal memory.
         *
         * @param guid the new value
         */
        void setGUID(const char*  guid);

        /**
         * Creates a new instance of SyncMap from the content of this
         * object. The new instance is created the the C++ new operator and
         * must be removed with the C++ delete operator.
         */
        ArrayElement* clone();
    };


END_NAMESPACE

/** @endcond */
#endif

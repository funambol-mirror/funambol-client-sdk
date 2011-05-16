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


#ifndef INCL_CTPROP_PARAM
#define INCL_CTPROP_PARAM
/** @cond DEV */

#include "base/fscapi.h"
#include "base/util/ArrayList.h"
#include "syncml/core/ContentTypeParameter.h"
#include "syncml/core/StringElement.h"
#include "base/globalsdef.h"

BEGIN_NAMESPACE

class CTPropParam : public ArrayElement{

     // ------------------------------------------------------------ Private data
    private:
        char*    propName;
        ArrayList* valEnum;       //String[]
        char*    displayName;
        char*    dataType;
        int size;
        ArrayList* ctParameters; //ContentTypeParameter[]

        void initialize();

    // ---------------------------------------------------------- Protected data
    public:
        CTPropParam();
        ~CTPropParam();

        CTPropParam(char*    propName,
                    ArrayList* valEnum,
                    char*    displayName,
                    ArrayList* ctParameters);


        /**
         * Creates a new ContentTypeProperty object with the given name, value and
         * display name
         *
         * @param propName corresponds to &lt;PropName&gt; element in the SyncML
         *                  specification - NOT NULL
         * @param dataType corresponds to &lt;DataType&gt; element in the SyncML
         *                  specification
         * @param size corresponds to &lt;Size&gt; element in the SyncML
         *                  specification
         * @param displayName corresponds to &lt;DisplayName&gt; element in the SyncML
         *                  specification
         * @param ctParameters the array of content type parameters - NOT NULL
         *
         */
        CTPropParam(char*  propName,
                           char*  dataType,
                           int size,
                           char*  displayName,
                           ArrayList* ctParameters);
        // ---------------------------------------------------------- Public methods

        /**
         * Gets the property name
         *
         * @return the property name
         */
        const char* getPropName();

        /**
         * Sets the property name
         *
         * @param propName the property name
         */
        void setPropName(const char* propName);

        /**
         * Gets the array of value for the property
         *
         * @return the array of value for the property
         */
        ArrayList* getValEnum();

        /**
         * Sets the array of enumerated value property
         *
         * @param valEnum the array of enumerated value property
         */
        void setValEnum(ArrayList* valEnum);

        /**
         * Gets the display name property
         *
         * @return the display name property
         */
        const char* getDisplayName();

        /**
         * Sets the display name of a given content type property
         *
         * @param displayName the display name of a given content type property
         */
        void setDisplayName(const char* displayName);

        /**
         * Gets the data type propeties
         *
         * @return the data type propeties
         */
        const char* getDataType();

        /**
         * Sets the data type of a given content type property
         *
         * @param dataType the data type of a given content type property
         */
        void setDataType(const char* dataType);

        /**
         * Gets the size propeties
         *
         * @return the size propeties
         */
        int getSize();

        /**
         * Sets the size of a given content type property
         *
         * @param size the size of a given content type property
         *
         */
        void setSize(int size);

        /**
         * Gets the array of ContentTypeParameter
         *
         * @return the size propeties
         */
        ArrayList* getContentTypeParameters();

        /**
         * Sets an array of content type properties
         *
         * @param ctParameters array of content type properties
         *
         */
        void setContentTypeParameters(ArrayList* ctParameters);

        ArrayElement* clone();
};


END_NAMESPACE

/** @endcond */
#endif

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


#ifndef INCL_CTCAP
#define INCL_CTCAP
/** @cond DEV */

#include "base/fscapi.h"
#include "base/util/ArrayList.h"
#include "syncml/core/CTTypeSupported.h"
#include "base/util/StringBuffer.h"
#include "base/globalsdef.h"

BEGIN_NAMESPACE


class CTCap : public ArrayElement {

     // ------------------------------------------------------------ Private data
    private:
        StringBuffer ctType;
        StringBuffer verCT ;

        bool   fieldLevel;
        ArrayList properties;

        CTCap(const CTCap&) {};
        CTCap() {};
        
    // ---------------------------------------------------------- Public data
    public:



        /**
         * Creates a CTCap object with the specified values
         */
        CTCap(
            const char *ct_Type,
            const char *ver_CT,
            bool fLevel,
            ArrayList& props );

        ~CTCap();

        /**
        * Get the CT Type
        *
        * @return the CT Type as a const string
        */
        const char* getCtType() const { return ctType.c_str(); } ;

        /**
        * Sets a CTType object
        *
        * @param ctInfo The CTType object
        */
        void setCTType(const char *ctType) { this->ctType = ctType; };

        /**
        * Get the CT Type
        *
        * @return the CT Type as a const string
        */
        const char* getVerCT() const { return verCT.c_str(); } ;

        /**
        * Sets a CTInfo object
        *
        * @param ctInfo The CTInfo object
        */
        void setVerCT(const char *verCT) { this->verCT = verCT; };


        /**
        * Sets the field-level
        *
        * @param fieldLevel the Boolean value of FieldLevel property
        */
        void setFieldLevel(bool fieldLevel) { this->fieldLevel = fieldLevel; };

        /**
        * Gets the value of FieldLevel property
        *
        * @return true if the sender is able to apply field-level replaces for the
        *         corresponding CTType property, otherwise false
        */
        bool getFieldLevel() const { return fieldLevel;};

        /**
        * Get an array of supported properties of a given content type
        *
        * @return an array of supported properties
        */
        const ArrayList& getProperties() const {return properties;};

        /**
        * Sets an array of supported properties of a given content type
        *
        * @param properties an array of supported properties
        */

        void setProperties(ArrayList& prop) {this->properties = prop;}

        ArrayElement* clone();


};


END_NAMESPACE

/** @endcond */
#endif

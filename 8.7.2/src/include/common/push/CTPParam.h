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
 * FOR A PARTICULAR PURPOSE. See the GNU General Public License for more 
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

#ifndef INCL_CTP_PARAM
#define INCL_CTP_PARAM
/** @cond DEV */

#include "base/fscapi.h"
#include "base/util/ArrayElement.h"
#include "base/Log.h"
   
/**
* Params
*/
#define P_DEVID       0x01
#define P_USERNAME    0x02
#define P_CRED        0x03
#define P_FROM        0x04
#define P_TO          0x05
#define P_NONCE       0x06
#define P_SAN         0x07   
#define P_SLEEP       0x09   
#include "base/globalsdef.h"

BEGIN_NAMESPACE

/**
* The single CTP Param class that represents the parameter that are exchanged
* from server to client
*/

class CTPParam : public ArrayElement {

    private:
        
        /**
        * The code of the param. It could be one of the #define in the class
        */
        int8_t  paramCode;

        /**
        * The length of the value
        */
        int32_t   valueLength;

        /**
        * The byte array of the value exchanged
        */
        char* value;

         /**
        * Set the value length
        */
        void setValueLength(int32_t v) { valueLength = v; }


    public:

        /**
        * Constructor
        */
        CTPParam();

        /**
        * Desctructor
        */
        ~CTPParam();
        
        /**
        * Copy constructor
        */
        CTPParam(const CTPParam& cp);

        /**
        * Set the param code
        */
        void setParamCode(int8_t v) { paramCode = v; }            
        
        /**
        * Return the param code
        */
        int8_t getParamCode() const { return paramCode; }
                   
        /**
        * Get the value length
        */
        int32_t getValueLength() const { return valueLength; }
                               
        /**
        * Set the value: in creates an internal byte array
        * of size length. It set the variable valueLength.
        * If succeded it return the pointer to the just byte array allocated
        * If failed it returns NULL
        */
        void* setValue(const void* v, int32_t length);

        /**
        * Return the pointer to the internal byte array. The caller
        * MUST NOT free the pointer
        */
        
        void* getValue() const { return value; }

        /**
         * ArrayElement interface implementation
         */
        ArrayElement* clone();
};



END_NAMESPACE

/** @endcond */
#endif

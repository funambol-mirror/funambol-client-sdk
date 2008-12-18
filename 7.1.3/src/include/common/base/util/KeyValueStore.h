/*
 * Funambol is a mobile platform developed by Funambol, Inc. 
 * Copyright (C) 2008 Funambol, Inc.
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

#ifndef INCL_KEY_VALUE_STORE
#define INCL_KEY_VALUE_STORE
/** @cond API */
/** @addtogroup Client */
/** @{ */

#include "base/fscapi.h"
#include "base/util/Enumeration.h"
#include "base/util/StringBuffer.h"
#include "base/globalsdef.h"

BEGIN_NAMESPACE

/**
 * This is the interface for the handling of key/value pairs.
 * Some implementations might store them in some kind of background
 * storage, others might only save them transiently in memory.
 *
 * This class defines the common methods that have to be 
 * specialized by implementation on filesystem, registry, db...
 */
class KeyValueStore {

public:

    /**
     * Virtual destructor
     */
    virtual ~KeyValueStore() {}
    
    /**
     * Returns the value of the given property
     *
     * @param prop - the property name
     *
     * @return  A NULL StringBuffer in the returned implies that
     *          the property was not set. Otherwise the value it was
     *          set to is returned (which can be "", the empty string).
     */
    virtual StringBuffer readPropertyValue(const char *prop) const = 0;

    /**
     * Sets a property value.
     *
     * The value might be cached inside the implementation of this
     * interface. To ensure that it is stored persistently and to do
     * error checking, call save().
     *
     * @param prop      - the property name
     * @param value     - the property value (zero terminated string)
     *
     * @return int 0 on success, an error code otherwise
     */
    virtual int setPropertyValue(const char *prop, const char *value) = 0;
    
     /**
      * Remove a certain property 
      *
      * @param prop    the name of the property which is to be removed   
      *
      * @return int 0 on success, an error code otherwise
      */
    virtual int removeProperty(const char *prop) = 0;
    
    /**
    * Remove all the properties in the value store
    */
    virtual int removeAllProperties() = 0;

    /**
     * Get all the properties that are currently defined.     
     */
    virtual Enumeration& getProperties() = 0;

    /**
     * Ensure that all properties are stored persistently.
     * If setting a property led to an error earlier, this
     * call will indicate the failure.
     *
     * @return 0 - success, failure otherwise
     */
    virtual int close() = 0;
};


END_NAMESPACE

/** @} */
/** @endcond */
#endif

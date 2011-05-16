/*
 * Funambol is a mobile platform developed by Funambol, Inc. 
 * Copyright (C) 2003 - 2011 Funambol, Inc.
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



#ifndef INCL_CUSTOM_CONFIG
#define INCL_CUSTOM_CONFIG
/** @cond API */
/** @addtogroup Client */
/** @{ */


#include "base/globalsdef.h"
#include "base/util/StringMap.h"
#include "base/Log.h"



BEGIN_NAMESPACE

/**
 * This class groups generic config properties into a StringMap.
 * Specific set/get methods are defined for easiest access of bool/long/int params.
 * Config classes may extend CustomConfig to store additional custom properties 
 * into the extraProperties map.
 */
class CustomConfig {

protected:

    /**
    * Contains the list of extra properties that the client wants to add.
    * They are strings but useful method of conversion are provided
    */
    StringMap extraProps;


public:

    /// Constructs a new CustomConfig object
    CustomConfig();

    /// Destructor
    virtual ~CustomConfig();

    /**
    * return the reference of the current extraProps
    */
    const StringMap& getExtraProps() const { return extraProps; }

    /**
    * set a key/value couple
    */
    void setProperty(const char* propertyName, const char* propertyValue);

    /**
    * set a key/ int value couple
    */
    void setIntProperty(const char* propertyName, int propertyValue);

    /**
    * set a key/ bool value couple
    */
    void setBoolProperty(const char* propertyName, bool propertyValue);

    /**
    * set a key/ long value couple
    */
    void setLongProperty(const char* propertyName, long propertyValue);

    /**
    * get a value from a given key. 
    * @return NULL if the key is not found
    */
    const char* getProperty(const char* propertyName);


    /**
    * get a int value from a given key. 
    * @return -1 and err = false if the key is not found
    */
    int getIntProperty(const char* propertyName, bool* err);


    /**
    * get a bool value from a given key. 
    * @return false and err = false if the key is not found
    */
    bool getBoolProperty(const char* propertyName, bool* err);


    /**
    * get a long value from a given key. 
    * @return -1 and err = false if the key is not found
    */
    long getLongProperty(const char* propertyName, bool* err);

    /**
    * removes a given key. 
    * @return true if the element has been succesfully removed.
    */
    bool removeProperty(const char* propertyName);

    /**
    * Initialize this object with the given CustomConfig
    *
    * @param sc the CustomConfig object
    */
    void assign(const CustomConfig& c);


    /**
    * Assign operator
    */
    CustomConfig& operator = (const CustomConfig& c) {
        assign(c);
        return *this;
    }

    /**
     * Useful debug method to log the values in the extra prop StringMap
     */
    void printStringMap();

};


END_NAMESPACE

/** @} */
/** @endcond */
#endif

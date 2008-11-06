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

#ifndef INCL_SQL_KEY_VALUE_STORE
#define INCL_SQL_KEY_VALUE_STORE
/** @cond API */
/** @addtogroup Client */
/** @{ */

#include "base/util/KeyValueStore.h"
#include "base/util/ArrayListEnumeration.h"
#include "base/util/Enumeration.h"

BEGIN_NAMESPACE

/**
 * This abstract class represent an implementation of KeyValueStore based on a
 * generic SQL database.
 *
 * It allows the developer to specify the column to use for the key and value,
 * the table name to use for the storage, and requires to implement the methods
 * to connect/disconnect to the database, which are specific to the db driver
 * in use (odbc, etc.)
 */
class SQLKeyValueStore : public KeyValueStore {
private:
    
    StringBuffer    table,
                    colKey,
                    colValue;
         
    ArrayListEnumeration toSet;
    ArrayListEnumeration toDel;
    
protected:

    
    /*
     * Execute a query to get a value, given the key.
     *
     * The sql query passed to this function 
     *
     * @param sql   - The sql command to execute.  This MUST select the key, and
     * the value in that order, as the first 2 columns selected.  Additional
     * columns will be ignored.
     *
     * @return      - The result of the query - an Enumeration of KeyValuePair s
     */
    virtual Enumeration& query(const StringBuffer & sql) const = 0;
    
    /*
     * Execute a non-select query.
     *
     * The sql query passed to this function MUST NOT return any data (e.g. not a select)
     *
     * @param sql   - The sql command to execute.
     *
     * @return      - Success or Failure
     */
    virtual int execute(const StringBuffer & sql) = 0;
    
    /*
     * Get the name of the key column
     *
     * @return      - A StringBuffer with the name of the column used for the key
     */
    virtual StringBuffer sqlColKey() const;
    
    /*
     * Get the name of the value column
     *
     * @return      - A StringBuffer with the name of the column used for the value
     */
    virtual StringBuffer sqlColValue() const;

    /*
     * Get the query to remove a property
     *
     * @param key   - The key to be removed
     *
     * @return      - A StringBuffer containing the query
     */
    virtual StringBuffer sqlRemovePropertyString(const StringBuffer & key) const;

    /*
     * Get the query to set a property
     *
     * @param key   - The key to have its value updated
     * @param value - The value to be saved
     *
     * @return      - A StringBuffer containing the query
     */
    virtual StringBuffer sqlSetPropertyString(const StringBuffer & key, const StringBuffer & value) const;
    
    /*
     * Get the query to get a property
     *
     * @param key   - The key to retreive
     *
     * @return      - A StringBuffer containing the query
     */
    virtual StringBuffer sqlGetPropertyString(const StringBuffer & key) const;
    
    /*
     * Get the query to get all properties
     *
     * @return      - A StringBuffer containing the query
     */
    virtual StringBuffer sqlGetAllString() const;
        
    /*
     * Get the query to count properties
     *
     * @return      - A StringBuffer containing the query
     */
    virtual StringBuffer sqlCountAllString() const;
    
public:
    
    /*
     * Constructor
     *
     * @param table     - The table to be used
     * @param colKey    - The column of the key
     * @param colValue  - The column of the value
     *
     */
    SQLKeyValueStore(const StringBuffer & table, const StringBuffer & colKey, const StringBuffer & colValue);
    
    /*
     * Destructor
     *
     * Subclasses MUST call disconnect() at the same at which
     * disconnect() is defined
     */
    virtual ~SQLKeyValueStore();
    
    /*
     * Connect to the database server.  The connection should be stored
     * within the subclass and destroyed in disconnect();  If connect is called
     * while a connection exists, nothing should happen.
     *
     * @return      - Success or Failure
     */
    virtual int connect() = 0;
    
    /*
     * Disconnect from the database server.  If the connection is not open,
     * do nothing.
     *
     * @return      - Success or Failure
     */
    virtual int disconnect() = 0;
    
    /*
     * Returns the value of the given property
     *
     *@param prop - the property name
     *
     *@return   A NULL StringBuffer in the returned implies that
     *          the property was not set. Otherwise the value it was
     *          set to is returned (which can be "", the empty string).
     */
    virtual StringBuffer readPropertyValue(const char *prop) const;

    /*
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
    virtual int setPropertyValue(const char *prop, const char *value);
    
     /**
     * Remove a certain property 
     *
     * @param prop    the name of the property which is to be removed   
     *
     * @return int 0 on success, an error code otherwise
     */
    virtual int removeProperty(const char *prop);
     
    /**
     * Get all the properties that are currently defined.     
     */
    virtual Enumeration& getProperties() const;

    /**
     * Ensure that all properties are stored persistently.
     * If setting a property led to an error earlier, this
     * call will indicate the failure.
     *
     * @return 0 - success, failure otherwise
     */
    //virtual int close(){};
    /**
     * Remove all properties
     *
     * @return int 0 on success, an error code otherwise
     */
     virtual int removeAllProperties();
};


END_NAMESPACE

/** @} */
/** @endcond */
#endif

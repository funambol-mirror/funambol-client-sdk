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

#ifndef INCL_ODBC_KEY_VALUE_STORE
#define INCL_ODBC_KEY_VALUE_STORE
/** @cond API */
/** @addtogroup Client */
/** @{ */


#include "base/util/KeyValueStore.h"
#include "client/SQLKeyValueStore.h"

BEGIN_NAMESPACE

class ODBCKeyValueStore : public SQLKeyValueStore {
private:
    
    // Connection * conn
    
protected:
    
    /*
     * Execute a query to get a value, given the key.   If a connection to
     * the database is not open, open it. 
     *
     * @param sql   - The sql command to execute.
     *
     * @return      - The result of the query - an Enumeration of KeyValuePair s
     */
    virtual ArrayListEnumeration * query(const StringBuffer & sql) const;
    
    /*
     * Execute a non-select query.  If a connection to the database is not open,
     * open it.
     *
     * @param sql   - The sql command to execute.
     *
     * @return      - Success or Failure
     */
    virtual bool execute(const StringBuffer & sql);
    
public:
    
    /*
     * Constructor
     *
     * @param uri       - The location of the server
     * @param database  - The database name
     * @param table     - The table to be used
     * @param username  - The username for authentication
     * @param password  - The password for authentication
     *
     */
    ODBCKeyValueStore(const char * uri, const char * database, const char * table, const char * username, const char * password);
    
    /*
     * Destructor
     *
     * Subclasses MUST disconnect in their own destructors
     */
    virtual ~ODBCKeyValueStore();
    
    /*
     * Connect to the database server.  The connection should be stored
     * within the subclass and destroyed in disconnect();  If connect is called
     * while a connection exists, nothing should happen.
     *
     * @return      - Success or Failure
     */
    bool connect();
    
    /*
     * Disconnect from the database server.  If the connection is not open,
     * do nothing.
     *
     * @return      - Success or Failure
     */
    bool disconnect();
};


END_NAMESPACE

/** @} */
/** @endcond */
#endif

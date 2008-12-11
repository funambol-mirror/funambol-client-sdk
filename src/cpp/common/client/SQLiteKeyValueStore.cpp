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

#include <sqlite3.h>

#include "client/SQLiteKeyValueStore.h"
#include "base/util/StringBuffer.h"
#include "base/util/KeyValuePair.h"
#include "base/Log.h"

    
BEGIN_NAMESPACE
    
SQLiteKeyValueStore::SQLiteKeyValueStore(const StringBuffer & table, const StringBuffer & colKey,
                                         const StringBuffer & colValue,
                                         const StringBuffer & thePath, bool isTransactional)
: SQLKeyValueStore(table,colKey,colValue), enumeration(*this), path(thePath)
{
    db = NULL;
    statement = NULL;
    
    this->isTransactional = isTransactional; 
    
    //bool toInit =!checkIfTableExists(table);
    bool toInit = true;
    if(connect() != SQLITE_OK) {
        LOG.error("SQLiteKeyValueStore: cannot connect to database.");
    }
    if(toInit){
        if(init(table,colKey,colValue,thePath) != SQLITE_OK){
            LOG.error("SQLiteKeyValueStore: cannot create database.");
        }
    }
}


SQLiteKeyValueStore::~SQLiteKeyValueStore()
{
    if (statement)
    {
        sqlite3_finalize(statement);
        statement = NULL;
    }
    disconnect();
}

Enumeration& SQLiteKeyValueStore::query(const StringBuffer & sql) const
{
    if (!db)
    {
        throw new StringBuffer /*Exception*/ ("Not connected to database");
    }
    
    if (statement)
    {
        sqlite3_finalize(statement);
        statement = NULL;
    }
    
    int ret = sqlite3_prepare_v2(db, sql.c_str(), sql.length(), &statement, NULL);
    if (ret == SQLITE_OK) {
        enumeration.reinit(sqlite3_step(statement));
    } else {
        LOG.error("Unable to prepare sqlite query in query(): %s", sql.c_str());
    }
    
    if (ret != SQLITE_OK) {
        LOG.error("SQLite execute failed: %s", sqlite3_errmsg(db));
    }
    
    return enumeration;
}

int SQLiteKeyValueStore::execute(const StringBuffer & sql)
{
    connect();

    sqlite3_stmt * stmt;
    
    int ret = sqlite3_prepare_v2(db, sql.c_str(), sql.length(), &stmt, NULL);
    if (ret == SQLITE_OK) {
        ret = sqlite3_step(stmt);
    } else {
        LOG.error("Unable to prepare sqlite query in execute(): %s", sql.c_str());
    }
    
    sqlite3_finalize(stmt);
    
    if (ret == SQLITE_DONE)
        ret = SQLITE_OK;
        
    if (ret != SQLITE_OK) {
        LOG.error("SQLite execute failed: %s", sqlite3_errmsg(db));
    }
        
    return ret;
}

/**
 * Get all the properties that are currently defined.     
 */
Enumeration& SQLiteKeyValueStore::getProperties()
{
    StringBuffer sqlQuery = sqlCountAllString();
    Enumeration& en = query(sqlQuery);
    int rows = 0;
    if (en.hasMoreElement())
    {
        KeyValuePair* kvp = (KeyValuePair*)en.getNextElement();
        sscanf(kvp->getValue(), "%d", &rows);
    }
    if (rows == 0)
        return en;
    
    sqlQuery = sqlGetAllString();
    SQLiteKeyValueStoreEnumeration& en2 = (SQLiteKeyValueStoreEnumeration&)query(sqlQuery);
    en2.setTotalRows(rows);
    
    return en;
}


int SQLiteKeyValueStore::connect()
{
    if (db)
        return true;
        
    int ret = sqlite3_open(path, &db);
    if (ret == SQLITE_OK )
    {   
        if(isTransactional){
            return execute("BEGIN TRANSACTION;");
        }
    }
    return ret;
}


int SQLiteKeyValueStore::disconnect()
{
    if (db == NULL)
        return true;
    if (isTransactional){
        execute("ROLLBACK TRANSACTION;");
    }
    
    
    int ret = sqlite3_close(db);
    db = NULL;
    
    return ret;
}

int SQLiteKeyValueStore::close()
{
    if (statement)
    {
        sqlite3_finalize(statement);
        statement = NULL;
    }
    if(isTransactional){
        return (
                ((execute("COMMIT TRANSACTION;") == SQLITE_OK) && (execute("BEGIN TRANSACTION;") == SQLITE_OK))
                ? 0 : 1);
    }else{
        return 0;
    }
    return 0;
}


bool SQLiteKeyValueStore::checkIfTableExists(const char* tableName){
/*    StringBuffer sql;
    sql.sprintf("select count (*) from sqlite_master where type='table' and name='%s';", tableName);
    LOG.debug("SQLiteKeyValueStore::checkIfTableExists with sql query %s", sql.c_str());
    Enumeration& res = query(sql);
    bool ret;
    if (res.hasMoreElement())
    {
        KeyValuePair* kvp = (KeyValuePair*)res.getNextElement();
        StringBuffer val = kvp->getValue();
        ret = val.icmp("1");
        delete kvp;
    }*/
    
    FILE* f = fopen(path.c_str(), "r");
    if ( !f ) {
        return false;
    }
    
    return true;
}

int SQLiteKeyValueStore::init(const char* table, const char* colKey,   const char* colValue,
                          const char* path){
    StringBuffer sql;
            
    sql.sprintf("CREATE TABLE  %s (%s TEXT PRIMARY KEY, %s TEXT)", table, colKey, colValue);        
    connect();
    //execute(sql);
    if (db == NULL){
        return 1;
    }
    sqlite3_exec(db,sql.c_str(), NULL, NULL, NULL);
    return close();
}

END_NAMESPACE


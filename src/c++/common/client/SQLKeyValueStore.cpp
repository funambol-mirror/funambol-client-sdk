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

#include "base/util/KeyValuePair.h"
#include "client/SQLKeyValueStore.h"

BEGIN_NAMESPACE

StringBuffer SQLKeyValueStore::sqlColKey() const
{
    return colKey;
}

StringBuffer SQLKeyValueStore::sqlColValue() const
{
    return colValue;
}

StringBuffer SQLKeyValueStore::sqlRemovePropertyString(const StringBuffer & key) const
{
    StringBuffer sb("");
    sb.append("DELETE FROM ").append(table).append(" WHERE ").append(sqlColKey()).append("='").append(key).append("'");// LIMIT 1");
    return sb;
}

StringBuffer SQLKeyValueStore::sqlSetPropertyString(const StringBuffer & key, const StringBuffer & value) const
{
    StringBuffer sb("");
    sb.append("INSERT OR REPLACE INTO ").append(table).append(" (").append(sqlColKey()).append(",").append(sqlColValue()).append(") VALUES ('").append(key).append("','").append(value).append("')");
    return sb;
}

StringBuffer SQLKeyValueStore::sqlGetPropertyString(const StringBuffer & key) const
{
    StringBuffer sb("");
    sb.append("SELECT ").append(sqlColKey()).append(",").append(sqlColValue()).append(" FROM ").append(table).append(" WHERE ").append(sqlColKey()).append("='").append(key).append("' LIMIT 1");
    return sb;
}

StringBuffer SQLKeyValueStore::sqlGetAllString() const
{
    StringBuffer sb("");
    sb.append("SELECT ").append(sqlColKey()).append(", ").append(sqlColValue()).append(" FROM ").append(table);
    return sb;
}

StringBuffer SQLKeyValueStore::sqlCountAllString() const
{
    StringBuffer sb("");
    sb.append("SELECT count(").append(sqlColKey()).append("), count(").append(sqlColValue()).append(") FROM ").append(table);
    return sb;
}

SQLKeyValueStore::SQLKeyValueStore(const StringBuffer & table, const StringBuffer & colKey, const StringBuffer & colValue)
{
    this->table    = table;
    this->colKey   = colKey;
    this->colValue = colValue;
}

SQLKeyValueStore::~SQLKeyValueStore()
{
}

/*
 * Returns the value of the given property
 *
 *@param prop - the property name
 *
 *@return   A NULL StringBuffer in the returned implies that
 *          the property was not set. Otherwise the value it was
 *          set to is returned (which can be "", the empty string).
 */
StringBuffer SQLKeyValueStore::readPropertyValue(const char *prop) const
{
    
    StringBuffer sqlQuery = sqlGetPropertyString(StringBuffer(prop));
    Enumeration& en = query(sqlQuery);
        
    if (en.hasMoreElement())
    {
        KeyValuePair * kvp = (KeyValuePair*)(en.getNextElement());
        return kvp->getValue();
    }
    return StringBuffer(NULL);
}

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
int SQLKeyValueStore::setPropertyValue(const char *prop, const char *value)
{
    return execute(sqlSetPropertyString(prop, value));
}

 /**
 * Remove a certain property 
 *
 * @param prop    the name of the property which is to be removed   
 *
 * @return int 0 on success, an error code otherwise
 */
int SQLKeyValueStore::removeProperty(const char *prop)
{
    return execute(sqlRemovePropertyString(prop));
}
 
/**
 * Get all the properties that are currently defined.     
 */
Enumeration& SQLKeyValueStore::getProperties() const
{
    StringBuffer sqlQuery = sqlGetAllString();
    Enumeration& en = query(sqlQuery);
    return en;
}



END_NAMESPACE


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

package com.funambol.syncclient.sps.common;

import java.util.Vector;
import java.util.Date;
import java.util.Hashtable;


/**
 * This class create DataStore
 * and provide method to call new record
 *
 *
 * @version $Id: DataStore.java,v 1.3 2007-12-22 18:09:17 nichele Exp $
 */
public abstract class DataStore {

    // ------------------------------------------------------------ Private data

    private String dataStoreName          = null;
    private RecordMetadata recordMetadata = null;
    private Hashtable dataStoreProperties = null;

    //------------------------------------------------------------- Constructors

    public DataStore (String dataStoreName, RecordMetadata recordMetadata) {
        this.dataStoreName = dataStoreName;
        this.recordMetadata = recordMetadata;
    }

    //----------------------------------------------------------- Public methods

    /** Getter for property dataStoreProperties
     * @return Value of Data Store Properties
     */
    public Hashtable getDataStoreProperties() {
        return this.dataStoreProperties;
    }

    /** Setter for property dataStoreProperties.
     * @param dataStoreProperties New value of data Store Properties.
     */
    public void setDataStoreProperties(Hashtable dataStoreProperties) {
        this.dataStoreProperties = dataStoreProperties;
    }


    public String getDataStoreName() {

        return this.dataStoreName;

    }

    public RecordMetadata getRecordMetadata() {

        return this.recordMetadata;

    }

    /**
     * create record
     *
     * @param key
     * @return record
     * @throws DataAccessException
     **/
    public abstract Record newRecord(String key)
    throws DataAccessException;

    /**
     * read record
     *
     * @param record
     * @throws DataAccessException
     **/
    public abstract Record readRecord(Record record)
    throws DataAccessException;

    /**
     * store record
     *
     * @param record
     * @throws DataAccessException
     **/
    public abstract Record storeRecord(Record record)
    throws DataAccessException;

    /**
     * delete record
     *
     * @param record
     * @throws DataAccessException
     **/
    public abstract void deleteRecord(Record record)
    throws DataAccessException;

    /**
     * return alla records of dataStore
     *
     * @return find records
     * @throws DataAccessException
     **/
    public abstract Vector findAllRecords()
    throws DataAccessException;

    /**
     * return a Vector of Record
     * found by state, last timestamp
     *
     * @param state state of record
     * @param since last timestamp
     * @return find records
     * @throws DataAccessException
     **/
    public abstract Vector findRecords(char state, Date since)
    throws DataAccessException;

    /**
     * return records of dataStore find by spsRecordFilter
     *
     * @param recordFilter filter
     * @return find records
     * @throws DataAccessException
     **/
    public abstract Vector findRecords(RecordFilter recordFilter)
    throws DataAccessException;

    /**
     * @return datastore next key
     * @throws DataAccessException
     **/
    public abstract long getNextKey()
    throws DataAccessException;

    /**
     * Method define start DB operations
     * @throws DataAccessException
     */
    public abstract void startDBOperations()
    throws DataAccessException;

    /**
     * Method define end DB operations
     * @throws DataAccessException
     */
    public abstract void commitDBOperations()
    throws DataAccessException;;

}

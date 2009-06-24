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

/**
 * This interface define Record method
 *
 *
 * $Id: Record.java,v 1.3 2007-12-22 18:09:17 nichele Exp $
 */
public class Record {

    //-------------------------------------------------------------- Private data

    private Object[] data;
    private DataStore dataStore;
    private int positionKeyField;


    //------------------------------------------------------------- Constructors
    public Record(DataStore dataStore, String key) {

        String function = null;
        Vector positions = null;

        RecordMetadata recordMetadata = null;

        this.data = new Object[dataStore.getRecordMetadata().getFieldMetadata().length];

        this.dataStore = dataStore;

        recordMetadata = this.dataStore.getRecordMetadata();

        positions = recordMetadata.getRecordFieldPosition("key");

        positionKeyField = ((Integer) positions.elementAt(0)).intValue();

        this.setField(positionKeyField, key);

    }


    //----------------------------------------------------------- Public methods

    /**
     * set record field
     *
     * @param i position [1...n]
     * @param dataField
     **/
    public void setField(int i, String dataField) {
        this.data[i - 1] = dataField;
    }

    /**
     * set key
     *
     * @param key
     **/
    public void setKey(String key) {
        this.data[positionKeyField] = (Object) key;
    }

    /**
     * set dataStore
     *
     * @param dataStore
     **/
    public void setDataStore(DataStore dataStore) {
        this.dataStore = dataStore;
    }

    /**
     * return key
     *
     **/
    public String getKey() {
        return (String) this.getString(positionKeyField);
    }

    /**
     * return dataStore
     *
     **/
    public DataStore getDataStore() {
        return this.dataStore;
    }

    /**
     * read String field
     *
     * @param i position [1...n]
     * @return field
     **/
    public String getString(int i) {
        return (String) this.data[i - 1];
    }

    /**
     * read int field
     *
     * @param i position [1...n]
     * @return field
     **/
    public int getInt(int i) {
        return ((Integer) this.data[i - 1]).intValue();
    }

    /**
     * @return record length
     **/
    public int getLength() {
        return this.data.length;
    }

    /**
     * @return position key field
     **/
    public int getPositionKeyField() {
        return this.positionKeyField;
    }

    /**
     * @param function
     * @return values of fields about setting function
     **/
    public Vector getRecordField(String function) {

        Vector positions = null;
        Vector values    = new Vector();

        positions = this.dataStore.getRecordMetadata().getRecordFieldPosition(function);

        Integer pos = null;

        int l = positions.size();

        for (int i=0; i < l; i++) {
            values.addElement(this.getString(((Integer) positions.elementAt(i)).intValue()));
        }

        return values;

    }

    /**
     * @param function
     * @param values values to set fields about setting function
     **/
    public void setRecordField(String function, Vector values) {

        Vector positions = null;

        positions = this.dataStore.getRecordMetadata().getRecordFieldPosition(function);

        int l = positions.size();

        for (int i=0; i < l; i++) {
            this.setField(((Integer) positions.elementAt(i)).intValue(), (String) values.elementAt(i));
        }

       return;

    }

}

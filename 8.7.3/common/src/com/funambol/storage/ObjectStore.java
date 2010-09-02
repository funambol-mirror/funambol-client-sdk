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

package com.funambol.storage;

import java.io.ByteArrayInputStream;
import java.io.ByteArrayOutputStream;
import java.io.DataInputStream;
import java.io.DataOutputStream;
import java.io.IOException;
import java.util.Enumeration;

import javax.microedition.rms.InvalidRecordIDException;
import javax.microedition.rms.RecordEnumeration;
import javax.microedition.rms.RecordStore;
import javax.microedition.rms.RecordStoreException;
import javax.microedition.rms.RecordStoreNotOpenException;

import com.funambol.util.Log;

/** 
 * This class uses the J2ME RMS to store and retrieve objects
 * using the rms positional access.
 *
 * To persist an object using ObjectStore, it must implement the
 * com.funambol.storage.Serializable interface.
 * 
 */
public class ObjectStore {

    // ---------------------------------------------- Attributes
    protected AbstractRecordStore rs;
    private RecordEnumeration re;
    private ObjectFilter of;
    private ObjectComparator oc;
    
    protected final Object mutex = new Object();

    protected String name = null;
    
    // ---------------------------------------------- Constructors
    /** 
     * Creates a new instance of ObjectStore.
     */
    public ObjectStore() {
        rs = null;
        re = null;
    }

    /**
     * Creates a new instance of ObjectStore given the record store name.
     */
    public ObjectStore(String name) {
        this();
        this.name = name;
    }
    
    /**
     * Get all object of the ObjectStore
     * @param s Serializable Object to be returned
     * @return Enumeration of Serializable object found 
     */
    public Enumeration getObjects(Serializable s) {
        return new ObjectEnumeration(this.rs, this.of, this.oc, s);
    }
    /**
     * @return Store name
     */
    public String getName() {
        return name;
    } 

    /** 
     * Open an existing RecordStore, or throws an exception if not present.
     * If the name is the same of the currently open one, no action is
     * taken, otherwise the old one is closed.
     *
     * @param name is the name of the RecordStore to be managed 
     * @return true if the record store has been open or created
     *         false if it was cached
     */
    public boolean open(String name) throws RecordStoreException {
        return openStore(name, false);
    }

    /**
     * Open the current RecordStore, or throws an exception if not present.
     *
     * @return true if the record store has been open or created
     *         false if it was cached
     */
    public boolean open() throws RecordStoreException {
        return openStore();
    }
    
    /** 
     * Creates a new RecordStore, or open an existing one.
     *
     * @param name is the name of the RecordStore to be managed
     * @return true if the record store has been open or created
     *         false if it was cached
     * 
     */
    public boolean create(String name) throws RecordStoreException {
        return openStore(name, true);
    }
    
    /**
     * private method used by open and create to share code.
     */
    protected synchronized boolean openStore(String name, boolean create)
    throws RecordStoreException  {
        // Check if is requested to open a new record store
        synchronized(mutex) {
            if(rs != null){
                if(rs.getName().equals(name)){
                    return false; // equal: keep the old one
                }
                else {
                    close();
                }
            }
            rs = AbstractRecordStore.openRecordStore(name, create);
            this.name = name;
        }
        return true;
    }

    /**
     * private method used by open and create to share code.
     */
    protected synchronized boolean openStore() throws RecordStoreException  {
        synchronized(mutex) {
            if(name != null) {
                rs = AbstractRecordStore.openRecordStore(name, false);
            } else {
                return false;
            }
        }
        if(rs != null) {
            return true;
        } else {
            return false;
        }
    }

    /** 
     * Close the current RecordStore, if open.
     *
     */
    public synchronized void close() throws RecordStoreException {
        synchronized(mutex) {
            if(rs != null){
                rs.closeRecordStore();
            }
            rs = null;
            re = null;
        }
    }

    /**
     * Removes this object store. The store is closed before trying to remove
     * it.
     */
    public synchronized void remove() throws RecordStoreException {
        synchronized(mutex) {
            if (rs != null) {
                rs.closeRecordStore();
            }
            rs = null;
            re = null;
            AbstractRecordStore.deleteRecordStore(name);
        }
    }
    
    /**
     * Return the number of records in this ObjectStore
     *
     * @return the number of records present or -1 if the ObjectStore is not
     *         open.
     */
    public int size() {
        int ret = 0;
        try {
            if(rs != null) {
                ret = rs.getNumRecords();
            } else {
                return -1;
            }
        } catch (RecordStoreNotOpenException ex) {
            Log.error("Can't get size of ObjectStore: recordstore not open.");
            ret = -1; 
        }
        return ret;
    }

    /**
     *  Get the first valid index in the record store.
     *
     *  @return the first valid index in the record store, or -1 if empty.
     */
    public int getFirstIndex() throws RecordStoreException {
        //TODO: could we use false here to improve performances?
        re = rs.enumerateRecords(null, null, true);
        return getNextIndex();
    }

    /**
     *  Get the next valid index in the record store.
     *
     *  @return the next valid index in the record store, or -1 if no
     *          more record are present.
     */
    public int getNextIndex() throws RecordStoreException {

        if(re != null && re.hasNextElement()){
            return re.nextRecordId();
        }
        else {
            return -1;
        }
    }

    /**
     *  Store the serializable object in a new record.
     *
     *  @param obj the serializable object
     *  @return the index in the recordstore
     */
    public int store(Serializable obj) throws RecordStoreException, IOException {
        ByteArrayOutputStream byteStream = new ByteArrayOutputStream();
        DataOutputStream out = new DataOutputStream(byteStream);

        obj.serialize(out);

        byte[] data = byteStream.toByteArray();
        int ret = rs.addRecord(data, 0, data.length);
        obj = null;
        data = null;
        byteStream = null;
        out = null;
        return ret; 
    }

    /**
     * Creates an empty record. Empty records are allowed by RMS as records with
     * 0 lenght.
     *
     * @return the index in the recordstore
     */
    public int createEmptyRecord() throws RecordStoreException, IOException {
        int ret = rs.addRecord(null, 0, 0);
        return ret;
    }

    /**
     *  Store the serializable object in an existent record.
     *
     *  @param obj the serializable object
     *  @return the index in the recordstore
     */
    public int store(int index, Serializable obj)
    throws RecordStoreException, IOException {
        ByteArrayOutputStream byteStream = new ByteArrayOutputStream();
        DataOutputStream out = new DataOutputStream(byteStream);

        obj.serialize(out);

        byte[] data = byteStream.toByteArray();
        rs.setRecord(index, data, 0, data.length);
        obj = null;
        return index;
    }
    
    /**
     *  Retrieve the serialize object from the record store.
     *
     *  @param obj the serializable object
     *  @param index the index in the recordstore
     * 
     *  @return a reference to the object, or null if the object is
     *          empty in the store.
     *
     * @throws RecordStoreException if the record is invalid
     * @throws IOException if the store is not accessible
     */
    public Serializable retrieve(int index, Serializable obj)
    throws RecordStoreException, IOException {
        byte data[] = rs.getRecord(index);

        if (data != null) {
            ByteArrayInputStream dataStream = new ByteArrayInputStream(data);
            DataInputStream in = new DataInputStream(dataStream);

            obj.deserialize(in);
        }
        else {
            return null;
        }

        return obj;
    }

    /**
     * Retrieve the DataInputStream corresponding to a record.
     *
     * @param index the index in the record store
     * @return an input stream that allows the record content reading (maybe
     *         empty if the record id empty)
     *
     * @throws RecordStoreException if the record is invalid
     * @throws IOException if the store is not accessible
     */
    public DataInputStream retrieveBytes(int index)
    throws RecordStoreException, IOException {
        byte data[] = rs.getRecord(index);
        if (data != null) {
            ByteArrayInputStream dataStream = new ByteArrayInputStream(data);
            DataInputStream in = new DataInputStream(dataStream);

            return in;
        } else {
            return null;
        }
    }
        

    /**
     *  Remove the object from the store.
     *
     *  @param index the index in the recordstore
     */
    public void remove(int index) throws RecordStoreException {
        rs.deleteRecord(index);
    }
    
    /**
     *  Returns the amount of additional room (in bytes) available for this
     *  record store to grow.
     *
     *  @return the amount of storage left for this store.
     */
    public int getAvaliableStorage() {
        try {
            return rs.getSizeAvailable();

        } catch(RecordStoreNotOpenException e) {
            // Should not happen
            e.printStackTrace();
            Log.error("ObjectStore.getAvaliableStorage: "+e.toString());
            return 0;
        }
    }

    /**
     * Add a RecordListener to the recordStore
     */
    public void addStoreListener(ObjectStoreListener listener) {
//        rs.addRecordListener(listener);
        //Does not appear to be currently used
        throw new RuntimeException("addStoreListener() not implemented");
    }
    
    /**
     * Add a RecordListener to the recordStore
     */
    public void removeStoreListener(ObjectStoreListener listener) {
//        rs.removeRecordListener(listener);
        //Does not appear to be currently used
        throw new RuntimeException("removeStoreListener() not implemented");
    }
    
    /**
     * Set Filter for this ObjectStore
     */
    public void setObjectFilter(ObjectFilter newOf) {
        this.of = newOf;
    }
    
    /**
     * Remove Filter after usage
     */
    public void removeObjectFilter() {
        this.of = null;
    }
    
    /**
     * Set Comparator for this ObjectStore
     */
    public void setObjectComparator(ObjectComparator newOc) {
        this.oc = newOc;
    }
    
    /**
     * Set Comparator for this ObjectStore
     */
    public void removeObjectComparator() {
        this.oc = null;
    }
}


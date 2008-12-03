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

/**
 * This class provides a storage mechanism for BlackBerry devices. Its
 * intent is to circumvent the 64k limit of javax.microedition.rms.RecordStore
 * on BlackBerry devices. It exposes methods of similar name 
 * to provide an interface similar to that of RMS. The BlackBerry model differs 
 * from the RMS model in that it isnt persistent. Changes made to a record are 
 * not automatically reflected in the repository. Consequentially, commits must be 
 * explicitly made after every transaction for the entire datastructure, and not just 
 * the affected record. There may be room for performance gains by using ObjectGroups 
 * and altering the implementation. 
 */

package com.funambol.storage;

import java.util.Enumeration;
import java.util.Vector;

import javax.microedition.rms.InvalidRecordIDException;
import javax.microedition.rms.RecordComparator;
import javax.microedition.rms.RecordEnumeration;
import javax.microedition.rms.RecordFilter;
import javax.microedition.rms.RecordStoreException;
import javax.microedition.rms.RecordStoreFullException;
import javax.microedition.rms.RecordStoreNotFoundException;
import javax.microedition.rms.RecordStoreNotOpenException;

import net.rim.device.api.system.Memory;
import net.rim.device.api.system.PersistentObject;
import net.rim.device.api.system.PersistentStore;
import net.rim.device.api.system.RuntimeStore;
import net.rim.device.api.util.Arrays;
import net.rim.device.api.util.Comparator;
import net.rim.device.api.util.IntEnumeration;
import net.rim.device.api.util.IntHashtable;
import net.rim.device.api.util.IntVector;
import net.rim.device.api.util.Persistable;

public class BlackberryRecordStore extends AbstractRecordStore {

    private final int ADDED = 1;

    private final int DELETED = 2;

    private final int CHANGED = 3;

    private Vector blackberryRecordListeners;

    private boolean closed;

    //Placeholder value for null since we are not allowed to hold null in the underlying data structure.
    private final byte[] IAMNULL = "IAMNULL".getBytes();

    //Name of this store.
    private String name;

    // RecordIds start at 1, according to the spec for recordstore
    // At the moment it is quite simple to synchronize access to this field. The
    // field is only written in the addRecord which is a synchronized method,
    // therefore we do not need any monitor on this object.
    private int nextRecordId;

    // Used to keep track of open references to this record store. 
    // We use an object because we need to associate a lock to it to
    // handle concurrency
    // This field is manipulated by the open and close methods. Therefore we
    // need to have a monitor on it. The monitor used is MutexStore. For each
    // store we have a mutex that is used to manipulate the global store status
    // (not its records). Before reading/writing the count it is necessary to
    // get the mutex.
    private int openCount = 0;

    //Our underlying data structure contain int-value mappings
    //This field mantains the records in the recordstore. For each store we have
    //a set of records. It is critical to preserve data integrity in multi
    //threaded situations. Any method accessing (read/write) this data structure
    //has to acquire a lock on it (by synchronizing on it)
    private IntHashtable records;
    
    //Each time the recordstore is modified, version is incremented
    private long version;

    /**
     * This is the list of mutex for the datastores. These mutex are used for
     * opening/closing datastores.
     */
    private static IntHashtable allMutex = new IntHashtable(10);
    
    public static synchronized void init(ObjectWrapperHandler owh) {
        PersistentStoreManager.setObjectWrapperHandler(owh);
    }
    
    /**
     * Deletes the named record store.
     * @param recordStoreName Name of the record store to delete
     * @throws RecordStoreNotFoundException If the record store could not be found
     * @throws RecordStoreException If the record store could not be found
     */
    static public synchronized void deleteRecordStore( String recordStoreName )
    throws RecordStoreException, RecordStoreNotFoundException {

        StoreMutex mutex = getStoreMutex( recordStoreName );
        synchronized(mutex) {
            if (!PersistentStoreManager.exists( recordStoreName )) {
                throw new RecordStoreNotFoundException( "The record store " + recordStoreName + " was not found" );
            }
 
            //Remove from runtime store first.
            if (isStoreLoaded( recordStoreName )) {
                RuntimeStore.getRuntimeStore().remove( recordStoreName.hashCode() );
            }
            PersistentStoreManager.removeObject( recordStoreName );
        }
    }

    /**
     * Obtains the names of contained BlackberryRecordStore objects. 
     * @return  Array of strings containing the names of stores
     */
    static public synchronized String[] listRecordStores() {

        Vector list = PersistentStoreManager.getNames();
        String[] ret = new String[list.size()];
        list.copyInto( ret );
        return ret;
    }


    /**
     * Open and possibly create a record store.
     * @param recordStoreName The name of the record store to open
     * @param createIfNecessary Whether or not we create the store if it does not exist
     * @return A store object
     * @throws RecordStoreException If a record store related exception occured
     * @throws RecordStoreFullException - If the record store is full
     * @throws RecordStoreNotFoundException - If the record store could not be found
     */
    static public synchronized AbstractRecordStore
    openRecordStore( String recordStoreName, boolean createIfNecessary )
    throws RecordStoreException, RecordStoreFullException, RecordStoreNotFoundException {
    //System.out.println("BBRecordstore: openrecordstore");
                    
        BlackberryRecordStore store = null;
        // We may have concurreny between open/close. Both methods update the
        // open counter and the RuntimeStore. We keep a lock on the open count
        // and this synchronize both objects

        StoreMutex mutex = getStoreMutex( recordStoreName );
        synchronized (mutex) {
            //System.out.println("BBRecordstore: entering syncronized code");
            
            try {
                if (PersistentStoreManager.exists( recordStoreName )) {
                    //System.out.println("BBRecordstore: store exists in persistentstoremanager");
                    if (!isStoreLoaded( recordStoreName )) {
                      //  System.out.println("BBRecordstore: store is not loaded");
                        IntHashtable recs;
                        recs = (IntHashtable)PersistentStoreManager.getObject(recordStoreName);
                       // System.out.println("BBRecordstore: opening store");
                        store = new BlackberryRecordStore(recordStoreName, recs);
                       // System.out.println("BBRecordstore: putting store in runtimestore");
                         try {
                        RuntimeStore.getRuntimeStore().put( recordStoreName.hashCode(), store );
                        } catch (IllegalArgumentException e ) {
                            // this means that a recordstore with given hash exists. 
                            // we can go on and live happy!
                       //     System.out.println("BBRecordstore: store it's already there! ");
                        /*    store.setOpenCount( store.getOpenCount() + 1 );
                            return store;*/
                        }
                        }
                    else {
                        //System.out.println("BBRecordstore: store is loaded");
                        RuntimeStore rs = RuntimeStore.getRuntimeStore();
                        //System.out.println("BBRecordstore: getting recordstore with hashcode " + recordStoreName.hashCode());
                        store = (BlackberryRecordStore) rs.get(recordStoreName.hashCode());

                        //Increment the open counter by 1. close() will be called an equal number of times before 
                        //the instance is flagged as closed.
                    }
                }
                else if (createIfNecessary) {
                    //System.out.println("BBRecordstore: store does not exists in persistentstoremanager, adding it to persistent store");
                    PersistentStoreManager.addPersistedObject( recordStoreName, new IntHashtable() );
                    //System.out.println("BBRecordstore: getting object from persistent");
                    IntHashtable list = (IntHashtable) PersistentStoreManager.getObject( recordStoreName );
                   //System.out.println("BBRecordstore: creating store with hashtable");
                    store = new BlackberryRecordStore( recordStoreName, list );
                    //System.out.println("BBRecordstore: putting store into runtime store");
                    try {
                    RuntimeStore.getRuntimeStore().put( recordStoreName.hashCode(), store );
                    } catch (IllegalArgumentException e ) {
                        
                        // this means that a recordstore with given hash exists. 
                        // we can go on and live happy!
                     //   System.out.println("BBRecordstore: store it's already there!");
                        store.setOpenCount( store.getOpenCount() + 1 );
                        return store;
                        
                        
                    }
                }
                else {
                    throw new RecordStoreNotFoundException( "openRecordSore() " +
                            "requested a store but none was found. createIfNecessary is false" );
                }
            } catch (Exception e) {
                throw new RecordStoreException("Error opening record store " + e.toString());
            }

            store.setOpenCount( store.getOpenCount() + 1 );
        }
        return store;
    }
    
    /**
     * Determines if the named store is loaded in the RuntimeStore. This method
     * uses the global RuntimeStore information and therefore must always be
     * invoked in a synchronized context (on MutexStore)
     *
     * @param name Name of the store to check
     * @return True if store is found. False if not.
     */
    static private synchronized boolean isStoreLoaded( String name ) {

        if (RuntimeStore.getRuntimeStore().get( name.hashCode() ) != null) {
            return true;
        }
        return false;
    }

    /**
     * Private constructor
     * @param name Name of the record store to open. It must be unique. 
     * @param records 
     */
    private BlackberryRecordStore(String name, IntHashtable records) {

        setName( name );
        setRecords( records );
        setBlackberryRecordListeners( new Vector() );
        setOpenCount( 0 );
        //Find the max recordId of the existing store. 
        int maxId;
        if (records.get( 0 ) != null) {
            Integer maxIdInt = (Integer) records.get( 0 );
            maxId = maxIdInt.intValue();
        } else {
            maxId = 0;
        }

        setNextRecordId( maxId+1 );
    }
    /**
     * Adds a new record to the record store. The recordId assigned to the
     * inserted record is returne to the user.
     * This method needs to be able to accept empty (null) records.  
     * @param data Byte array to store
     * @param offset Starting offset of the passed byte array
     * @param numBytes Number of bytes from the offset to save
     * @return The recordId for the new record
     * @throws RecordStoreNotOpenException If the record store is not open
     * @throws RecordStoreFullException If the operation cannot be completed
     *         because the record store is full
     * @throws RecordStoreException If any record store related exception occurs. 
     */
    public synchronized int addRecord( byte[] data, int offset, int numBytes )
    throws RecordStoreNotOpenException, RecordStoreException, RecordStoreFullException {

        //Throws RecordStoreNotOpenException if the record store is not open
        ensureOpen();

        int ret = getNextRecordID();

        byte[] bytesToSave = null;
        //Hack to handle null record insertion.
        if (data == null) {
            bytesToSave = IAMNULL;
        }
        else {
            bytesToSave = Arrays.copy( data, offset, numBytes );
        }

        synchronized (getRecords()) {
            getRecords().put( getNextRecordID(), bytesToSave );
            getRecords().put( 0, new Integer(getNextRecordID()) );
            PersistentStoreManager.putObject( getName(), getRecords() );
            notifyListeners( ADDED, ret );
            setNextRecordId( getNextRecordID() + 1 );
            this.version++;
        }

        return ret;
    }

    public synchronized void closeRecordStore()
    throws RecordStoreNotOpenException, RecordStoreException {

        //Throws RecordStoreNotOpenException if the record store is not open
        ensureOpen();

        // We may have concurreny between open/close. Both methods update the
        // open counter and the RuntimeStore. We keep a lock on the open count
        // and this synchronize both objects
        StoreMutex mutex = getStoreMutex( name );
        synchronized (mutex) {

            try {
                setOpenCount( getOpenCount() - 1 );
                if (getOpenCount() <= 0) {
                    synchronized (getRecords()) {
                        // TODO: is this put really necessary? We put at each
                        // modification already
                        //PersistentStoreManager.putObject( getName(), getRecords() );
                        RuntimeStore.getRuntimeStore().remove( getName().hashCode() );
                    }
                    setBlackberryRecordListeners( new Vector() );
                }
            }
            catch (Exception e) {
                throw new RecordStoreException( "Exception encountered while " +
                                     "attempting to close the record store." );
            }
        }
    }

    /**
     * Deletes a record from the record store. 
     * @param recordId The recordId to be removed from the store
     * @return void
     * @throws RecordStoreNotOpenException If the record store is closed
     * @throws InvalidRecordIDException If the recordId does not exist
     * @throws RecordStoreException Any other error
     */
    public synchronized void deleteRecord( int recordId )
    throws RecordStoreNotOpenException, InvalidRecordIDException, RecordStoreException {

        //Throws RecordStoreNotOpenException if the record store is not open
        ensureOpen();

        synchronized (getRecords()) {
            if (!isValidRecordId( recordId )) {
                throw new InvalidRecordIDException( "No record found with recordId: " + recordId );
            }

            getRecords().remove( recordId );
            PersistentStoreManager.putObject( getName(), getRecords() );
            notifyListeners( DELETED, recordId );
            this.version++;
        }
    }

    public synchronized RecordEnumeration
    enumerateRecords( RecordFilter filter, RecordComparator comparator, boolean keepUpdated )
    throws RecordStoreNotOpenException {

        ensureOpen();

        BlackberryRecordEnumeration enumeration;
        enumeration = new BlackberryRecordEnumeration( this,
                           filterAndSort( filter, comparator ), keepUpdated );
        enumeration.setParentRecordComparator( comparator );
        enumeration.setParentRecordFilter( filter );

        addListener( enumeration );

        return enumeration;
    }

    public IntVector filterAndSort( RecordFilter filter, RecordComparator comparator )
    throws RecordStoreNotOpenException {

        Vector filteredList = new Vector();

        //Traverse refrs and add to temp list where filter match is true and element is not deleted.
        synchronized (getRecords()) {
            IntEnumeration recordKeys = getRecords().keys();
            while (recordKeys.hasMoreElements()) {
                int currentKey = recordKeys.nextElement();
                // Skip the max id item
                if (currentKey == 0) {
                    continue;
                }
                byte[] currentRecord = (byte[]) getRecords().get( currentKey );

                if (filter != null) {
                    //A filter is present. Add to filtered list only on match
                    if (filter.matches( currentRecord )) {
                        IntByteArrayPair pair = new IntByteArrayPair( currentKey, currentRecord );
                        filteredList.addElement( pair );
                    }
                }
                else {
                    //No filter present. Copy everything.
                    IntByteArrayPair pair = new IntByteArrayPair( currentKey, currentRecord );
                    filteredList.addElement( pair );
                }
            }
        }

        //Cannot get an array out of Vector. Allocate an array and copy
        //Vector contents into it
        IntByteArrayPair[] sortedArray = new IntByteArrayPair[filteredList.size()];
        filteredList.copyInto( sortedArray );

        if (comparator != null) {
            //A comparator is present. Sort the list.
            Arrays.sort( sortedArray, new IntValuePairComparator( comparator ) );
        }

        IntVector valueSortedKeys = new IntVector();
        for (int i = 0; i < filteredList.size(); i++) {
            valueSortedKeys.addElement( sortedArray[i].getKey() );
        }

        return valueSortedKeys;
    }

    public String getName() throws RecordStoreNotOpenException {

        //Throws RecordStoreNotOpenException if the record store is not open
        //        ensureOpen();
        return this.name;
    }

    /**
     * Returns the recordId that will be assigned to the next object added to the store.
     * According to the spec for javax.rms.RecordStore, this should start at 1.
     * @return RecordId of the next object to be added to the store
     * @throws RecordStoreNotOpenException If the record store is not open
     * @throws RecordStoreException If any other record store related exception occurs.
     */
    public int getNextRecordID() throws RecordStoreNotOpenException, RecordStoreException {

        //Throws RecordStoreNotOpenException if the record store is not open
        ensureOpen();

        return this.nextRecordId;
    }

    /**
     * Returns the number of records currently in the store
     * @return The number of records in the store.
     * @throws RecordStoreNotOpenException If the record store is not open
     */
    public synchronized int getNumRecords() throws RecordStoreNotOpenException {

        //Throws RecordStoreNotOpenException if the record store is not open
        ensureOpen();

        synchronized (getRecords()) {
            int size = getRecords().size();
            // There are three possible situations:
            // 1) 0 items: size is 0
            // 2) 1 item:  size is 0 (the only item is the max id)
            // 3) >1 item: size is number of items - 1
            if (size <= 1) {
                return 0;
            } else {
                return --size;
            }
        }
    }

    /**
     * Returns a copy of the data stored in the given record. If data is 0 length, return null. 
     * @param recordId The ID of the record to use in this operation
     * @return Byte array of the data stored in the given record
     * @throws RecordStoreNotOpenException If the record store is not open
     * @throws InvalidRecordIDException If the recordId is invalid
     * @throws RecordStoreException if a general record store exception occurs
     */
    public synchronized byte[] getRecord( int recordId )
    throws RecordStoreNotOpenException, InvalidRecordIDException, RecordStoreException {

        //Throws RecordStoreNotOpenException if the record store is not open
        ensureOpen();

        synchronized (getRecords()) {
            if (!isValidRecordId( recordId )) {
                throw new InvalidRecordIDException( "Record with id: " + recordId +
                                              " not found in store: " + getName() );
            }

            byte[] bytes = (byte[]) getRecords().get( recordId );

            //Special case. Return null if our null placeholder is found 
            if (Arrays.equals( bytes, IAMNULL )) {
                return null;
            }

            return Arrays.copy( bytes );
        }
    }

    /**
     * Returns a copy of the data stored in the given record. If data is 0 length, return null. 
     * @param recordId The ID of the record to use in this operation
     * @param buffer The byte array in which to copy the data
     * @param offset The index into the buffer in which to start copying
     * @return Byte array of the data stored in the given record
     * @throws RecordStoreNotOpenException If the record store is not open
     * @throws InvalidRecordIDException If the recordId is invalid
     * @throws RecordStoreException if a general record store exception occurs
     */
    public synchronized int getRecord( int recordId, byte[] buffer, int offset )
    throws RecordStoreNotOpenException, InvalidRecordIDException,
           RecordStoreException, ArrayIndexOutOfBoundsException {

        byte[] bytes = getRecord( recordId );
        for (int i = 0; i < bytes.length; i++) {
            buffer[i + offset] = bytes[i];
        }

        return bytes.length;
    }

    /**
     * Returns the size in bytes of the data available for the given recordId. If data is 0 length, return null.
     * @param recordId RecordId to find the size of
     * @throws RecordStoreNotOpenException If the record store is closed
     * @throws InvalidRecordIDException If the recordId does not exist 
     * @throws RecordStoreException Any oher record store exception
     */
    public synchronized int getRecordSize( int recordId )
    throws RecordStoreNotOpenException, InvalidRecordIDException, RecordStoreException {

        byte[] record = (byte[]) getRecord( recordId );
        if (Arrays.equals( record, IAMNULL )) {
            return 0;
        }
        return record.length;
    }

    /**
     * Gets the size in bytes of the current store.
     * @return The size in bytes of data in the store. 
     * @throws RecordStoreNotOpenException If the record store is not open
     */
    public synchronized int getSize() throws RecordStoreNotOpenException {

        //Throws RecordStoreNotOpenException if the record store is not open
        ensureOpen();

        int totalSize = 0;
        synchronized (getRecords()) {
            Enumeration records = getRecords().elements();
            IntEnumeration recordKeys = getRecords().keys();
            while (records.hasMoreElements()) {
                int currentKey = recordKeys.nextElement();
                // Skip the max id item
                if (currentKey == 0) {
                    continue;
                }
                byte[] bytes = (byte[]) records.nextElement();
                if (!Arrays.equals( bytes, IAMNULL )) {
                    totalSize += bytes.length;
                }
            }
        }
        return totalSize;
    }

    /**
     * Returns the amount of additional space in bytes this store can hold
     * @return Free ram
     * @throws RecordStoreNotOpenException if the record store is not open
     */
    public int getSizeAvailable() throws RecordStoreNotOpenException {

        //Throws RecordStoreNotOpenException if the record store is not open
        ensureOpen();
        //We have less RAM than flash. Return free ram since object is kept in memory. 
        return Memory.getRAMStats().getFree();
    }

    /**
     * Notifies associated enumerator objects and other listeners that may be added
     * @param operation Integer constant corresponding to the action being performed
     * @param recordId RecordID of the record being manipulated.
     */
    public void notifyListeners( int operation, int recordId ) {

        for (int i = 0; i < getBlackberryRecordListeners().size(); i++) {
            BlackberryRecordListener l;
            l = (BlackberryRecordListener) getBlackberryRecordListeners().elementAt( i );
            switch (operation) {
                case ADDED:
                    l.recordAdded( recordId );
                    break;
                case DELETED:
                    l.recordDeleted( recordId );
                    break;
                case CHANGED:
                    l.recordChanged( recordId );
                    break;
             }
        }
    }

    /**
     * Sets the data in the given record. 
     */
    public void setRecord( int recordId, byte[] newData, int offset, int numBytes )
    throws RecordStoreNotOpenException, InvalidRecordIDException,
           RecordStoreException, RecordStoreFullException {

        //Throws RecordStoreNotOpenException if the record store is not opens
        ensureOpen();

        synchronized (getRecords()) {
            if (!isValidRecordId( recordId )) {
                throw new InvalidRecordIDException( "Record with id: " + recordId +
                                                    " not found in store: " + getName() );
            }
            getRecords().put( recordId, newData );
            PersistentStoreManager.putObject( getName(), getRecords() );
            notifyListeners( CHANGED, recordId );
            this.version++;
        }
    }

    private void addListener( BlackberryRecordListener listener ) {

        this.blackberryRecordListeners.addElement( listener );
    }

    /**
     * Throws an exception if the record store is not open
     * @throws RecordStoreNotOpenException If the current record store is not open;
     */
    private void ensureOpen() throws RecordStoreNotOpenException {

        StoreMutex mutex = getStoreMutex( name );
        synchronized (mutex) {
            if (getOpenCount() <= 0) {
                throw new RecordStoreNotOpenException( "RecordStore not open" );
            }
        }
    }

    private Vector getBlackberryRecordListeners() {

        return blackberryRecordListeners;
    }

    private int getOpenCount() {

        return openCount;
    }

    private IntHashtable getRecords() throws RecordStoreNotOpenException {

        return this.records;
    }

    /**
     * This method checks if a given record id is present in the set of records
     * belonging to the recordstore. The hashmap containing the records can be
     * manipulated by other threads, so this method should always be invoked in
     * a synchronized context. We leave this duty the caller to minimize the
     * blocks of synchronized code.
     *
     * @param recordId the reocordId (>= 1)
     */
    private boolean isValidRecordId( int recordId )
    throws InvalidRecordIDException, RecordStoreNotOpenException {

        //        String keyString = Integer.toString( recordId );
        return (getRecords().containsKey( recordId ) && recordId != 0);
    }

    private void removeListener( BlackberryRecordListener listener ) {

        this.blackberryRecordListeners.removeElement( listener );
    }

    private void setBlackberryRecordListeners( Vector blackberryRecordListeners ) {

        this.blackberryRecordListeners = blackberryRecordListeners;
    }

    private void setName( String name ) {

        this.name = name;
    }

    private void setNextRecordId( int nextRecordId ) {

        this.nextRecordId = nextRecordId;
    }

    private void setOpenCount( int openCount ) {

        this.openCount = openCount;
    }

    private void setRecords( IntHashtable records ) {

        this.records = records;
    }

    /**
     * Used to keep key-object pairs together when sorting.
     *
     */
    public final class IntByteArrayPair {

        private int key;
        private byte[] value;

        public IntByteArrayPair(int key, byte[] value) {

            this.key = key;
            this.value = value;
        }

        public byte[] getBytes() {

            return this.value;
        }

        public int getKey() {

            return this.key;
        }
    }

    /**
     * Wraps a J2ME RecordComparator in a RIM Comparator.
     *
     */
    private final class IntValuePairComparator implements Comparator {

        RecordComparator valueComparator;

        public IntValuePairComparator(RecordComparator valueComparator) {

            this.valueComparator = valueComparator;
        }

        public int compare( Object o1, Object o2 ) {

            IntByteArrayPair a = (IntByteArrayPair) o1;
            IntByteArrayPair b = (IntByteArrayPair) o2;
            valueComparator.compare( a.getBytes(), b.getBytes() );
            return 0;
        }
    }

    
    /**
     * Inner class to provide helper functions for blackberry's Persistent Store.
     * We need to also wrap persisted objects in one of ours so that it is cleared
     * when the application is remove. BlackBerry will check to see what package an 
     * object comes from and only remove it from the persistent store if the package
     * does not exist anymore. 
     *
     */
    private static final class PersistentStoreManager {

        static private final String KEY = "com.funambol.storage.BlackberryRecordStore.PersistentStoreManager.class";
        
        private static ObjectWrapperHandler objectWrapperHandler;
    
        private static synchronized void setObjectWrapperHandler(ObjectWrapperHandler owh) {
            objectWrapperHandler = owh;
        }
        synchronized static private void addPersistedObject( String name, Object o ) {

            if (!exists( name )) {
                //Update the list
                Vector currentStores = getNames();
                currentStores.addElement( name );
                setNames( currentStores );

                //Create the store list
                putObject( name, o );

            }
        }

        synchronized static private boolean exists( String name ) {

            if (PersistentStore.getPersistentObject( hash( name ) ).getContents() != null) {
                return true;
            }

            return false;
        }

        /**
         * Returns a list of record stores. Will never return null.
         * @return Vector containing record store names
         */
        synchronized static private Vector getNames() {

            if (!exists( KEY )) {
                putObject( KEY, new Vector() );
            }

            return (Vector) getObject( KEY );

        }

        synchronized static private Object getObject( String name ) {

            long persistentStoreKey = hash( name );
            PersistentObject pobj = PersistentStore.getPersistentObject(persistentStoreKey);
            Persistable object = (Persistable) pobj.getContents();
            if (object == null) {
                throw new IllegalArgumentException( "Cannot get an object that does not exist" );
            }
            if (objectWrapperHandler == null) {
                throw new IllegalArgumentException("Invalid object wrapper hanlder");
            }
            return objectWrapperHandler.getObject(object);
        }

        static private long hash( String name ) {

            return KEY.hashCode() + name.hashCode();

        }

        synchronized static private void putObject( String name, Object o ) {

            long persistentStoreKey = hash( name );
            PersistentObject po = PersistentStore.getPersistentObject( persistentStoreKey );
            if (objectWrapperHandler == null) {
                throw new IllegalArgumentException("Invalid object wrapper hanlder");
            }
            Persistable p = objectWrapperHandler.createObjectWrapper(o);
            po.setContents( p );
            po.commit();
        }

        synchronized static private void removeObject( String name ) {

            if (exists( name )) {
                PersistentStore.destroyPersistentObject( hash(name) );
            }
            Vector namesVector = getNames();
            namesVector.removeElement( name );
            setNames( namesVector );
        }

        synchronized static private void setNames( Vector persistedObjectList ) {

            putObject( KEY, persistedObjectList );

        }
    }

    private static StoreMutex getStoreMutex(String recordStoreName) {
        if (allMutex == null) {
            allMutex = new IntHashtable(10);
        }

        int idx = recordStoreName.hashCode();
        StoreMutex mutex = (StoreMutex)allMutex.get(idx);
        if (mutex == null) {
            mutex = new StoreMutex(recordStoreName);
            allMutex.put(idx, mutex);
        }
        return mutex;
    }

    private static class StoreMutex {
        private String storeName = null;

        public StoreMutex(String storeName) {
            this.storeName = storeName;
        }
    }

    //Mimics javax.microedition.rms.RecordListener
    //Not currently used. 
    interface BlackberryRecordListener {

        public void recordAdded( int recordId );

        public void recordChanged( int recordId );

        public void recordDeleted( int recordId );
    }
}

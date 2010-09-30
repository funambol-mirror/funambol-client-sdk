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

import java.util.Hashtable;

import javax.microedition.rms.InvalidRecordIDException;
import javax.microedition.rms.RecordComparator;
import javax.microedition.rms.RecordEnumeration;
import javax.microedition.rms.RecordFilter;
import javax.microedition.rms.RecordStore;
import javax.microedition.rms.RecordStoreException;
import javax.microedition.rms.RecordStoreFullException;
import javax.microedition.rms.RecordStoreNotFoundException;
import javax.microedition.rms.RecordStoreNotOpenException;

public class RmsRecordStoreWrapper extends AbstractRecordStore {

    private RecordStore rs;
    
    /**
     * This is the list of mutex for the datastores. These mutex are used for
     * opening/closing datastores.
     */
    private static Hashtable allMutex = new Hashtable();

    private RmsRecordStoreWrapper(RecordStore rs) {
        super();
        this.rs = rs;
    }
    
    public synchronized int addRecord( byte[] data, int offset, int numBytes )
            throws RecordStoreNotOpenException, RecordStoreException, RecordStoreFullException {
        synchronized(rs) {
            return rs.addRecord( data, offset, numBytes );
        }
    }

    public synchronized void closeRecordStore()
            throws RecordStoreNotOpenException, RecordStoreException {
        synchronized (getStoreMutex(getName())) {
            rs.closeRecordStore();
        }
    }

    public synchronized void deleteRecord(int recordId)
            throws RecordStoreNotOpenException, InvalidRecordIDException, RecordStoreException {
        synchronized(rs) {
            rs.deleteRecord( recordId );
        }
    }

    static synchronized public void deleteRecordStore( String recordStoreName )
            throws RecordStoreException, RecordStoreNotFoundException {
        synchronized (getStoreMutex(recordStoreName)) {
            RecordStore.deleteRecordStore( recordStoreName );
        }
    }

    public synchronized RecordEnumeration enumerateRecords( RecordFilter filter,
            RecordComparator comparator, boolean keepUpdated ) throws RecordStoreNotOpenException {
        synchronized(rs) {
            return rs.enumerateRecords( filter, comparator, keepUpdated );
        }
    }

    public synchronized String getName() throws RecordStoreNotOpenException {
        synchronized(rs) {
            return rs.getName();
        }
    }

    public synchronized int getNextRecordID()
            throws RecordStoreNotOpenException, RecordStoreException {
        synchronized(rs) {
            return rs.getNextRecordID();
        }
    }

    public synchronized int getNumRecords() throws RecordStoreNotOpenException {
        synchronized(rs) {
            return rs.getNumRecords();
        }
    }

    public synchronized byte[] getRecord( int recordId )
            throws RecordStoreNotOpenException, InvalidRecordIDException, RecordStoreException {
        synchronized(rs) {
            return rs.getRecord( recordId );
        }
    }

    public synchronized int getRecord( int recordId, byte[] buffer, int offset )
            throws RecordStoreNotOpenException, InvalidRecordIDException, RecordStoreException, ArrayIndexOutOfBoundsException {
        synchronized(rs) {
            return rs.getRecord( recordId, buffer, offset );
        }
    }

    public synchronized void setRecord( int recordId, byte[] newData, int offset, int numBytes ) throws RecordStoreNotOpenException, InvalidRecordIDException, RecordStoreException, RecordStoreFullException {
        synchronized(rs) {
            rs.setRecord( recordId, newData, offset, numBytes );
        }
    }

    public synchronized int getRecordSize( int recordId ) throws RecordStoreNotOpenException, InvalidRecordIDException, RecordStoreException {
        synchronized(rs) {
            return rs.getRecordSize( recordId );
        }
    }

    public int getSize() throws RecordStoreNotOpenException {
        synchronized(rs) {
            return rs.getSize();
        }
    }

    public synchronized int getSizeAvailable() throws RecordStoreNotOpenException {
        synchronized(rs) {
            return rs.getSizeAvailable();
        }
    }

    static synchronized public String[] listRecordStores() {
        return RecordStore.listRecordStores();
    }

    static synchronized public AbstractRecordStore openRecordStore( String recordStoreName, boolean createIfNecessary ) 
            throws RecordStoreException, RecordStoreFullException, RecordStoreNotFoundException {
        synchronized (getStoreMutex(recordStoreName)) {
            return new RmsRecordStoreWrapper(RecordStore.openRecordStore( recordStoreName, createIfNecessary ));
        }
    }

    private static class StoreMutex {
        private String storeName = null;

        public StoreMutex(String storeName) {
            this.storeName = storeName;
        }
    }

    private static StoreMutex getStoreMutex(String recordStoreName) {
        if (allMutex == null) {
            allMutex = new Hashtable();
        }
        StoreMutex mutex = (StoreMutex)allMutex.get(recordStoreName);
        if (mutex == null) {
            mutex = new StoreMutex(recordStoreName);
            allMutex.put(recordStoreName, mutex);
        }
        return mutex;
    }
}

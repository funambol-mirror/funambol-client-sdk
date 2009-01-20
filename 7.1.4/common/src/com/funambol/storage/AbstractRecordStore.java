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

import javax.microedition.rms.InvalidRecordIDException;
import javax.microedition.rms.RecordComparator;
import javax.microedition.rms.RecordEnumeration;
import javax.microedition.rms.RecordFilter;
import javax.microedition.rms.RecordStore;
import javax.microedition.rms.RecordStoreException;
import javax.microedition.rms.RecordStoreFullException;
import javax.microedition.rms.RecordStoreNotFoundException;
import javax.microedition.rms.RecordStoreNotOpenException;

public abstract class AbstractRecordStore {

    static int AUTHMODE_ANY = RecordStore.AUTHMODE_ANY;
    static int AUTHMODE_PRIVATE = RecordStore.AUTHMODE_PRIVATE;

    public abstract int addRecord( byte[] data, int offset, int numBytes )
    throws RecordStoreNotOpenException, RecordStoreException, RecordStoreFullException;

    //    public abstract void addRecordListener(RecordListener listener);
    public abstract void closeRecordStore()
    throws RecordStoreNotOpenException, RecordStoreException;

    public abstract void deleteRecord( int recordId )
    throws RecordStoreNotOpenException, InvalidRecordIDException, RecordStoreException;

    static public void deleteRecordStore( String recordStoreName )
    throws RecordStoreException, RecordStoreNotFoundException {

        //#ifdef isBlackberry
        //# BlackberryRecordStore.deleteRecordStore( recordStoreName );
        //#else
        RmsRecordStoreWrapper.deleteRecordStore( recordStoreName );
        //#endif
    }

    /**
     * @param recordStoreName
     * @param createIfNecessary
     * @return
     * @throws RecordStoreException
     * @throws RecordStoreFullException
     * @throws RecordStoreNotFoundException
     */
    static public AbstractRecordStore openRecordStore( String recordStoreName,
                                                       boolean createIfNecessary )
    throws RecordStoreException, RecordStoreFullException, RecordStoreNotFoundException {

        //#ifdef isBlackberry
        //# return BlackberryRecordStore.openRecordStore( recordStoreName,createIfNecessary );
        //#else
        return RmsRecordStoreWrapper.openRecordStore( recordStoreName, createIfNecessary );
        //#endif
    }

    static public String[] listRecordStores() {
        //#ifdef isBlackberry
        //# return BlackberryRecordStore.listRecordStores( );
        //#else
        return RmsRecordStoreWrapper.listRecordStores( );
        //#endif
    }

    public abstract RecordEnumeration enumerateRecords( RecordFilter filter,
                                                        RecordComparator comparator,
                                                        boolean keepUpdated )
    throws RecordStoreNotOpenException;

    public abstract String getName() throws RecordStoreNotOpenException;

    public abstract int getNextRecordID()
    throws RecordStoreNotOpenException, RecordStoreException;

    public abstract int getNumRecords() throws RecordStoreNotOpenException;

    public abstract byte[] getRecord( int recordId )
    throws RecordStoreNotOpenException, InvalidRecordIDException, RecordStoreException;

    public abstract int getRecord( int recordId, byte[] buffer, int offset )
    throws RecordStoreNotOpenException, InvalidRecordIDException,
           RecordStoreException, ArrayIndexOutOfBoundsException;

    public abstract int getRecordSize( int recordId )
    throws RecordStoreNotOpenException, InvalidRecordIDException, RecordStoreException;

    public abstract int getSize() throws RecordStoreNotOpenException;

    public abstract int getSizeAvailable() throws RecordStoreNotOpenException;

    public abstract void setRecord( int recordId, byte[] newData, int offset, int numBytes )
    throws RecordStoreNotOpenException, InvalidRecordIDException, RecordStoreException,
           RecordStoreFullException;

    //    Other methods from RecordStore that are not implemented.
    //    public abstract RecordStore openRecordStore(String recordStoreName,
    //                                                boolean createIfNecessary,
    //                                                int authomode, boolean writeable)
    //    throws RecordStoreException, RecordStoreFullException,
    //           RecordStoreNotFoundException, IllegalArgumentException;
    //
    //    public abstract RecordStore openRecordStore(String recordStoreName,
    //                                                String vendorName, String suiteName)
    //    throws RecordStoreException, RecordStoreNotFoundException, SecurityException, IllegalArgumentException;
    //
    //    public abstract void removeRecordListener(RecordListener listener);
    //
    //    public abstract void setMode(int authmode, boolean writeable)
    //    throws RecordStoreException, SecurityException, IllegalArgumentException;
    //    public abstract int getVersion() throws RecordStoreNotOpenException;
    //
    //    public abstract long getLastModified() throws RecordStoreNotOpenException;
}

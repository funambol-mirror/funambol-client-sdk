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

//#ifndef isBlackberry
//# public class BlackberryRecordEnumeration {}
//#else

//# import javax.microedition.rms.InvalidRecordIDException;
//# import javax.microedition.rms.RecordComparator;
//# import javax.microedition.rms.RecordEnumeration;
//# import javax.microedition.rms.RecordFilter;
//# import javax.microedition.rms.RecordStoreException;
//# import javax.microedition.rms.RecordStoreNotOpenException;
//# 
//# import net.rim.device.api.util.IntVector;
//# 
//# import com.funambol.storage.BlackberryRecordStore.BlackberryRecordListener;
//# 
//# public class BlackberryRecordEnumeration implements RecordEnumeration, BlackberryRecordListener {
//# 
//#     private int position;
//#     private boolean keepUpdated;
//#     private IntVector valueSortedKeys;
//#     private BlackberryRecordStore parentRecordStore;
//#     private RecordFilter parentRecordFilter;
//#     private RecordComparator parentRecordComparator;
//#     private boolean destroyed = false;
//# 
//#     public BlackberryRecordEnumeration(BlackberryRecordStore records, IntVector valueSortedKeys, boolean keepUpdated) {
//# 
//#         setRecords( records );
//#         setValueSortedKeys( valueSortedKeys );
//#         keepUpdated( keepUpdated );
//# 
//#         //Start at -1 so our next is index 0;
//#         setPosition( -1 );
//#     }
//# 
//#     private BlackberryRecordStore getRecords() {
//# 
//#         return parentRecordStore;
//#     }
//# 
//#     private void setRecords( BlackberryRecordStore records ) {
//# 
//#         this.parentRecordStore = records;
//#     }
//# 
//#     private int getPosition() {
//# 
//#         return position;
//#     }
//# 
//#     private void setPosition( int position ) {
//# 
//#         this.position = position;
//#     }
//# 
//#     public void destroy() {
//# 
//#         setRecords( null );
//#         setValueSortedKeys( null );
//#         setParentRecordComparator( null );
//#         setParentRecordFilter( null );
//#         setParentRecordStore( null );
//#         setDestroyed( true );
//#     }
//# 
//#     public boolean hasNextElement() {
//# 
//#         if (isDestroyed()) {
//#             throw new IllegalStateException( "Enumeration is destroyed" );
//#         }
//# 
//#         if (( getPosition() + 1 ) >= 0 && ( getPosition() + 1 ) < getValueSortedKeys().size()) {
//#             return true;
//#         }
//#         return false;
//#     }
//# 
//#     public boolean hasPreviousElement() {
//# 
//#         if (isDestroyed()) {
//#             throw new IllegalStateException( "Enumeration is destroyed" );
//#         }
//# 
//#         if (getPosition() - 1 >= 0) {
//#             return true;
//#         }
//#         return false;
//#     }
//# 
//#     public boolean isKeptUpdated() {
//# 
//#         return this.keepUpdated;
//#     }
//# 
//#     public void keepUpdated( boolean keepUpdated ) {
//# 
//#         this.keepUpdated = keepUpdated;
//# 
//#     }
//# 
//#     /**
//#      * Returns a copy of the next record in this enumeration. Changes made to 
//#      * the object will not be reflected in the RecordStore
//#      * @return The next record in the enumeration
//#      * @throws InvalidRecordIDException When no more records are available
//#      * @throws RecordStoreNotOpenException If the record store is not open
//#      */
//#     public byte[] nextRecord() throws InvalidRecordIDException, RecordStoreNotOpenException, RecordStoreException {
//# 
//#         if (isDestroyed()) {
//#             throw new IllegalStateException( "Enumeration is destroyed" );
//#         }
//# 
//#         if (!hasNextElement()) {
//#             throw new InvalidRecordIDException();
//#         }
//#         setPosition( getPosition() + 1 );
//# 
//#         //getCurrentRecord() will throw RecordStoreNotOpenException
//#         return getCurrentRecord();
//#     }
//# 
//#     /**
//#      * Returns the next recordId in this enumeration.  
//#      * @return The next record in the enumeration
//#      * @throws InvalidRecordIDException When no more records are available
//#      */
//#     public int nextRecordId() throws InvalidRecordIDException {
//# 
//#         if (isDestroyed()) {
//#             throw new IllegalStateException( "Enumeration is destroyed" );
//#         }
//# 
//#         if (!hasNextElement()) {
//#             throw new InvalidRecordIDException();
//#         }
//# 
//#         setPosition( getPosition() + 1 );
//#         return getValueSortedKeys().elementAt( getPosition() );
//# 
//#     }
//# 
//#     /**
//#      * Returns the number of records available in this enumeration. 
//#      * @return Number of records in the enumeration
//#      */
//#     public int numRecords() {
//# 
//#         if (isDestroyed()) {
//#             throw new IllegalStateException( "Enumeration is destroyed" );
//#         }
//#         return getValueSortedKeys().size();
//#     }
//# 
//#     /**
//#      * Returns a copy of the previous record in this enumeration. The byte array returned is a copy 
//#      * of the RecordStore record and changes will not be reflected in the store.
//#      * @return Byte array of the record
//#      * @throws InvalidRecordIDException When no more records are available
//#      * @throws RecordStoreNotOpenException When the parent record store is closed
//#      * @throws RecordStoreException If a general record store exception occurs 
//#      */
//#     public byte[] previousRecord() throws InvalidRecordIDException, RecordStoreNotOpenException, RecordStoreException {
//# 
//#         if (isDestroyed()) {
//#             throw new IllegalStateException( "Enumeration is destroyed" );
//#         }
//# 
//#         if (!hasPreviousElement()) {
//#             throw new InvalidRecordIDException( "There are no previous records" );
//#         }
//#         setPosition( getPosition() - 1 );
//# 
//#         //getCurrentRecord() will throw other exceptions
//#         return getCurrentRecord();
//#     }
//# 
//#     /**
//#      * Returns the recordId of the previous record in the enumeration. 
//#      * @return RecordId of the next element
//#      * @throws InvalidRecordIDException when no more records are available;s
//#      */
//#     public int previousRecordId() throws InvalidRecordIDException {
//# 
//#         if (isDestroyed()) {
//#             throw new IllegalStateException( "Enumeration is destroyed" );
//#         }
//# 
//#         if (!hasPreviousElement()) {
//#             throw new InvalidRecordIDException( "There are no previous records" );
//#         }
//#         return getPosition() - 1;
//#     }
//# 
//#     /**
//#      * Rebuilds the current enumeration to reflect changes in the record store. 
//#      */
//#     public void rebuild() {
//# 
//#         if (isDestroyed()) {
//#             throw new IllegalStateException( "Enumeration is destroyed" );
//#         }
//# 
//#         try {
//#             IntVector newKeys = getParentRecordStore().filterAndSort( getParentRecordFilter(), getParentRecordComparator() );
//#             setValueSortedKeys( newKeys );
//#             if (getPosition() >= newKeys.size()) {
//#                 setPosition( newKeys.size() - 1 );
//#             }
//#         }
//#         catch (Throwable t) {
//#             t.printStackTrace();
//#         }
//#     }
//# 
//#     public void reset() {
//# 
//#         if (isDestroyed()) {
//#             throw new IllegalStateException( "Enumeration is destroyed" );
//#         }
//# 
//#         setPosition( -1 );
//# 
//#     }
//# 
//#     private IntVector getValueSortedKeys() {
//# 
//#         return valueSortedKeys;
//#     }
//# 
//#     private void setValueSortedKeys( IntVector valueSortedKeys ) {
//# 
//#         this.valueSortedKeys = valueSortedKeys;
//#     }
//# 
//#     private byte[] getCurrentRecord() throws InvalidRecordIDException, RecordStoreException {
//# 
//#         int key = getValueSortedKeys().elementAt( getPosition() );
//#         byte[] record = null;
//#         record = getRecords().getRecord( key );
//#         return record;
//#     }
//# 
//#     public void recordAdded( int recordId ) {
//# 
//#         if (isKeptUpdated()) {
//#             rebuild();
//#         }
//# 
//#     }
//# 
//#     public void recordChanged( int recordId ) {
//# 
//#         if(!getValueSortedKeys().contains( recordId )) {
//#             rebuild();
//#         }
//#     }
//# 
//#     public void recordDeleted( int recordId ) {
//# 
//#         if (isKeptUpdated()) {
//#             getValueSortedKeys().removeElement( recordId );
//#         }
//# 
//#     }
//# 
//#     public void recordStoreClosing() {
//# 
//#     }
//# 
//#     public RecordFilter getParentRecordFilter() {
//# 
//#         return parentRecordFilter;
//#     }
//# 
//#     public void setParentRecordFilter( RecordFilter parentRecordFilter ) {
//# 
//#         this.parentRecordFilter = parentRecordFilter;
//#     }
//# 
//#     public RecordComparator getParentRecordComparator() {
//# 
//#         return parentRecordComparator;
//#     }
//# 
//#     public void setParentRecordComparator( RecordComparator parentRecordComparator ) {
//# 
//#         this.parentRecordComparator = parentRecordComparator;
//#     }
//# 
//#     private BlackberryRecordStore getParentRecordStore() {
//# 
//#         return parentRecordStore;
//#     }
//# 
//#     private void setParentRecordStore( BlackberryRecordStore parentRecordStore ) {
//# 
//#         this.parentRecordStore = parentRecordStore;
//#     }
//# 
//#     private boolean isDestroyed() {
//# 
//#         return destroyed;
//#     }
//# 
//#     private void setDestroyed( boolean destroyed ) {
//# 
//#         this.destroyed = destroyed;
//#     }
//# }

//#endif
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

/**
 * This class is to serve as a supertype for record storage in order to
 * reuse code between Blackberry devices and J2ME.
 * 
 * @author nroy
 *
 */
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

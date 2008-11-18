package com.funambol.storage;

import javax.microedition.rms.InvalidRecordIDException;
import javax.microedition.rms.RecordComparator;
import javax.microedition.rms.RecordEnumeration;
import javax.microedition.rms.RecordFilter;
import javax.microedition.rms.RecordListener;
import javax.microedition.rms.RecordStore;
import javax.microedition.rms.RecordStoreException;
import javax.microedition.rms.RecordStoreFullException;
import javax.microedition.rms.RecordStoreNotFoundException;
import javax.microedition.rms.RecordStoreNotOpenException;


public class RmsRecordStoreWrapper extends AbstractRecordStore {
    RecordStore rs;

    public int addRecord( byte[] data, int offset, int numBytes ) throws RecordStoreNotOpenException, RecordStoreException, RecordStoreFullException {
        return rs.addRecord( data, offset, numBytes );
    }

    public void closeRecordStore() throws RecordStoreNotOpenException, RecordStoreException {

        rs.closeRecordStore();
        
    }

    public void deleteRecord(int recordId) throws RecordStoreNotOpenException, InvalidRecordIDException, RecordStoreException {

        rs.deleteRecord( recordId );
        
    }

    static public void deleteRecordStore( String recordStoreName ) throws RecordStoreException, RecordStoreNotFoundException {
        RecordStore.deleteRecordStore( recordStoreName );
    }

    public RecordEnumeration enumerateRecords( RecordFilter filter, RecordComparator comparator, boolean keepUpdated ) throws RecordStoreNotOpenException {

        return rs.enumerateRecords( filter, comparator, keepUpdated );
    }

    public String getName() throws RecordStoreNotOpenException {

        return rs.getName();
    }

    public int getNextRecordID() throws RecordStoreNotOpenException, RecordStoreException {

        return rs.getNextRecordID();
    }

    public int getNumRecords() throws RecordStoreNotOpenException {

        return rs.getNumRecords();
    }

    public byte[] getRecord( int recordId ) throws RecordStoreNotOpenException, InvalidRecordIDException, RecordStoreException {
        return rs.getRecord( recordId );
    }

    public int getRecord( int recordId, byte[] buffer, int offset ) throws RecordStoreNotOpenException, InvalidRecordIDException, RecordStoreException, ArrayIndexOutOfBoundsException {

        return rs.getRecord( recordId, buffer, offset );
    }

    public int getRecordSize( int recordId ) throws RecordStoreNotOpenException, InvalidRecordIDException, RecordStoreException {
        return rs.getRecordSize( recordId );
    }

    public int getSize() throws RecordStoreNotOpenException {
        return rs.getSize();
    }

    public int getSizeAvailable() throws RecordStoreNotOpenException {
        return rs.getSizeAvailable();
    }

    static public String[] listRecordStores() {
        return RecordStore.listRecordStores();
    }

    static public AbstractRecordStore openRecordStore( String recordStoreName, boolean createIfNecessary ) throws RecordStoreException, RecordStoreFullException, RecordStoreNotFoundException {
        
        return new RmsRecordStoreWrapper(RecordStore.openRecordStore( recordStoreName, createIfNecessary ));
    }

    public void setRecord( int recordId, byte[] newData, int offset, int numBytes ) throws RecordStoreNotOpenException, InvalidRecordIDException, RecordStoreException, RecordStoreFullException {
        rs.setRecord( recordId, newData, offset, numBytes );
    }

    public RmsRecordStoreWrapper(RecordStore rs) {

        super();
        this.rs = rs;
    }
}
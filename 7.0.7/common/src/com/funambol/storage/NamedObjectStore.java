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

import java.util.Enumeration;
import java.util.Hashtable;

import java.io.DataInputStream;
import java.io.DataOutputStream;

import javax.microedition.rms.RecordStore;

import java.io.IOException;

import javax.microedition.rms.RecordStoreException;
import javax.microedition.rms.RecordStoreNotOpenException;

import com.funambol.util.Log;

/**
 * 
 * This class uses the J2ME RMS to store and retrieve objects using 
 * a name.
 * 
 * The first record of the record store is used to save the index.
 * A recordstore managed by NamedObjectStore must be modified using 
 * this class methods only, or the index will be currupted.
 * 
 * To persist an object using ObjectStore, it must implement the
 * com.funambol.storage.Serializable interface.
 * 
 * @param name is the name of the RecordStore to be managed
 */
public class NamedObjectStore {

    // -------------------------------------------------------------- Attributes
    /* Object Store to be managed*/
    private ObjectStore objs = null;
    /* Object Store index map*/
    private ObjectMap objmap = null;

    // ------------------------------------------------------------ Constructors

    /** 
     * Creates a new instance of ObjectStore.
     *
     */
    public NamedObjectStore(){
        objs = new ObjectStore();
        objmap = new ObjectMap(objs);
    }

    //----------------------------------------------------------- Public methods

    /** 
     * Open an existing RecordStore, or throws an exception if not present.
     * If the name is the same of the currently open one, no action is
     * taken, otherwise the old one is closed.
     *
     * @param name is the name of the RecordStore to be managed 
     * @return true if the record store has been open or created
     *         false if it was cached
     */
    public boolean open(String name) throws RecordStoreException, IOException {
        // If the record store was actually opened, get the map
        if(objs.open(name)){
            objmap.load();
            return true;
        }
        return false;
    }
    
    /** 
     * Close the current RecordStore, if open.
     *
     * @throws RecordStoreException if the ObjectStore was not open.
     */
    public void close() throws RecordStoreException {
        objs.close();
    }
    
    /** 
     * Creates a new RecordStore, or open an existing one.
     *
     * @param name is the name of the RecordStore to be managed
     * @return true if the record store has been open or created
     *         false if it was cached
     * 
     */
    public boolean create(String name) throws RecordStoreException, IOException {
        // If the record store was actually opened,
        if(objs.create(name)){
            // and it was already created
            if(objs.size() > 0) {
                // get the map
                objmap.load();
            }
            else {
                // otherwise initialize it
                objmap.init();
            }
            return true;
        }
        return false;
    }
    
    /**
     *  Store the serializable object in the ObjectStore, using the name
     *  hashcode to index it.
     *
     *  @param name the name of the object
     *  @param obj the serializable object
     *
     *  @return true if the record has been added by this call
     */
    public boolean store(String name, Serializable obj)
    throws RecordStoreException, IOException {
        int index = objmap.lookup(name);
        
        // If this object is already in the store, update the record
        if(index > 0){
            objs.store(index, obj);
            return false;
        }
        else {
            // store the object and update the object map
            index = objs.store(obj);
            objmap.add(name, index);
            return true;
        }
    }
    
    /**
     *  Retrieve the serializable object from the record store, using
     *  name to index it.
     *
     *  @param name the name of the object
     *  @param obj the serializable object
     * 
     *  @return a reference to the object
     */
    public Serializable retrieve(String name, Serializable obj)
    throws RecordStoreException, IOException {
        int index = objmap.lookup(name);
        if(index == 0){
            throw new RecordStoreException("Object not found: "+name);
        }
        return objs.retrieve(index, obj);
    }

    /**
     *  Retrieve the serializable object from the record store, using a
     *  positional index.
     *
     *  @param index the index in the recordstore.
     *  @param obj the serializable object
     * 
     *  @return a reference to the object
     */
    public Serializable retrieve(int index, Serializable obj)
    throws RecordStoreException, IOException {
        return objs.retrieve(index, obj);
    }

    /**
     *  Retrieve the list of names from the record store.
     *  The order is not guaranteed.
     *
     *  @return an array of String containing the names
     */
    public String[] names(){
        return objmap.list();
    }

    /**
     *  Search for a name in the record store, returning the object index
     *
     *  @param name the name of the object
     * 
     *  @return the index in the record store, or 0 if not found
     */
    public int lookup(String name) {
        int index = objmap.lookup(name);
        if(index == 0){
            return 0;
        }
        return index;
    }

    /**
     *  Retrieve the first serializable object from the record store.
     *
     *  @param obj the serializable object
     * 
     *  @return a reference to the object, or null if no objects found.
     */
    public Serializable getFirstObject(Serializable obj)
    throws RecordStoreException, IOException {

        int index = objs.getFirstIndex();
        // No records
        if (index == 0) {
            return null;
        }
        // This is the index table, skip it
        if (index == 1) {
            index = objs.getNextIndex();
            if (index == 0) {
                return null;
            }
        }
        // Okay, is a record
        return objs.retrieve(index, obj);
    }

    /**
     *  Retrieve the next serializable object from the record store.
     *
     *  @param obj the serializable object
     * 
     *  @return a reference to the object, or null if there are no more objects
     *          in the store
     */
    public Serializable getNextObject(Serializable obj)
    throws RecordStoreException, IOException {
        
        int index = objs.getNextIndex();
        // No more records
        if (index == 0) {
            return null;
        }
        // This is the index table, skip it
        if (index == 1) {
            index = objs.getNextIndex();
            if (index == 0) {
                return null;
            }
        }
        // Get the record
        return objs.retrieve(index, obj);
    }

    /**
     *  Remove the object from the store.
     *
     *  @param name the serializable object name
     * 
     *  @return true if the object has been removed, false if not found
     */
    public boolean remove(String name)
    throws RecordStoreException, IOException {
        int index = objmap.lookup(name);
        if(index == 0){
            Log.error("NamedObjStore.remove(): " + name + " not found");
            return false;
        }
        objs.remove(index);
        objmap.del(name);

        return true;
    }

    /**
     *  Returns the number of objects in the store.
     *
     *  @return the number of objectss in the store, or -1 on error.
     */
    public int size(){
        int ret = objs.size();
        if(ret == -1){
            return -1;
        }
        // The index record shall not be counted.
        return ret-1;
    }

    /**
     *  Returns the amount of additional room (in bytes) available for this
     *  record store to grow.
     *
     *  @return the amount of storage left for this store.
     */
    public int getAvaliableStorage() {
        return objs.getAvaliableStorage();
    }

    /*public void makeRoom(int size) throws RecordStoreException {
        objs.makeRoom(size);
    }*/


}

/*
 * Private class to handle the object map.
 */
class ObjectMap implements Serializable {

    private ObjectStore objs;
    private Hashtable objmap;
    private Enumeration e;

    ObjectMap(ObjectStore s) {
        objs = s;
        objmap = new Hashtable();
    }

    // Load the map from the first Record of the ObjectStore
    public void init() throws RecordStoreException, IOException {
        objmap.clear();
        if(objs.size()>0) {
            objs.store(1, this);
        }
        else {
            objs.store(this);
        }
    }

    // Load the map from the first Record of the ObjectStore
    public void load() throws RecordStoreException, IOException {
        objs.retrieve(1, this);
    }

    // Save the map from the first Record of the ObjectStore
    public void save() throws RecordStoreException, IOException {
        objs.store(1, this);
    }

    // Add a new index to the map and save it
    public void add(String name, int index)
    throws RecordStoreException, IOException{
        objmap.put(name, new Integer(index));
        save();
    }

    // Remove an index from the map and save it
    public void del(String name) throws RecordStoreException, IOException{
        objmap.remove(name);
        save();
    }
    
    public String[] list() {
        int size = objmap.size();
        String[] ret = new String[size];
        Enumeration e = objmap.keys();

        for(int i=0; i < size; i++){
            ret[i] = (String)e.nextElement();
        }
        return ret;
    }

    public int lookup(String name){
        Integer val = (Integer) objmap.get(name);
        if(val != null){
            return val.intValue();
        }
        else{
            return 0;
        }
    }

    /**
     * Write object fields to the output stream.
     * @param out Output stream
     * @throws IOException
     */
    public void serialize( DataOutputStream out ) throws IOException {
        out.writeInt(objmap.size());
        for(Enumeration e = objmap.keys(); e.hasMoreElements(); ){
            String key = (String) e.nextElement();
            int val = ((Integer)objmap.get(key)).intValue();
            
            out.writeUTF(key);
            out.writeInt(val);
        }
    }

    /**
     * Read object field from the input stream.
     * @param in Input stream
     * @throws IOException
     */
    public void deserialize( DataInputStream in ) throws IOException {
        objmap.clear();
        int size = in.readInt();
        
        for(int i=0; i<size; i++ ){
            String key = in.readUTF();
            Integer val = new Integer(in.readInt());
            objmap.put(key, val);
        }
    }
}



/*
 * Funambol is a mobile platform developed by Funambol, Inc. 
 * Copyright (C) 2010 Funambol, Inc.
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
import java.util.Hashtable;

import javax.microedition.rms.InvalidRecordIDException;
import javax.microedition.rms.RecordEnumeration;
import javax.microedition.rms.RecordStore;
import javax.microedition.rms.RecordStoreException;
import javax.microedition.rms.RecordStoreNotOpenException;

import com.funambol.util.Log;

/** 
 * This class extends the ObjectStore and changes its behavior in that stores
 * are not closed when the close method is invoken. Open is also redefined so
 * that a Store is opened only once.
 */
public class NonClosingObjectStore extends ObjectStore {

    private static Hashtable openedStores = new Hashtable();

    public NonClosingObjectStore() {
        super();
    }

    public NonClosingObjectStore(String name) {
        super(name);
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
     * private method used by open and create to share code.
     */
    protected synchronized boolean openStore(String name, boolean create)
    throws RecordStoreException  {
        // Check if is requested to open a new record store
        synchronized(mutex) {

            rs = (AbstractRecordStore) openedStores.get(name);
            if (rs == null) {
                // The store needs to be opened
                rs = AbstractRecordStore.openRecordStore(name, create);
                openedStores.put(name, rs);
            }
            this.name = name;
            return false;
        }
    }

    protected synchronized boolean openStore() throws RecordStoreException  {
        return openStore(name, false);
    }


    public synchronized void close() throws RecordStoreException {
        // Nothing to do here
    }

    /**
     * Removes this object store. The store is closed before trying to remove
     * it.
     */
    public synchronized void remove() throws RecordStoreException {
        synchronized(mutex) {
            rs = (AbstractRecordStore) openedStores.get(name);
            if (rs != null) {
                openedStores.remove(name);
            }
        }
        super.remove();
    }
}


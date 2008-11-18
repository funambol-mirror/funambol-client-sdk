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

import com.funambol.util.Log;
import java.io.ByteArrayInputStream;
import java.io.DataInputStream;
import java.io.DataOutputStream;
import java.io.IOException;
import java.util.Enumeration;
import javax.microedition.rms.InvalidRecordIDException;
import javax.microedition.rms.RecordComparator;
import javax.microedition.rms.RecordEnumeration;
import javax.microedition.rms.RecordFilter;
import javax.microedition.rms.RecordStore;
import javax.microedition.rms.RecordStoreException;
import javax.microedition.rms.RecordStoreNotOpenException;

/**
 * Useful class to enumerate sorted and filtered contact records
 */
public class ObjectEnumeration implements Enumeration {
    int index = 0;
    byte[] filter = null;
    AbstractRecordStore rs = null;
    Serializable s = null;
    RecordEnumeration re = null;
    int size = 0;
    
    public ObjectEnumeration() {
    }
    
    /**
     * Constructor
     */
    public ObjectEnumeration(AbstractRecordStore refrs, ObjectFilter rf,
            ObjectComparator rc, Serializable obj) {
        //int recordIndex = 0;
        this.rs = refrs;
        this.s = obj;
        try {
            this.re = rs.enumerateRecords((RecordFilter)rf,
                    (RecordComparator) rc, false);
            this.size = re.numRecords();
        } catch (RecordStoreNotOpenException ex) {
            Log.debug(this,"objectEnumeration constructor: " +
                    "RecordStoreBotFoundException ");
            ex.printStackTrace();
        }
    }
    
    /**
     * Enumeration interface implemented method
     * @return true if enumeration has more elements
     */
    public boolean hasMoreElements() {
        return this.re.hasNextElement();
    }
    
    /**
     * Enumeration interface implemented method
     * @return Object to fill enumeration
     */
    public Object nextElement() {
        ByteArrayInputStream dataStream = null;
        DataInputStream in = null;
        try {
            int recordIndex = re.nextRecordId();
            
            dataStream = new ByteArrayInputStream(rs.getRecord(recordIndex));
            Serializable so;
            so = (Serializable) s.getClass().newInstance();
            
            return new Serialized(new DataInputStream(dataStream), so, recordIndex);
        } catch (RecordStoreException ex) {
            ex.printStackTrace();
            Log.error(this, "nextElement() RecordStoreException ");
        } catch (IOException ex) {
            Log.error(this, "nextElement() IOException ");
            ex.printStackTrace();
        } catch (InstantiationException ex) {
            Log.error(this, "nextElement() InstantittionException ");
            ex.printStackTrace();
        } catch (IllegalAccessException ex) {
            Log.error(this, "nextElement() IllegalAccessException ");
            ex.printStackTrace();
        }
        return null;
    }
    
    /**
     * return the number of objects within the enumeration
     * @return size is the number of objects into this enumeration
     */
    public int getSize() {
        return this.size;
    }
    
}


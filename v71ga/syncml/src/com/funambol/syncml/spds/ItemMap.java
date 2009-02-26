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
 * The interactive user interfaces in modified sourceName and object code versions
 * of this program must display Appropriate Legal Notices, as required under
 * Section 5 of the GNU Affero General Public License version 3.
 *
 * In accordance with Section 7(b) of the GNU Affero General Public License
 * version 3, these Appropriate Legal Notices must retain the display of the
 * "Powered by Funambol" logo. If the display of the logo is not reasonably
 * feasible for technical reasons, the Appropriate Legal Notices must display
 * the words "Powered by Funambol".
 */


package com.funambol.syncml.spds;

import com.funambol.storage.ComplexSerializer;
import com.funambol.storage.Serializable;
import java.io.DataInputStream;
import java.io.DataOutputStream;
import java.io.IOException;
import java.util.Hashtable;

/**
 * The serializable object used to persist mapping informations
 */
public class ItemMap implements Serializable {

    String sourceName = "";
    Hashtable mappings = new Hashtable();
    
    
    public ItemMap() {
        mappings = new Hashtable();
    }
    
    public ItemMap(String sourceName) {
        this.sourceName=sourceName;
        this.mappings=new Hashtable();
    }    
    
    public ItemMap(String sourceName, Hashtable mappings) {
        this.sourceName=sourceName;
        this.mappings=mappings;
    }    
    
    /**
     * The interface method use by the store to serialize this object
     * @param out is where the object is serialized
     * @throws java.io.IOException
     */
    public void serialize(DataOutputStream out) throws IOException {
        out.writeUTF(sourceName);
        ComplexSerializer.serializeHashTable(out, mappings);
    }

    /**
     * The interface method use by the store to deserialize this object
     * @param in is where the object is read
     * @throws java.io.IOException
     */
    public void deserialize(DataInputStream in) throws IOException {
        this.sourceName = in.readUTF();
        this.mappings = ComplexSerializer.deserializeHashTable(in);
    }
    
    /**
     * Accessor method
     * @return Hashtable related mappings 
     */
    public Hashtable getMappings() {
        return this.mappings;
    }
    
    /**
     * Accessor method
     * @return sourceName of the related source
     */
    public String getSourceName() {
        return this.sourceName;
    }
    
}

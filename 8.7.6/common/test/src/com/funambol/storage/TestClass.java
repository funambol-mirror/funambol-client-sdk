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

import java.io.DataInputStream;
import java.io.DataOutputStream;
import java.io.IOException;
import java.util.Enumeration;
import java.util.Hashtable;

/**
     * A test Serializable class, with some atributes and an hashtable
     */
    public class TestClass implements Serializable {
        private int val;
        private String name;
        private Hashtable ht;

        // Default constructor
        public TestClass() {
            val = 0;
            name = null;
            ht = null;
        }

        // Init the class
        public TestClass(String name) {
            val = 9;
            this.name = name;

            ht = new Hashtable(5);
            ht.put("Funambol", "Developers");
            ht.put("John", "Doe");
            ht.put("Andrea", "Doria");
            ht.put("Johann S.", "Bach");
            ht.put("SomeValue", new Integer(10));
        }
        
        public void serialize(DataOutputStream out) throws IOException {
            out.writeInt(val);
            out.writeUTF(name);
            ComplexSerializer.serializeHashTable(out, ht);            
        }

        public void deserialize(DataInputStream in) throws IOException {
            val=in.readInt();
            name=in.readUTF();
            ht=(Hashtable) ComplexSerializer.deserializeHashTable(in);
        }
        
        public String toString(){
            StringBuffer ret = new StringBuffer(this.getClass().getName());
            ret.append("\nval: ").append(val);
            ret.append("\nname: ").append(name).append("\n");
            
            for(Enumeration e = ht.keys(); e.hasMoreElements(); ){
                Object k = e.nextElement();
                ret.append(k).append(": ").append(ht.get(k)).append("\n");
            }
            return ret.toString();
        }
    }
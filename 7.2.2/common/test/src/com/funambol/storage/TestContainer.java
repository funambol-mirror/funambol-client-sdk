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
import java.io.DataInputStream;
import java.io.DataOutputStream;
import java.io.IOException;
import java.util.Vector;

/** Another test class. This is a cointainer that uses a Vector. */
    public class TestContainer implements Serializable {
        protected Vector v;

        public TestContainer() {
            v = null;
        }

        public TestContainer(TestClass element) {
            v = new Vector(3);
            v.addElement(element);
            v.addElement(new String("String element"));
            v.addElement(new Integer(10));
        }

        public void serialize(DataOutputStream out) throws IOException {
            ComplexSerializer.serializeVector(out, v);            
        }

        public void deserialize(DataInputStream in) throws IOException {
            v=(Vector) ComplexSerializer.deserializeVector(in);
        }

        public boolean equals(TestContainer c) {
            TestClass tc1 = (TestClass)v.elementAt(0);
            TestClass tc2 = (TestClass)c.v.elementAt(0);
            if(!tc1.toString().equals(tc2.toString())){
                Log.info("Not equals");
                Log.info(tc1.toString());
                Log.info(tc2.toString());
                return false;
            }

            String s1 = (String)v.elementAt(1);
            String s2 = (String)c.v.elementAt(1);
            if(!s1.equals(s2)){
                Log.info("Not equals: '"+s1+"' '"+s2+"'");
                return false;
            }

            Integer i1 = (Integer)v.elementAt(2);
            Integer i2 = (Integer)c.v.elementAt(2);
            if(!i1.equals(i2)){
                Log.info("Not equals: '"+i1+"' '"+i2+"'");
                return false;
            }
            return true;
        }
    }

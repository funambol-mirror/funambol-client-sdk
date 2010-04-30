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

import com.funambol.util.StringUtil;
import java.io.DataInputStream;
import java.io.DataOutputStream;
import java.util.Date;
import java.util.Hashtable;
import java.util.Vector;
import java.util.Enumeration;
import java.io.IOException;

import com.funambol.util.Log;

/**
 * A helper class useful to persist objects like <code>Vector</code>s and
 * <code>Hashtable</code>s into the device's store
 */
public class ComplexSerializer {
    
    private static final int NULL = 0;
    private static final int INTEGER = 1;
    private static final int STRING = 2;
    private static final int SERIALIZED = 3;
    private static final int BYTE_ARRAY = 4;
    private static final int HASHTABLE = 5;
    private static final int VECTOR = 6;
    
    
    /**
     * A helper method to serialize an <code>Object</code>.<p>
     * 
     * It can serialize basic objects and the ones implementing the Serializable
     * interface.
     * 
     * TODO: implement more basic objects.
     * @param dout The stream to write data to
     * @param obj The Object to be serialized
     */
    public static void serializeObject(DataOutputStream dout, Object obj)
            throws IOException {
        if (obj instanceof String) {
            dout.writeByte(STRING);
            dout.writeUTF((String)obj);
        } else if (obj instanceof Integer) {
            dout.writeByte(INTEGER);
            dout.writeInt(((Integer)obj).intValue());
        } else if (obj instanceof byte[]) {
            dout.writeByte(BYTE_ARRAY);
            dout.writeInt(((byte[])obj).length);
            dout.write((byte[])obj, 0, ((byte[])obj).length);
        } else if (obj instanceof Serializable) {
            dout.writeByte(SERIALIZED);
            dout.writeUTF(obj.getClass().getName());
            ((Serializable)obj).serialize(dout);
        } else if (obj instanceof Hashtable) {
            dout.writeByte(HASHTABLE);
            serializeHashTable(dout, (Hashtable) obj);
        } else if (obj instanceof Vector) {
            dout.writeByte(VECTOR);
            serializeVector(dout, (Vector) obj);
        } else if (obj == null) {
            dout.writeByte(NULL);
        } else {
            throw new IOException("Cannot serialize object of type "
                    + obj.getClass().getName());
        }
    }
    
    
    /**
     * A helper method to deserialize an <code>Object</code>.<p> It can
     * deserialize basic objects and the ones implementing the Serializable
     * interface.
     * 
     * TODO: implement more basic objects.
     * @param din The stream to write data from
     * @return The Object deserialzed
     */
    public static Object deserializeObject(DataInputStream din)
            throws IOException {

        int type = din.readByte();

        if (type == NULL) {
            return null;
        } else if (type == INTEGER) {
            return (Object)(new Integer(din.readInt()));
        } else if (type == BYTE_ARRAY) {
            byte[] array = new byte[din.readInt()];
            din.read(array);
            return array;
        } else if (type == STRING) {
            return (Object)(din.readUTF());
        } else if (type == HASHTABLE) {
            return deserializeHashTable(din);
        } else if (type == VECTOR) {
            return deserializeVector(din);
        } else if (type == SERIALIZED) {
            String cname = din.readUTF();

            try {
                Class cl = Class.forName(cname);
                Object obj = cl.newInstance();
                ((Serializable)obj).deserialize(din);
                return obj;
            } catch (IOException ioe) {
                Log.error("[deserializeObject] cname: " + cname + " - " + 
                        ioe.toString());
               
                throw ioe;
            } catch (IllegalAccessException iae) {
                String msg = "[deserializeObject] Cannot instantiate class: " +
                        cname + " - " + iae.toString();
                Log.error(msg);
            
                throw new IOException(msg);
            } catch (Exception e) {
                String msg = "[deserializeObject] Exception on cname: " + 
                        cname + " - " + e.toString();
                Log.error(msg);
           
                throw new IOException(msg);
            }
        } else {
            throw new IOException("Deserialization error. Unknown type: [" + 
                    type + "]");
        }
    }
    
    
    /**
     * A helper method to serialize a <code>Hashtable</code> <p>
     *
     * @param dout
     *            The stream to write data to
     * @param ht
     *            The Hashtable to be serialized
     */
    public static void serializeHashTable(DataOutputStream dout, Hashtable ht)
    throws IOException {
        // Store size
        dout.writeInt(ht.size());
        // Iterate through keys
        for(Enumeration e = ht.keys(); e.hasMoreElements(); ){
            Object key = e.nextElement();
            Object val = ht.get(key);
            serializeObject(dout, key);
            serializeObject(dout, val);
        }
    }
    
    
    /**
     * A helper method to deserialize a <code>Hashtable</code> <p>
     *
     * @param din
     *            The stream to write data from
     * @return
     *            The Hashtable deserialzed
     */
    public static Hashtable deserializeHashTable(DataInputStream din)
    throws IOException {
        // Retrieve size
        int size = din.readInt();
        Hashtable ht = new Hashtable();
        
        for(int i=0; i<size; i++){
            Object key = deserializeObject(din);
            Object val = deserializeObject(din);
            ht.put(key, val);
        }
        return ht;
    }
    
    
    /**
     * A helper method to serialize a <code>Vector</code> <p>
     * @param dout The stream to write data to
     * @param v The Vector to be serialized
     */
    public static void serializeVector(DataOutputStream dout, Vector v ) 
                                                            throws IOException {
        
        int n = v.size();
        try {
            dout.writeInt( n );
        } catch (IOException ex) {
            ex.printStackTrace();
            Log.error("IOException  in serializeVector!");
        }
        
        for( int i = 0; i < n; i++ ){
                serializeObject(dout, v.elementAt(i));
        }
    }
    
    
    /**
     * A helper method to deserialize a <code>Vector</code> <p>
     * @param din The stream to write data to
     * @return The deserialized Vector
     */
    public static Vector deserializeVector(DataInputStream din)
    throws IOException {
        
        Vector v = new Vector();
        int n = din.readInt();
        
        for( int i = 0; i < n; ++i ){
            Object obj = deserializeObject(din);
            v.addElement(obj);
        }
        
        return v;
        
    }
    
    /**
     * Write a UTF field to the given DataOutputStream
     * If the field is not null or empty write a "true" before the field, 
     * just "False" otherwise
     * @param out is the DataOutputStream to be written on
     * @param field is the field to be written on the 
     * @throws IOException
     * <p><b>USE THIS METHOD JUST TO WRITE A STRING INTO AN OBJECT WHICH STATE 
     * IS UNKNOWN AT THE MOMENT OF THE DESERIALIZATION: null field will be  
     * written as boolean false into the OutputStream. </p>
     * <p> Read the written Stream using the
     * com.funambol.common.ComplexSerializer.readField(DataOutputStream in) 
     * method.</b></p>
     *
    public static void writeNullOrEmptyField(DataOutputStream out, String field) throws IOException {
        if (!StringUtil.isNullOrEmpty(field)) {
            out.writeBoolean(true);
            out.writeUTF(field);
        } else {
            out.writeBoolean(false);
        }
    }*/
  
    /**
     * Write a UTF field to the given DataOutputStream
     * If the field is not null write a "true" before the field, 
     * just "False" otherwise. This method write true for Empty strings.
     * @param out is the DataOutputStream to be written on
     * @param field is the field to be written on the 
     * @throws IOException
     *
     * <p>Use this method to write a string that can be null at the moment
     * of the serialization: null field will be written as boolean false 
     * into the OutputStream. </p>
     * <p> Read the written Stream using the
     * com.funambol.common.ComplexSerializer.readField(DataOutputStream in) 
     * method.</p>
     */
    public static void writeField(DataOutputStream out, String field) throws IOException {
        if (field!=null) {
            out.writeBoolean(true);
            out.writeUTF(field);
        } else {
            out.writeBoolean(false);
        }
    }
  
    
    /**
     * Read a UTF field to the given DataInputStream
     * If the field exists write a "true", "False" otherwise
     * @param in is the DataInputStream to be read
     * @throws IOException
     * @return String if field exists, null otherwise
     * <p>Use this method to read a string written with:
     * com.funambol.common.ComplexSerializer.writeField(DataInputStream out, 
     * String field) method.</p>
     */
    public static String readField(DataInputStream in) throws IOException {
        if (in.readBoolean()) {
            return in.readUTF();
        }
        return null;
    }
    
    /**
     * Serialize a given array of objects into a given DataOutputStream
     * @param out is the stream to be written
     * @param s is the Serializable object array to be serialized
     */
    public static void serializeObjectArray(DataOutputStream out, 
                                          Object[] obj/*Serializable[] s*/) throws IOException {
        
        /*out.writeInt(s.length);
        for (int i=0; i<s.length; i++) {
            s[i].serialize(out);
        }*/
        
        out.writeInt(obj.length);
        for (int i=0; i<obj.length; i++) {
            serializeObject(out, obj[i]);
        }
    }
    
    /**
     * Deserialize a given DataInputStream into a given array of objects.
     *
     * @param out is the stream to be written
     * @param s is the Serializable object array to be serialized
     */
    public static Object[] deserializeObjectArray(
                                        DataInputStream in) throws IOException {
        
        /*int length = in.readInt();
        for (int i=0; i<length; i++) {
            s[i].deserialize(in);
        }
        return s;*/
        
        int length = in.readInt();
        Object[] obj = new Object[length];
        for (int i=0; i<length; i++) {
            obj[i] = deserializeObject(in);
        }
        return obj;
    }
    
    /**
     * Write a Date field to the given DataOutputStream.
     * <p>Use this method to write a Date field that can be null: null field 
     * will be written as 0L into the OutputStream. </p>
     * <p> Read the written Stream using the
     * com.funambol.common.ComplexSerializer.readDateField(DataOutputStream in) 
     * method.
     *
     * @param out is the DataOutputStream to be written on
     * @param Date is the field to be written on the 
     * @throws IOException
     *
     * Note: the midnight of the 01/01/1970 will be considered null.</p>
     */
    public static void writeDateField(DataOutputStream out, Date field)
    throws IOException {   
        if (field!=null) {
            out.writeLong(field.getTime());
        } else {
            out.writeLong(0L);
        }
    }
  
    
    /**
     * Read a UTF field to the given DataInputStream.
     * <p>Use this method to read a string written with:
     * com.funambol.common.ComplexSerializer.writeDateField(DataInputStream out, 
     * String field) method.</b></p>
     *
     * @param in is the DataInputStream to be read
     * @throws IOException
     * @return Date value if field exists, null otherwise
     */
    public static Date readDateField(DataInputStream in) throws IOException {
        long d = in.readLong();
        if (d > 0) {
            return new Date(d);
        }
        return null;
    }
    
}


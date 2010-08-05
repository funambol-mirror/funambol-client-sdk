/*
 * Funambol is a mobile platform developed by Funambol, Inc. 
 * Copyright (C) 2009 Funambol, Inc.
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

package com.funambol.client.ipc.rpc;

/**
 * This class represents a parameter (both input and output) in remote procedure
 * calls. This implementation supports the types specified by Xml Rpc. A
 * parameter can be a scalar type (int, string and so on) or a complex one, like
 * an array or a struct.
 * The definition of RPCParam is recursive to accomodate the recursive nature of
 * types. Arrays and structs have elements which are RPCParam, allowing for
 * arbitrary complex data to be represented.
 */
public class RPCParam {

    public static final int TYPE_INT       = 0;
    public static final int TYPE_STRING    = 1;
    public static final int TYPE_BOOLEAN   = 2;
    public static final int TYPE_BASE64    = 3;
    public static final int TYPE_DOUBLE    = 4;
    public static final int TYPE_DATETIME  = 5;
    public static final int TYPE_ARRAY     = 6;
    public static final int TYPE_STRUCT    = 7;

    private int        type = TYPE_STRING;

    private String     stringValue;
    private int        intValue;
    private double     doubleValue;
    private boolean    booleanValue;
    private RPCParam[] arrayValue;
    private RPCParam[] structValue;
    private String     name;

    /**
     * Construct an empty parameter, with type STRING
     */
    public RPCParam(){
    }

    /**
     * Get the parameter type.
     */
    public int getType(){
        return type;
    }

    /**
     * Get the parameter value as a string. This method may return null if the
     * parameter has a non STRING type.
     * @return the string value
     */
    public String getStringValue(){
        return stringValue;
    }

    /**
     * Get the parameter value as a integer. This method may return a
     * meaningless value if the parameter has a non INT type.
     * @return the string value
     */
    public int getIntValue() {
        return intValue;
    }

    /**
     * Get the parameter value as a double. This method may return a
     * meaningless value if the parameter has a non DOUBLE type.
     * @return the string value
     */
    public double getDoubleValue() {
        return doubleValue;
    }

    /**
     * Get the parameter value as a string representing date time. This method may return null if the
     * parameter has a non DATETIME type.
     * @return the string value
     */
    public String getDateTimeValue() {
        return stringValue;
    }

    /**
     * Get the parameter value as a string representing base64 data. This method may return null if the
     * parameter has a non BASE64 type.
     * @return the string value
     */
    public String getBase64Value() {
        return stringValue;
    }

    /**
     * Get the parameter value as a boolean. This method may return a
     * meaningless value if the parameter has a non BOOLEAN type.
     * @return the boolean value
     */
    public boolean getBooleanValue() {
        return booleanValue;
    }

    /**
     * Get the parameter value as an array. This method may return null
     * if the parameter has a non ARRAY type.
     * Each value of the array is represented as an RPCParam, with its type and
     * its value
     * @return the array value
     */
    public RPCParam[] getArrayValue() {
        return arrayValue;
    }

    /**
     * Get the parameter value as a struct. This method may return null
     * if the parameter has a non STRUCT type.
     * Each member of the struct is represented as an RPCParam, with its type, value
     * and name.
     * @return the struct value
     */
    public RPCParam[] getStructValue() {
        return structValue;
    }

    /**
     * Get the parameter name. This value is only used/meaningful for struct
     * where fields are named.
     */
    public String getName() {
        return name;
    }

    /**
     * Set the integer value of this parameter. The type is set to INT
     * automatically.
     * @param value the value
     */
    public void setIntValue(int value) {
        intValue = value;
        type     = TYPE_INT;
    }

    /**
     * Set the double value of this parameter. The type is set to DOUBLE
     * automatically.
     * @param value the value
     */
    public void setDoubleValue(double value) {
        doubleValue = value;
        type        = TYPE_DOUBLE;
    }

    /**
     * Set the string value of this parameter. The type is set to STRING
     * automatically.
     * @param value the value
     */
    public void setStringValue(String value) {
        stringValue = value;
        type        = TYPE_STRING;
    }

    /**
     * Set the base 64 (string) value of this parameter. The type is set to
     * BASE64 automatically.
     * @param value the value
     */
    public void setBase64Value(String value) {
        stringValue = value;
        type        = TYPE_BASE64;
    }

    /**
     * Set the date time (string) value of this parameter. The type is set to
     * DATETIME automatically.
     * @param value the value
     */
    public void setDateTimeValue(String value) {
        stringValue = value;
        type        = TYPE_DATETIME;
    }

    /**
     * Set the boolean value of this parameter. The type is set to
     * BOOLEAN automatically.
     * @param value the value
     */
    public void setBooleanValue(boolean value) {
        booleanValue = value;
        type         = TYPE_BOOLEAN;
    }

    /**
     * Set the array value of this parameter. The type is set to
     * ARRAY automatically.
     * @param value the value
     */
    public void setArrayValue(RPCParam[] value) {
        arrayValue = value;
        type       = TYPE_ARRAY;
    }

    /**
     * Set the struct value of this parameter. The type is set to
     * STRUCT automatically.
     * @param value the value
     */
    public void setStructValue(RPCParam[] value) {
        structValue = value;
        type        = TYPE_STRUCT;
    }

    /**
     * Set the parameter name. This value is only used/meaningful for struct
     * where fields are named.
     */
    public void setName(String name) {
        this.name = name;
    }
}

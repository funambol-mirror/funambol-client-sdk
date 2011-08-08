/*
 * Funambol is a mobile platform developed by Funambol, Inc. 
 * Copyright (C) 2011 Funambol, Inc.
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

import java.util.Vector;

import com.funambol.util.ComparableI;

public class Tuple implements ComparableI {

    private Vector fields = null;
    private int arity;

    private static final String UNDEFINED = "UNDEFINED";

    private int colsType[];
    private int keyIdx;
    private int orderingField = -1;

    public Tuple(int colsType[], int keyIdx) {
        this.colsType = colsType;
        this.arity = colsType.length;
        this.keyIdx = keyIdx;
        fields = new Vector(arity);
        for(int i=0;i<arity;++i) {
            fields.addElement(UNDEFINED);
        }
    }

    public int getType(int idx) {
        if (idx >= arity) {
            throw new IllegalArgumentException("Invalid index");
        }
        return colsType[idx];
    }

    public void setField(int idx, long value) {
        if (Table.TYPE_LONG != colsType[idx]) {
            throw new IllegalArgumentException("trying to set a long value into a non long column "
                                               + colsType[idx]);
        }
        fields.setElementAt(new Long(value), idx);
    }

    public void setField(int idx, Object value) {
        if (value instanceof String) {
            if (Table.TYPE_STRING != colsType[idx]) {
                throw new IllegalArgumentException("trying to set a string value into a non string column "
                        + colsType[idx]);
            }
        } else if (value instanceof Long) {
            if (Table.TYPE_LONG != colsType[idx]) {
                throw new IllegalArgumentException("trying to set a long value into a non long column "
                        + colsType[idx]);
            }
        } else if(value == null) {
            value = UNDEFINED;
        } else {
            throw new IllegalArgumentException("Trying to set a value of unsupported type " + value);
        }
        fields.setElementAt(value, idx);
    }

    public void removeField(int idx) {
        setField(idx, UNDEFINED);
    }

    public int getArity() {
        return arity;
    }

    public Object getKey() {
        return getField(keyIdx);
    }

    public String getStringField(int idx) {
        if (isUndefined(idx)) {
            throw new IllegalArgumentException("Field " + idx + " is undefined");
        } else {
            return (String)fields.elementAt(idx);
        }
    }

    public Long getLongField(int idx) {
        if (isUndefined(idx)) {
            throw new IllegalArgumentException("Field " + idx + " is undefined");
        } else {
            return (Long)fields.elementAt(idx);
        }
    }

    public boolean isUndefined(int idx) {
        // We do not want to compare string values, but just their reference
        return fields.elementAt(idx) == UNDEFINED;
    }

    public int compareTo(Object v2) {
        Tuple t2 = (Tuple)v2;
        if (orderingField == -1) {
            throw new IllegalStateException("Cannot compare items whose ordering field has not been set");
        }
        if (colsType[orderingField] == Table.TYPE_STRING) {
            String s1 = getStringField(orderingField);
            String s2 = t2.getStringField(orderingField);
            return s1.compareTo(s2);
        } else if (colsType[orderingField] == Table.TYPE_LONG) {
            Long l1 = getLongField(orderingField);
            Long l2 = t2.getLongField(orderingField);
            if (l1.longValue() == l2.longValue()) {
                return 0;
            } else if (l1.longValue() > l2.longValue()) {
                return 1;
            } else {
                return -1;
            }
        } else {
            throw new IllegalStateException("Unknown field type in tuple comparison");
        }
    }

    protected Object getField(int idx) {
        return fields.elementAt(idx);
    }

    protected void setOrderingField(int idx) {
        orderingField = idx;
    }
}


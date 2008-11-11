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

package com.funambol.syncclient.spds.engine;

/**
 * Represents a property (a piece of information) of a <i>SyncItem</i>.
 *
 *
 *
 * @version Stefano Fornari
 */
public class SyncItemProperty {

    // -------------------------------------------------------------- Properties

    /**
     * The property name
     */
    private String name = null;
    public String getName(){
        return this.name;
    }

    public void setName(String name){
        this.name = name;
    }

    /**
     * The property type
     */
    private String type = null;
    public String getType(){
        return type;
    }

    public void setType(String type){
        this.type = type;
    }

    /**
     * Is the property a key?
     */
    private boolean key = false;
    public boolean isKey(){
        return key;
    }

    /**
     * The property value
     */
    private Object value = null;
    public Object getValue(){
        return value;
    }

    public void setValue(Object value){
        this.value = value;
    }

    // ------------------------------------------------------------- Contructors

    public SyncItemProperty(String name, Object value) {
        this(name, value, value.getClass().getName());
    }

    public  SyncItemProperty(String name, Object value, String type) {
        this.name  = name;
        this.value = value;
        this.type  = type;
    }
}

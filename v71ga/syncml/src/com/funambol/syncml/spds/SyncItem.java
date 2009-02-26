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

package com.funambol.syncml.spds;

/**
 * This class is a container for the items exchanged between the SyncManager
 * and the SyncSources
 */
public class SyncItem {
    
    //----------------------------------------------------------------- Constants
    public static final char STATE_NEW = 'N';
    public static final char STATE_UPDATED = 'U';
    public static final char STATE_DELETED = 'D';
    public static final char STATE_UNDEF = ' ';

    //-------------------------------------------------------------- Private data

    /** The key of this item.  */
    private String key;
    
    /** The mime-type of this item. Default is text/plain.  */
    private String type;
    
    /** The state of this item ([N]ew, [U]pdated, [D]eleted) */
    private char state;

    /** The name of the parent folder of the item.  */
    private String parent;
    
    /** The content of this item */
    private byte[] content;

    /** The client representation of this item (may be null) */
    private Object clientRepresentation;

    
    //------------------------------------------------------------- Constructors
    
    /**
     * Basic constructor. Only the key is required, the others
     * are set to a default and can be set later.
     */
    public SyncItem(String key) {
        this(key, null, STATE_NEW, null, null);
    }
    
    /**
     * Full contructor. All the item's fields are passed by the caller.
     */
    public SyncItem(String key, String type, char state,
                    String parent, byte[] content) {
        this.key = key;
        this.type = type;
        this.state = state;
        this.parent = parent;
        this.content = content;
        this.clientRepresentation = null;
    }

    /**
     * Copy constructor. The item is created using the values from another
     * one.
     */
    public SyncItem(SyncItem that) {
        this.key = that.key;
        this.type = that.type;
        this.state = that.state;
        this.parent = that.parent;
        this.content = that.content;
        this.clientRepresentation = that.clientRepresentation;
    }

    //----------------------------------------------------------- Public methods
    
    /**
     * Get the current key
     */
    public String getKey() {
        return this.key;
    }
    
    /**
     * Set the current key
     */
    public void setKey(String key) {
        this.key = key;
    }
    
    /**
     * Get the item type (this property may be null)
     * A value whose type is null has the type of the SyncSource
     * it belongs to.
     */
    public String getType() {
        return this.type;
    }
    
    /**
     * Set the item type
     */
    public void setType(String type) {
        this.type = type;
    }
    
    /**
     * Get the item state
     */
    public char getState() {
        return this.state;
    }
    
    /**
     * Set the item state
     */
    public void setState(char state) {
        this.state = state;
    }
    
    /**
     * Get the item parent
     */
    public String getParent() {
        return this.parent;
    }
    
    /**
     * Set the item parent
     */
    public void setParent(String parent) {
        this.parent = parent;
    }

    /**
     * Get the content of this item
     */
    public byte[] getContent() {
        return this.content;
    }
    
    /**
     * Set the content of this item
     */
    public void setContent(byte[] content) {
        this.content = content;
    }

    /**
     * Get the client representation of this item (maybe null)
     */
    public Object getClientRepresentation() {
        return this.clientRepresentation;
    }

    /**
     * Set the client representation of this item (maybe null)
     */
    public void setClientRepresentation(Object clientRepresentation) {
        this.clientRepresentation = clientRepresentation;
    }
}

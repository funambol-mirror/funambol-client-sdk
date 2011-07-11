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

package com.funambol.syncml.protocol;

import java.util.Vector;

/**
 * This class is a container for SyncML command
 */
public class SyncMLCommand implements ReusableObject {
    
    //----------------------------------------------------------------- Constants

    //-------------------------------------------------------------- Private data

    /** The command tag name */
    private String name;

    /** The id of this command */
    private String cmdId;
    
    /** The mime-type of the items in this command.  */
    private String type;

    private Vector items;
    private Meta   meta;

    private boolean noResp = false;

    /* This attribute is not specific to SycnML, it is rather an attribute used
     * for convenience during size computation
     */
    private int size;
    
    //------------------------------------------------------------- Constructors
    SyncMLCommand(String name, String cmdId) {
        this(name, cmdId, null);
    }
    
    SyncMLCommand(String name, String cmdId, String type) {
        this.name = name;
        this.cmdId = cmdId;
        this.type = type;
    }

    //----------------------------------------------------------- Public methods
    public static SyncMLCommand newInstance(String name) {
        return ObjectsPool.createSyncMLCommand(name);
    }
    
    /**
     * Get the command tag name
     */
    public String getName() {
        return this.name;
    }
    
    /**
     * Get the command id
     */
    public String getCmdId() {
        return this.cmdId;
    }

    public void setCmdId(int id) {
        this.cmdId = "" + id;
    }

    public void setCmdId(String cmdId) {
        this.cmdId = cmdId;
    }
    
    /**
     * Get the mime type of the items of this command
     */
    public String getType() {
        return this.type;
    }
    
    /**
     * Set the mime type of the items of this command
     */
    public void setType(String type) {
        this.type = type;
    }

    public void setItems(Vector items) {
        this.items = items;
    }

    public void setMeta(Meta meta) {
        this.meta = meta;
    }

    public Meta getMeta() {
        return meta;
    }

    public Vector getItems() {
        return items;
    }

    public void setSize(int size) {
        this.size = size;
    }

    public int getSize() {
        return size;
    }

    public boolean getNoResp() {
        return noResp;
    }

    public void setNoResp(boolean value) {
        this.noResp = value;
    }

    public void setName(String name) {
        this.name = name;
    }

    public void init() {
        name  = null;
        cmdId = null;
        type  = null;
        items = null;
        meta  = null;
        size  = 0;
    }
 

}


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

package com.funambol.syncclient.spds.source;

import java.security.Principal;
import java.util.Date;

import com.funambol.syncclient.spds.engine.*;
import com.funambol.syncclient.spds.SyncException;


/**
 * This class implements a dummy <i>SyncSource</i> that just displays the calls
 * to its methods
 *
 *
 *
 * @version $Id: DummySyncSource.java,v 1.3 2007-12-22 18:09:17 nichele Exp $
 *
 */
public class DummySyncSource implements SyncSource {

    private String name       = null;
    private String type       = null;
    private String sourceURI  = null;

    private SyncItem[] allItems     = null;
    private SyncItem[] newItems     = null;
    private SyncItem[] deletedItems = null;
    private SyncItem[] updatedItems = null;

    // ------------------------------------------------------------ Constructors

    /** Creates a new instance of AbstractSyncSource */
    public DummySyncSource() {
        newItems     = new SyncItem[] {
                           createItem("10", "This is a new item", SyncItemState.NEW)
                       };
        deletedItems = new SyncItem[] {
                           createItem("20", "This is a deleted item", SyncItemState.DELETED)
                       };
        updatedItems = new SyncItem[] {
                           createItem("30", "This is an updated item", SyncItemState.UPDATED)
                       };

        allItems = new SyncItem[newItems.length + updatedItems.length + 1];

        allItems[0] = createItem("40", "This is an unchanged item", SyncItemState.SYNCHRONIZED);
        allItems[1] = newItems[0];
        allItems[2] = updatedItems[0];
    }


    // ---------------------------------------------------------- Public methods

    public String getName() {
        return name;
    }

    public void setName(String name) {
        System.out.println("setName(" + name + ")");
        this.name = name;
    }

    public String getType() {
        return this.type;
    }

    public void setType(String type) {
        System.out.println("setType(" + type + ")");
        this.type = type;
    }

    /** Getter for property uri.
     * @return Value of property uri.
     */
    public String getSourceURI() {
        return sourceURI;
    }

    /** Setter for property uri.
     * @param sourceURI New value of property uri.
     */
    public void setSourceURI(String sourceURI) {
        System.out.println("setSourceURI(" + sourceURI + ")");
        this.sourceURI = sourceURI;
    }

    /**
     * Some other initialization parameter
     */
    public void setParam1(String value) {
        System.out.println("setParam1(" + value + ")");
    }

    /**
     * Returns a string representation of this object.
     *
     * @return a string representation of this object.
     */
    public String toString() {
        StringBuffer sb = new StringBuffer(super.toString());

        sb.append(" - {name: ").append(getName()     );
        sb.append(" type: "   ).append(getType()     );
        sb.append(" uri: "    ).append(getSourceURI());
        sb.append("}"         );

        return sb.toString();
    }


    public void beginSync(int type) throws SyncException {
        System.out.println("beginSync(" + type + ")");
    }

    public void endSync() throws SyncException {
        System.out.println("endSync()");
    }

    public SyncItem[] getAllSyncItems(Principal principal) throws SyncException {
        System.out.println("getAllSyncItems(" + principal + ")");
        return allItems;
    }


    public SyncItem[] getDeletedSyncItems(Principal principal,
                                          Date since    ) throws SyncException {
        System.out.println("getDeletedSyncItems(" + principal + " , " + since + ")");

        return deletedItems;

    }


    public SyncItem[] getNewSyncItems(Principal principal,
                                      Date since    ) throws SyncException {
        System.out.println("getNewSyncItems(" + principal + " , " + since + ")");

        return newItems;

    }

    public SyncItem[] getUpdatedSyncItems(Principal principal,
                                          Date since    ) throws SyncException {
        System.out.println("getUpadtedSyncItems(" + principal + " , " + since + ")");

        return updatedItems;

    }

    public void removeSyncItem(Principal principal, SyncItem syncItem) throws SyncException {
        System.out.println("removeSyncItem(" + principal + " , " + syncItem.getKey().getKeyAsString() + ")");
    }

    public SyncItem setSyncItem(Principal principal, SyncItem syncItem) throws SyncException {
        System.out.println("setSyncItem(" + principal + " , " + syncItem.getKey().getKeyAsString() + ")");
        return new SyncItemImpl(this, syncItem.getKey().getKeyAsString()+"-1");
    }

    public void beginSync() {
    }

    public void commitSync() {
    }

    // ------------------------------------------------------------ Private data

    private SyncItem createItem(String id, String content, char state) {
        SyncItem item = new SyncItemImpl(this, id, state);

        item.setProperty(
            new SyncItemProperty(SyncItem.PROPERTY_BINARY_CONTENT, content.getBytes())
        );
        item.setProperty(
                new SyncItemProperty(SyncItem.PROPERTY_TYPE,this.getType())
        );

        return item;
    }
}

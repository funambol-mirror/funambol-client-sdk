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

package com.funambol.syncclient.spds.engine.recovery;

import java.io.*;

import java.util.Date;

import java.security.Principal;

import com.funambol.syncclient.spds.SyncException;
import com.funambol.syncclient.spds.engine.SyncItem;
import com.funambol.syncclient.spds.engine.SyncItemImpl;
import com.funambol.syncclient.spds.engine.SyncItemState;
import com.funambol.syncclient.spds.engine.SyncSource;

/**
 * This class defines <b>recovery</b> method to manage recovery
 * database in slowsync status.
 * From server reads those parameters:
 * <pre>
 *    &lt;recovery-details>
 *      &lt;source-list>value&lt;/source-list>
 *      &lt;last>value&lt;/last>
 *      &lt;uri>value&lt;/uri>
 *    &lt;/recovery-details>
 * </pre>
 *
 *
 *  @version $Id: AbstractRecovery.java,v 1.3 2007-12-22 18:09:17 nichele Exp $
 **/

public abstract class AbstractRecovery implements SyncSource {


    //----------------------------------------------------------------- Constants

    private static final String PROP_RECOVERY_DETAILS = "recovery-details" ;

    private static final String PROP_SOURCE_LIST      = "source-list"      ;
    private static final String PROP_LAST             = "last"             ;
    private static final String PROP_URI              = "uri"              ;

    //----------------------------------------------------------------- Protected data

    protected String sourceList  = null;
    protected String last        = null;
    protected String uri         = null;

    //----------------------------------------------------------------- Public methods

    /** Getter for property sourceList.
     * @return Value of property sourceList.
     */
    public String getSourceList() {
        return this.sourceList;
    }

    /** Setter for property sourceList.
     * @param sourceList New value of property sourceList.
     */
    public void setSourceList(String sourceList) {
        this.sourceList=sourceList;
    }

    /** Getter for property last.
     * @return Value of property last.
     */
    public String getLast() {
        return this.last;
    }

    /** Setter for property last.
     * @param last New value of property last.
     */
    public void setLast(String last) {
        this.last=last;
    }

    /** Getter for property uri.
     * @return Value of property uri.
     */
    public String getUri() {
        return this.uri;
    }

    /** Setter for property uri.
     * @param uri New value of property uri.
     */
    public void setUri(String uri) {
        this.uri=uri;
    }

    /**
     * This method manage recovery
     * database in slowsync status
     * @param p Principal
     */
    public abstract void recovery (Principal p);


    /**
     * Returns all items in the data store belonging to the given principal.
     *
     * @param principal the principal for which the data has to be considered
     *                  Null means all principals
     *
     * @return an array of all <i>SyncItem</i>s stored in this source. If there
     *         are no items an empty array is returned.
     *
     * @throws SyncException in case of error (for instance if the
     *         underlying data store runs into problems)
     */
    public SyncItem[] getAllSyncItems(Principal principal) throws SyncException {

        SyncItem[] syncItems = new SyncItem[0];

        return syncItems;
    }

    /**
     * Returns all deleted items belonging to the given principal and deleted
     * after the given point in time.
     *
     * @param principal the principal for which the data has to be considered
     *                  Null means all principals
     * @param since consider the changes since this point in time. Null means
     *              all items regardless when they were changed.
     *
     * @return an array of keys containing the <i>SyncItem</i>'s key of the deleted
     *         items after the last synchronizazion. If there are no deleted
     *         items an empty array is returned.
     *
     * @throws SyncException in case of error (for instance if the
     *         underlying data store runs into problems)
     */
    public SyncItem[] getDeletedSyncItems(Principal principal,
                                          Date since    ) throws SyncException {
        SyncItem[] syncItems = new SyncItem[0];

        return syncItems;

    }

    /**
     * Returns all new items belonging to the given principal and created
     * after the given point in time.
     *
     * @param principal the principal for which the data has to be considered
     *                  Null means all principals
     * @param since consider the changes since this point in . Null means
     *              all items regardless when they were changed.
     *
     * @return an array of items containing representing the newly created items.
     *         If there are no new items an empty array MUST BE returned.
     *
     * @throws SyncException in case of error (for instance if the
     *         underlying data store runs into problems)
     */
    public SyncItem[] getNewSyncItems(Principal principal,
                                      Date since    ) throws SyncException {
        SyncItem[] syncItems = new SyncItem[0];

        return syncItems;
    }

    /**
     * Returns all updated items belonging to the given principal and modified
     * after the given point in time.
     *
     * @param principal the principal for which the data has to be considered
     *                  Null means all principals
     * @param since consider the changes since this point in time. Null means
     *              all items regardless when they were changed.
     *
     * @return an array of keys containing the <i>SyncItem</I>'s key of the updated
     *         items after the last synchronizazion. It MUST NOT return null for
     *         no keys, but instad an empty array.
     */
    public SyncItem[] getUpdatedSyncItems(Principal principal,
                                          Date since    ) throws SyncException {
        SyncItem[] syncItems = new SyncItem[0];

        return syncItems;

    }

    /**
     * Removes a SyncItem given its key.
     *
     * @param principal the entity that wants to do the operation
     * @param syncItem  the item to remove
     *
     * @throws SyncException in case of error (for instance if the
     *         underlying data store runs into problems)
     */
    public void removeSyncItem(Principal principal, SyncItem syncItem) throws SyncException {

    }

    /**
     * Method implements start sync operations
     */
    public void beginSync(int syncMode) throws SyncException {

    }

    /**
     * Method implements end sync operations
     */
    public void commitSync() throws SyncException {

    }

    /**
     * Replaces an existing <i>SyncItem</i> or adds a new <i>SyncItem</i> if it
     * does not exist.
     *
     * @return the inserted
     *
     * @throws SyncException in case of error (for instance if the
     *         underlying data store runs into problems)
     */
    public SyncItem setSyncItem(Principal principal, SyncItem syncItem)
    throws SyncException {

        String xmlValue           = null;
        String xmlRecoveryDetails = null;

        xmlValue = new String((byte[])syncItem.getPropertyValue(SyncItem.PROPERTY_BINARY_CONTENT));

        try {
            xmlRecoveryDetails  = getXMLTagValue(xmlValue, PROP_RECOVERY_DETAILS);
        } catch (Exception e) {
            throw new SyncException("Tag not found: " + PROP_RECOVERY_DETAILS);
        }

        try {
            this.sourceList = getXMLTagValue(xmlRecoveryDetails, PROP_SOURCE_LIST);
        } catch (Exception e) {
            throw new SyncException("Tag not found: " + PROP_SOURCE_LIST);
        }

        try {
            this.last = getXMLTagValue(xmlRecoveryDetails, PROP_LAST);
        } catch (Exception e) {
            throw new SyncException("Tag not found: " + PROP_LAST);
        }

        try {
            this.uri = getXMLTagValue(xmlRecoveryDetails, PROP_URI);
        } catch (Exception e) {
            throw new SyncException("Tag not found: " + PROP_URI);
        }

        recovery(principal);

        SyncItem newSyncItem =
                        new SyncItemImpl(this, syncItem.getKey().getKeyAsString(), SyncItemState.NEW);

        newSyncItem.setProperties(syncItem.getProperties());

        return newSyncItem;

    }

    //----------------------------------------------------------------- Private methods

    /**
     * Make a String by value of <i>tag</i>.
     *
     * @param xml xml msg
     * @param tag tag to find
     * @return tag value
     **/
    private String getXMLTagValue(String xml, String tag) {

        String startTag  = null;
        String endTag    = null;

        String value     = null;

        startTag = "("  + tag + ")";
        endTag   = "(/" + tag + ")";

        value = xml.substring(xml.indexOf(startTag) + startTag.length(), xml.indexOf(endTag));

        return value;
    }

}

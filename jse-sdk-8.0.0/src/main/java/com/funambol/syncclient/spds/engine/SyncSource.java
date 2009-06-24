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

import java.io.*;
import java.util.Date;
import java.security.Principal;

import com.funambol.syncclient.spds.SyncException;

/**
 * A <i>SyncSource</i> is responsible for storing and retrieving <i>SyncItem</i>
 * objects from/to an external data source.
 * Note that the <i>SyncSource</i> interface makes no assumptions about the
 * underlying data source or about how data are formatted: each concrete
 * implementation will use its specific storage and formae.
 * <p>
 * A <i>SyncSource</i> is not used directly by the host application, instead its
 * methods are called by the synchronization engine during modifications analysis.
 * <p>
 * The SyncSource methods are designed to perform an efficient synchronization
 * process, letting the source selecting the changed items instead of doing more
 * complex field by field comparison. It is responsibility of the source
 * developer to make sure that the <i>getNew/Updated/DeletedSyncItems()</i>
 * methods return the correct values.
 * <p>
 * The configuration information required to set up a SyncSource is stored in
 * the device management store under the context ./syncml/sources with the
 * structure below:
 * <pre>
 *    .../<i>{source name}</i>
 *        + class
 *        + uri
 *        + type
 *        + ... other properties ...
 * </pre>
 *
 * Where <i>other properties</i> are implementation specific and are set by the
 * engine when the source is instantiated.
 *
 * @version $Id: SyncSource.java,v 1.3 2007-12-22 18:09:18 nichele Exp $
 **/
public interface SyncSource {

    /**
     * Returns the name of the source
     *
     * @return the name of the source
     */
    public String getName();

    /**
     * Returns the source URI
     *
     * @return the absolute URI of the source
     */
    public String getSourceURI();

    /**
     * Returns the type of the source.
     * The types are defined as mime-types, for instance * text/x-vcard).
     * @return the type of the source
     */
    public String getType();

    /**
     * Returns all items in the data store belonging to the given principal.
     *
     * @param principal not used, always null
     *
     * @return an array of all <i>SyncItem</i>s stored in this source. If there
     *         are no items an empty array is returned.
     *
     * @throws SyncException in case of error (for instance if the
     *         underlying data store runs into problems)
     */
    public SyncItem[] getAllSyncItems(Principal principal)
    throws SyncException;


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
    public SyncItem[] getDeletedSyncItems(Principal principal, Date since)
    throws SyncException;


    /**
     * Returns all new items belonging to the given principal and created
     * after the given point in time.
     *
     * @param principal not used, always null
     * @param since consider the changes since this point in . Null means
     *              all items regardless when they were changed.
     *
     * @return an array of items containing representing the newly created items.
     *         If there are no new items an empty array MUST BE returned.
     *
     * @throws SyncException in case of error (for instance if the
     *         underlying data store runs into problems)
     */
    public SyncItem[] getNewSyncItems(Principal principal, Date since)
    throws SyncException;

    /**
     * Returns all updated items belonging to the given principal and modified
     * after the given point in time.
     *
     * @param principal not used, always null
     * @param since consider the changes since this point in time. Null means
     *              all items regardless when they were changed.
     *
     * @return an array of keys containing the <i>SyncItem</I>'s key of the updated
     *         items after the last synchronizazion. It MUST NOT return null for
     *         no keys, but instad an empty array.
     */
    public SyncItem[] getUpdatedSyncItems(Principal principal, Date since)
    throws SyncException;


    /**
     * Removes a SyncItem given its key.
     *
     * @param principal not used, always null
     * @param syncItem  the item to remove
     *
     * @throws SyncException in case of error (for instance if the
     *         underlying data store runs into problems)
     */
    public void removeSyncItem(Principal principal, SyncItem syncItem)
    throws SyncException;

    /**
     * Replaces an existing <i>SyncItem</i> or adds a new <i>SyncItem</i> if it
     * does not exist. The item is also returned giving the opportunity to the
     * source to modify its content and return the updated item (i.e. updating
     * the id to the GUID).
     *
     * @param principal not used, always null
     * @param syncItem  the item to replace/add
     *
     * @return the inserted/updated item
     *
     * @throws SyncException in case of error (for instance if the
     *         underlying data store runs into problems)
     */
    public SyncItem setSyncItem(Principal principal, SyncItem syncItem)
    throws SyncException;


    /**
     * Called after SyncManager preparation and initialization just before start
     * the synchronization of the SyncSource.
     *
     * @param syncMode the synchronization type: one of the values in
     *                 sync4j.framework.core.AlertCode
     *
     * @throws SyncException in case of error. This will stop the sync process
     */
    public void beginSync(int syncMode) throws SyncException;

    /**
     * Called just before committing the synchronization process by the
     * SyncManager. If an error is detected and
     *
     * @throws SyncException
     */
    public void commitSync() throws SyncException;


}

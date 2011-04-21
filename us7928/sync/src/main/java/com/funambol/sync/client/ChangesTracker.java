/*
 * Funambol is a mobile platform developed by Funambol, Inc.
 * Copyright (C) 2008 Funambol, Inc.
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

package com.funambol.sync.client;

import java.util.Enumeration;
import java.util.Vector;

import com.funambol.sync.SyncItem;

/**
 * This interface can be used by TrackableSyncSource to detect changes occourred
 * since the last synchronization. The API provides a basic implementation
 * in CacheTracker which detects changes comparing fingerprints.
 * Client can implement this interface and use it in the TrackableSyncSource if more
 * efficient methods are available.
 */
public interface ChangesTracker {

    /**
     * This method associates this tracker to the given Sync Source. This method
     * must be called once before using the tracker. The tracker is invalid
     * until this method is invoked.
     */
    public void setSyncSource(TrackableSyncSource ss);

    /**
     * This method allows implementations to get ready to return list of
     * changes. An implementation may decide to detect all changes at this point
     * or not, the interface does not enforce any semantics on this.
     * The only requirement is that any change made before this method is
     * invoked must be part of the lists returned by other methods.
     *
     * @param syncMode The sync mode
     * @param resume true if this sync is resuming a previously interrupted one
     */
    public void begin(int syncMode, boolean resume) throws TrackerException;

    /**
     * This method allows implementations to clean data. After this method is
     * invoked it is illegal to fetch list of changes. A new call to "begin" is
     * required first.
     */
    public void end() throws TrackerException;

    /**
     * Returns the list of new items. The list of new items is frozen when the
     * method is invoked, after this method returns any change is not part of
     * the list of new items returned. Changes that happen after this method
     * returns will be part of the next changes set.
     * There is no guarantee that the list will contain all the changes that
     * happened after the begin method was invoked. This is an implementation
     * choice, it is only guaranteed that the list includes all the changes at
     * the time "begin" was invoked.
     *
     * @return the list of new items as an Enumeration
     *         of SyncItem
     */
    public Enumeration getNewItems() throws TrackerException;

    /**
     * Returns the number of new items that will be returned by the getNewItems
     * method
     *
     * @return the number of items
     */
    public int getNewItemsCount() throws TrackerException;

    /**
     * Returns the list of updated items. The list of updated items is frozen when the
     * method is invoked, after this method returns any change is not part of
     * the list of new items returned. Changes that happen after this method
     * returns will be part of the next changes set.
     * There is no guarantee that the list will contain all the changes that
     * happened after the begin method was invoked. This is an implementation
     * choice, it is only guaranteed that the list includes all the changes at
     * the time "begin" was invoked.
     *
     * @return the list of updated items as an Enumeration
     *         of SyncItem
     */
    public Enumeration getUpdatedItems() throws TrackerException;

    /**
     * Returns the number of updated items that will be returned by the getUpdatedItems
     * method
     *
     * @return the number of items
     */
    public int getUpdatedItemsCount() throws TrackerException;


    /**
     * Returns the list of deleted items. The list of deleted items is frozen when the
     * method is invoked, after this method returns any change is not part of
     * the list of new items returned. Changes that happen after this method
     * returns will be part of the next changes set.
     * There is no guarantee that the list will contain all the changes that
     * happened after the begin method was invoked. This is an implementation
     * choice, it is only guaranteed that the list includes all the changes at
     * the time "begin" was invoked.
 
     *
     * @return the list of updated items as an Enumeration
     *         of strings (SyncItem's keys)
     */
    public Enumeration getDeletedItems() throws TrackerException;

    /**
     * Returns the number of deleted items that will be returned by the getDeletedItems
     * method
     *
     * @return the number of items
     */
    public int getDeletedItemsCount() throws TrackerException;

    /**
     * Set the status for a group of items. The status is returned by the server as result of the
     * synchronization. This method is needed by the tracker to decide
     * if an items can be removed from the list of changes or they must be kept
     * for the next run
     *
     * @param key the item key (cannot be null)
     * @param status the syncml status for this item
     */
    public void setItemsStatus(Vector itemsStatus) throws TrackerException;

    /**
     * This method resets the list of changes. The lists (new, upd, del) will be empty
     * immediately after this call is invoked.
     */
    public void reset() throws TrackerException;

    /**
     * Remove the given item from the list of changes
     *
     * @param item is the item to be removed
     * @return true if an item was found and deleted
     */
    public boolean removeItem(SyncItem item) throws TrackerException;

    /**
     * Empty the tracker status. After this method is invoked all the items
     * currently present will be reported as new items.
     */
    public void empty() throws TrackerException;

    /**
     * Returns true if this tracker supports sync resume.
     */
    public boolean supportsResume();

    /**
     * Checks if the given item has changed since the beginning of the last sync
     *
     * @param key the item key
     * @param ts the last sync start time
     *
     * @return true if the item has changed since the last sync
     */
    public boolean hasChangedSinceLastSync(String key, long ts);
    
    
    /**
     * Checks if an item should be filtered out or not.
     * Typical implementation will call the {@link TrackableSyncSource#filterOutgoingItem(String)}
     * method, leaving to SyncSource all the filtering logic.
     * Simply return false if tracker (and linked SyncSource) don't implement
     * any filters.
     * 
     * @param key
     * @return true if the item must be filtered out, otherwise false
     */
    public boolean filterItem(String key);

}


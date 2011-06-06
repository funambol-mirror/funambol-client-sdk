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

package com.funambol.sync;

public interface ResumableSource {

    /** This method returns true if the source is in a state where it can resume
     * an interrupted synchronization.
     */
    public boolean readyToResume();
    
    /**
     * This method checks if a given items exists locally.
     * @param key the item key
     * @return true if the item exists in the local data store
     * @throws SyncException if the local data store cannot be properly accessed
     */
    public boolean exists(String key) throws SyncException;

    /**
     * This method checks if a given item has changed since the last sync, whose start timestamp
     * is provided in case the source needs it.
     * @param key the item key
     * @param lastSyncStartTime the time at which the last sync started
     * @return true if the item has changed
     */
    public boolean hasChangedSinceLastSync(String key, long lastSyncStartTime);

    /**
     * Returns the size of a partially received item. 
     * This method allow engines capable of single items
     * resuming to resume the download.
     * If the size is unknown or cannot be retrieved, -1 must be returned.
     */
    public long getPartiallyReceivedItemSize(String key);

    /**
     * This method returns the luid of an item received from the server. This
     * method is only needed for sources that support single item resuming.
     * If the source does not support single item resuming, then null must be
     * returned.
     */
    public String getLuid(SyncItem item);


}


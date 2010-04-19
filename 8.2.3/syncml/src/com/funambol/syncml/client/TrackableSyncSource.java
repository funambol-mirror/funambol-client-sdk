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

package com.funambol.syncml.client;

import java.util.Enumeration;
import java.util.Vector;
import java.io.IOException;

import com.funambol.syncml.spds.SourceConfig;
import com.funambol.syncml.spds.SyncItem;
import com.funambol.syncml.spds.SyncSource;
import com.funambol.syncml.spds.SyncException;
import com.funambol.syncml.spds.SyncListener;

import com.funambol.syncml.protocol.SyncFilter;
import com.funambol.syncml.protocol.SyncML;

import com.funambol.util.Log;

/**
 * An abstract implementation of BaseSyncSource, providing
 * the ability to use a ChangesTracker to automatically trace
 * changes.
 * This class needs a ChangesTracker to be able to track changes
 * and it provides initNewItems, initUpdItems, initDelItems and initAllItems.
 */
public abstract class TrackableSyncSource extends BaseSyncSource {
    
    //--------------------------------------------------------------- Attributes
    protected ChangesTracker tracker;

    //------------------------------------------------------------- Constructors

    /**
     * TrackableSyncSource constructor: initialize source config
     */
    public TrackableSyncSource(SourceConfig config, ChangesTracker tracker) {

        super(config);
        // Set up the tracker
        this.tracker = tracker;
        tracker.setSyncSource(this);
    }

    public void beginSync(int syncMode) throws SyncException {
        // The tracker must be initialized before the source
        // as it may invoke the initXXXItems which depend on the tracker
        tracker.begin();
        super.beginSync(syncMode);
    }

    public void endSync() throws SyncException {
        super.endSync();
        tracker.end();
    }

    protected void initAllItems() throws SyncException {
        Log.trace("TrackableSyncSource.initAllItems");
        Vector tempItems = new Vector();
        Enumeration all = getAllItemsKeys();
        while (all.hasMoreElements()) {
            String key = (String)all.nextElement();
            SyncItem item = new SyncItem(key);
            tempItems.addElement(item);
        }
        allItems = new SyncItem[tempItems.size()];
        for(int i = 0;i<allItems.length;++i) {
            allItems[i] = (SyncItem)tempItems.elementAt(i);
        }
    }

    protected void initNewItems() throws SyncException {
        Log.trace("TrackableSyncSource.initNewItems");
        Enumeration newItemsEnum = tracker.getNewItems();
        int newItemsCount        = tracker.getNewItemsCount();
        newItems = initChangedItems(newItemsEnum, newItemsCount);
    }

    protected void initUpdItems() throws SyncException {
        Log.trace("TrackableSyncSource.initUpdItems");
        Enumeration updItemsEnum = tracker.getUpdatedItems();
        int updItemsCount        = tracker.getUpdatedItemsCount();
        updItems = initChangedItems(updItemsEnum, updItemsCount);
    }

    protected void initDelItems() throws SyncException {
        Log.trace("TrackableSyncSource.initDelItems");
        Enumeration delItemsEnum = tracker.getDeletedItems();
        int delItemsCount        = tracker.getDeletedItemsCount();
        delItems = initChangedItems(delItemsEnum, delItemsCount);
    }

    protected SyncItem[] initChangedItems(Enumeration itemsEnum, int itemsCount) {
        SyncItem res[] = new SyncItem[itemsCount];
        for(int i=0;i<itemsCount;++i) {
            if (!itemsEnum.hasMoreElements()) {
                throw new SyncException(SyncException.CLIENT_ERROR, "Items count mismatch");
            }
            String key = (String)itemsEnum.nextElement();
            SyncItem item = new SyncItem(key);
            res[i] = item;
        }
        return res;
    }

    public void setItemStatus(String key, int status) throws SyncException {
        super.setItemStatus(key, status);
        tracker.setItemStatus(key, status);
    }

    protected abstract Enumeration getAllItemsKeys();
}


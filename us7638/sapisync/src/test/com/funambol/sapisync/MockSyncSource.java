/*
 * Funambol is a mobile platform developed by Funambol, Inc.
 * Copyright (C) 2003-2007 Funambol, Inc.
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

package com.funambol.sapisync;

import java.util.Hashtable;

import com.funambol.sync.SourceConfig;
import com.funambol.sync.SyncItem;
import com.funambol.sync.SyncException;
import com.funambol.sync.SyncSource;
import com.funambol.sync.client.BaseSyncSource;

/**
 * Implements a Mock for the SyncSource interface
 */
public class MockSyncSource extends BaseSyncSource {
    
    private Hashtable statusTable = null;

    public int initialItemsCount = 100;

    public int initialNewItemsCount     = 5;
    public int initialUpdatedItemsCount = 5;
    public int initialDeletedItemsCount = 5;

    public int addedItemsCount   = 0;
    public int updatedItemsCount = 0;
    public int deletedItemsCount = 0;

    public MockSyncSource(SourceConfig config) {
        super(config);
    }

    public Hashtable getStatusTable() {
      return statusTable;
    }

    public void setInitialItemsCount(int count) {
        this.initialItemsCount = count;
    }

    public void setInitialNewItemsCount(int count) {
        this.initialNewItemsCount = count;
    }

    public void setInitialUpdatedItemsCount(int count) {
        this.initialUpdatedItemsCount = count;
    }

    public void setInitialDeletedItemsCount(int count) {
        this.initialDeletedItemsCount = count;
    }

    public int getAddedItemsCount() {
        return addedItemsCount;
    }

    public int getUpdatedItemsCount() {
        return updatedItemsCount;
    }

    public int getDeletedItemsCount() {
        return deletedItemsCount;
    }
    
    public int addItem(SyncItem item) {
        addedItemsCount++;
        return SyncSource.SUCCESS_STATUS;
    }
    
    public int updateItem(SyncItem item) {
        updatedItemsCount++;
        return SyncSource.SUCCESS_STATUS;
    }
    
    public int deleteItem(String key) {
        deletedItemsCount++;
        return SyncSource.SUCCESS_STATUS;
    }
    
    public void setItemStatus(String key, int status) throws SyncException {
        statusTable.put(key, new Integer(status));
    }

    public void beginSync(int syncMode, boolean resume) throws SyncException {
        super.beginSync(syncMode, resume);
        statusTable = new Hashtable();
    }
    
    protected void initAllItems() {
        allItems = new SyncItem[initialItemsCount];
        for(int i=0; i<initialItemsCount; i++) {
            allItems[i] = new SyncItem("Item"+i);
        }
    }

    protected void initNewItems() {
        newItems = new SyncItem[initialNewItemsCount];
        for(int i=0; i<initialNewItemsCount; i++) {
            String key = "Item"+(i+11);
            newItems[i] = new SyncItem(key, getType(),
                    SyncItem.STATE_NEW, null, null);
        }
    }

    protected void initUpdItems() {
        updItems = new SyncItem[initialUpdatedItemsCount];
        for(int i=0; i<initialUpdatedItemsCount; i++) {
            String key = "Item"+(i+4);
            updItems[i]= new SyncItem(key, getType(),
                    SyncItem.STATE_UPDATED, null, null);
        }
    }

    protected void initDelItems() {
        delItems = new SyncItem[initialDeletedItemsCount];
        for(int i=0; i<initialDeletedItemsCount; i++) {
            String key = "Item"+(i+8);
            delItems[i] = new SyncItem(key, getType(),
                    SyncItem.STATE_DELETED, null, null);
        }
    }

    protected SyncItem getItemContent(final SyncItem item) {
        SyncItem ret = new SyncItem(item);
        ret.setContent(("This is the content of item: " +
                item.getKey()).getBytes());
        return ret;
    }
}


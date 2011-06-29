/**
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

package com.funambol.syncml.spds;

import com.funambol.sync.SourceConfig;
import com.funambol.sync.SyncException;
import com.funambol.sync.SyncItem;
import com.funambol.sync.client.BaseSyncSource;

public class TestSyncSource extends BaseSyncSource {

    private boolean done = false;
    private int luid = 0;
    private int firstLuid = -1;
    private int lastLuid  = -1;

    public TestSyncSource(SourceConfig sc) {
        super(sc);
    }

    public int addItem(SyncItem item) throws SyncException {
        return 200;
    }

    public int updateItem(SyncItem item) throws SyncException {
        return 200;
    }

    public int deleteItem(String key) throws SyncException {
        return 200;
    }

    public SyncItem getNextItem() throws SyncException {

        if (firstLuid == -1) {
            firstLuid = luid;
        }
        lastLuid = luid;

        if (!done) {
            SyncItem res = new SyncItem("" + luid);
            res.setContent("Test item, make is of a signficant length".getBytes());
            luid++;
            return res;
        } else {
            return null;
        }
    }

    public void endSending() {
        done = true;
    }

    public void resetLuidCounters() {
        firstLuid = -1;
        lastLuid  = -1;
    }

    public int getFirstLuid() {
        return firstLuid;
    }

    public int getLastLuid() {
        return lastLuid;
    }

    protected void initAllItems() throws SyncException {
    }

    protected void initNewItems() throws SyncException {
    }

    protected void initUpdItems() throws SyncException {
    }

    protected void initDelItems() throws SyncException {
    }

    protected SyncItem getItemContent(final SyncItem item) throws SyncException {
        return null;
    }

    public void cancel() {
    }
}

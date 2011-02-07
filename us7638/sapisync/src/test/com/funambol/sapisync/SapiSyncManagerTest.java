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

package com.funambol.sapisync;

import com.funambol.sync.SourceConfig;
import com.funambol.sync.SyncConfig;
import com.funambol.util.ConsoleAppender;
import com.funambol.util.Log;

import java.util.Vector;

import junit.framework.*;

public class SapiSyncManagerTest extends TestCase {

    private SapiSyncManager syncManager = null;
    private MockSapiSyncHandler sapiSyncHandler = null;
    private MockSyncSource src = null;

    public SapiSyncManagerTest(String name) {
        super(name);
        Log.initLog(new ConsoleAppender(), Log.TRACE);
    }

    public void setUp() {
        
        syncManager = new SapiSyncManager(new SyncConfig());

        sapiSyncHandler = new MockSapiSyncHandler();
        syncManager.setSapiSyncHandler(sapiSyncHandler);
        
        src = new MockSyncSource(new SourceConfig(
                "test", "application/*", "test"));
    }

    public void tearDown() {
        syncManager = null;
    }

    public void testFullUpload() {

        SapiSyncAnchor anchor = new SapiSyncAnchor();
        anchor.setDownloadAnchor(0);
        anchor.setUploadAnchor(0);

        src.setSyncAnchor(anchor);

        syncManager.sync(src);

        assertEquals(sapiSyncHandler.getLoginCount(), 1);
        assertEquals(sapiSyncHandler.getLogoutCount(), 1);

        Vector items = sapiSyncHandler.getUploadedItems();
        assertEquals(items.size(), 300);   
    }

    public void testIncrementalUpload() {

        SapiSyncAnchor anchor = new SapiSyncAnchor();
        anchor.setDownloadAnchor(0);
        anchor.setUploadAnchor(100);

        src.setSyncAnchor(anchor);

        syncManager.sync(src);

        assertEquals(sapiSyncHandler.getLoginCount(), 1);
        assertEquals(sapiSyncHandler.getLogoutCount(), 1);

        Vector items = sapiSyncHandler.getUploadedItems();
        assertEquals(items.size(), 5);
    }
}

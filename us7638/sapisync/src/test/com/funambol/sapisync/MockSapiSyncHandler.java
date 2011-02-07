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

package com.funambol.sapisync;

import java.util.Vector;
import java.util.Date;

import com.funambol.sync.SyncException;
import com.funambol.sync.SyncItem;
import com.funambol.sync.SyncListener;

/**
 * Implements a Mock for the SapiSyncHandler interface
 */
public class MockSapiSyncHandler extends SapiSyncHandler {

    private static final String TEST_SERVER = "http://test.server.url";
    private static final String TEST_USERNAME = "user";
    private static final String TEST_PASSWORD = "pwd";

    private Vector itemsToDownload = new Vector();
    private Vector uploadedItems = new Vector();
    private int loginCount = 0;
    private int logoutCount = 0;

    public MockSapiSyncHandler() {
        super(TEST_SERVER, TEST_USERNAME, TEST_PASSWORD);
    }

    public void login() throws SyncException {
        loginCount++;
    }

    public void logout() throws SyncException {
        logoutCount++;
    }

    public void uploadItem(SyncItem item, SyncListener listener)
            throws SyncException {
        uploadedItems.addElement(item);
    }

    public Vector incrementalDownload(Date from, Date to, String dataName)
            throws SyncException {
        return itemsToDownload;
    }

    public void setItemsToDownload(Vector itemsToDownload) {
        this.itemsToDownload = itemsToDownload;
    }

    public Vector getUploadedItems() {
        return uploadedItems;
    }

    public int getLoginCount() {
        return loginCount;
    }

    public int getLogoutCount() {
        return logoutCount;
    }
}

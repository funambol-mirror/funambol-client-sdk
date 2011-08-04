/*
 * Funambol is a mobile platform developed by Funambol, Inc. 
 * Copyright (C) 2010 Funambol, Inc.
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

package com.funambol.client.test.util;

import com.funambol.client.engine.SyncReportMessage;
import com.funambol.client.engine.SyncTaskMessage;
import com.funambol.client.source.AppSyncSource;
import com.funambol.sync.SyncReport;
import com.funambol.util.bus.BusMessage;
import com.funambol.util.bus.BusMessageHandler;
import com.funambol.util.bus.BusService;
import java.util.Hashtable;

/**
 * Collects usefull methods to monitor the synchronization state.
 */
public class SyncMonitor {

    private SyncTaskHandler syncTaskHandler;
    private static SyncMonitor instance;

    private boolean syncing = false;
    
    private AppSyncSource lastAppSource = null;
    private Hashtable lastSyncReports = new Hashtable();
    
    private SyncMonitor() {
        syncTaskHandler = new SyncTaskHandler();
        BusService.registerMessageHandler(SyncTaskMessage.class, syncTaskHandler);
        BusService.registerMessageHandler(SyncReportMessage.class, syncTaskHandler);
    }

    public synchronized  static SyncMonitor getInstance() {
        if(instance == null) {
            instance = new SyncMonitor();
        }
        return instance;
    }

    public boolean isSyncing() {
        return syncing;
    }

    public SyncReport getLastSyncReport(AppSyncSource appSource) {
        return (SyncReport)lastSyncReports.get(appSource);
    }

    private class SyncTaskHandler implements BusMessageHandler {

        public void receiveMessage(BusMessage message) {
            if(message instanceof SyncTaskMessage) {
                SyncTaskMessage syncMessage = (SyncTaskMessage)message;
                if(syncMessage.getMessageCode() == SyncTaskMessage.MESSAGE_SYNC_STARTED) {
                    syncing = true;
                } else if(syncMessage.getMessageCode() == SyncTaskMessage.MESSAGE_SYNC_ENDED) {
                    syncing = false;
                } else if(syncMessage.getMessageCode() == SyncTaskMessage.MESSAGE_SOURCE_STARTED) {
                    lastAppSource = syncMessage.getAppSource();
                }
            } else if(message instanceof SyncReportMessage) {
                SyncReport report = ((SyncReportMessage)message).getReport();
                lastSyncReports.put(lastAppSource, report);
            }
        }
    }
}


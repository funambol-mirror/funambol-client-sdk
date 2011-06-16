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

package com.funambol.client.test.basic;

import java.util.Enumeration;
import java.util.Hashtable;

import com.funambol.client.configuration.Configuration;
import com.funambol.client.controller.Controller;
import com.funambol.client.controller.NotificationData;
import com.funambol.client.source.AppSyncSourceManager;
import com.funambol.client.source.AppSyncSource;
import com.funambol.client.test.ClientTestException;
import com.funambol.client.test.Robot;
import com.funambol.client.test.util.SyncMonitor;
import com.funambol.client.test.util.TestFileManager;
import com.funambol.sapisync.source.FileSyncSource;
import com.funambol.sync.SyncReport;
import com.funambol.sync.SyncSource;
import com.funambol.syncml.spds.SyncStatus;
import com.funambol.util.Log;
import com.funambol.util.StringUtil;


public abstract class BasicRobot extends Robot {
   
    private static final String TAG_LOG = "BasicRobot";

    protected TestFileManager fileManager;

    protected Hashtable vars = null;


    public BasicRobot(TestFileManager fileManager, Hashtable vars) {
        this.fileManager = fileManager;
        this.vars = vars;
    }

    public TestFileManager getTestFileManager() {
        return fileManager;
    }

    public void waitForSyncToComplete(int minStart, int max,
            SyncMonitor syncMonitor) throws Throwable {
        
        if (Log.isLoggable(Log.DEBUG)) {
            Log.debug(TAG_LOG, "waiting for sync to complete");
        }

        // We wait no more than minStart for sync client to start
        while(!syncMonitor.isSyncing()) {
            Thread.sleep(WAIT_DELAY);
            minStart -= WAIT_DELAY;
            if (minStart < 0) {
                throw new ClientTestException("Sync did not start within time limit");
            }
        }

        boolean done = false;

        do {
            // Now wait until the busy is in progress for a max amount of time
            while(syncMonitor.isSyncing()) {
                Thread.sleep(WAIT_DELAY);
                max -= WAIT_DELAY;
                if (max < 0) {
                    throw new ClientTestException("Sync did not complete before timeout");
                }
            }
            // Wait a couple of seconds. If the current sync is a sync all then
            // a new sync will fire right away
            Thread.sleep(3000);
            done = !syncMonitor.isSyncing();
        } while(!done);

        // Wait one extra second to be really sure everything is terminated and
        // the client is ready for another sync
        Thread.sleep(1000);
    }

    public void interruptSyncAfterPhase(String phase, int num, String reason, SyncMonitor syncMonitor) throws Throwable {
        if (Log.isLoggable(Log.DEBUG)) {
            Log.debug(TAG_LOG, "Preparing to interrupt after phase " + phase + "," + num);
        }
        syncMonitor.interruptSyncAfterPhase(phase, num, reason);
    }

    public void checkLastSyncRequestedSyncMode(String source, int mode,
            SyncMonitor syncMonitor) throws Throwable {
        if (Log.isLoggable(Log.DEBUG)) {
            Log.debug(TAG_LOG, "check last sync requested sync mode");
        }

        SyncReport sr = (SyncReport)syncMonitor.getSyncStatus(source);
        assertTrue(sr != null, "source has no report associated");
        assertTrue(sr.getRequestedSyncMode() == mode, "Requested sync mode mismatch");
    }

    public void checkLastSyncAlertedSyncMode(String source, int mode,
            SyncMonitor syncMonitor) throws Throwable {
        if (Log.isLoggable(Log.DEBUG)) {
            Log.debug(TAG_LOG, "check last sync alerted sync mode");
        }

        SyncReport sr = (SyncReport)syncMonitor.getSyncStatus(source);
        assertTrue(sr != null, "source has no report associated");

        assertTrue(sr instanceof SyncStatus, "Invalid sync report format");
        assertTrue(((SyncStatus)sr).getAlertedSyncMode() == mode, "Alerted sync mode mismatch");
    }

    public void checkLastSyncRemoteUri(String source, String uri,
            SyncMonitor syncMonitor) throws Throwable {
        if (Log.isLoggable(Log.DEBUG)) {
            Log.debug(TAG_LOG, "check last sync remote URI");
        }

        SyncReport sr = (SyncReport)syncMonitor.getSyncStatus(source);
        assertTrue(sr != null, "source has no report associated");
        assertTrue(sr.getRemoteUri(), uri, "Requested remote URI mismatch");
    }

    public void checkLastSyncExchangedData(String source,
            int sentAdd, int sentReplace, int sentDelete,
            int receivedAdd, int receivedReplace, int receivedDelete,
            SyncMonitor syncMonitor) throws Throwable
    {
        if (Log.isLoggable(Log.DEBUG)) {
            Log.debug(TAG_LOG, "check last sync exchanged data");
        }

        SyncReport sr = (SyncReport)syncMonitor.getSyncStatus(source);
        assertTrue(sr != null, "source has no report associated");

        assertTrue(receivedAdd, sr.getReceivedAddNumber(),
                "Received add mismatch");
        assertTrue(receivedReplace, sr.getReceivedReplaceNumber(),
                "Received replace mismatch");
        assertTrue(receivedDelete, sr.getReceivedDeleteNumber(),
                "Received delete mismatch");
        assertTrue(sentAdd, sr.getSentAddNumber(),
                "Sent add mismatch");
        assertTrue(sentReplace, sr.getSentReplaceNumber(),
                "Sent replace mismatch");
        assertTrue(sentDelete, sr.getSentDeleteNumber(),
                "Sent delete mismatch");
    }

    public void checkLastSyncErrors(String source, int sendingErrors,
            int receivingErrors, SyncMonitor syncMonitor) throws Throwable
    {
        if (Log.isLoggable(Log.DEBUG)) {
            Log.debug(TAG_LOG, "check last sync exchanged data");
        }

        SyncReport sr = (SyncReport)syncMonitor.getSyncStatus(source);
        assertTrue(sr != null, "source has no report associated");

        assertTrue(sendingErrors, sr.getNumberOfSentItemsWithError(),
                "Sending errors mismatch");
        assertTrue(receivingErrors, sr.getNumberOfReceivedItemsWithError(),
                "Receiving errors mismatch");
    }

    public void checkLastSyncStatusCode(String source,
            int code,
            SyncMonitor syncMonitor) throws Throwable
    {
        if (Log.isLoggable(Log.DEBUG)) {
            Log.debug(TAG_LOG, "check last sync status code");
        }
        
        SyncReport sr = (SyncReport)syncMonitor.getSyncStatus(source);
        assertTrue(sr != null, "source has no report associated");
        assertTrue(code, sr.getStatusCode(), "Status code mismatch");

    }

   /** 
    * Checks the last notification's data. If an integer parameter is -1,
    * it's not checked. If a string parameter is null, it's not checked.
    * 
    * @param id -1 means don't check
    * @param severity -1 means don't check
    * @param ticker null means don't check
    * @param title null means don't check
    * @param message null means don't check
    */
    public void checkLastNotification(int id, int severity,
            String ticker, String title, String message) throws Throwable {
        if (Log.isLoggable(Log.DEBUG)) {
            Log.debug(TAG_LOG, "check last notification");
        }
        
        NotificationData lastNotification = 
            getController().getNotificationController().getLastNotification();
        
        assertTrue(lastNotification != null, 
                "No notification was shown");
        if (id != -1) {
            assertTrue(id, lastNotification.getId(), 
                    "Notification ID mismatch");
        }
        if (severity != -1) {
            assertTrue(severity, lastNotification.getSeverity(), 
                    "Notification severity mismatch");
        }
        if (ticker != null) {
            assertTrue(ticker, lastNotification.getTicker(), 
                    "Notification ticker mismatch");
        }
        if (title != null) {
            assertTrue(title, lastNotification.getTitle(), 
                    "Notification title mismatch");
        }
        if (message != null) {
            assertTrue(message, lastNotification.getMessage(), 
                    "Notification message mismatch");
        }
    }

    public void checkLastSyncResumedData(String source, int sentResumed,
            int receivedResumed, SyncMonitor syncMonitor) throws Throwable
    {
        if (Log.isLoggable(Log.DEBUG)) {
            Log.debug(TAG_LOG, "check last sync resumed data");
        }

        SyncReport sr = (SyncReport)syncMonitor.getSyncStatus(source);
        assertTrue(sr != null, "source has no report associated");

        assertTrue(sentResumed, sr.getSentResumedNumber(),
                "Sent resumed mismatch");
        assertTrue(receivedResumed, sr.getReceivedResumedNumber(),
                "Received resumed mismatch");
    }

    public void resetSourceAnchor(String sourceName) throws Throwable {
        if (Log.isLoggable(Log.DEBUG)) {
            Log.debug(TAG_LOG, "resetting source anchor");
        }

        SyncSource source = getSyncSource(sourceName);
        source.getSyncAnchor().reset();
        saveSourceConfig(sourceName);

    }

    public void resetFirstRunTimestamp() throws Throwable {
        Configuration configuration = getConfiguration();
        long newTS = System.currentTimeMillis();
        configuration.setFirstRunTimestamp(newTS);
        configuration.save();

        // Update the ts in the BasicMediaSS
        AppSyncSourceManager appSyncSourceManager = getAppSyncSourceManager();
        Enumeration sources = appSyncSourceManager.getWorkingSources();
        while(sources.hasMoreElements()) {
            AppSyncSource appSource = (AppSyncSource)sources.nextElement();
            SyncSource source = appSource.getSyncSource();
            if (source instanceof FileSyncSource) {
                ((FileSyncSource)source).setOldestItemTimestamp(newTS);
            }
        }
    }
    

    public void syncAll() {
       
    }

    public void saveSourceConfig(String sourceName) throws Exception {
        getAppSyncSource(sourceName).getConfig().save();
    }

    public AppSyncSource getAppSyncSource(String sourceName) throws Exception {
        AppSyncSource source = null;
        if(StringUtil.equalsIgnoreCase(BasicUserCommands.SOURCE_NAME_CONTACTS,sourceName)) {
            source = getAppSyncSourceManager().getSource(AppSyncSourceManager.CONTACTS_ID);
        } else if(StringUtil.equalsIgnoreCase(BasicUserCommands.SOURCE_NAME_CALENDAR,sourceName)) {
            source = getAppSyncSourceManager().getSource(AppSyncSourceManager.EVENTS_ID);
        } else if(StringUtil.equalsIgnoreCase(BasicUserCommands.SOURCE_NAME_PICTURES,sourceName)) {
            source = getAppSyncSourceManager().getSource(AppSyncSourceManager.PICTURES_ID);
        } else if(StringUtil.equalsIgnoreCase(BasicUserCommands.SOURCE_NAME_VIDEOS,sourceName)) {
            source = getAppSyncSourceManager().getSource(AppSyncSourceManager.VIDEOS_ID);
        } else if(StringUtil.equalsIgnoreCase(BasicUserCommands.SOURCE_NAME_FILES,sourceName)) {
            source = getAppSyncSourceManager().getSource(AppSyncSourceManager.FILES_ID);
        } else {
            Log.error(TAG_LOG, "Unknown source: " + sourceName);
            throw new IllegalArgumentException("Unknown source: " + sourceName);
        }
        return source;
    }

    public SyncSource getSyncSource(String sourceName) throws Exception {
        return getAppSyncSource(sourceName).getSyncSource();
    }

    public void setVariable(String variable, String value) throws Throwable {
        assertTrue(variable != null, "Variable cannot be null");
        assertTrue(value != null, "Value cannot be null");
        vars.put(variable, value);
    }

    protected abstract void startMainApp() throws Throwable;
    protected abstract void closeMainApp() throws Throwable;

    public abstract void waitForAuthToComplete(int minStart, int max, SyncMonitor syncMonitor) throws Throwable;

    public abstract void keyPress(String keyName, int count) throws Throwable;
    public abstract void writeString(String text) throws Throwable;

    protected abstract Configuration getConfiguration();
    protected abstract Controller getController();
    protected abstract AppSyncSourceManager getAppSyncSourceManager();

    
}

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

package com.funambol.client.controller;

import java.util.Vector;
import java.util.Hashtable;

import com.funambol.client.source.AppSyncSource;
import com.funambol.client.source.AppSyncSourceManager;
import com.funambol.client.ui.HomeScreen;
import com.funambol.client.ui.DisplayManager;
import com.funambol.client.ui.view.SourceThumbnailsView;
import com.funambol.client.ui.view.ThumbnailView;
import com.funambol.sync.SyncListener;
import com.funambol.util.Log;
import com.funambol.util.StringUtil;
import com.funambol.platform.NetworkStatus;

/**
 * This class represents the controller for the home screen. Since the
 * HomeScreen is a screen where synchronizations can be performed, the
 * class extends the SynchronizationController. On top of this the class adds
 * the ability of handling the home screen.
 */
public class HomeScreenController extends SynchronizationController {

    private static final String TAG_LOG = "HomeScreenController";

    protected HomeScreen         mHomeScreen;

    private Hashtable            mPushRequestQueue = new Hashtable();

    /**
     *  This flag is to switch off the storage limit warning after
     *  it is displayed once. The warning must be displayed also more
     *  than once if an individual-source sync is fired, but not for
     *  multiple-source sync, scheduled sync and push sync.
     *  See US7498.
     */
    protected boolean mDontDisplayStorageLimitWarning = false;

    /**
     *  This flag is to switch off the server quota warning after
     *  it is displayed once. The warning must be displayed also more
     *  than once if an individual-source sync is fired, but not for
     *  multiple-source sync, scheduled sync and push sync.
     *  See US7499.
     */
    protected boolean mDontDisplayServerQuotaWarning = false;

    private boolean mHomeScreenInForeground = false;

    public HomeScreenController(Controller controller, HomeScreen homeScreen,
            NetworkStatus networkStatus) {
        super(controller, homeScreen, networkStatus);
        this.mHomeScreen = homeScreen;
    }

    public HomeScreen getHomeScreen() {
        return mHomeScreen;
    }

    public void setHomeScreen(HomeScreen homeScreen) {
        this.mHomeScreen = homeScreen;
        super.setScreen(homeScreen);
    }

    public void initializeHomeScreen() {
        int[] order = mCustomization.getSourcesOrder();
        for(int i=0; i<order.length; i++) {
            AppSyncSource source = mAppSyncSourceManager.getSource(order[i]);
            if(source.isVisible()) {
                SourceThumbnailsView sourceView = mHomeScreen.createSourceThumbnailsView(source);

                int count = 25;
                if(source.getId() == AppSyncSourceManager.CONTACTS_ID) {
                    count = 3;
                } else if(source.getId() == AppSyncSourceManager.VIDEOS_ID) {
                    count = 0;
                } else if(source.getId() == AppSyncSourceManager.FILES_ID) {
                    count = 5;
                }
                for(int t=0; t<count; t++) {
                    ThumbnailView thumbView = mHomeScreen.createThumbnailView();
                    
                    // TODO: FIXME set correct thumbnail
                    thumbView.setThumbnail(null);
                    
                    sourceView.addThumbnail(thumbView);
                }
                mHomeScreen.addSourceThumbnailsView(sourceView);
            }
        }
    }

    public void updateAvailableSources() {
        // TODO: FIXME
    }

    public void syncStarted(Vector sources) {
        if (Log.isLoggable(Log.TRACE)) {
            Log.trace(TAG_LOG, "syncStarted");
        }
        super.syncStarted(sources);
        lockHomeScreen(sources);
    }

    public void syncEnded() {
        if (Log.isLoggable(Log.TRACE)) {
            Log.trace(TAG_LOG, "syncEnded");
        }
        super.syncEnded();
        unlockHomeScreen();
    }
    
    protected void displayStorageLimitWarning(Vector localStorageFullSources) {
        logSyncSourceErrors(localStorageFullSources);
        if (isInForeground()) {
            if (!mDontDisplayStorageLimitWarning) {
                String message = mLocalization.getLanguage("message_storage_limit");
                mController.getDialogController().showMessageAndWaitUserConfirmation(message);
                mDontDisplayStorageLimitWarning = true; // Once is enough
            }
        } else {
            super.displayStorageLimitWarning(localStorageFullSources);
        }
    }
    
    protected void displayServerQuotaWarning(Vector serverQuotaFullSources) {
        logSyncSourceErrors(serverQuotaFullSources);

        // if we had at least one device full error, we must choose how show
        // these errors to the user, according to US7498 and US7499
        if (isInForeground()) {
            if (!mDontDisplayServerQuotaWarning) {
                StringBuffer sourceNames = new StringBuffer(""); 
                for(int i=0; i<serverQuotaFullSources.size(); i++) {
                    AppSyncSource appSource = (AppSyncSource)serverQuotaFullSources.elementAt(i);
                    if (sourceNames.length() > 0) {
                        sourceNames.append(",");
                    }
                    sourceNames.append(appSource.getName().toLowerCase());
                }
                String msg = mLocalization.getLanguage("dialog_server_full");
                msg = StringUtil.replaceAll(msg, "__source__", sourceNames.toString());
                mController.getDialogController().showMessageAndWaitUserConfirmation(msg);
            }
        } else {
            super.displayServerQuotaWarning(serverQuotaFullSources);
        }
    }

    /**
     * This method enques a sync request coming from a push notification. This
     * method can be used when there is a sync running and a new request comes
     * in. The request is enqueued and server as soon as the current sync
     * terminates.
     *
     * @param sources the sources to be enqueued
     */
    public void enquePushSyncRequest(Vector sources) {
        synchronized(mPushRequestQueue) {
            for(int i=0;i<sources.size();++i) {
                AppSyncSource source = (AppSyncSource)sources.elementAt(i);
                mPushRequestQueue.put(source, source);
            }
        }
    }

    /**
     * This method enques a sync request coming from a push notification. This
     * method can be used when there is a sync running and a new request comes
     * in. The request is enqueued and server as soon as the current sync
     * terminates.
     */
    public void enquePushSyncRequest() {
        synchronized(mPushRequestQueue) {
            // TODO FIXME
            /*for(int i=0;i<items.size();++i) {
                AppSyncSource appSource = (AppSyncSource)items.elementAt(i);
                if (appSource.getConfig().getEnabled() && appSource.isWorking()) {
                    mPushRequestQueue.put(appSource, appSource);
                }
            }*/
        }
    }

    protected void lockHomeScreen(Vector sources) {
        if (mHomeScreen == null) {
            return;
        }
        mHomeScreen.lock();
    }

    protected void unlockHomeScreen() {
        if (mHomeScreen == null) {
            return;
        }
        mHomeScreen.unlock();
    }

    public void syncAllSources(String syncType, int retryCount) {
        // TODO: FIXME
        syncAllSources(syncType);
    }
    
    public void syncAllSources(String syncType) {
        if (Log.isLoggable(Log.INFO)) {
            Log.info(TAG_LOG, "syncAllSources");
        }

        Vector sources = new Vector();        

        // TODO: FIXME
        synchronize(syncType, sources);
    }

    protected void syncSource(String syncType, AppSyncSource appSource) {
        // TODO FIXME
    }
    
    /**
     * Triggers a synchronization for the given syncSources. The caller can
     * specify its type (manual, scheduled, push) to change the error handling
     * behavior
     *
     * @param syncType the caller type (SYNC_TYPE_MANUAL, SYNC_TYPE_SCHEDULED)
     * @param syncSources is a vector of AppSyncSource to be synchronized
     *
     */
    public synchronized void synchronize(String syncType, Vector syncSources) {
        
        // For manual sync, always show alert message for storage/server
        // quota limit. For other sync modes, doesn't display message if
        // the previous sync ended with the same error.
        if (MANUAL.equals(syncType)) {
            mDontDisplayStorageLimitWarning = false;
            mDontDisplayServerQuotaWarning = false;
        } else {
            for(int i = 0 ; i < syncSources.size(); ++i) {
                AppSyncSource appSource = (AppSyncSource)syncSources.elementAt(i);
                    
                switch (appSource.getConfig().getLastSyncStatus()) {
                case SyncListener.LOCAL_CLIENT_FULL_ERROR:
                    // If for at least one source the storage limit warning has
                    // already been shown, no warning should be displayed again
                    mDontDisplayStorageLimitWarning = true;
                    break;
                case SyncListener.SERVER_FULL_ERROR:
                    // If for at least one source the server full quota warning has
                    // already been shown, no warning should be displayed again
                    mDontDisplayServerQuotaWarning = true;
                    break;
                }
            }
        }
        super.synchronize(syncType, syncSources);
    }

    public void updateMenuSelected() {
        mController.promptUpdate();
    }

    public void showConfigurationScreen() {
        Controller globalController = getController();
        // If a sync is running, we wait for its termination before opening the
        // settings screen
        if (isSynchronizing()) {
            showSyncInProgressMessage();
        } else {
            globalController.showScreen(mHomeScreen, Controller.CONFIGURATION_SCREEN_ID);
        }
    }

    public void showAboutScreen() {
        Controller globalController = getController();
        globalController.showScreen(mHomeScreen, Controller.ABOUT_SCREEN_ID);
    }

    public void showAccountScreen() {
        Controller globalController = getController();
        globalController.showScreen(mHomeScreen, Controller.ACCOUNT_SCREEN_ID);
    }

    /**
     * Returns true when the associated screen is in foreground (visible
     * to the user and with focus)
     */
    public boolean isInForeground() {
        if (mHomeScreen == null) {
            return false;
        }
        return mHomeScreenInForeground ;
    }
    
    /**
     * Sets foreground status of the screen
     */
    public void setForegroundStatus(boolean newValue) {
        mHomeScreenInForeground = newValue;
    }

    protected void showSyncInProgressMessage() {
        // If the home screen is not displayed, we cannot show any warning and
        // just ignore this event
        Controller globalController = getController();
        if (mHomeScreen != null) {
            DisplayManager dm = globalController.getDisplayManager();
            String msg = mLocalization.getLanguage("message_sync_running_wait");
            dm.showMessage(mHomeScreen, msg);
        }
    }

    /**
     * Logs sync sources where server full quota or storage limit error happened 
     * @param storageLimitOrserverQuotaFullSources
     */
    protected void logSyncSourceErrors(Vector storageLimitOrserverQuotaFullSources) {
        for(int i=0; i<storageLimitOrserverQuotaFullSources.size(); i++) {
            AppSyncSource appSource = (AppSyncSource)storageLimitOrserverQuotaFullSources.elementAt(i);
            switch (appSource.getConfig().getLastSyncStatus()) {
            case SyncListener.LOCAL_CLIENT_FULL_ERROR:
                Log.error(TAG_LOG, "Storage limit reached for source " + appSource.getName());
                break;
            case SyncListener.SERVER_FULL_ERROR:
                Log.error(TAG_LOG, "Server quota full for source " + appSource.getName());
                break;
            }
        }
    }
    
}

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

package com.funambol.client.controller;

import java.util.Vector;
import java.util.Enumeration;

import com.funambol.client.configuration.Configuration;
import com.funambol.client.localization.Localization;
import com.funambol.client.customization.Customization;
import com.funambol.client.controller.Controller;
import com.funambol.client.engine.SyncEngine;
import com.funambol.client.engine.SyncEngineListener;
import com.funambol.client.engine.AppSyncRequest;
import com.funambol.client.source.AppSyncSource;
import com.funambol.client.source.AppSyncSourceManager;
import com.funambol.client.source.AppSyncSourceConfig;
import com.funambol.client.push.SyncScheduler;
import com.funambol.client.ui.Screen;
import com.funambol.platform.NetworkStatus;
import com.funambol.sync.SyncListener;
import com.funambol.sync.SyncException;
import com.funambol.sapisync.sapi.SapiHandler;
import com.funambol.org.json.me.JSONObject;
import com.funambol.org.json.me.JSONArray;
import com.funambol.util.StringUtil;
import com.funambol.util.Log;

/**
 * This interface includes all basic functions of a SynchronizationController
 * implementation that are currently shared between Android and BlackBerry
 * versions of SynchronizationController.
 */
public abstract class BasicSynchronizationController implements SyncEngineListener {

    private static final String TAG_LOG = "BasicSynchronizationController";

    public static final String MANUAL    = "manual";
    public static final String SCHEDULED = "scheduled";
    public static final String PUSH      = "push";

    protected Configuration configuration;
    protected Screen        screen;
    protected AppSyncSourceManager appSyncSourceManager;
    protected Controller    controller;
    protected Customization customization;
    protected Localization  localization;
    protected SyncEngine    engine;
    protected RequestHandler reqHandler;
    protected final AppSyncRequest appSyncRequestArr[] = new AppSyncRequest[1];
    protected SyncScheduler  syncScheduler;

    protected int   RETRY_POLL_TIME = 1;

    protected NetworkStatus networkStatus;
    
    private Vector localStorageFullSources = new Vector();
    private Vector serverQuotaFullSources = new Vector();

    public BasicSynchronizationController(Controller controller, Screen screen, NetworkStatus networkStatus) {
        this.controller = controller;
        this.screen     = screen;
        this.networkStatus = networkStatus;

        configuration = controller.getConfiguration();
        appSyncSourceManager = controller.getAppSyncSourceManager();
        localization = controller.getLocalization();
        customization = controller.getCustomization();

        initSyncScheduler();
    }

    public BasicSynchronizationController(Controller controller, Customization customization,
                                          Configuration configuration, Localization localization,
                                          AppSyncSourceManager appSyncSourceManager, Screen screen,
                                          NetworkStatus networkStatus) {
        this.controller    = controller;
        this.customization = customization;
        this.configuration = configuration;
        this.localization  = localization;
        this.appSyncSourceManager = appSyncSourceManager;
        this.screen        = screen;
        this.networkStatus = networkStatus;

        initSyncScheduler();
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
        synchronize(syncType, syncSources, 0);
    }

    /**
     * Schedules a synchronization for the given syncSources. The sync is
     * scheduled in "delay" milliseconds from now. The caller can
     * specify its type (manual, scheduled, push) to change the error handling
     * behavior
     *
     * @param syncType the caller type (SYNC_TYPE_MANUAL, SYNC_TYPE_SCHEDULED)
     * @param syncSources is a vector of AppSyncSource to be synced
     * @param delay the interval at which the sync shall be performed (relative
     *              to now)
     *
     */
    public synchronized void synchronize(String syncType, Vector syncSources, int delay) {
        synchronize(syncType, syncSources, delay, false);
    }


    /**
     * Displays warnings in the proper form if the outcome of the latest sync requires so.
     * This method must be called when all synchronization operations are finished and the
     * user can be warned about problems that trigger a notification or a pop-up message
     * like those connected with storage limits (locally or in the cloud). 
     */
    protected void displayEndOfSyncWarnings() {

        if (localStorageFullSources != null && localStorageFullSources.size() > 0) {
            Log.debug(TAG_LOG, "Notifying storage limit warning");
            displayStorageLimitWarning(localStorageFullSources);
            localStorageFullSources.removeAllElements();
        }
        if (serverQuotaFullSources != null && serverQuotaFullSources.size() > 0) {
            Log.debug(TAG_LOG, "Notifying server quota warning");
            displayServerQuotaWarning(serverQuotaFullSources);
            serverQuotaFullSources.removeAllElements();
        }

    }

    protected void checkSourcesForStorageOrQuotaFullErrors(Vector sources) {
        for (int i = 0; i < sources.size(); i++) {
            AppSyncSource appSource = (AppSyncSource) sources.elementAt(i);

            switch (appSource.getConfig().getLastSyncStatus()) {
                case SyncListener.LOCAL_CLIENT_FULL_ERROR:
                    // If one of the sources has risked to break the storage limit,
                    // a warning message can have to be displayed
                    localStorageFullSources.addElement(appSource);
                break;
                case SyncListener.SERVER_FULL_ERROR:
                    serverQuotaFullSources.addElement(appSource);
                break;
            }
        }
    }

    /**
     * Display a background notification when max storage limit on local device
     * is reached. Children can override this method and implement
     * a foreground behavior
     * @param localStorageFullSources
     */
    protected void displayStorageLimitWarning(Vector localStorageFullSources) {
        getBasicController().getNotificationController().showNotificationClientFull();
        localStorageFullSources.removeAllElements();
    }

    /**
     * Display a background notification when server quota is reached. Children
     * can override this method and implement a foreground behavior
     * @param serverQuotaFullSources
     */
    protected void displayServerQuotaWarning(Vector serverQuotaFullSources) {
        getBasicController().getNotificationController().showNotificationServerFull();
        serverQuotaFullSources.removeAllElements();
    }

    protected SyncEngine createSyncEngine() {
        return new SyncEngine(customization, configuration, appSyncSourceManager, null);
    }

    
    /**
     * Applies the Bandwidth Saver by filtering out some sources or by populating
     * the Vector of sources that need to be synchronized only if the user accepts
     * to do so.
     * The synchronizations for sources that are filtered out are immediately set 
     * as pending and terminated.
     * This method has to be called before the synchronizations actually start.
     * 
     * @param syncSources all sources to be synchronized
     * @param sourcesWithQuestion an empty Vector
     * @param syncType the synchronization type
     * @return a sub-vector of sync sources containing only those sources that have
     *         passed the check
     */
    protected Vector applyBandwidthSaver(Vector syncSources, Vector sourcesWithQuestion, String syncType) {

        // This class cannot guarantee that these two members are not null,
        // therefore we check for their validity and do not filter if any of
        // these two is undefined
        if (configuration == null || networkStatus == null) {
            return syncSources;
        }
        
        if (configuration.getBandwidthSaverActivated() && !networkStatus.isWiFiConnected()) {

            if (Log.isLoggable(Log.TRACE)) {
                Log.trace(TAG_LOG, "Bandwidth saver is enabled, wifi not connected and sync type " + syncType);
            }
            
            // If the syncType is automatic (i.e. not manual) and WiFi is not available, 
            // we shall skip all the sources which are to be synchronized only in WiFi 
            if (!MANUAL.equals(syncType)) {
                Vector prefilteredSources = new Vector();
                for (int i = 0; i < syncSources.size(); ++i) {
                    AppSyncSource appSource = (AppSyncSource)syncSources.elementAt(i);
                    // We need to check if the source requires to be sync'ed only in WiFi
                    // In v9 we excluded also sync sources with online quota full, but this
                    // behavior was modified in v10 
                    if (appSource.getBandwidthSaverUse()) {
                        // Skip this source because of the Bandwidth Saver.
                        // Remember that we have a pending sync now
                        AppSyncSourceConfig sourceConfig = appSource.getConfig();
                        sourceConfig.setPendingSync(syncType, sourceConfig.getSyncMode());
                        configuration.save();
                        // The sync for this source is terminated
                        if (Log.isLoggable(Log.INFO)) {
                            Log.info(TAG_LOG, "Ignoring sync for source: " + appSource.getName());
                        }
                        sourceEnded(appSource);
                    } else {
                        // It's OK
                        prefilteredSources.addElement(appSource);
                    }
                }
                syncSources = prefilteredSources;
                
            } else {
                // Now check if any source to be synchronized requires user confirmation
                // because of the bandwidth saver
                for(int y = 0; y < syncSources.size(); ++y) {
                    AppSyncSource appSource = (AppSyncSource)syncSources.elementAt(y);
                    if(appSource.getBandwidthSaverUse()) {
                        if (Log.isLoggable(Log.TRACE)) {
                            Log.trace(TAG_LOG, "Found a source which requires bandwidth saver question");
                        } 
                        sourcesWithQuestion.addElement(appSource);
                    }
                }
            }
        }
        return syncSources;
    }

    public void sourceFailed(AppSyncSource appSource, SyncException se) {

        int code = se.getCode();

        if (code == SyncException.PAYMENT_REQUIRED) {
            // In order to sync the user shall accept a payment
            // TODO FIXME: we need the list of sources to be initially
            // synchronized
            Vector nextSources = new Vector();
            nextSources.addElement(appSource);
            askForPayment(nextSources);
        }
    }

    protected void initSyncScheduler() {
        engine = createSyncEngine();
        syncScheduler = new SyncScheduler(engine);
        // The request handler is a daemon serving external requests
        reqHandler = new RequestHandler();
        reqHandler.start();
    }


    /**
     * Returns true iff a synchronization is in progress
     */
    public boolean isSynchronizing() {
        return engine.isSynchronizing();
    }

    /**
     * Returns the sync source currently being synchronized. If a sync is not
     * in progress, then null is returned. Please note that this method is not
     * completely equivalent to isSynchronizing. At the beginning of a sync,
     * isSynchronizing returns true, but getCurrentSource may return null until
     * the source is prepared for the synchronization.
     */
    public AppSyncSource getCurrentSource() {
        return engine.getCurrentSource();
    }

    /**
     * @return the current <code>SyncEngine</code> instance
     */
    public SyncEngine getSyncEngine() {
        return engine;
    }



    protected void askForPayment(Vector nextSources) {

        // On BB dialogs are blocking, here we really need to create a thread so
        // that the current sync terminates and a new one is restarted afterward
        // (otherwise events get messed up)
        // Creating the thread on all platforms is a safe solution.
        PaymentThread pt = new PaymentThread(nextSources);
        pt.start();
    }

    protected class PaymentYesAction implements Runnable {
        private Vector syncSources;
        private String syncType;

        public PaymentYesAction(Vector syncSources, String syncType) {
            this.syncSources = syncSources;
            this.syncType = syncType;
        }

        public void run() {
            String sapiUrl = StringUtil.extractAddressFromUrl(configuration.getSyncUrl());
            SapiHandler sapiHandler = new SapiHandler(sapiUrl, configuration.getUsername(),
                                                      configuration.getPassword());
            if (Log.isLoggable(Log.INFO)) {
                Log.info(TAG_LOG, "User accepted payment request, continue sync");
            }
 
            try {
                JSONObject req = new JSONObject();
                JSONArray  sources = new JSONArray();
                Enumeration workingSources = appSyncSourceManager.getWorkingSources();
                while(workingSources.hasMoreElements()) {
                    AppSyncSource appSource = (AppSyncSource)workingSources.nextElement();
                    JSONObject restoreSource = new JSONObject();
                    restoreSource.put("service","restore");
                    restoreSource.put("resource",appSource.getSyncSource().getConfig().getRemoteUri());
                    sources.put(restoreSource);
                }
                req.put("data", sources);

                sapiHandler.query("system/payment","buy",null,null,req);

                // Restart the sync for the given sources
                continueSyncAfterNetworkUsage(syncType, syncSources, 0, false);
            } catch (Exception e) {
                Log.error(TAG_LOG, "Cannot perform payment", e);
                // TODO FIXME: show an error to the user
            }
        }
    }

    protected class PaymentNoAction implements Runnable {
        public void run() {
            if (Log.isLoggable(Log.INFO)) {
                Log.info(TAG_LOG, "User did not accept payment request, stop sync");
            }
        }
    }

    protected class PaymentThread extends Thread {
        private Vector sources;

        public PaymentThread(Vector sources) {
            this.sources = sources;
        }

        public void run() {
            DialogController dc = controller.getDialogController();
            String syncType = com.funambol.client.controller.SynchronizationController.MANUAL;
            PaymentYesAction yesAction = new PaymentYesAction(sources, syncType);
            PaymentNoAction  noAction  = new PaymentNoAction();
            // TODO FIXME: use a localized message
            dc.askYesNoQuestion(screen, "A payment is required", false, yesAction, noAction);
        }
    }

    private class RequestHandler extends Thread {

        private boolean stop = false;

        public RequestHandler() {
        }

        public void run() {
            if (Log.isLoggable(Log.INFO)) {
                Log.info(TAG_LOG, "Starting request handler");
            }
            while (!stop) {
                try {
                    synchronized (appSyncRequestArr) {
                        appSyncRequestArr.wait();
                        syncScheduler.addRequest(appSyncRequestArr[0]);
                    }
                } catch (Exception e) {
                    // All handled exceptions are trapped below, this is just a
                    // safety net for runtime exception because we don't want
                    // this thread to die.
                    Log.error(TAG_LOG, "Exception while performing a programmed sync " + e.toString());
                }
            }
        }
    }


    protected abstract void continueSyncAfterNetworkUsage(String syncType, Vector syncSources,
                                                          int delay, boolean fromOutside);

    protected abstract BasicController getBasicController();

    public abstract void synchronize(String syncType, Vector sources, int dealy, boolean fromOutside) throws SyncException;
}

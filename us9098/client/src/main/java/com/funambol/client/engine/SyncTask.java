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

package com.funambol.client.engine;

import java.util.Vector;
import java.util.Hashtable;

import com.funambol.client.source.AppSyncSource;
import com.funambol.client.source.AppSyncSourceManager;
import com.funambol.client.configuration.Configuration;
import com.funambol.client.customization.Customization;
import com.funambol.client.controller.ProfileUpdateHelper;
import com.funambol.concurrent.ResumableTask;
import com.funambol.concurrent.Task;
import com.funambol.syncml.spds.CompressedSyncException;
import com.funambol.syncml.spds.DeviceConfig;
import com.funambol.syncml.spds.SyncManager;
import com.funambol.syncml.spds.SyncMLAnchor;
import com.funambol.sync.SyncSource;
import com.funambol.sync.SyncException;
import com.funambol.sync.SyncConfig;
import com.funambol.sync.SyncManagerI;
import com.funambol.syncml.protocol.DevInf;
import com.funambol.sapisync.SapiSyncManager;
import com.funambol.sapisync.SapiException;
import com.funambol.platform.NetworkStatus;
import com.funambol.util.TransportAgent;
import com.funambol.util.StringUtil;
import com.funambol.util.Log;
import com.funambol.util.bus.BusService;

/**
 * This class represents an engine for synchronizations and has the following
 * main goals:
 *
 * 1) Perform some basic checks before firing a sync. For example it checks if
 *    radio signal is good. These checks are platform dependent and not
 *    performed by the APIs
 * 2) Incapsulate error handling and sync threading. When a sync is requested it
 *    is run in a separate thread, and this thread is monitored by the
 *    SyncEngine which intercepts exceptions and handle them
 * 3) Support compression error recovering. If a sync throws an error because
 *    compression is not supported, then the sync is resumed without compression
 * 4) Add a listener for the entire sync. Each sync source has its own listener
 *    for source specific events, but the SyncEngine has a different listener
 *    that generates events global to the entire synchronization.
 *
 */
public class SyncTask implements ResumableTask {

    private static final String TAG_LOG = "SyncTask";

    private Customization          customization = null;
    private AppSyncSourceManager   appSyncSourceManager = null;
    private Configuration          configuration = null;
    
    private SyncThread             syncThread;
    private NetworkStatus          networkStatus;
    private boolean                spawnThread;
    private TransportAgent         customTransportAgent = null;
    private Hashtable              customHeaders = null;

    private boolean                isSynchronizing = false;
    private boolean                isCancelled     = false;
    private AppSyncSource          currentSource   = null;
    private Vector                 sourcesToSync   = null;

    public SyncTask(Customization customization, Configuration configuration,
            AppSyncSourceManager appSyncSourceManager, NetworkStatus networkStatus)
    {
        this.customization = customization;
        this.configuration = configuration;
        this.appSyncSourceManager = appSyncSourceManager;
        this.networkStatus = networkStatus;

        // By default the task runs on the caller's thread
        spawnThread = false;
    }

    /**
     * Set whether the sync shall start in a separate thread
     * @param value
     */
    public void setSpawnThread(boolean value) {
        spawnThread = value;
    }

    public void setNetworkStatus(NetworkStatus networkStatus) {
        this.networkStatus = networkStatus;
    }

    public void cancelSync() {
        if (Log.isLoggable(Log.INFO)) {
            Log.info(TAG_LOG, "Cancelling sync");
        }
        if (isSynchronizing && syncThread != null) {
            if (Log.isLoggable(Log.DEBUG)) {
                Log.debug(TAG_LOG, "Cancelling sync on sync thread");
            }
            syncThread.cancelSync();
            isCancelled = true;
        }
    }

    public void setTransportAgent(TransportAgent ta) {
        this.customTransportAgent = ta;
    }

    public void addTranportAgentHeaders(Hashtable headers) {
        this.customHeaders = headers;
    }

    /**
     * Returns true iff a sync is in progress
     */
    public boolean isSynchronizing(){
        return isSynchronizing;
    }

    public void setSourcesToSync(Vector sources) {
        this.sourcesToSync = sources;
    }
    
    /**
     * Returns the source which is currently being synchronized. This method
     * returns a non null value only if isSynchronizing returns true. The method
     * may return null even during a synchronization. In particular a
     * synchronization is requested and this immediately triggers the
     * "isSynchronizing" but the currentSource is not set until the source
     * really starts synchronizing. Users must be ready to handle null return
     * values.
     */
    public AppSyncSource getCurrentSource() {
        return currentSource;
    }

    /**
     * @see Task#run() 
     */
    public void run() {
        if(sourcesToSync == null) {
            Log.error(TAG_LOG, "You must set the sources to synchronize first");
            return;
        }
        synchronize(sourcesToSync);
    }

    /**
     * @see ResumableTask#suspend()
     */
    public boolean suspend() {
        cancelSync();
        // are we sure the sync will be cancelled?
        return true;
    }

    /**
     * @see ResumableTask#resume()
     */
    public void resume() {
        // TODO: handle resume
    }

    public boolean synchronize(Vector sources) {
        if (Log.isLoggable(Log.INFO)) {
            Log.info(TAG_LOG, "synchronize");
        }

        isCancelled = false;
        isSynchronizing = true;

        sendSyncTaskMessage(SyncTaskMessage.MESSAGE_BEGIN_SYNC);

        if (configuration.getUsername() == null || 
            configuration.getUsername().length() == 0 ||
            configuration.getPassword() == null ||
            configuration.getPassword().length() == 0) {
            sendSyncTaskMessage(SyncTaskMessage.MESSAGE_NO_CREDENTIALS);
            syncEnded();
            return false;
        }
        // Safety check. If there are no ready to use sources the sync is
        // stopped. This case should be captured earlier in the flow, but
        // just in case....
        if (appSyncSourceManager.numberOfEnabledAndWorkingSources() == 0) {
            sendSyncTaskMessage(SyncTaskMessage.MESSAGE_NO_SOURCES);
            syncEnded();
            return false;
        }
        if (networkStatus != null && !networkStatus.isConnected()) {
            if (networkStatus.isRadioOff()) {
                sendSyncTaskMessage(SyncTaskMessage.MESSAGE_NO_CONNECTION);
            } else {
                sendSyncTaskMessage(SyncTaskMessage.MESSAGE_NO_SIGNAL);
            }
            syncEnded();
            return false;
        }
        Vector checkSources = new Vector();
        for (int x = 0; x < sources.size(); x++) {
            AppSyncSource appSource = (AppSyncSource) sources.elementAt(x);
            SyncSource    source    = appSource.getSyncSource();
            int mode = source.getConfig().getSyncMode();
            if (mode != SyncSource.FULL_UPLOAD && mode != SyncSource.FULL_DOWNLOAD) {
                checkSources.addElement(source);
            }
        }
        if (sources.isEmpty()) {
            sendSyncTaskMessage(SyncTaskMessage.MESSAGE_NO_SOURCES);
            syncEnded();
            return false;
        } else {
            if (isCancelled) {
                syncEnded();
                return false;
            }
            syncThread = new SyncThread(sources);
            if (spawnThread) {
                syncThread.setPriority(Thread.MIN_PRIORITY);
                syncThread.start();
            } else {
                syncThread.sync();
            }
        }
        return true;
    }

    /**
     * Utility method to be invoked at the end of a sync. The method resets all
     * the necessary internal variables and invoke the listener.
     */
    private void syncEnded() {
        // Save the latest authentication config.
        if (configuration != null) {
            if (configuration.getSyncConfig() != null) {
                configuration.setClientNonce(configuration.getSyncConfig().clientNonce);
            }
            configuration.save();
        }
        
        isSynchronizing = false;
        isCancelled = false;
        currentSource = null;
        sourcesToSync = null;
        syncThread = null;

        sendSyncTaskMessage(SyncTaskMessage.MESSAGE_SYNC_ENDED);
    }

    protected SyncManagerI createManager(AppSyncSource source, SyncConfig config, DeviceConfig dc) {
        // We must create the proper sync manager instance, depending on the
        // source type/properties
        if (source.getIsMedia()) {
            SapiSyncManager sm = new SapiSyncManager(config, dc);
            return sm;
        } else {
            // We apply some logic to decide some of the synchronization configuration properties.
            DevInf serverDevInf = configuration.getServerDevInf();
            adaptSyncConfig(config, dc, serverDevInf);

            SyncManager sm = new SyncManager(config, dc);
            if(customTransportAgent != null) {
                sm.setTransportAgent(customTransportAgent);
            }
            if (customHeaders != null) {
                sm.addTranportAgentHeaders(customHeaders);
            }
            return sm;
        }
    }

    private class SyncThread extends Thread {

        private final Vector appSources;
        private boolean compressionRetry;
        private SyncConfig   syncConfig;
        private DeviceConfig   deviceConfig;
        private SyncManagerI manager;

        public SyncThread(Vector sources) {
            this.appSources       = sources;
            this.compressionRetry = false;
        }

        public SyncConfig getSyncConfig() {
            return syncConfig;
        }

        public void cancelSync() {
            if (Log.isLoggable(Log.INFO)) {
                Log.info(TAG_LOG, "Cancelling sync");
            }
            if (manager != null) {
                manager.cancel();
            }
        }

        public void run() {
            sync();
        }

        public synchronized void sync() {
            if (Log.isLoggable(Log.INFO)) {
                Log.info(TAG_LOG, "SyncThread.run");
            }
            syncConfig = configuration.getSyncConfig();
            deviceConfig = configuration.getDeviceConfig();
            if (compressionRetry) {
                // We are trying without compression, make sure compression is
                // really disabled
                syncConfig.compress = false;
            }
            sendSyncTaskMessage(SyncTaskMessage.MESSAGE_SYNC_STARTED, appSources);
            try {
                synchronize();
            } catch (CompressedSyncException e) {
                if (!compressionRetry) {
                    // Only retry because of compression once
                    if (Log.isLoggable(Log.INFO)) {
                        Log.info(TAG_LOG, "Sync failed because compression failed - Retrying");
                    }
                    compressionRetry = true;
                    // Recurse
                    this.run();
                    return;
                }
            } catch (Throwable e) {
                // This is unexpected, but we don't want the app to die
                Log.error(TAG_LOG, "Exception caught during synchronization", e);
            } finally {
                syncEnded();
            }
        }

        /**
         * The main procedure for the sync thread
         * 
         * @throws Exception
         */
        private Vector synchronize() throws Exception {

            if (Log.isLoggable(Log.INFO)) {
                Log.info(TAG_LOG, "synchronize");
            }

            Vector failedSources = new Vector();
            boolean firstSource = true;

            for (int x = 0; x < appSources.size(); x++) {

                if (isCancelled) {
                    // If the sync got cancelled then we must exit the loop
                    // Even if the user cancel the sync the SyncSource may be
                    // unable to throw the proper exception if other errors
                    // (such as network error) kick in. So we must recheck here
                    // and stop the sync if necessary
                    break;
                }

                AppSyncSource appSource = (AppSyncSource)appSources.elementAt(x);
                SyncSource source = appSource.getSyncSource();

                if (Log.isLoggable(Log.INFO)) {
                    Log.info(TAG_LOG, "Firing sync for source " + appSource.getName());
                }

                // We need to create one manager for each source
                manager = createManager(appSource, syncConfig, deviceConfig);

                sendSyncTaskMessage(SyncTaskMessage.MESSAGE_SOURCE_STARTED, appSource);

                try {

                    // Before synchronizing, we may need to update the user profile
                    // we do it only once per sync session
                    if (firstSource && customization.getUserProfileSupported() &&
                        configuration.getServerType() == Configuration.SERVER_TYPE_FUNAMBOL_CARED)
                    {
                        updateProfile(appSources);
                    }

                    // The call to updateProfile may have change the allow status
                    // for the current source.
                    if (!appSource.getConfig().getAllowed()) {
                        if (Log.isLoggable(Log.INFO)) {
                            Log.info(TAG_LOG, "Skipping not allowed source " + appSource.getName());
                        }
                        SyncException se = new SyncException(SyncException.FORBIDDEN_ERROR, "Source not allowed");
                        sendSyncTaskMessage(SyncTaskMessage.MESSAGE_SOURCE_FAILED, appSource, se);
                        continue;
                    }

                    // Set this source as the one currently synchronized
                    currentSource = appSource;

                    //This sync could have been cancelled when the client was
                    //deleting the device items during a refresh from server operation
                    if (!isCancelled) {
                        //Ask for server dev inf if this is required
                        boolean askServerCaps = configuration.getCredentialsCheckPending() ||
                                                configuration.getForceServerCapsRequest()  ||
                                                configuration.getServerDevInf() == null;
                        if (Log.isLoggable(Log.DEBUG)) {
                            Log.debug(TAG_LOG, "Asking for server caps: " + askServerCaps);
                        }

                        int syncMode = source.getConfig().getSyncMode();
                        if (!appSource.getIsMedia()) {
                            SyncMLAnchor anchor = (SyncMLAnchor)source.getConfig().getSyncAnchor();
                            if (anchor.getLast() == 0) {
                                if (Log.isLoggable(Log.INFO)) {
                                    Log.info(TAG_LOG, "Forcing a full sync because the anchor is zero");
                                }
                                syncMode = SyncSource.FULL_SYNC;
                            }
                        }

                        fireSync(manager, source, syncMode, askServerCaps);
                    }

                    currentSource = null;

                    if (source.getStatus() != SyncSource.STATUS_SUCCESS) {
                        failedSources.addElement(appSource);
                    } 

                    // If we get here, it means that the sync was succesfull and
                    // we can update the anchors. A corner case is that a cancel 
                    // operation has been performed when the client was removing
                    // all data during a refresh from server operation. In that 
                    // case the sync was never started but a slow sync must take 
                    // place next time in order not to have intem deleted whitin
                    // a reset operation to be sent to the server on the next 
                    // fast sync
                    sendSyncTaskMessage(SyncTaskMessage.MESSAGE_SOURCE_ENDED, appSource);
                } catch (final Exception e) {

                    boolean compressError = (e instanceof CompressedSyncException);
                    if (compressError) {
                        if (!compressionRetry) {
                            if (Log.isLoggable(Log.INFO)) {
                                Log.info(TAG_LOG, "Retrying without compression");
                            }
                        }

                        throw new CompressedSyncException(e.getMessage());
                    }

                    SyncException se;
                    if (e instanceof SyncException) {
                        se = (SyncException)e;
                    } else {
                        se = new SyncException(SyncException.CLIENT_ERROR, e.toString());
                    }

                    sendSyncTaskMessage(SyncTaskMessage.MESSAGE_SOURCE_FAILED, appSource, se);

                    // After this point, we know the source really
                    // failed

                    failedSources.addElement(appSource);

                    // Depending on the reason why the sync failed, we may
                    // want to abort the other sources as well (e.g. invalid
                    // credentials)
                    if (se.getCode() == SyncException.AUTH_ERROR ||
                        se.getCode() == SyncException.FORBIDDEN_ERROR ||
                        se.getCode() == SyncException.CANCELLED ||
                        se.getCode() == SyncException.PAYMENT_REQUIRED) {

                        break;
                    }
                } finally {
                    source = null;
                    currentSource = null;
                    firstSource = false;
                }
            }

            sendSyncTaskMessage(SyncTaskMessage.MESSAGE_END_SYNC, appSources,
                    failedSources.size() > 0);

            return failedSources;
        }
    }

    protected void fireSync(SyncManagerI manager, SyncSource source, int syncMode, boolean askServerCaps) {
        manager.sync(source, syncMode, askServerCaps);
    }

    /**
     * TODO FIXME: this method shall be based on a compatibility table of some kind
     */
    protected void adaptSyncConfig(SyncConfig syncConfig, DeviceConfig deviceConfig, DevInf serverDevInf) {

        // If the customization forces the usage of WBXML, then we always use it
        // otherwise we use it only for servers that we know are compatible with
        // our implementation
        if (customization.getUseWbxml()) {
            if (Log.isLoggable(Log.TRACE)) {
                Log.trace(TAG_LOG, "WBXML usage is forced by Customization");
            }
            deviceConfig.setWBXML(true);
        } else {
            if (serverDevInf != null) {
                String man = serverDevInf.getMan();
                if (StringUtil.equalsIgnoreCase(man, "funambol")) {
                    if (Log.isLoggable(Log.TRACE)) {
                        Log.trace(TAG_LOG, "WBXML enabled");
                    }
                    deviceConfig.setWBXML(true);
                }
            } else {
                // We don't know yet who we are talking to, for this reason we try to use the most conservative
                // configuration
                if (Log.isLoggable(Log.TRACE)) {
                    Log.trace(TAG_LOG, "WBXML disabled");
                }
                deviceConfig.setWBXML(false);
            }
        }
    }

    private void updateProfile(Vector syncSources) throws SyncException {
        try {
            ProfileUpdateHelper puh = new ProfileUpdateHelper(appSyncSourceManager, configuration);
            puh.updateProfile();  
            syncSources = filterNonAllowedSources(syncSources);
        } catch (SapiException se) {
            Log.error(TAG_LOG, "Cannot update profile", se);
            SyncException syncExc;
            if (SapiException.NO_CONNECTION.equals(se.getCode()) ||
                SapiException.HTTP_400.equals(se.getCode()))
            {
                // Network error
                syncExc = new SyncException(SyncException.CONN_NOT_FOUND, se.toString());
            } else {
                syncExc = new SyncException(SyncException.CLIENT_ERROR, se.toString());
            }
            throw syncExc;
        } catch (Exception e) {
            Log.error(TAG_LOG, "Config sync failed ", e);            
            SyncException se = new SyncException(SyncException.CLIENT_ERROR, e.toString());
            throw se;
        }
    }

    private Vector filterNonAllowedSources(Vector sources) {
        for(int i=sources.size() - 1;i>=0;--i) {
            AppSyncSource appSource = (AppSyncSource)sources.elementAt(i);
            if (!appSource.getConfig().getAllowed()) {
                sources.removeElementAt(i);
            }
        }
        return sources;
    }

    private void sendSyncTaskMessage(int code) {
        SyncTaskMessage message = new SyncTaskMessage(code);
        BusService.sendMessage(message);
    }

    private void sendSyncTaskMessage(int code, AppSyncSource source) {
        SyncTaskMessage message = new SyncTaskMessage(code, source);
        BusService.sendMessage(message);
    }

    private void sendSyncTaskMessage(int code, Vector sources) {
        SyncTaskMessage message = new SyncTaskMessage(code, sources);
        BusService.sendMessage(message);
    }

    private void sendSyncTaskMessage(int code, AppSyncSource source, SyncException ex) {
        SyncTaskMessage message = new SyncTaskMessage(code, source, ex);
        BusService.sendMessage(message);
    }

    private void sendSyncTaskMessage(int code, Vector sources, boolean hadErrors) {
        SyncTaskMessage message = new SyncTaskMessage(code, sources, hadErrors);
        BusService.sendMessage(message);
    }
}

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

package com.funambol.client.configuration;

import java.util.Date;
import java.util.Enumeration;
import java.util.Vector;

import java.io.ByteArrayOutputStream;
import java.io.ByteArrayInputStream;
import java.io.DataOutputStream;
import java.io.DataInputStream;

import com.funambol.client.customization.Customization;
import com.funambol.client.controller.Controller;
import com.funambol.client.source.AppSyncSource;
import com.funambol.client.source.AppSyncSourceManager;
import com.funambol.client.source.AppSyncSourceConfig;
import com.funambol.syncml.protocol.SyncML;
import com.funambol.syncml.spds.DeviceConfig;
import com.funambol.syncml.spds.SourceConfig;
import com.funambol.syncml.spds.SyncListener;
import com.funambol.syncml.spds.SyncSource;
import com.funambol.syncml.spds.SyncConfig;
import com.funambol.util.Log;
import com.funambol.util.StringUtil;
import com.funambol.syncml.protocol.SyncML;

/**
 * Configuration class for configuration details
 */
public abstract class Configuration {

    private static final String TAG_LOG = "Configuration";

    // ------------------------------------------------------------ Constants

    /**
     * These constants specify the sync mode in one of three possible ways:
     * 1) manual
     * 2) push
     * 3) scheduled
     */
    public static int    SYNC_MODE_PUSH      = 0;
    public static int    SYNC_MODE_MANUAL    = 1;
    public static int    SYNC_MODE_SCHEDULED = 2;

    public static final int    CONF_OK                         = 0;
    public static final int    CONF_NOTSET                     = -1;
    public static final int    CONF_INVALID                    = -2;

    private static final String CONF_KEY_VERSION                = "VERSION";
    private static final String CONF_KEY_LOG_LEVEL              = "LOG_LEVEL";
    private static final String CONF_KEY_SYNC_URL               = "SYNC_URL";
    private static final String CONF_KEY_USERNAME               = "USERNAME";
    private static final String CONF_KEY_PASSWORD               = "PASSWORD";

    private static final String CONF_KEY_CLIENT_NONCE           = "CLIENT_NONCE";
    private static final String CONF_KEY_CRED_CHECK_PENDING     = "CRED_CHECK_PENDING";
    private static final String CONF_KEY_CRED_CHECK_REMEMBER    = "CRED_CHECK_REMEMBER";
    private static final String CONF_KEY_POLL_FLAG              = "POLL_PIM";
    private static final String CONF_KEY_POLL_TIME              = "POLL_PIM_TIME";
    private static final String CONF_KEY_POLL_TIMESTAMP         = "POLL_PIM_TIMESTAMP";
    private static final String CONF_KEY_SYNC_MODE              = "SYNC_MODE";
    //private static final String CONF_KEY_SYNC_TYPE              = "SYNC_TYPE";
    //private static final String CONF_KEY_BLOCK_INCOMING_INVITES = "BLOCK_INVITES";
    //private static final String CONF_KEY_SYNC_STATUS            = "SOURCE_STATUS";
    private static final String CONF_KEY_FIRST_RUN_TIMESTAMP    = "FIRST_RUN_TIMESTAMP";
    //private static final String PICTURES_SYNC_DIR               = "PICTURES_DIR";
    private static final String UPDATE_URL                      = "UPDATE_URL";
    private static final String UPDATE_TYPE                     = "UPDATE_TYPE";
    private static final String AVAILABLE_VERSION               ="AVAILABLE_VERSION";
    private static final String LAST_UPDATE_CHECK               = "LAST_UPDATE_CHECK";
    private static final String CHECK_INTERVAL                  = "CHECK_INTERVAL";
    private static final String REMINDER_INTERVAL               = "REMINDER_INTERVAL";
    private static final String LAST_REMINDER                   = "LAST_REMINDER";
    private static final String SKIP_UPDATE                     = "SKIP_UPDATE";
    private static final String MAXMSGSIZE                      = "16000";
    private static final String CONFIG_VERSION                  = "5";
    private static String       OLD_CONFIG_VERSION              = "4";
    private static String       PROTOCOL;
    private static String       RETRY;

    private int          logLevel                        = 0;              // disabled
    private String       syncUrl;
    private String       username;
    private String       password;

    private String       clientNonce;
    
    private boolean      credentialsCheckPending         = true;
    private boolean      credentialsCheckRemember        = false;
    private int          syncMode;
    private int          pollingInterval;
    private int          rangePast;
    private int          rangeFuture;
    private boolean      blockIncomingInvites            = false;
    private boolean      initialized                     = false;
    private boolean      loaded                          = false;
    private String       picturesSyncDir                 = null;
    private boolean      picturesSyncOld                 = false;
    //update parameters
    private String       downloadUrl                     = " ";;
    private String       updateType                      = " ";
    private String       availableVersion                = " ";;
    private long         lastUpdateCheck                 = 0L;
    private long         checkInterval                   = 24 * 60 * 60 * 1000; // 24hours in millisecs
    private long         reminderInterval;
    private long         lastReminder;
    private boolean      skip                            = false;
    private long         pollingTimestamp                = 0;
    private String       version;
    private long         firstRunTimestamp               = 0;

    // These values don't need to be saved/restored
    private int          origLogLevel                      = 0;
    private boolean      dirtyAccount                    = false;
    private boolean      dirtyUpdater                    = false;
    private boolean      dirtySyncMode                   = false;
    private boolean      dirtyMisc                       = false;

    private Customization customization                  = null;
    private Controller    controller                     = null;
    private AppSyncSourceManager appSyncSourceManager    = null;

    public Configuration(Customization customization, AppSyncSourceManager appSyncSourceManager) {

        this.customization = customization;
        this.appSyncSourceManager = appSyncSourceManager;
    }

    private void copyDefaults() {

        Log.info(TAG_LOG, "copyDefaults");

        // Jad has precedence over Customization as it allows a per user
        // settings
        // TODO FIXME MARCO
        syncUrl  = "";
        username = "";
        password = "";
        String checkIntervalprop = "";
        String reminderIntervalprop = "";

        if (syncUrl == null) {
            syncUrl  = customization.getServerUriDefault();
        }

        if (username == null) {
            username = customization.getUserDefault();
        }

        if (password == null) {
            password = customization.getPasswordDefault();
        }
        
        if(checkIntervalprop == null){
            checkInterval = customization.getCheckUpdtIntervalDefault();
        }else{
            checkInterval = Long.parseLong(checkIntervalprop);
        }
        
        if(reminderIntervalprop == null){
            reminderInterval = customization.getReminderUpdtIntervalDefault();
        }else{
            reminderInterval = Long.parseLong(reminderIntervalprop);
        }

        credentialsCheckPending  = true;
        credentialsCheckRemember = false;

        syncMode = customization.getDefaultSyncMode();
        pollingInterval = customization.getDefaultPollingInterval();

        pollingTimestamp = 0;

        rangePast = customization.getDefaultRangePast();
        rangeFuture = customization.getDefaultRangeFuture();

        blockIncomingInvites = customization.getDefaultBlockInvites();
        
        picturesSyncDir    = customization.getPicturesDir();
        picturesSyncOld    = false;

        firstRunTimestamp  = 0;
    }

    // ------------------------------------------------------------ Public
    /**
     * Load the current config from the persistent store.
     * 
     * @return: <li><b>CONF_OK</b>: if all the data were present in the store.
     *          <li><b>CONF_NOTSET</b>: if the store is not present.
     *          Configuration remains untouched. <li><b>CONF_INVALID</b>: if the
     *          store does not contain valid data. Configuration is reverted to
     *          default. Note: if a parameter is not present in the store, the
     *          current value is kept for it.
     */
    public int load() {
        if (loaded) {
            return CONF_OK;
        }
        boolean available = loadKey(CONF_KEY_VERSION) != null;

        // The config needs to be loaded from the storage
        version  = loadStringKey(CONF_KEY_VERSION, CONFIG_VERSION);

        logLevel = loadIntKey(CONF_KEY_LOG_LEVEL, Log.ERROR);
        syncUrl  = loadStringKey(CONF_KEY_SYNC_URL, customization.getServerUriDefault());
        username = loadStringKey(CONF_KEY_USERNAME, customization.getUserDefault());
        password = loadStringKey(CONF_KEY_PASSWORD, customization.getPasswordDefault());

        clientNonce = loadStringKey(CONF_KEY_CLIENT_NONCE, null);
        
        credentialsCheckPending = loadBooleanKey(CONF_KEY_CRED_CHECK_PENDING, true);
        credentialsCheckRemember = loadBooleanKey(CONF_KEY_CRED_CHECK_REMEMBER, false);
        pollingInterval = loadIntKey(CONF_KEY_POLL_TIME, customization.getDefaultPollingInterval());

        // Compute "now"
        Date now = new Date();
        firstRunTimestamp = loadLongKey(CONF_KEY_FIRST_RUN_TIMESTAMP, now.getTime());

        // Set the default sync mode
        syncMode = loadIntKey(CONF_KEY_SYNC_MODE, customization.getDefaultSyncMode());

        if (available) {
            return CONF_OK;
        } else {
            return CONF_NOTSET;
        }
    }

    public boolean loadBooleanKey(String key, boolean defaultValue) {
        String v = loadKey(key);
        boolean bv;
        if (v == null) {
            bv = defaultValue;
        } else {
            if (v.equals("TRUE")) {
                bv = true;
            } else {
                bv = false;
            }
        }
        return bv;
    }

    public void saveBooleanKey(String key, boolean value) {
        String v;
        if (value) {
            v = "TRUE";
        } else {
            v = "FALSE";
        }
        saveKey(key, v);
    }

    public int loadIntKey(String key, int defaultValue) {
        String v = loadKey(key);
        int iv;
        if (v == null) {
            iv = defaultValue;
        } else {
            try {
                iv = Integer.parseInt(v);
            } catch (Exception e) {
                iv = defaultValue;
            }
        }
        return iv;
    }

    public void saveIntKey(String key, int value) {
        String v = String.valueOf(value);
        saveKey(key, v);
    }

    public long loadLongKey(String key, long defaultValue) {
        String v = loadKey(key);
        long iv;
        if (v == null) {
            iv = defaultValue;
        } else {
            try {
                iv = Long.parseLong(v);
            } catch (Exception e) {
                iv = defaultValue;
            }
        }
        return iv;
    }

    public void saveLongKey(String key, long value) {
        String v = String.valueOf(value);
        saveKey(key, v);
    }

    public String loadStringKey(String key, String defaultValue) {
        String v = loadKey(key);
        if (v == null) {
            v = defaultValue;
        }
        return v;
    }

    public void saveStringKey(String key, String value) {
        saveKey(key, value);
    }

    public void setController(Controller controller) {
        this.controller = controller;
    }

    public int save() {

         // The config needs to be loaded from the storage
        saveStringKey(CONF_KEY_VERSION, CONFIG_VERSION);
        saveIntKey(CONF_KEY_LOG_LEVEL, logLevel);
        saveStringKey(CONF_KEY_SYNC_URL, syncUrl);
        saveStringKey(CONF_KEY_USERNAME, username);
        saveStringKey(CONF_KEY_PASSWORD, password);

        saveStringKey(CONF_KEY_CLIENT_NONCE, clientNonce);
        saveBooleanKey(CONF_KEY_CRED_CHECK_PENDING, credentialsCheckPending);
        saveBooleanKey(CONF_KEY_CRED_CHECK_REMEMBER, credentialsCheckRemember);
        saveIntKey(CONF_KEY_SYNC_MODE, syncMode);
        saveLongKey(CONF_KEY_FIRST_RUN_TIMESTAMP, firstRunTimestamp);
        saveIntKey(CONF_KEY_POLL_TIME, pollingInterval);

        // Save each source configuration parameters
        Enumeration workingSources = appSyncSourceManager.getWorkingSources();
        while(workingSources.hasMoreElements()) {
            AppSyncSource appSource = (AppSyncSource)workingSources.nextElement();
            AppSyncSourceConfig sc = appSource.getConfig();
            if (sc.isDirty()) {
                sc.save();
            }
        }

        if (controller != null) {
            // Notify the controller on the config changes so that
            // proper actions can be taken
            if (dirtyAccount) {
                controller.reapplyAccountConfiguration();
                dirtyAccount = false;
            }
            if (dirtyUpdater) {
                controller.reapplyUpdaterConfiguration();
                dirtyUpdater = false;
            }
            if (dirtySyncMode) {
                controller.reapplySyncModeConfiguration();
                dirtySyncMode = false;
            }
            if (dirtyMisc) {
                controller.reapplyMiscConfiguration();
                dirtyMisc = false;
            }
        }

        // finally we commit changes
        boolean res = commit();
        int retValue;

        if (res) {
            retValue = CONF_OK;
        } else {
            retValue = CONF_INVALID;
        }
        return retValue;
    }

    public void notifySourceConfigChanged(AppSyncSource appSource) {
        if (controller != null) {
            controller.reapplySourceConfiguration(appSource);
        }
    }

    public int getLogLevel() {
        return logLevel;
    }

    public void setLogLevel(int logLevel) {
        if(this.logLevel != logLevel) {
            dirtyMisc = true;
            this.logLevel = logLevel;
        }
    }

    public boolean getBlockIncomingInvites() {
        return blockIncomingInvites;
    }

    public void setBlockIncomingInvites(boolean blockIncomingInvites) {
        if(this.blockIncomingInvites != blockIncomingInvites) {
            dirtyMisc = true;
            this.blockIncomingInvites = blockIncomingInvites;
        }
    }

    public String getUsername() {
        return username;
    }

    public void setUsername(String username) {
        if(!username.equals(this.username)) {
            dirtyAccount = true;
            this.username = username;
        }
    }

    public String getPassword() {
        return password;
    }

    public void setPassword(String password) {
        if(!password.equals(this.password)) {
            dirtyAccount = true;
            this.password = password;
        }
    }

    public String getSyncUrl() {
        return syncUrl;
    }

    public void setSyncUrl(String syncUrl) {
        if(!syncUrl.equals(this.syncUrl)) {
            dirtyAccount = true;
            this.syncUrl = syncUrl;
        }
    }

    public String getClientNonce() {
        return clientNonce;
    }

    public void setClientNonce(String nonce) {
        if((nonce != null && !nonce.equals(this.clientNonce)) ||
           (nonce == null && this.clientNonce != null)) {
            dirtyMisc = true;
            this.clientNonce = nonce;
        }
    }

    public int getSyncMode() {
        return syncMode;
    }

    public void setSyncMode(int modeIndex) {
        if(syncMode != modeIndex) {
            dirtySyncMode = true;
            syncMode = modeIndex;
        }
    }

    public int getPollingInterval() {
        return pollingInterval;
    }

    public long getPollingTimestamp() {
        return pollingTimestamp;
    }

    public long getFirstRunTimestamp() {
        return firstRunTimestamp;
    }

    public boolean getCredentialsCheckPending() {
        return credentialsCheckPending;
    }

    public void setCredentialsCheckPending(boolean value) {
        if(credentialsCheckPending != value) {
            dirtyUpdater = true;
            credentialsCheckPending = value;
        }
    }

    public void setPollingInterval(int interval) {
        if(pollingInterval != interval) {
            dirtySyncMode = true;
            pollingInterval = interval;
        }
    }

    public void setPollingTimestamp(long timestamp) {
        if(pollingTimestamp != timestamp) {
            dirtySyncMode = true;
            pollingTimestamp = timestamp;
        }
    }

    public int getRangePast() {
        return rangePast;
    }

    public void setRangePast(int range) {
        if(rangePast != range) {
            dirtyMisc = true;
            rangePast = range;
        }
    }
    
    public int getRangeFuture() {
        return rangeFuture;
    }

    public void setRangeFuture(int range) {
        if(rangeFuture != range) {
            dirtyMisc = true;
            rangeFuture = range;
        }
    }

    public boolean isInitialized() {
        return initialized;
    }

    public String getDownloadUrl() {
        return downloadUrl;
    }

    public void setDownloadUrl(String url) {
        if(!url.equals(this.downloadUrl)) {
            dirtyUpdater = true;
            downloadUrl = url;
        }
    }

    public String getUpdateType() {
        return updateType;
    }

    public void setUpdateType(String type) {
        if(!type.equals(this.updateType)) {
            dirtyUpdater = true;
            updateType = type;
        }
    }

    public String getAvailableVersion() {
        return availableVersion;
    }

    public void setAvailableVersion(String version) {
        if(!version.equals(this.availableVersion)) {
            dirtyUpdater = true;
            availableVersion = version;
        }
    }

    public long getLastUpdateCheck() {
        return lastUpdateCheck;
    }

    public void setLastUpdateCheck(long when) {
        if(this.lastUpdateCheck != when) {
            dirtyUpdater = true;
            lastUpdateCheck = when;
        }
    }

    public long getCheckInterval() {
        return checkInterval;
    }

    public void setCheckInterval(long interval) {
        if(this.checkInterval != interval) {
            dirtyUpdater = true;
            checkInterval = interval;
        }
    }

    public long getReminderInterval() {
        return reminderInterval;
    }

    public void setReminderInterval(long interval) {
        if(this.reminderInterval != interval) {
            dirtyUpdater = true;
            reminderInterval = interval;
        }
    }

    public long getLastReminder() {
        return lastReminder;
    }

    public void setLastReminder(long when) {
        if(this.lastReminder != when) {
            dirtyUpdater = true;
            lastReminder = when;
        }
    }

    public boolean getSkip() {
        return skip;
    }

    public void setSkip(boolean skip) {
        if(this.skip != skip) {
            dirtyUpdater = true;
            this.skip = skip;
        }
    }

    public SyncConfig getSyncConfig() {
        SyncConfig syncConfig = new SyncConfig();

        // TODO set before this runs
        syncConfig.syncUrl = this.syncUrl;
        syncConfig.lastServerUrl = this.syncUrl;
        syncConfig.userName = this.username;
        syncConfig.password = this.password;

        syncConfig.clientNonce = this.clientNonce;
        syncConfig.preferredAuthType = customization.getDefaultAuthType();
        
        // Remember to update the blackberry synclet pattern (server side) when changing the user agent
        // TODO FIXME MARCO
        syncConfig.userAgent = getUserAgent();
        syncConfig.deviceConfig = getDeviceConfig();
        syncConfig.forceCookies = false;

        return syncConfig;
    }

    public void setTempLogLevel(int tempLogLevel) {
        origLogLevel = logLevel;
        logLevel = tempLogLevel;
    }

    public void restoreLogLevel() {
        logLevel = origLogLevel;
    }

    public abstract void    saveByteArrayKey(String key, byte[] value);
    public abstract byte[]  loadByteArrayKey(String key, byte[] defaultValue);
    public abstract boolean commit();

    protected abstract String  loadKey(String key);
    protected abstract void    saveKey(String key, String value);
    protected abstract DeviceConfig getDeviceConfig();
    protected abstract String  getUserAgent();
}

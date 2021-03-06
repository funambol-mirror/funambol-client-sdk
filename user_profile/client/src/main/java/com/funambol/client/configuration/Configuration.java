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

import com.funambol.client.customization.Customization;
import com.funambol.client.controller.Controller;
import com.funambol.client.source.AppSyncSource;
import com.funambol.client.source.AppSyncSourceManager;
import com.funambol.client.source.AppSyncSourceConfig;
import com.funambol.sync.SyncConfig;
import com.funambol.syncml.spds.DeviceConfig;
import com.funambol.syncml.spds.DevInfSerializer;
import com.funambol.syncml.protocol.DevInf;
import com.funambol.platform.net.ProxyConfig;
import com.funambol.util.Log;
import com.funambol.util.AppProperties;

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
    public static final int SYNC_MODE_PUSH      = 0;
    public static final int SYNC_MODE_MANUAL    = 1;
    public static final int SYNC_MODE_SCHEDULED = 2;

    public static final int S2C_PUSH_MODE_DISABLED    = 0;
    public static final int S2C_PUSH_MODE_SMS         = 1;
    public static final int S2C_PUSH_MODE_WAP         = 2;
    public static final int S2C_PUSH_MODE_SMS_AND_WAP = 3;

    public static final int FIRST_SYNC_NOT_YET        = 0;
    public static final int FIRST_SYNC_SIGN_UP        = 1;
    public static final int FIRST_SYNC_LOG_IN         = 2;
    /** Dialog already shown */
    public static final int FIRST_SYNC_DONE           = 3;

    public static final int CONF_OK      = 0;
    public static final int CONF_NOTSET  = -1;
    public static final int CONF_INVALID = -2;

    public static final int SERVER_TYPE_UNKNOWN        = -1;
    public static final int SERVER_TYPE_FUNAMBOL_COMED = 0;
    public static final int SERVER_TYPE_FUNAMBOL_CARED = 1;
    public static final int SERVER_TYPE_OTHER          = 2;

    protected static final String CONF_KEY_VERSION   = "VERSION";
    protected static final String CONF_KEY_LOG_LEVEL = "LOG_LEVEL";
    protected static final String CONF_KEY_SYNC_URL  = "SYNC_URL";
    protected static final String CONF_KEY_USERNAME  = "USERNAME";
    protected static final String CONF_KEY_PASSWORD  = "PASSWORD";

    protected static final String CONF_KEY_SIGNUP_ACC_CREATED  = "SIGNUP_ACCOUNT_CREATED";
    protected static final String CONF_KEY_FIRST_SYNC_FROM_SCRATCH = "FIRST_SYNC_FROM_SCRATCH";

    protected static final String CONF_KEY_BANDWIDTH_SAVER     = "BANDWIDTH_SAVER";

    protected static final String CONF_KEY_CLIENT_NONCE        = "CLIENT_NONCE";
    protected static final String CONF_KEY_CRED_CHECK_PENDING  = "CRED_CHECK_PENDING";
    protected static final String CONF_KEY_CRED_CHECK_REMEMBER = "CRED_CHECK_REMEMBER";
    protected static final String CONF_KEY_POLL_TIME           = "POLL_PIM_TIME";
    protected static final String CONF_KEY_POLL_TIMESTAMP      = "POLL_PIM_TIMESTAMP";
    protected static final String CONF_KEY_SYNC_MODE           = "SYNC_MODE";
    protected static final String CONF_KEY_S2C_PUSH_MODE       = "S2C_PUSH_MODE";
    protected static final String CONF_KEY_C2S_PUSH            = "C2S_PUSH";
    
    protected static final String CONF_KEY_FIRST_RUN_TIMESTAMP = "FIRST_RUN_TIMESTAMP";
    protected static final String CONF_KEY_SERVER_FIRST_RUN_TIMESTAMP = "CONF_KEY_SERVER_FIRST_RUN_TIMESTAMP";

    protected static final String CONF_KEY_UPDATE_URL        = "UPDATE_URL";
    protected static final String CONF_KEY_UPDATE_TYPE       = "UPDATE_TYPE";
    protected static final String CONF_KEY_AVAILABLE_VERSION = "AVAILABLE_VERSION";
    protected static final String CONF_KEY_LAST_UPDATE_CHECK = "LAST_UPDATE_CHECK";
    protected static final String CONF_KEY_CHECK_INTERVAL    = "CHECK_INTERVAL";
    protected static final String CONF_KEY_REMINDER_INTERVAL = "REMINDER_INTERVAL";
    protected static final String CONF_KEY_LAST_REMINDER     = "LAST_REMINDER";
    protected static final String CONF_KEY_SKIP_UPDATE       = "SKIP_UPDATE";
    protected static final String CONF_KEY_ACTIVATION_DATE   = "ACTIVATION_DATE";

    protected static final String CONF_KEY_FORCE_SERVER_CAPS_REQ    = "FORCE_SERVER_CAPS_REQ";
    protected static final String CONF_KEY_SOURCE_SYNC_TYPE_CHANGED = "SOURCE_SYNC_TYPE_CHANGED";
    protected static final String CONF_KEY_SERVER_DEV_INF    = "SERVER_DEV_INF";
    protected static final String CONF_KEY_SERVER_TYPE       = "SERVER_TYPE";

    protected static final String CONF_KEY_CURRENT_SYNC_RETRY_COUNT = "CURRENT_SYNC_RETRY_COUNT";

    protected static final String CONF_KEY_PROFILE_EXPIRE_DATE = "PROFILE_EXPIRE_DATE";
    protected static final String CONF_KEY_PROFILE_MANUAL_ONLY = "PROFILE_MANUAL_ONLY";
    protected static final String CONF_KEY_PROFILE_NETWORK_USAGE_WARNING = "PROFILE_NETWORK_USAGE_WARNING";

    protected static final String CONFIG_VERSION = "11";

    protected String       version;
    
    protected int          logLevel                  = Log.ERROR;

    protected String       syncUrl;
    protected String       username;
    protected String       password;

    protected boolean      signupAccountCreated      = false;
    protected int          firstSyncFromScratch      = FIRST_SYNC_NOT_YET;
    
    protected int          syncMode;
    protected String       clientNonce;

    protected boolean      c2sPushEnabled            = false;

    protected boolean      bandwidthSaverChecked     = false;
    
    protected boolean      credentialsCheckPending   = true;
    protected boolean      credentialsCheckRemember  = false;
    
    protected int          pollingInterval;
    protected long         pollingTimestamp          = 0;

    protected String       downloadUrl               = " ";;
    protected String       updateType                = " ";
    protected String       availableVersion          = " ";;
    protected long         lastUpdateCheck           = 0L;
    protected long         checkInterval             = 24 * 60 * 60 * 1000; // 24hours in millisecs
    protected long         activationDate            = 0;
    protected long         reminderInterval;
    protected long         lastReminder;
    protected boolean      skip                      = false;

    protected long         firstRunTimestamp         = 0;
    protected boolean      forceServerCapsRequest    = false;

    protected boolean      pimSourceSyncTypeChanged  = false;

    protected int          currentSyncRetryCount     = -1; // -1 means no retry

    protected int          serverType                = SERVER_TYPE_UNKNOWN;

    /**
     * This is the proxy configuration. Note that at the moment this value is
     * not persisted in the configuration.
     */
    protected ProxyConfig  proxyConfig               = null;

    /**
     * These are the server dev inf. The information must be wiped each time the sync url gets changed.
     */
    protected DevInf       serverDevInf              = null;

    /**
     * Profile information
     */
    protected long profileExpireDate                 = -1;
    protected boolean profileManualOnly              = false;
    protected boolean profileNetworkUsageWarning     = false;


    // These values don't need to be saved/restored
    protected boolean      initialized               = false;
    protected boolean      loaded                    = false;
    protected int          origLogLevel              = -1;
    protected boolean      dirtyAccount              = false;
    protected boolean      dirtyUpdater              = false;
    protected boolean      dirtySyncMode             = false;
    protected boolean      dirtyMisc                 = false;
    protected Runnable     postConfigurationTask     = null;

    protected Customization customization                = null;
    protected Controller    controller                   = null;
    protected AppSyncSourceManager appSyncSourceManager  = null;

    public Configuration(Customization customization, AppSyncSourceManager appSyncSourceManager) {
        this.customization = customization;
        this.appSyncSourceManager = appSyncSourceManager;
    }

    protected void copyDefaults() {

        if (Log.isLoggable(Log.INFO)) {
            Log.info(TAG_LOG, "Copying default configuration values");
        }

        AppProperties properties = new AppProperties(null);
        syncUrl  = properties.get(AppProperties.URL_ATTR);
        username = properties.get(AppProperties.USER_ATTR);
        password = properties.get(AppProperties.PASSWORD_ATTR);

        if (syncUrl == null) {
            syncUrl  = customization.getServerUriDefault();
        }

        if (username == null) {
            username = customization.getUserDefault();
        }

        if (password == null) {
            password = customization.getPasswordDefault();
        }
        
        checkInterval = customization.getCheckUpdtIntervalDefault();
        reminderInterval = customization.getReminderUpdtIntervalDefault();

        credentialsCheckPending  = true;
        credentialsCheckRemember = false;

        // Set the default sync mode
        syncMode = customization.getDefaultSyncMode();
        pollingInterval = customization.getDefaultPollingInterval();

        pollingTimestamp = 0;

        bandwidthSaverChecked = false;

        c2sPushEnabled = false;

        // Compute "now"
        Date now = new Date();
        firstRunTimestamp = now.getTime();

        forceServerCapsRequest = false;
        clientNonce = "";

        pimSourceSyncTypeChanged = false;

        currentSyncRetryCount = -1;

        logLevel = Log.ERROR;

        serverType = SERVER_TYPE_UNKNOWN;

        version = CONFIG_VERSION;
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
        if (Log.isLoggable(Log.TRACE)) {
            Log.trace(TAG_LOG, "Loading config");
        }
        boolean available = loadKey(CONF_KEY_VERSION) != null;

        if (available) {

            // The config needs to be loaded from the storage
            version  = loadStringKey(CONF_KEY_VERSION, CONFIG_VERSION);

            logLevel = loadIntKey(CONF_KEY_LOG_LEVEL, Log.ERROR);

            syncUrl  = loadStringKey(CONF_KEY_SYNC_URL, customization.getServerUriDefault());
            username = loadStringKey(CONF_KEY_USERNAME, customization.getUserDefault());
            password = loadStringKey(CONF_KEY_PASSWORD, customization.getPasswordDefault());

            signupAccountCreated = loadBooleanKey(CONF_KEY_SIGNUP_ACC_CREATED, false);
            firstSyncFromScratch = loadIntKey(CONF_KEY_FIRST_SYNC_FROM_SCRATCH, FIRST_SYNC_NOT_YET);

            syncMode = loadIntKey(CONF_KEY_SYNC_MODE, customization.getDefaultSyncMode());
            
            clientNonce = loadStringKey(CONF_KEY_CLIENT_NONCE, null);

            credentialsCheckPending = loadBooleanKey(CONF_KEY_CRED_CHECK_PENDING, true);
            credentialsCheckRemember = loadBooleanKey(CONF_KEY_CRED_CHECK_REMEMBER, false);

            pollingInterval  = loadIntKey(CONF_KEY_POLL_TIME,
                    customization.getDefaultPollingInterval());
            pollingTimestamp = loadLongKey(CONF_KEY_POLL_TIMESTAMP, 0);

            bandwidthSaverChecked = loadBooleanKey(CONF_KEY_BANDWIDTH_SAVER, false);
            c2sPushEnabled = loadBooleanKey(CONF_KEY_C2S_PUSH, false);
            forceServerCapsRequest = loadBooleanKey(CONF_KEY_FORCE_SERVER_CAPS_REQ, false);

            pimSourceSyncTypeChanged = loadBooleanKey(CONF_KEY_SOURCE_SYNC_TYPE_CHANGED, false);

            currentSyncRetryCount = loadIntKey(CONF_KEY_CURRENT_SYNC_RETRY_COUNT, -1);

            // Updater properties
            downloadUrl      = loadStringKey(CONF_KEY_UPDATE_URL, " ");
            updateType       = loadStringKey(CONF_KEY_UPDATE_TYPE, " ");
            availableVersion = loadStringKey(CONF_KEY_AVAILABLE_VERSION, " ");
            lastUpdateCheck  = loadLongKey(CONF_KEY_LAST_UPDATE_CHECK, 0);
            checkInterval    = loadLongKey(CONF_KEY_CHECK_INTERVAL,
                    customization.getCheckUpdtIntervalDefault());
            reminderInterval = loadLongKey(CONF_KEY_REMINDER_INTERVAL, 0);
            lastReminder     = loadLongKey(CONF_KEY_LAST_REMINDER, 0);
            skip             = loadBooleanKey(CONF_KEY_SKIP_UPDATE, false);
            activationDate   = loadLongKey(CONF_KEY_ACTIVATION_DATE, 0);

            Date now = new Date();
            firstRunTimestamp = loadLongKey(CONF_KEY_FIRST_RUN_TIMESTAMP, now.getTime());

            // Profile properties
            profileExpireDate = loadLongKey(CONF_KEY_PROFILE_EXPIRE_DATE, -1);
            profileManualOnly = loadBooleanKey(CONF_KEY_PROFILE_MANUAL_ONLY, false);
            profileNetworkUsageWarning = loadBooleanKey(CONF_KEY_PROFILE_NETWORK_USAGE_WARNING, false);

            String devInf = loadStringKey(CONF_KEY_SERVER_DEV_INF, null);
            if (devInf != null && devInf.length() > 0) {
                DevInfSerializer devInfStore = new DevInfSerializer();
                try {
                    serverDevInf = devInfStore.deserialize(devInf);
                } catch (Exception e) {
                    Log.error(TAG_LOG, "Cannot deserialize server dev inf", e);
                    serverDevInf = null;
                }
            }

            // Server type
            serverType = loadIntKey(CONF_KEY_SERVER_TYPE, SERVER_TYPE_UNKNOWN);

            // Hook to migrate config (and upgrade the current version id if
            // possible)
            migrateConfig();
            loaded = true;
            return CONF_OK;
        } else {
            copyDefaults();
            loaded = true;
            return CONF_NOTSET;
        }
    }

    public boolean loadBooleanKey(String key, boolean defaultValue) {
        String v = loadKey(key);
        boolean bv;
        if (v == null) {
            bv = defaultValue;
        } else {
            // For backward compatibility issue, we must transform into upper
            // case
            v = v.toUpperCase();
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

        saveBooleanKey(CONF_KEY_SIGNUP_ACC_CREATED, signupAccountCreated);
        saveIntKey(CONF_KEY_FIRST_SYNC_FROM_SCRATCH, firstSyncFromScratch);

        saveStringKey(CONF_KEY_CLIENT_NONCE, clientNonce);
        saveBooleanKey(CONF_KEY_CRED_CHECK_PENDING, credentialsCheckPending);
        saveBooleanKey(CONF_KEY_CRED_CHECK_REMEMBER, credentialsCheckRemember);
        saveIntKey(CONF_KEY_SYNC_MODE, syncMode);
        saveLongKey(CONF_KEY_FIRST_RUN_TIMESTAMP, firstRunTimestamp);
        saveIntKey(CONF_KEY_POLL_TIME, pollingInterval);
        saveLongKey(CONF_KEY_POLL_TIMESTAMP, pollingTimestamp);

        saveBooleanKey(CONF_KEY_FORCE_SERVER_CAPS_REQ, forceServerCapsRequest);
        saveBooleanKey(CONF_KEY_SOURCE_SYNC_TYPE_CHANGED, pimSourceSyncTypeChanged);

        saveIntKey(CONF_KEY_CURRENT_SYNC_RETRY_COUNT, currentSyncRetryCount);

        saveBooleanKey(CONF_KEY_BANDWIDTH_SAVER, bandwidthSaverChecked);
        saveBooleanKey(CONF_KEY_C2S_PUSH, c2sPushEnabled);

        saveStringKey(CONF_KEY_UPDATE_URL, downloadUrl);
        saveStringKey(CONF_KEY_UPDATE_TYPE, updateType);
        saveStringKey(CONF_KEY_AVAILABLE_VERSION, availableVersion);
        saveLongKey(CONF_KEY_LAST_UPDATE_CHECK, lastUpdateCheck);
        saveLongKey(CONF_KEY_CHECK_INTERVAL, checkInterval);
        saveLongKey(CONF_KEY_REMINDER_INTERVAL, reminderInterval);
        saveLongKey(CONF_KEY_LAST_REMINDER, lastReminder);
        saveBooleanKey(CONF_KEY_SKIP_UPDATE, skip);
        saveLongKey(CONF_KEY_ACTIVATION_DATE, activationDate);

        // Profile information
        saveLongKey(CONF_KEY_PROFILE_EXPIRE_DATE, profileExpireDate);
        saveBooleanKey(CONF_KEY_PROFILE_MANUAL_ONLY, profileManualOnly);
        saveBooleanKey(CONF_KEY_PROFILE_NETWORK_USAGE_WARNING, profileNetworkUsageWarning);

        if (serverDevInf != null) {
            DevInfSerializer devInfStore = new DevInfSerializer();
            try {
                String devInf = devInfStore.serialize(serverDevInf);
                saveStringKey(CONF_KEY_SERVER_DEV_INF, devInf);
            } catch (Exception e) {
                Log.error(TAG_LOG, "Cannot serialize server dev inf", e);
                saveStringKey(CONF_KEY_SERVER_DEV_INF, "");
            }
        } else {
            saveStringKey(CONF_KEY_SERVER_DEV_INF, "");
        }

        saveIntKey(CONF_KEY_SERVER_TYPE, serverType);

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
        // If a temporary log level was set, we still report the real log level
        // and not the temporary one
        if (origLogLevel == -1) {
            return logLevel;
        } else {
            return origLogLevel;
        }
    }

    public void setLogLevel(int logLevel) {
        if(this.logLevel != logLevel) {
            dirtyMisc = true;
            this.logLevel = logLevel;
        }
    }

    public boolean getBandwidthSaverActivated() {
        return bandwidthSaverChecked;
    }

    public void setBandwidthSaver(boolean bandwidthSaverChecked) {
        if(this.bandwidthSaverChecked != bandwidthSaverChecked) {
            dirtyMisc = true;
            this.bandwidthSaverChecked = bandwidthSaverChecked;
        }
    }

    public boolean isC2SPushEnabled() {
        return c2sPushEnabled;
    }

    public void setC2SPushEnabled(boolean c2sPushEnabled) {
        this.c2sPushEnabled = c2sPushEnabled;
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

     public boolean getSignupAccountCreated() {
        return signupAccountCreated;
    }

    public void setSignupAccountCreated(boolean signupAccountCreated) {
        this.signupAccountCreated = signupAccountCreated;
    }

    /**
     * Gets the state of the application as far as the display of the first-sync
     * dialog is concerned.
     *
     * @return a value among FIRST_SYNC_* constants
     */
    public int getFirstSyncFromScratch() {
        return firstSyncFromScratch;
    }

    /**
     * Changes the configuration state in order to keep track of whether the
     * first-sync dialog has been already shown or has to be shown. In the
     * latter case, it discriminates also between the sign-up and the log-in
     * cases.
     * If the configuration state is already set to {@link #FIRST_SYNC_DONE},
     * it means that the first sync dialog has already been shown and the
     * setting will not be changed.
     *
     * @param firstSyncFromScratch either {@link #FIRST_SYNC_LOG_IN} or
     *                             {@link #FIRST_SYNC_SIGN_UP}
     */
    public void setFirstSyncFromScratch(int firstSyncFromScratch) {
        if (this.firstSyncFromScratch != FIRST_SYNC_DONE) {
            this.firstSyncFromScratch = firstSyncFromScratch;
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

    public void setFirstRunTimestamp(long value) {
        firstRunTimestamp = value;
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

    public long getActivationDate() {
        return activationDate;
    }

    public void setActivationDate(long expDate) {
        if(this.activationDate != expDate) {
            dirtyUpdater = true;
            activationDate = expDate;
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

    // Profile information section
    public long getProfileExpireDate() {
        return profileExpireDate;
    }

    public void setProfileExpireDate(long expireDate) {
        this.profileExpireDate = expireDate;
    }

    public boolean getProfileManualOnly() {
        return profileManualOnly;
    }

    public void setProfileManualOnly(boolean manualOnly) {
        this.profileManualOnly = manualOnly;
    }

    public boolean getProfileNetworkUsageWarning() {
        return profileNetworkUsageWarning;
    }

    public void setProfileNetworkUsageWarning(boolean value) {
        this.profileNetworkUsageWarning = value;
    }

    public int getServerType() {
        return serverType;
    }

    public void setServerType(int serverType) {
        this.serverType = serverType;
    }

    public SyncConfig getSyncConfig() {
        SyncConfig syncConfig = new SyncConfig();

        // TODO set before this runs
        syncConfig.syncUrl = getSyncUrl();
        syncConfig.userName = getUsername();
        syncConfig.password = getPassword();

        syncConfig.clientNonce = this.clientNonce;
        syncConfig.preferredAuthType = customization.getDefaultAuthType();

        if (proxyConfig != null) {
            syncConfig.setProxyConfig(proxyConfig);
        }
        
        // Remember to update the blackberry synclet pattern (server side) when changing the user agent
        syncConfig.userAgent = getUserAgent();
        syncConfig.forceCookies = false;

        return syncConfig;
    }


    public void setTempLogLevel(int tempLogLevel) {
        origLogLevel = logLevel;
        logLevel = tempLogLevel;
    }

    public void restoreLogLevel() {
        logLevel = origLogLevel;
        origLogLevel = -1;
    }

    public boolean getForceServerCapsRequest() {
        return forceServerCapsRequest;
    }

    public void setForceServerCapsRequest(boolean value) {
        forceServerCapsRequest = value;
    }

    public boolean getPimSourceSyncTypeChanged() {
        return pimSourceSyncTypeChanged;
    }

    public void setPimSourceSyncTypeChanged(boolean value) {
        pimSourceSyncTypeChanged = value;
    }

    public int getCurrentSyncRetryCount() {
        return currentSyncRetryCount;
    }

    public void setCurrentSyncRetryCount(int value) {
        currentSyncRetryCount = value;
    }

    public Runnable getPostConfigurationTask() {
        return postConfigurationTask;
    }

    public ProxyConfig getProxyConfig() {
        return proxyConfig;
    }

    public void setProxyConfig(ProxyConfig proxyConfig) {
        this.proxyConfig = proxyConfig;
    }

    public DevInf getServerDevInf() {
        return serverDevInf;
    }

    public void setServerDevInf(DevInf serverDevInf) {
        this.serverDevInf = serverDevInf;
    }

    protected void migrateConfig() {
        // We must migrate from "version" to CONFIG_VERSION
        try {
            /////////////////////////////////////////////////////////////////
            /////////////////////  Migrate from 5 to 6 //////////////////////
            /////////////////////////////////////////////////////////////////
            if ("5".equals(version)) {
                // Nothing to migrate in the general config
                version = "6";
            }

            /////////////////////////////////////////////////////////////////
            /////////////////////  Migrate from 6 to 7 //////////////////////
            /////////////////////////////////////////////////////////////////
            // Nothing to migrate in the general config
            if ("6".equals(version)) {
                version = "7";
            }

            /////////////////////////////////////////////////////////////////
            /////////////////////  Migrate from 7 to 8 //////////////////////
            /////////////////////////////////////////////////////////////////
            // Nothing to migrate in the general config
            if ("7".equals(version)) {
                version = "8";
            }

            /////////////////////////////////////////////////////////////////
            /////////////////////  Migrate from 8 to 9 //////////////////////
            /////////////////////////////////////////////////////////////////
            // In v9 we added videos sync. Ask for server caps to verify if the
            // source is supported
            if ("8".equals(version)) {
                setForceServerCapsRequest(true);
                version = "9";
            }

            /////////////////////////////////////////////////////////////////
            /////////////////////  Migrate from 9 to 10 //////////////////////
            /////////////////////////////////////////////////////////////////
            // In version 10 we introduced the signup account created field
            // If the user already logged in with an existing account, there is
            // no need to offer mobile signup.
            if ("9".equals(version)) {
                if(!credentialsCheckPending) {
                    setSignupAccountCreated(true);
                }
                version = "10";
            }

            /////////////////////////////////////////////////////////////////
            /////////////////////  Migrate from 10 to 11 ////////////////////
            /////////////////////////////////////////////////////////////////
            // Nothing to migrate in the general config
            if ("10".equals(version)) {
                version = "11";
            }

            /////////////////////////////////////////////////////////////////
            /////////////////////  Migrate from 11 to 12 ////////////////////
            /////////////////////////////////////////////////////////////////
            // Scheduling interval 5 and 10 mins got removed
            if ("11".equals(version)) {
                int minPollInterval = customization.getPollingPimIntervalChoices()[0];
                if (getPollingInterval() < minPollInterval) {
                    if (Log.isLoggable(Log.INFO)) {
                        Log.info(TAG_LOG, "Changing polling interval to new min value " + minPollInterval);
                    }
                    setPollingInterval(minPollInterval);
                }
                version = "12";
            }

            // Migration completed
            version = CONFIG_VERSION;
        } catch (Exception e) {
            Log.error(TAG_LOG, "Cannot migrate configuration", e);
        }
    }

    public abstract void    saveByteArrayKey(String key, byte[] value);
    public abstract byte[]  loadByteArrayKey(String key, byte[] defaultValue);
    public abstract boolean commit();
    public abstract DeviceConfig getDeviceConfig();

    protected abstract String  loadKey(String key);
    protected abstract void    saveKey(String key, String value);
    protected abstract String  getUserAgent();
}

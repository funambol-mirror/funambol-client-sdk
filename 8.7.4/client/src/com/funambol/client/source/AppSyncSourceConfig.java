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

package com.funambol.client.source;

import java.io.ByteArrayOutputStream;
import java.io.DataOutputStream;
import java.io.ByteArrayInputStream;
import java.io.DataInputStream;

import com.funambol.client.configuration.Configuration;
import com.funambol.client.customization.Customization;
import com.funambol.syncml.spds.SourceConfig;
import com.funambol.syncml.spds.SyncSource;
import com.funambol.syncml.spds.SyncListener;
import com.funambol.syncml.protocol.SyncML;

import com.funambol.util.Log;

/**
 * This class represents the configuration of an AppSyncSource. In particular it
 * holds all the values that are kept across application resets. These values
 * are loaded/saved using a Configuration object.
 */
public class AppSyncSourceConfig {

    private static final String TAG_LOG = "AppSyncSourceConfig";

    private static final String CONF_KEY_SYNC_URI               = "SYNC_SOURCE_URI";
    private static final String CONF_KEY_SYNC_TYPE              = "SYNC_TYPE";
    private static final String CONF_KEY_SOURCE_FULL            = "SYNC_SOURCE_FULL";
    private static final String CONF_KEY_SOURCE_SYNCED          = "SYNC_SOURCE_SYNCED";
    private static final String CONF_KEY_SOURCE_ACTIVE          = "SYNC_SOURCE_ACTIVE";
    private static final String CONF_KEY_BLOCK_INCOMING_INVITES = "BLOCK_INVITES";
    private static final String CONF_KEY_SYNC_STATUS            = "SOURCE_STATUS";
    private static final String CONF_KEY_FIRST_RUN_TIMESTAMP    = "FIRST_RUN_TIMESTAMP";
    private static final String CONF_KEY_SOURCE_CONFIG          = "SOURCE_CONFIG";
    private static final String CONF_KEY_UPLOAD_CONTENT_VIA_HTTP= "UPLOAD_CONTENT_VIA_HTTP";

    private String uri;
    private boolean enabled;
    private boolean active;
    protected AppSyncSource appSource;
    private int syncType = -1;
    private boolean deviceFullShown = false;
    private boolean sourceSynced;
    private int lastSyncStatus = SyncListener.SUCCESS;
    protected boolean dirty = false;
    protected boolean uploadContentViaHttp = false;

    protected Configuration configuration;
    private Customization customization;

    public AppSyncSourceConfig(AppSyncSource appSource, Customization customization, Configuration configuration) {
        this.appSource     = appSource;
        this.configuration = configuration;
        this.customization = customization;
        appSource.setConfig(this);
    }

    public boolean getEnabled() {
        return enabled;
    }

    public void setEnabled(boolean enabled) {
        this.enabled = enabled;
        dirty = true;
    }

    public boolean getActive() {
        return active;
    }

    public void setActive(boolean active) {
        this.active = active;
        dirty = true;
    }

    public int getLastSyncStatus() {
        return lastSyncStatus;
    }

    public void setLastSyncStatus(int lastSyncStatus) {
        this.lastSyncStatus = lastSyncStatus;
        dirty = true;
    }

    public String getUri() {
        return uri;
    }

    public void setUri(String uri) {
        this.uri = uri;
        dirty = true;
    }

    public boolean getDeviceFullShown() {
        return deviceFullShown;
    }

    public void setDeviceFullShown(boolean value) {
        this.deviceFullShown = value;
        dirty = true;
    }

    public void setSynced(boolean sourceSynced) {
        this.sourceSynced = sourceSynced;
        dirty = true;
    }

    public boolean getSynced() {
        return sourceSynced;
    }

    public int getSyncType() {
        // If this ss has its own specific sync mode then we use it,
        // otherwise we use the global one in the Configuration
        return syncType;
    }

    public void setSyncType(int syncType) {
        this.syncType = syncType;
        dirty = true;
    }

    public boolean getUploadContentViaHttp() {
        return uploadContentViaHttp;
    }

    public void setUploadContentViaHttp(boolean value) {
        Log.info(TAG_LOG, "Setting upload content via http to " + value);
        uploadContentViaHttp = value;
    }

    public void saveSourceSyncConfig() {
        int sourceId = appSource.getId();
        Log.debug(TAG_LOG, "Storing SourceConfig for " + appSource.getName());

        SyncSource source = appSource.getSyncSource();
        if (source != null) {
            SourceConfig config = source.getConfig();
            try {
                String storageKey = CONF_KEY_SOURCE_CONFIG + sourceId;
                ByteArrayOutputStream buff = new ByteArrayOutputStream(512);
                DataOutputStream temp = new DataOutputStream(buff);
                config.serialize(temp);
                configuration.saveByteArrayKey(storageKey, buff.toByteArray());
                temp.close();
            } catch (final Exception e) {
                Log.error(TAG_LOG, "Exception while storing SourceConfig [" + config.getName() + "] ", e);
            }
        }
    }

    /**
     * Saves a syncsource config to storage. Classes that extend the
     * AppSyncSourceConfig shall always save their custom data before invoking
     * this method, because the method before returning notifies the
     * configuration that a source has changed.
     */
    public synchronized void save() {
        int sourceId = appSource.getId();

        // Save the low level sync config
        saveSourceSyncConfig();

        if (!dirty) {
            return;
        }

        // Now save all the high level source parameters
        StringBuffer key = new StringBuffer();

        // Save the remote URI
        key = new StringBuffer();
        key.append(CONF_KEY_SYNC_URI).append("-").append(sourceId)
            .append("-").append("URI");
        configuration.saveStringKey(key.toString(), getUri());

        // Save the last sync status
        key = new StringBuffer();
        key.append(CONF_KEY_SYNC_STATUS).append("-").append(sourceId);
        configuration.saveIntKey(key.toString(), getLastSyncStatus());

        // Save the active flag
        key = new StringBuffer();
        key.append(CONF_KEY_SOURCE_ACTIVE).append("-").append(sourceId);
        configuration.saveBooleanKey(key.toString(), getActive());

        // Save the sync type
        key = new StringBuffer();
        key.append(CONF_KEY_SYNC_TYPE).append("-").append(sourceId);
        configuration.saveIntKey(key.toString(), getSyncType());

        // Save if the source showed device full already
        key = new StringBuffer();
        key.append(CONF_KEY_SOURCE_FULL).append("-").append(sourceId);
        configuration.saveBooleanKey(key.toString(), getDeviceFullShown());

        // Save if the source got synced at least once
        key = new StringBuffer();
        key.append(CONF_KEY_SOURCE_SYNCED).append("-").append(sourceId);
        configuration.saveBooleanKey(key.toString(), getSynced());

        // Save if content shall be sent via http
        key = new StringBuffer();
        key.append(CONF_KEY_UPLOAD_CONTENT_VIA_HTTP).append("-").append(sourceId);
        configuration.saveBooleanKey(key.toString(), getUploadContentViaHttp());

        // Clear the dirty flag
        dirty = false;

        configuration.notifySourceConfigChanged(appSource);
    }

    public void load(SourceConfig config) {
        int sourceId = appSource.getId();
        Log.debug(TAG_LOG, "Loading config for " + appSource.getName());

        // Load the source config
        if (config != null) {
            try {
                String storageKey = CONF_KEY_SOURCE_CONFIG + sourceId;
                byte[] byteArray = configuration.loadByteArrayKey(storageKey, null);
                if (byteArray != null) {
                    Log.debug(TAG_LOG, "Data Found");
                    DataInputStream temp = new DataInputStream(new ByteArrayInputStream(byteArray));
                    config.deserialize(temp);
                    temp.close();
                }
            } catch (final Exception e) {
                Log.error(TAG_LOG, "Exception while initializating (reading) of SourceConfig ["
                        + config.getName() + "] " + e.toString());
            }
        }

        // Now load all the high level source parameters
        StringBuffer key = new StringBuffer();

        // Load the last sync status
        key = new StringBuffer();
        key.append(CONF_KEY_SYNC_STATUS).append("-").append(sourceId);
        lastSyncStatus = configuration.loadIntKey(key.toString(), SyncListener.SUCCESS);

        // Load the sync type
        key = new StringBuffer();
        key.append(CONF_KEY_SYNC_TYPE).append("-").append(sourceId);
        syncType = configuration.loadIntKey(key.toString(),
                                            customization.getDefaultSourceSyncMode(sourceId));

        // Load the remote URI
        key = new StringBuffer();
        key.append(CONF_KEY_SYNC_URI).append("-").append(sourceId)
            .append("-").append("URI");
        uri = configuration.loadStringKey(key.toString(), customization.getDefaultSourceUri(sourceId));

        // Update the source config
        if (config != null) {
            config.setRemoteUri(uri);
            config.setSyncMode(syncType);
        }

        // Load the enable property
        enabled = syncType != SyncML.ALERT_CODE_NONE;

        // Load if the source is active
        key = new StringBuffer();
        key.append(CONF_KEY_SOURCE_ACTIVE).append("-").append(sourceId);
        active = configuration.loadBooleanKey(key.toString(), customization.isSourceActive(sourceId));

        // Load if the source showed device full warning already
        key = new StringBuffer();
        key.append(CONF_KEY_SOURCE_FULL).append("-").append(sourceId);
        deviceFullShown = configuration.loadBooleanKey(key.toString(), deviceFullShown);

        // Load if the source got synced at least once
        key = new StringBuffer();
        key.append(CONF_KEY_SOURCE_SYNCED).append("-").append(sourceId);
        sourceSynced = configuration.loadBooleanKey(key.toString(), sourceSynced);

        // Load if the source shall use http based upload
        key = new StringBuffer();
        key.append(CONF_KEY_UPLOAD_CONTENT_VIA_HTTP).append("-").append(sourceId);
        uploadContentViaHttp = configuration.loadBooleanKey(key.toString(), uploadContentViaHttp);
    }

    public void commit() {
        // We commit the whole configuration
        Log.trace(TAG_LOG, "Committing config for: " + appSource.getName());
        if (dirty) {
            save();
        }
        configuration.commit();
    }

    public boolean isDirty() {
        return dirty;
    }

    public void migrateFrom5To6() {
    }

    public void migrateFrom6To7() {
    }
}


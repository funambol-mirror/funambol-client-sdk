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

import java.util.Hashtable;

import com.funambol.storage.Table;
import com.funambol.client.controller.SourceThumbnailsViewController;
import com.funambol.client.controller.SynchronizationController;
import com.funambol.client.customization.Customization;
import com.funambol.client.ui.SettingsUISyncSource;
import com.funambol.client.ui.DevSettingsUISyncSource;
import com.funambol.client.ui.UISyncSource;
import com.funambol.client.ui.Screen;
import com.funambol.client.ui.view.SourceThumbnailsView;
import com.funambol.client.ui.view.ThumbnailView;
import com.funambol.sync.SyncSource;

import com.funambol.util.Log;

/**
 * This class represents a sync source in the application. In the application a
 * sync source is a pluggable component that allow users to deal with the
 * synchronization of a given type of data. An AppSyncSource abstracts the
 * following concepts:
 *
 * 1) SyncSource (at SyncML API level)
 * 2) Displayable name
 * 3) Icon (optional)
 * 4) Main configuration screen
 * 5) numerical ID (set by the application and optional)
 * 6) Persistable configuration (stored in AppSyncSourceConfig)
 * 7) Position in the UI
 * 8) UI representation in home and settings screens (the UI in the home screen
 *    can be different for a stand alone source, and a multi sources situation)
 * 9) UI controller
 *
 * An abstract sync source can be in different status, depending on various
 * things:
 *
 * 1) active if it is enabled in the customization and the server supports it
 * 2) enabled if the user enabled it in the settings (or if enabled by default)
 * 3) working if the client was able to set up all the bits and pieces to
 *    perform synchronizations
 *
 */
public class AppSyncSource {

    private static final String TAG_LOG = "AppSyncSource";

    // Possible AppSyncSource settings
    public static final int SYNC_MODE_SETTING   = 0;
    public static final int SYNC_FOLDER_SETTING = 1;

    protected SyncSource           source;
    private String                 name;
    private int                    id     = -1;
    private String                 enabledLabel     = null;
    private String                 disabledLabel    = null;
    private String                 iconName         = null;
    private String                 iconDisabledName = null;
    private ExternalAppManager     appManager       = null;
    private int                    uiSourceIndex    = -1;
    protected UISyncSource         uiSource         = null;
    private SourceThumbnailsViewController controller       = null;
    protected SettingsUISyncSource settingsUISource = null;
    protected DevSettingsUISyncSource devSettingsUISource = null;
    protected boolean              refreshFromServerSupported = true;
    protected boolean              refreshToServerSupported   = true;     
    private Object                 nativeListener   = null;
    private boolean                visible          = true;
    protected boolean              useBandwidthSaver  = false;
    protected AppSyncSourceConfig  config;
    private boolean                syncedInSession  = false;
    private boolean                isMedia          = false;

    protected Class                settingsClass = null;
    protected Class                devSettingsClass = null;
    protected Class                sourceThumbnailsViewControllerClass = null;
    protected Class                sourceThumbnailsViewClass = null;
    protected Class                thumbnailViewClass = null;

    // Lists all the settings with the possible values assiciated with this source
    private Hashtable settings = new Hashtable();
    
    public AppSyncSource(String name, SyncSource source) {
        this.name   = name;
        this.source = source;
        this.enabledLabel = "";
        this.disabledLabel = "";

    }

    public AppSyncSource(String name) {
        this(name, null);
    }

    public void setConfig(AppSyncSourceConfig config) {
        this.config = config;
    }

    public AppSyncSourceConfig getConfig() {
        return config;
    }

    public void setId(int id) {
        this.id = id;
    }

    public int getId() {
        return id;
    }

    /**
     * Convenience method to get if a source is enabled.
     * @deprecated Use AppSyncSourceConfig directly instead
     */
    public boolean isEnabled() {
        return config.getEnabled();
    }

    public void setBandwidthSaverUse(boolean activate){
        useBandwidthSaver = activate;
    }

    public boolean getBandwidthSaverUse(){
        return useBandwidthSaver;
    }

    public int getUiSourceIndex() {
        return uiSourceIndex;
    }
    
    public void setUiSourceIndex(int index) {
        this.uiSourceIndex=index;
    }
    
    public void setSyncSource(final SyncSource source) {
        this.source = source;
    }

    public void setAppManager(ExternalAppManager manager) {
        this.appManager = manager;
    }

    public ExternalAppManager getAppManager() {
        return appManager;
    }

    public SyncSource getSyncSource() {
        return source;
    }

    public String getName() {
        return name;
    }

    public String getEnabledLabel() {
        return enabledLabel;
    }

    public void setEnabledLabel(String enabledLabel) {
        this.enabledLabel = enabledLabel;
    }

    public String getDisabledLabel() {
        return disabledLabel;
    }

    public void setDisabledLabel(String disabledLabel) {
        this.disabledLabel = disabledLabel;
    }

    public boolean isWorking() {
        return source != null;
    }

    public String getIconName() {
        return iconName;
    }

    public void setIconName(String name) {
        iconName = name;
    }

    public void setDisabledIconName(String name) {
        iconDisabledName = name;
    }

    public String getDisabledIconName() {
        return iconDisabledName;
    }


    public UISyncSource getUISyncSource() {
        return uiSource;
    }

    public void setSourceThumbnailsViewClass(Class sourceThumbnailsViewClass) {
        this.sourceThumbnailsViewClass = sourceThumbnailsViewClass;
    }

    public void setThumbnailViewClass(Class thumbnailViewClass) {
        this.thumbnailViewClass = thumbnailViewClass;
    }

    public void setSourceThumbnailsViewControllerClass(Class sourceThumbnailsViewControllerClass) {
        this.sourceThumbnailsViewControllerClass = sourceThumbnailsViewControllerClass;
    }

    public SourceThumbnailsViewController getSourceThumbnailsViewController() {
        return controller;
    }

    public void setSourceThumbnailsViewController(SourceThumbnailsViewController controller) {
        this.controller = controller;
    }

    public Class getSettingsUIClass() {
        return settingsClass;
    }

    public void setSettingsUIClass(Class settingsClass) {
        this.settingsClass = settingsClass;
    }

    public Class getDevSettingsUIClass() {
        return devSettingsClass;
    }

    public void setDevSettingsUIClass(Class devSettingsClass) {
        this.devSettingsClass = devSettingsClass;
    }

    public SettingsUISyncSource getSettingsUISyncSource() {
        return settingsUISource;
    }

    public DevSettingsUISyncSource getDevSettingsUISyncSource() {
        return devSettingsUISource;
    }

    public boolean hasSettings() {
        return settings.size() > 0;
    }

    public boolean hasSetting(int setting) {
        return settings.containsKey(new Integer(setting));
    }

    public Hashtable getSettings() {
        return settings;
    }

    public void setHasSetting(int setting, boolean value, Object possibleValues) {
        if(value) {
            settings.put(new Integer(setting), possibleValues);
        } else {
            settings.remove(new Integer(setting));
        }
    }

    public int prepareRefresh(int direction) {
        int syncMode = 0;
        switch (direction) {
            case SynchronizationController.REFRESH_FROM_SERVER:
                syncMode = SyncSource.FULL_DOWNLOAD;
                break;
            case SynchronizationController.REFRESH_TO_SERVER:
                syncMode = SyncSource.FULL_UPLOAD;
                break;
            default:
                throw new IllegalArgumentException("Invalid refresh direction " + direction);
        }
        return syncMode;
    }

    public boolean isRefreshSupported(int direction) {
        boolean supported;
        switch (direction) {
            case SynchronizationController.REFRESH_FROM_SERVER:
                supported = refreshFromServerSupported;
                break;
            case SynchronizationController.REFRESH_TO_SERVER:
                supported = refreshToServerSupported;
                break;
            default:
                throw new IllegalArgumentException("Invalid refresh direction " + direction);
        }
        return supported;
    }

    public void setIsRefreshSupported(int direction, boolean value) {
        if (direction == SynchronizationController.REFRESH_FROM_SERVER) {
            refreshFromServerSupported = value;
        } else if (direction == SynchronizationController.REFRESH_TO_SERVER) {
            refreshToServerSupported = value;
        } else {
            throw new IllegalArgumentException("Invalid refresh direction " + direction);
        }
    }

    public void setIsRefreshSupported(boolean value) {
        refreshFromServerSupported = value;
        refreshToServerSupported   = value;
    }

    public Object getNativeListener() {
        return nativeListener;
    }

    public void setNativeListener(Object nativeListener) {
        this.nativeListener = nativeListener;
    }

    public boolean isVisible() {
        return visible;
    }

    public void setIsVisible(boolean value) {
        visible = value;
    }

    /**
     * Returns true if this source was synced in this session. This is a
     * volatile property and it is reset across application restarts.
     * AppSyncSourceConfig has a similar property that lives across restarts
     */
    public boolean getSyncedInSession() {
        return syncedInSession;
    }

    public void setSyncedInSession(boolean value) {
        syncedInSession = value;
    }

    /**
     * Returns true if this source is for media content
     */
    public boolean getIsMedia() {
        return isMedia;
    }

    public void setIsMedia(boolean value) {
        isMedia = value;
    }


    /**
     * This method is invoked when there is a change in the configuration that
     * impacts the source config. By default the method does not do anything,
     * but the class can be derived to force specific behaviors
     */
    public void reapplyConfiguration() {
    }

    public SettingsUISyncSource createSettingsUISyncSource(Screen screen) {
        if (Log.isLoggable(Log.TRACE)) {
            Log.trace(TAG_LOG, "Creating settings UI for source: " + getName());
        }
        if (settingsClass != null) {
            try {
                settingsUISource = (SettingsUISyncSource) settingsClass.newInstance();
            } catch (Exception e) {
                Log.error(TAG_LOG, "Cannot instantiate settings class", e);
            }
        } else {
            Log.error(TAG_LOG, "Cannot create settings instance");
        }

        if (settingsUISource == null) {
            throw new IllegalStateException("Cannot create UI settings");
        }
        return settingsUISource;
    }

    /**
     * Factory method to create dev settings UI component. Dev Settings are
     * optional for each source, so this method may return null if a given
     * source has no advanced settings associated.
     */
    public DevSettingsUISyncSource createDevSettingsUISyncSource(Screen screen) {
        if (Log.isLoggable(Log.TRACE)) {
            Log.trace(TAG_LOG, "Creating dev settings UI for source: " + getName());
        }
        if (devSettingsClass != null) {
            try {
                devSettingsUISource = (DevSettingsUISyncSource) devSettingsClass.newInstance();
            } catch (Exception e) {
                Log.error(TAG_LOG, "Cannot instantiate dev settings class", e);
            }
        } else {
            Log.info(TAG_LOG, "No dev settings for source " + name);
        }

        return devSettingsUISource;
    }

    public SourceThumbnailsView createSourceThumbnailsView(Screen screen) {
        SourceThumbnailsView tv = null;

        if (sourceThumbnailsViewClass != null) {
            try {
                tv = (SourceThumbnailsView)sourceThumbnailsViewClass.newInstance();
            } catch (Exception e) {
                Log.error(TAG_LOG, "Cannot instantiate thumbnails class");
            }
        }
        if (tv == null) {
            throw new IllegalStateException("Cannot create representation for source");
        }
        return tv;
    }

    public ThumbnailView createThumbnailView(Screen screen) {
        ThumbnailView tv = null;

        if (thumbnailViewClass != null) {
            try {
                tv = (ThumbnailView)thumbnailViewClass.newInstance();
            } catch (Exception e) {
                Log.error(TAG_LOG, "Cannot instantiate thumbnail view class");
            }
        }
        if (tv == null) {
            throw new IllegalStateException("Cannot create representation for source");
        }
        return tv;
    }

    /**
     * Creates a controller for the main thumbnails view
     */
    public SourceThumbnailsViewController createSourceThumbnailsViewController(
            Customization customization) {
        SourceThumbnailsViewController sourceThumbnailsViewController = null;
        if (sourceThumbnailsViewControllerClass != null) {
            try {
                sourceThumbnailsViewController = (SourceThumbnailsViewController)
                        sourceThumbnailsViewControllerClass.newInstance();
            } catch (Exception e) {
                Log.error(TAG_LOG, "Cannot instantiate button class, revert to default one");
            }
        }
        if (sourceThumbnailsViewController == null) {
            sourceThumbnailsViewController =
                    new SourceThumbnailsViewController(this, customization);
        }
        return sourceThumbnailsViewController;
    }

    /**
     * This method returns the table where metadata is stored for this source.
     * This method must be re-defined if a source provides its data in a table.
     */
    public Table getMetadataTable() {
        return null;
    }

}

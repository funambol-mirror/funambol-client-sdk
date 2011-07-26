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

import java.util.Enumeration;

import com.funambol.client.configuration.Configuration;
import com.funambol.client.customization.Customization;
import com.funambol.client.source.AppSyncSource;
import com.funambol.client.source.AppSyncSourceManager;
import com.funambol.client.ui.DisplayManager;
import com.funambol.client.ui.Screen;
import com.funambol.syncml.protocol.DevInf;
import com.funambol.util.Log;
import com.funambol.client.localization.Localization;

public class Controller extends BasicController {

    private static final String TAG_LOG = "Controller";

    public static final int HOME_SCREEN_ID = 0;
    public static final int CONFIGURATION_SCREEN_ID = 1;
    public static final int LOGIN_SCREEN_ID = 2;
    public static final int SIGNUP_SCREEN_ID = 3;
    public static final int ACCOUNT_SCREEN_ID = 4;
    public static final int ABOUT_SCREEN_ID = 5;
    public static final int ADVANCED_SETTINGS_SCREEN_ID = 6;
    public static final int DEV_SETTINGS_SCREEN_ID = 7;
    public static final int SOURCES_SELECTOR_SCREEN_ID = 8;

    protected DisplayManager displayManager = null;

    protected HomeScreenController             homeScreenController;
    protected SyncSettingsScreenController     syncSettingsScreenController;
    protected DevSettingsScreenController      devSettingsScreenController;
    protected AccountScreenController          loginScreenController;
    protected SignupScreenController           signupScreenController;
    protected AdvancedSettingsScreenController advancedSettingsScreenController;
    protected AboutScreenController            aboutScreenController;
    protected DialogController                 dialogController;
    private SourcesSelectorScreenController    sourcesSelectorScreenController;
    
    protected Customization customization = null;

    protected Configuration configuration = null;

    protected Localization  localization  = null;

    protected AppSyncSourceManager appSyncSourceManager = null;

    protected SyncModeHandler syncModeHandler;



    // Constructor
    public Controller(ControllerDataFactory fact, Configuration configuration,
                      Customization customization, Localization localization,
                      AppSyncSourceManager appSyncSourceManager)
    {
        this.configuration = configuration;
        this.customization = customization;
        this.localization  = localization;
        this.appSyncSourceManager = appSyncSourceManager;
        this.syncModeHandler = new SyncModeHandler(configuration);

        fact.setController(this);

        this.displayManager = fact.getDisplayManager();

        dialogController = new DialogController(displayManager, this);
        notificationController = new NotificationController(displayManager, this);
    }

    /**
     * This constructor is here ONLY for backward compatibility. Do not use it
     * unless you know what you are doing.
     */
    public Controller() {
    }

    public void setHomeScreenController(HomeScreenController homeScreenController) {
        this.homeScreenController = homeScreenController;
    }

    public HomeScreenController getHomeScreenController() {
         return homeScreenController;
    }

    public void setLoginScreenController(
            AccountScreenController loginScreenController) {
        this.loginScreenController = loginScreenController;
    }

    public AccountScreenController getLoginScreenController() {
         return loginScreenController;
    }

    public void setSignupScreenController(
            SignupScreenController signupScreenController) {
        this.signupScreenController = signupScreenController;
    }

    public SignupScreenController getSignupScreenController() {
         return signupScreenController;
    }

    public void setSyncSettingsScreenController(
            SyncSettingsScreenController syncSettingsScreenController) {
        this.syncSettingsScreenController = syncSettingsScreenController;
    }

    public SyncSettingsScreenController getSyncSettingsScreenController() {
        return syncSettingsScreenController;
    }

    public void setDevSettingsScreenController(
            DevSettingsScreenController devSettingsScreenController) {
        this.devSettingsScreenController = devSettingsScreenController;
    }

    public DevSettingsScreenController getDevSettingsScreenController() {
        return devSettingsScreenController;
    }

    public void setAdvancedSettingsScreenController(
            AdvancedSettingsScreenController advancedSettingsScreenController) {
        this.advancedSettingsScreenController = advancedSettingsScreenController;
    }

    public AdvancedSettingsScreenController getAdvancedSettingsScreenController() {
        return advancedSettingsScreenController;
    }

    public void setAboutScreenController(AboutScreenController aboutScreenController) {
        this.aboutScreenController = aboutScreenController;
    }

    public AboutScreenController getAboutScreenController() {
        return aboutScreenController;
    }
    
    public SourcesSelectorScreenController getSourcesSelectorScreenController() {
        return sourcesSelectorScreenController;
    }

    /**
     * @return the DialogController;
     */
    public DialogController getDialogController() {
        return dialogController;
    }

    public DisplayManager getDisplayManager() {
        return displayManager;
    }

    public void showScreen(Screen screen, int screenId) {
        try {
            getDisplayManager().showScreen(screen, screenId);
        } catch (Exception e) {
            Log.error(TAG_LOG, "Cannot show screen: " + screenId, e);
        }
    }

    public void hideScreen(Screen screen) {
        try {
            getDisplayManager().hideScreen(screen);
        } catch (Exception e) {
            Log.error(TAG_LOG, "Cannot hide screen", e);
        }
    }

    // Misc interface functions

    public void toForeground() {
        displayManager.toForeground();
    }

    public void toBackground() {
        displayManager.toBackground();
    }

    public boolean isUpdate() {
        // TODO FIXME: MARCO
        //return client.getUpdater().isUpdate();
        return false;
    }

    public void reapplyConfiguration() {
        reapplyAccountConfiguration();
        reapplyUpdaterConfiguration();
        reapplySyncModeConfiguration();
        reapplyMiscConfiguration();
    }


    public void reapplyAccountConfiguration() {
        // The account has changed, we shall propagate this information
        // to the sync source because they might want to know (for example the
        // photo source has the cache file whose name depends on the
        // credentials)
        Enumeration appSources = appSyncSourceManager.getWorkingSources();
        while(appSources.hasMoreElements()) {
            AppSyncSource appSource = (AppSyncSource)appSources.nextElement();
            appSource.reapplyConfiguration();
        }

        // Re-set the Updater url if enabled
        if(customization.enableUpdaterManager()) {
            // TODO FIXME MARCO
            //String server = StringUtil.extractAddressFromUrl(configuration.getSyncUrl());
            //UpdaterManager.getInstance().setUrl(server);
        }
    }

    public void reapplyUpdaterConfiguration() {
    }

    public void reapplySourceConfiguration(AppSyncSource appSource) {
        if (Log.isLoggable(Log.TRACE)) {
            Log.trace(TAG_LOG, "Reapplying configuration for source " + appSource.getName());
        }
        // Update the list of enabled sources
        if(homeScreenController != null) {
            homeScreenController.updateEnabledSources();
        }
        appSource.reapplyConfiguration();
    }

    public void reapplySyncModeConfiguration() {
        // Handle the current selected SyncMode
        syncModeHandler.setSyncMode(this);

        // Note: C2S push does not need to be activated/deactivated. It always
        // listens to system changes, but it reads the config to know if syncs
        // shall be triggered
    }

    public void reapplyMiscConfiguration() {

        if (Log.isLoggable(Log.DEBUG)) {
            Log.debug(TAG_LOG, "Reapply misc configuration");
        }

        // Re-set the log level
        Log.setLogLevel(configuration.getLogLevel());

        // Block incoming invites for events
        // TODO FIXME: MARCO
        //client.setEnableBlockInvites(configuration.getBlockIncomingInvites());
    }

    public void reapplyServerCaps(DevInf devInf) {

        if (Log.isLoggable(Log.DEBUG)) {
            Log.debug(TAG_LOG, "Reapply Server Caps: " + devInf);
        }

        /*
         * TODO: FIXME The datastores shall be applied only while syncing
         * against a funambol server. It is not required that the server must
         * specify all the sources caps in the config sync
         
        // Get the list of data stores
        Vector dataStores;
        if (devInf != null) {
            dataStores = devInf.getDataStores();
        } else {
            dataStores = new Vector();
        }

        // We have two sets of sources to compare. One is the set of sources
        // supported by this client, the other is the set of sources supported
        // by the server. We shall find a match for each source and
        // disable/configure everything which is supported
        Enumeration appSources = appSyncSourceManager.getRegisteredSources();

        while(appSources.hasMoreElements()) {
            
            AppSyncSource appSource = (AppSyncSource)appSources.nextElement();
            AppSyncSourceConfig config = appSource.getConfig();

            // Search for a match for this source

            // Media sources shall be activated by default
            if (appSource.getIsMedia()) {
                config.setActive(true);
                continue;
            }

            // Search a corresponding source in the list of sources supported by
            // the server
            String  sourceUri   = null;
            for(int i=0;i<dataStores.size();++i) {
                DataStore ds = (DataStore)dataStores.elementAt(i);
                if (Log.isLoggable(Log.TRACE)) {
                    Log.trace(TAG_LOG, "Found source with ref: " + ds.getSourceRef().getValue());
                }
                String defaultUri = customization.getDefaultSourceUri(appSource.getId());
                if (defaultUri.equals(ds.getSourceRef().getValue())) {
                    sourceUri = ds.getSourceRef().getValue();
                    break;
                }
            }

            if (Log.isLoggable(Log.TRACE)) {
                Log.trace(TAG_LOG, "Found source uri = " + sourceUri);
            }

            if (sourceUri != null) {
                // Found it!, enable the sync source (if it works)
                config.setUri(sourceUri);
                try {
                    if(customization.isSourceActive(appSource.getId())) {
                        if (Log.isLoggable(Log.INFO)) {
                            Log.info(TAG_LOG, "Activating source " + appSource.getName() + "," + appSource.getId());
                        }
                        config.setActive(true);

                        // Depending on the version of the server, we pick the
                        // best upload strategy
                        Vector exts = devInf.getExts();
                        boolean httpUpload = false;
                        if (exts != null) {
                            for(int j=0;j<exts.size();++j) {
                                Ext ext = (Ext)exts.elementAt(j);
                                if (Ext.X_FUNAMBOL_MEDIA_UPLOAD_HTTP.equals(ext.getXNam())) {
                                    httpUpload = true;
                                    break;
                                }
                            }
                        }
                        appSource.getConfig().setUploadContentViaHttp(httpUpload);
                    }
                } catch (Exception e) {
                    Log.error(TAG_LOG, "Source " + appSource.getName() + " not working, keep it disabled", e);
                }
            } else {
                // Disable the sync source
                if (Log.isLoggable(Log.INFO)) {
                    Log.info(TAG_LOG, "Deactivating source " + appSource.getName() + "," + appSource.getId());
                }
                config.setActive(false);
            }
        } */
        // Update the home screen
        if(homeScreenController != null) {
            homeScreenController.updateAvailableSources();
        }
        
        // Update the configuration screen
        if (syncSettingsScreenController != null) {
            syncSettingsScreenController.updateListOfSources();
        }

        configuration.setForceServerCapsRequest(false);
        configuration.setServerDevInf(devInf);
    }


    /**
     * Called when user selected Download Update from the menu
     */
    public void promptUpdate() {
        // TODO FIXME MARCO: TO BE DONE
        //if (UpdaterManager.getInstance().isUpdateAvailable()) {
        //}
    }

    /**
     * This method checks if a source must be displayed or not.
     */
    public boolean isVisible(AppSyncSource appSource) {
        if (appSource == null) {
            return false;
        }

        boolean visible = (appSource.isWorking() && appSource.getConfig().getActive());
        visible = visible && appSource.isVisible();
        visible = visible || customization.showNonWorkingSources();
        return visible;
    }

    public int computeNumberOfVisibleSources() {
        Enumeration sources = appSyncSourceManager.getRegisteredSources();
        int count = 0;
        while (sources.hasMoreElements()) {
            AppSyncSource appSource = (AppSyncSource)sources.nextElement();
            if (isVisible(appSource)) {
                count++;
            }
        }
        return count;
    }


    public Configuration getConfiguration() {
        return configuration;
    }

    public Customization getCustomization() {
        return customization;
    }

    public Localization getLocalization() {
        return localization;
    }

    public AppSyncSourceManager getAppSyncSourceManager() {
        return appSyncSourceManager;
    }

    public boolean shallLoginScreenBeRun() {
        // If needed, bring up the account screen to log in
        return configuration.getCredentialsCheckPending();
    }

    public void setSourcesSelectorScreenController(
            SourcesSelectorScreenController sourcesSelectorScreenController) {
        this.sourcesSelectorScreenController = sourcesSelectorScreenController;
        
    }
}

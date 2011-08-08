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

import com.funambol.client.source.AppSyncSource;
import com.funambol.client.source.AppSyncSourceConfig;
import com.funambol.client.source.AppSyncSourceManager;
import com.funambol.client.ui.DisplayManager;
import com.funambol.client.ui.SourcesSelectorScreen;
import com.funambol.client.customization.Customization;
import com.funambol.client.localization.Localization;
import com.funambol.util.Log;

/**
 */
public class SourcesSelectorScreenController {

    private static final String TAG_LOG = "SourcesSelectorScreenController";

    private SourcesSelectorScreen sourcesSelectorScreen;
    private Controller            controller;

    public SourcesSelectorScreenController(
            Controller controller, SourcesSelectorScreen sourcesSelectorScreen) {
        
        this.controller = controller;
        controller.setSourcesSelectorScreenController(this);

        initScreen(sourcesSelectorScreen);
    }
    
    public void initScreen(SourcesSelectorScreen sourcesSelectorScreen) {

        this.sourcesSelectorScreen = sourcesSelectorScreen;
        
        Localization localization = controller.getLocalization();

        sourcesSelectorScreen.setMessage(localization.getLanguage("what_to_sync_message"));
        sourcesSelectorScreen.setWarning(localization.getLanguage("what_to_sync_warning"));
        sourcesSelectorScreen.setButtonLabel(localization.getLanguage("what_to_sync_button"));
       
        if (Log.isLoggable(Log.TRACE)) {
            Log.trace(TAG_LOG, "Refreshing selectable sync sources");
        }
        
        Customization customization = controller.getCustomization();
        
        sourcesSelectorScreen.clearSources();        
        int[] sourcesOrder = customization.getSourcesOrder();
        AppSyncSourceManager appSyncSourceManager = controller.getAppSyncSourceManager();
        for(int i=0;i<sourcesOrder.length;++i) {
            AppSyncSource appSource = appSyncSourceManager.getSource(sourcesOrder[i]);
            if (appSource.getConfig().getActive()) {
                sourcesSelectorScreen.addSource(appSource,
                                                customization.getSourcePlaceHolderIcon(appSource.getId()),
                                                appSource.getConfig().getEnabled());
            }
        }
    }
    
    public void select(AppSyncSource appSource, boolean state) {
        if (Log.isLoggable(Log.INFO)) {
            Log.info(TAG_LOG, (state ? "Enabling" : "Disabling") +
                    " sync source " + appSource.getName());
        }
        appSource.getConfig().setEnabled(state);
        appSource.getConfig().save();
    }

    public void close() {
        // Show the sources selector screen
        try {
            DisplayManager dm = controller.getDisplayManager();
            dm.showScreen(sourcesSelectorScreen, Controller.HOME_SCREEN_ID);
        } catch (Exception e) {
            Log.error(TAG_LOG, "Unable to show home screen",e);
        }
        sourcesSelectorScreen.close();
    }

    public SourcesSelectorScreen getSourcesSelectorScreen() {
        return sourcesSelectorScreen;
    }
}



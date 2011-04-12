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
import com.funambol.sync.SyncListener;
import com.funambol.util.ConnectionListener;
import com.funambol.util.Log;

/**
 * This interface includes all basic functions of a SynchronizationController
 * implementation that are currently shared between Android and BlackBerry
 * versions of SynchronizationController.
 */
public abstract class BasicSynchronizationController
        implements ConnectionListener {

    private static final String TAG_LOG = "SynchronizationController"; // sic

    public static final String MANUAL    = "manual";
    public static final String SCHEDULED = "scheduled";
    public static final String PUSH      = "push";

    protected String syncType        = null;

    private Vector localStorageFullSources = new Vector();
    private Vector serverQuotaFullSources = new Vector();

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

    protected abstract BasicController getBasicController();
}

/*
 * Funambol is a mobile platform developed by Funambol, Inc.
 * Copyright (C) 2003 - 2007 Funambol, Inc.
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
package com.funambol.updater;

import java.util.Date;
import com.funambol.util.TransportAgent;
import com.funambol.util.HttpTransportAgent;
import com.funambol.util.Log;

public class Updater {

    private UpdaterConfig config;
    private String currentVersion;
    private String component;
    private TransportAgent userTA = null;
    private UpdaterListener listener  = null;


    public Updater(UpdaterConfig config, String currentVersion, String component) {
        this.config = config;
        this.currentVersion = currentVersion;
        this.component = component;
    }

    public Updater(UpdaterConfig config, String currentVersion,
                   TransportAgent userTA)
    {
        this.config = config;
        this.currentVersion = currentVersion;
        this.userTA = userTA;
    }

    public void setListener(UpdaterListener listener) {
        this.listener = listener;
    }

    /**
     */
    public void check() {

        // TODO: check if network is available in a platform neutral way

        // Check if updates are available from server
        checkUpdateFromServer();

        if (updateIsReportable() && isNewVersionAvailable()) {
            Log.info("[Updater] - available update");

            if (listener != null) {

                if (config.isMandatory()) {
                    Log.info("[Updater] - Mandatory update is available");
                    listener.mandatoryUpdateAvailable(config.getAvailableVersion());
                } else {
                    if (!config.isOptional()) {
                        Log.error("[Updater] - Unknwon update type, assume it is optional");
                    }
                    Log.info("[Updater] - Optional update is available");
                    listener.optionalUpdateAvailable(config.getAvailableVersion());
                }
            }
        }
    }

    private boolean isNewVersionAvailable() {

        final String VER_SEP = ".";
        boolean possibleUpdate = false;

        String version = currentVersion;

        Log.info("[Updater] - current version : " + currentVersion);
        String fversion = config.getAvailableVersion();
        if (fversion == null || " ".equals(fversion)) {
            fversion = currentVersion;
        }
        Log.info("[Updater] - available version : " + fversion);

        int vpos = 0;
        int vfpos = 0;
        do {
            vpos = version.indexOf(VER_SEP, 0);
            vfpos = fversion.indexOf(VER_SEP, 0);
            if (vpos < 0 && version.length() > 0) {
                vpos = version.length();
            }
            if (vfpos < 0 && fversion.length() > 0) {
                vfpos = fversion.length();
            }

            if (vpos > 0 && vfpos > 0) {
                int val = Integer.parseInt(version.substring(0, vpos));
                int fval = Integer.parseInt(fversion.substring(0, vfpos));
                if (val < fval) {
                    Log.debug("[Updater] - Current version is old");
                    possibleUpdate = true;
                     break;
                } else if (val > fval) {
                    Log.debug("[Updater] - Current version isn't old");
                    break;
                }
                if (vpos < version.length()) {
                    version = version.substring(vpos + 1);
                } else {
                    vpos = -1;
                }
                if (vfpos < fversion.length()) {
                    fversion = fversion.substring(vfpos + 1);
                } else {
                    vfpos = -1;
                }
            }
        } while (vpos > 0 && vfpos > 0);

        return possibleUpdate;
    }

    private String getValueTag(String string, String tag) {
        int index = string.indexOf(tag);
        if (index < 0) {
            return null;
        }
        String tmp = string.substring(index);
        int end = tmp.indexOf("\n");
        if (end < 0) {
            end = tmp.indexOf("\r");
            if (end < 0) {
                return null;
            }
        }

        String value = tmp.substring(tag.length(), end);
        return value;
    }

    private boolean isTimeToRefresh() {
        long now = System.currentTimeMillis();
        boolean refresh = false;

        long lastCheck = config.getLastCheck();
        Log.info("[Updater] - isTimeToRefresh - Now Date is: " + new Date(now) +
                 " Last Check Date was " + new Date(lastCheck));
        if (((now - lastCheck) >= config.getCheckInterval())) {
            Log.info("[Updater] - isTimeToRefresh - Update info need to be refreshed. " +
                     "Last Check was " + new Date(lastCheck));
            refresh = true;
            config.save();
        }
        return refresh;
    }

    private void checkUpdateFromServer() {
        final String VERSION_TAG = "version=";
        final String URL_TAG = "url=";
        final String TYPE_TAG = "type=";
        final String BEGIN_TAG = "swup_begin";
        final String END_TAG = "swup_end";
        final String NL = "\n";

        if (isTimeToRefresh()) {

            StringBuffer urlParams = new StringBuffer();
            // The url may need to be made flexible, we can add a property
            // beside the server address
            urlParams.append("/updateserver/update?component=").append(component);
            urlParams.append("&version=" + currentVersion);
            
            String url = config.getUrl() + urlParams.toString();

            Log.info("[Updater] - checkUpdateFromServer - update url: " + url);

            TransportAgent ta;
            if (userTA != null) {
                ta = userTA;
            } else {
                ta = new HttpTransportAgent(url, false, false);
            }

            try {
                String updateProperties = ta.sendMessage("");
                String version = getValueTag(updateProperties, VERSION_TAG);
                if (version != null) {
                    if (!version.equals(config.getAvailableVersion())) {
                        // There is a new version on the server, even if the
                        // user decided to skip the current version, we must
                        // inform about the new one. Superseed user decision
                        // in this case and change the config
                        config.setSkip(false);
                    }
                    config.setAvailableVersion(version);
                }
                String type = getValueTag(updateProperties, TYPE_TAG);
                if (type != null) {
                    config.setType(type);
                }
                String downloadUrl = getValueTag(updateProperties, URL_TAG);
                if (downloadUrl != null) {
                    config.setDownloadUrl(downloadUrl);
                }
                config.setLastCheck(System.currentTimeMillis());
                config.save();

                Log.info("[Updater] - availableVersion :" + config.getAvailableVersion());
                Log.info("[Updater] - updateType :" + config.getType());
                Log.info("[Updater] - updateURL :" + config.getUrl());
                Log.info("[Updater] - lastUpdateCheck :" + config.getLastCheck());
            } catch (Throwable t) {
                Log.error("[Updater] - checkUpdateFromServer - " + t.toString());
            }
        } else {
            Log.info("[Updater] - No refresh update info from server needs");
        }
    }

    private boolean updateIsReportable() {
        if (config.getSkip()) {
            return false;
        }

        long now = System.currentTimeMillis();
        Log.info("[Updater] - now: " + new Date(now));
        Date next = new Date(config.getLastReminder() + config.getReminderInterval());
        Log.info("[Updater] - next update remind: " + next);
        if ((config.getLastReminder() + config.getReminderInterval()) > now) {
            return false;
        }

        return true;
    }

    public void setLastReminder(long time) {
        config.setLastReminder(time);
        config.save();
    }

    public void setSkip() {
        config.setSkip(true);
        config.save();
    }

    public boolean isUpdateAvailable() {
        String availableVersion = config.getAvailableVersion();
        return (availableVersion != null && isNewVersionAvailable());
    }
}

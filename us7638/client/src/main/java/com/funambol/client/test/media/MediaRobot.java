/*
 * Funambol is a mobile platform developed by Funambol, Inc. 
 * Copyright (C) 2010 Funambol, Inc.
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

package com.funambol.client.test.media;

import com.funambol.client.source.AppSyncSource;
import com.funambol.client.source.AppSyncSourceManager;
import com.funambol.client.test.BasicScriptRunner;
import com.funambol.client.test.Robot;
import com.funambol.client.test.util.CheckSyncClient;
import com.funambol.platform.HttpConnectionAdapter;
import com.funambol.sapisync.source.JSONSyncSource;
import com.funambol.sync.SyncSource;
import com.funambol.util.ConnectionManager;

import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;


public abstract class MediaRobot extends Robot {
   
    private static final String TAG_LOG = "MediaRobot";

    protected AppSyncSourceManager appSourceManager;

    public MediaRobot(AppSyncSourceManager appSourceManager) {
        this.appSourceManager = appSourceManager;
    }

    public MediaRobot() {
    }

    protected AppSyncSourceManager getAppSyncSourceManager() {
        return appSourceManager;
    }
    
    protected AppSyncSource getAppSyncSource(String type) {
        if(CheckSyncClient.SOURCE_NAME_PICTURES.equals(type)) {
            return getAppSyncSourceManager().getSource(AppSyncSourceManager.PICTURES_ID);
        } else if(CheckSyncClient.SOURCE_NAME_VIDEOS.equals(type)) {
            return getAppSyncSourceManager().getSource(AppSyncSourceManager.VIDEOS_ID);
        } else {
            throw new IllegalArgumentException("Invalid type: " + type);
        }
    }
    
    public void addMedia(String type, String filename) throws Throwable {

        SyncSource source = getAppSyncSource(type).getSyncSource();
        
        assertTrue(source instanceof JSONSyncSource, "Sync source format not supported");

        downloadMediaFile(filename, ((JSONSyncSource)source).getDownloadOutputStream(
                getAppSyncSource(type).getName(), 0, false, false));
    }
    
    public abstract void addMediaOnServer(String type, String filename) throws Throwable;

    public abstract void deleteMedia(String type, String filename) throws Throwable;
    public abstract void deleteMediaOnServer(String type, String filename) throws Throwable;

    public abstract void deleteAllMedia(String type) throws Throwable;
    public abstract void deleteAllMediaOnServer(String type) throws Throwable;

    /**
     * Downloads a media file to the given output stream
     * @param filename
     * @param output
     * @throws Throwable
     */
    protected void downloadMediaFile(String filename, OutputStream output) throws Throwable {
        String baseUrl = BasicScriptRunner.getBaseUrl();
        String url = baseUrl + "/" + filename;
        HttpConnectionAdapter conn = null;
        InputStream is = null;
        OutputStream os = null;
        try {
            conn = ConnectionManager.getInstance().openHttpConnection(url, null);
            conn.setRequestMethod(HttpConnectionAdapter.GET);
            os = conn.openOutputStream();
            os.flush();
            if (conn.getResponseCode() == HttpConnectionAdapter.HTTP_OK) {
                is = conn.openInputStream();
                int b;
                do {
                    b = is.read();
                    if (b != -1) {
                        output.write(b);
                    }
                } while(b != -1);
                output.flush();
                output.close();
            }
        } finally {
            if (os != null) {
                try {
                    os.close();
                } catch (IOException e) {}
                os = null;
            }
            if (is != null) {
                try {
                    is.close();
                } catch (IOException e) {}
                is = null;
            }
            if (conn != null) {
                try {
                    conn.close();
                } catch (IOException ioe) {}
                conn = null;
            }
        }
    }
    
}

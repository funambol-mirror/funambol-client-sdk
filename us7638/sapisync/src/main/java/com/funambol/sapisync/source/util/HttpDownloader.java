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

package com.funambol.sapisync.source.util;

import com.funambol.platform.HttpConnectionAdapter;
import com.funambol.util.ConnectionManager;
import com.funambol.util.Log;

import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;


public class HttpDownloader  {

    private static final String TAG_LOG = "HttpDownloader";

    private final int DEFAULT_CHUNK_SIZE = 4096;

    private DownloadListener listener = null;
    
    //------------------------------------------------------------- Constructors

    public HttpDownloader() {
    }

    public HttpDownloader(DownloadListener listener) {
        this.listener = listener;
    }

    public void setDownloadListener(DownloadListener listener) {
        this.listener = listener;
    }

    public void download(String url, OutputStream os, long size) throws IOException {
        HttpConnectionAdapter conn = null;
        InputStream is = null;
        if(listener != null) {
            listener.downloadStarted(size);
        }
        try {
            if (Log.isLoggable(Log.DEBUG)) {
                Log.debug(TAG_LOG, "Sending http request to: " + url);
            }
            conn = ConnectionManager.getInstance().openHttpConnection(
                    url, "wrapper");
            conn.setRequestMethod(HttpConnectionAdapter.GET);
            if (Log.isLoggable(Log.DEBUG)) {
                Log.debug(TAG_LOG, "Response is: " + conn.getResponseCode());
            }
            if (conn.getResponseCode() == HttpConnectionAdapter.HTTP_OK) {
                is = conn.openInputStream();
                byte[] data = new byte[DEFAULT_CHUNK_SIZE];
                int n = 0;
                while ((n = is.read(data)) != -1) {
                    os.write(data, 0, n);
                    if(listener != null) {
                        listener.downloadChunkReceived(n);
                    }
                }
            } else {
                Log.error(TAG_LOG, "Http request failed. Server replied: " +
                        conn.getResponseCode() + ", message: " +
                        conn.getResponseMessage());
                throw new IOException("HTTP error code: " + conn.getResponseCode());
            }
        } catch(IOException ex) {
            Log.error(TAG_LOG, "Http download failed", ex);
            throw ex;
        } finally {
            // Release all resources
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
                } catch (IOException e) {}
                conn = null;
            }
            if(listener != null) {
                listener.downloadEnded();
            }
        }
    }

    /**
     * Used to monitor a download operation
     */
    public interface DownloadListener {

        /**
         * Called as soon as the download starts
         * @param totalSize the total size to download
         */
        public void downloadStarted(long totalSize);

        /**
         * Called as soon as a download chunk has been received
         * @param chunkSize the size of the chunk
         */
        public void downloadChunkReceived(int chunkSize);

        /**
         * Called as soon as the download finishes
         */
        public void downloadEnded();

    }
}

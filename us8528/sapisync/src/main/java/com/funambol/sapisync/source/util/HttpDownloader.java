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

package com.funambol.sapisync.source.util;

import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;

import com.funambol.platform.HttpConnectionAdapter;
import com.funambol.util.ConnectionManager;
import com.funambol.util.Log;

public class HttpDownloader  {

    private static final String TAG_LOG = "HttpDownloader";

    private static final int DEFAULT_CHUNK_SIZE = 4096;
    private static final int MAX_RETRY = 1;

    private DownloadListener listener = null;

    /**
     * This is the flag used to indicate that the current download shall be
     * cancelled
     */
    private boolean cancel = false;
    
    //------------------------------------------------------------- Constructors

    public HttpDownloader() {
    }

    public HttpDownloader(DownloadListener listener) {
        this.listener = listener;
    }

    public void setDownloadListener(DownloadListener listener) {
        this.listener = listener;
    }

    /**
     * Download the content at the given url and writes it into the given output stream
     * @param url
     * @param os
     * @param size
     * @param testName is the name of the item being downloaded to which
     *        automatic tests can refer to
     * @throws DownloadException in case there is a non temporary network error of any kind (for example auth error)
     * @throws IOException in case there is a problem writing the output stream
     *
     * @return the total number of downloaded bytes. If this is smaller than size, then the download
     *         was interrupted and can be resumed later. The user shall always check the returned value.
     */
    public long download(String url, OutputStream os, long size, String testName) throws DownloadException, IOException {
        boolean retry;
        int i = 0;
        long downloadedSize = 0;
        do {
            ++i;
            retry = false;
            try {
                if (downloadedSize == 0) {
                    // Perform a full download
                    downloadedSize = download(url, os, size, -1, -1, testName);
                } else {
                    // Try to resume previous download
                    if (Log.isLoggable(Log.INFO)) {
                        Log.info(TAG_LOG, "Resuming download at " + downloadedSize);
                    }
                    downloadedSize = download(url, os, size, downloadedSize, size - 1, testName);
                }
            } catch (DownloadException de) {
                // If we had a network error, then we try to resume the download
                // (until we reach a max number of errors)
                if (i < MAX_RETRY) {
                    retry = true;
                    downloadedSize = de.getPartialLength();
                } else {
                    throw de;
                }
            } catch (ResumeException re) {
                if (i < MAX_RETRY) {
                    retry = true;
                    // Get ready to start download from 0
                    downloadedSize = 0;
                } else {
                    throw new DownloadException("Cannot download item", downloadedSize);
                }
            }
        } while(retry && downloadedSize < size);
        return downloadedSize;
    }

    /**
     * Resumes the download of an interrupted item
     * @param url
     * @param os
     * @param size
     * @param startOffset
     * @param testName
     * @throws DownloadException
     * @throws IOException
     */
    public long resume(String url, OutputStream os, long size, long startOffset, String testName)
    throws DownloadException, IOException, ResumeException {
        return download(url, os, size, startOffset, size - 1, testName);
    }

    /**
     * Cancels the current download
     */
    public void cancel() {
        if (Log.isLoggable(Log.DEBUG)) {
            Log.debug(TAG_LOG, "Cancelling current download");
        }
        cancel = true;
    }

    private boolean isDownloadCancelled() {
        return cancel;
    }

    /**
     * Download an item and store it into the given output stream. The method
     * supports also resuming if the provided startOffset is &gt; 0
     *
     * @param the url to be downloaded
     * @param os the output stream to be written
     * @param size the total size of the item being downloaded
     * @param startOffset the byte from which the download shall start
     * @param endOffset the end byte in the resume request
     * @param testItemName is the name of the item being downloaded to which
     *
     * @throws DownloadException if a network error occurs
     * @throws IOException if the output stream cannot be written
     * @throws ResumeException if the download cannot be resumed
     */
    protected long download(String url, OutputStream os, long size, long startOffset, long endOffset, String testItemName)
    throws DownloadException, IOException, ResumeException {

        cancel = false;

        long downloadedSize = startOffset > 0 ? startOffset : 0;

        HttpConnectionAdapter conn = null;
        InputStream is = null;
        if(listener != null) {
            listener.downloadStarted(size);
        }

        boolean errorWritingStream = false;
        boolean resume = false;

        try {
            if (Log.isLoggable(Log.DEBUG)) {
                Log.debug(TAG_LOG, "Sending http request to: " + url);
            }
            conn = ConnectionManager.getInstance().openHttpConnection(url, "wrapper,key," + testItemName + ",phase,receiving");
            conn.setRequestMethod(HttpConnectionAdapter.GET);
            if (startOffset > 0  && endOffset > 0) {
                // This is a resume request. Add the proper header
                StringBuffer range = new StringBuffer();
                range.append("bytes=").append(startOffset).append("-");
                conn.setRequestProperty("Range", range.toString());
                resume = true;
                if (Log.isLoggable(Log.INFO)) {
                    Log.info(TAG_LOG, "Resuming download for: " + url + " with range request " + range.toString());
                }
            }
            if (Log.isLoggable(Log.DEBUG)) {
                Log.debug(TAG_LOG, "Response is: " + conn.getResponseCode());
            }

            boolean ok;
            conn.execute(null);

            int respCode = conn.getResponseCode();
            if (resume) {
                if (respCode == HttpConnectionAdapter.HTTP_PARTIAL) {
                    ok = true;
                } else {
                    if (Log.isLoggable(Log.INFO)) {
                        Log.info(TAG_LOG, "Server refused resuming download");
                    }
                    throw new ResumeException("Cannot resume download");
                }
            } else {
                ok = respCode == HttpConnectionAdapter.HTTP_OK;
            }

            if (ok) {
                is = conn.openInputStream();
                byte[] data = new byte[DEFAULT_CHUNK_SIZE];
                int n = 0;
                while ((n = is.read(data)) != -1 && !isDownloadCancelled()) {

                    // We intercept the IO exception during writing because this
                    // must generate a generic error for this specific item, but not
                    // interrupt the sync like a network error

                    try {
                        os.write(data, 0, n);
                    } catch (IOException ioe) {
                        Log.error(TAG_LOG, "Cannot write output stream", ioe);
                        errorWritingStream = true;
                        break;
                    }

                    // Keep track of how many bytes were downloaded and locally saved
                    downloadedSize += n;

                    if(listener != null) {
                        listener.downloadProgress(downloadedSize);
                    }
                }
                if(isDownloadCancelled()) {
                    throw new DownloadException("Download cancelled",
                            downloadedSize, DownloadException.CODE_CANCELLED);
                }
            } else {
                Log.error(TAG_LOG, "Http request failed. Server replied: " +
                        conn.getResponseCode() + ", message: " +
                        conn.getResponseMessage());

                throw new DownloadException("HTTP error code: " + conn.getResponseCode(), 0);
            }
        } catch(IOException ex) {
            Log.error(TAG_LOG, "Http download failed with a network error " + downloadedSize, ex);
            throw new DownloadException("Download failed with a network error", downloadedSize);
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

            // If we get here with a stream error we shall report an IOException
            if (errorWritingStream) {
                throw new IOException("Cannot write output stream");
            }
        }
        return downloadedSize;
    }

    /**
     * Used to monitor a download operation
     */
    public interface DownloadListener {

        /**
         * Reports that a new download operation started.
         * @param totalSize the total size of bytes to download
         */
        public void downloadStarted(long totalSize);

        /**
         * Reports the progress of a download operation.
         * @param size the total number of bytes received from the beginning
         */
        public void downloadProgress(long size);

        /**
         * Reports that a download operation ended.
         */
        public void downloadEnded();

    }
}

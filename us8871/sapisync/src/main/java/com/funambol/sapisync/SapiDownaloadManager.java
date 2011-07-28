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

package com.funambol.sapisync;

import java.util.Vector;
import java.io.IOException;
import java.io.OutputStream;

import com.funambol.org.json.me.JSONException;
import com.funambol.org.json.me.JSONObject;
import com.funambol.sync.SyncConfig;
import com.funambol.sync.SyncException;
import com.funambol.sync.SyncItem;
import com.funambol.sync.SyncListener;
import com.funambol.sync.SyncSource;
import com.funambol.sync.ItemDownloadInterruptionException;
import com.funambol.sapisync.source.JSONSyncItem;
import com.funambol.sapisync.source.util.ResumeException;
import com.funambol.sapisync.source.util.HttpDownloader;
import com.funambol.sapisync.source.util.DownloadException;
import com.funambol.util.Log;

class SapiDownloadManager {

    private static final String TAG_LOG = "SapiDownloadManager";

    private final Vector activeThreads = new Vector();
    private final Vector queue = new Vector();

    private SyncSource src;
    private SyncConfig syncConfig;
    private SapiDownloadListener listener;
    private DownloadDaemon downloadDaemon = null;

    // TODO: this must be computed somehow dynamically
    private int maxThreads = 5;

    public SapiDownloadManager(SyncConfig config, SyncSource src) {
        this.syncConfig = config;
        this.src = src;
    }

    public void setListener(SapiDownloadListener listener) {
        this.listener = listener;
    }

    public void dispose() {
        if (downloadDaemon != null) {
            downloadDaemon.setDone();
            // In case the daemon is in its wait state, we notify it
            synchronized(queue) {
                queue.notify();
            }
        }
    }

    public void download(JSONObject jsonItem, JSONSyncItem item) {
        if (downloadDaemon == null) {
            downloadDaemon = new DownloadDaemon();
            // Start the daemon
            downloadDaemon.setDaemon(true);
            downloadDaemon.setPriority(Thread.MAX_PRIORITY);
            downloadDaemon.start();

            // Make sure the daemon has started
            do {
                try {
                    Thread.sleep(100);
                } catch (Exception e) {}
            } while(!downloadDaemon.isAlive());
        }

        // Enqueue the new request
        Request r = new Request(jsonItem, item);
        synchronized(queue) {
            queue.addElement(r);
            queue.notify();
        }
    }

    private class DownloadDaemon extends Thread {

        private boolean done = false;

        public void setDone() {
            done = true;
        }

        public void run() {
            while(!done) {
                synchronized(queue) {
                    try {
                        queue.wait();
                    } catch (Exception e) {}

                    while(!done && queue.size() > 0) {
                        // Grab the first request
                        Request r = (Request)queue.elementAt(0);

                        int numRunningThreads;
                        synchronized(activeThreads) {
                            numRunningThreads = activeThreads.size();
                        }

                        if (numRunningThreads < maxThreads) {
                            // There is room to fire a new download thread
                            SingleDownloadThread dt = new SingleDownloadThread(r);
                            synchronized(activeThreads) {
                                activeThreads.addElement(dt);
                            }
                            queue.removeElementAt(0);
                            // Fire the download thread
                            if (Log.isLoggable(Log.DEBUG)) {
                                Log.debug(TAG_LOG, "Starting a new download thread for " + r.getItem().getKey());
                            }
                            dt.start();
                        } else {
                            if (Log.isLoggable(Log.TRACE)) {
                                Log.trace(TAG_LOG, "Reached max number of download thread");
                            }
                            break;
                        }
                    }
                }
            }
        }
    }

    private class SingleDownloadThread extends Thread {

        private Request request;

        public SingleDownloadThread(Request r) {
            this.request = r;
        }

        public void run() {
            if (Log.isLoggable(Log.INFO)) {
                Log.info(TAG_LOG, "Starting download of item " + request.getItem().getKey());
            }
            try {
                downloadItemContent(request.getItem());
                if (listener != null) {
                    listener.onItemDownloadResult(src, request.getJSONItem(), request.getItem(), null);
                }
            } catch(Exception e) {
                Log.error(TAG_LOG, "Cannot download item", e);
                if (listener != null) {
                    listener.onItemDownloadResult(src, request.getJSONItem(), request.getItem(), e);
                }
            } finally {
                // This thread is over
                synchronized(activeThreads) {
                    activeThreads.removeElement(this);
                }
                // Give a change to another thread to fire
                synchronized(queue) {
                    queue.notify();
                }
            }
        }

        private void downloadItemContent(JSONSyncItem item) throws SyncException, IOException {
            // Check if this item has a link to data that must be downloaded,
            // otherwise we just take the item content
            String url = item.getContentUrl(syncConfig.getSyncUrl());
            if (url != null) {
                if (Log.isLoggable(Log.DEBUG)) {
                    Log.debug(TAG_LOG, "item has remote content, prepare to download it");
                }
                HttpDownloader downloader = new HttpDownloader();
                downloader.setDownloadListener(new DownloadSyncListener(item, src.getListener()));

                OutputStream fileos = null;

                try {
                    fileos = item.getOutputStream();
                    long partialLength = item.getPartialLength();

                    long actualSize;
                    if (partialLength > 0) {
                        actualSize = downloader.resume(url, fileos, partialLength, item.getContentName());
                    } else {
                        actualSize = downloader.download(url, fileos, item.getContentName());
                    }
                    if (Log.isLoggable(Log.DEBUG)) {
                        Log.debug(TAG_LOG, "actual size is " + actualSize);
                    }
                } catch (ResumeException re) {
                    // The item download cannot be resumed properly
                    // Re-create a new output stream without appending
                    fileos.close();
                    fileos = item.getOutputStream();
                    // Download the item from scratch
                    try {
                        long actualSize = downloader.download(url, fileos, item.getContentName());
                    } catch (DownloadException de) {
                        throw new ItemDownloadInterruptionException(item, de.getPartialLength());
                    }
                } catch (DownloadException de) {
                    // We had a network error while download the item. Propagate the
                    // exception as the sync must be interrupted
                    if (Log.isLoggable(Log.DEBUG)) {
                        Log.debug(TAG_LOG, "Cannot download item, interrupt sync " + de.getPartialLength());
                    }
                    if(de.getCode() == DownloadException.CODE_CANCELLED) {
                        throw new ItemDownloadInterruptionException(SyncException.CANCELLED, item, de.getPartialLength());
                    } else {
                        throw new ItemDownloadInterruptionException(item, de.getPartialLength());
                    }
                } finally {
                    if (fileos != null) {
                        try {
                            fileos.close();
                        } catch (IOException ioe) {
                            Log.error(TAG_LOG, "Cannot close output stream", ioe);
                        }
                    }
                }
            } else {
                // there is no remote content to download
                OutputStream os = null;
                try {
                    os = item.getOutputStream();
                    os.write(item.getContent());
                } finally {
                    if (os != null) {
                        try {
                            os.close();
                        } catch (IOException ioe) {
                            Log.error(TAG_LOG, "Cannot close output stream", ioe);
                        }
                    }
                }
            }
        }
    }

    private class Request {

        private JSONObject jsonItem;
        private JSONSyncItem item;

        public Request(JSONObject jsonItem, JSONSyncItem item) {
            this.jsonItem = jsonItem;
            this.item     = item;
        }

        public JSONObject getJSONItem() {
            return jsonItem;
        }

        public JSONSyncItem getItem() {
            return item;
        }
    }

    /**
     * Translates the HttpDownloader.DownloadListener calls into SyncListener calls.
     */
    private class DownloadSyncListener implements HttpDownloader.DownloadListener {

        private SyncListener syncListener = null;
        private String itemKey = null;
        private String itemParent = null;
        private char itemState;

        public DownloadSyncListener(SyncItem item, SyncListener syncListener) {
            this.syncListener = syncListener;
            this.itemKey = item.getKey();
            this.itemParent = item.getParent();
            this.itemState = item.getState();
        }

        public void downloadStarted(long totalSize) {
        }

        public void downloadProgress(long size) {
            if(syncListener != null) {
                if(itemState == SyncItem.STATE_NEW) {
                    syncListener.itemAddReceivingProgress(itemKey, itemParent, size);
                } else {
                    syncListener.itemReplaceReceivingProgress(itemKey, itemParent, size);
                }
            }
        }

        public void downloadEnded() {
        }
    }
}

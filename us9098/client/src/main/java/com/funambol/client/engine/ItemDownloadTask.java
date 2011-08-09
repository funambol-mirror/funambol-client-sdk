/**
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

package com.funambol.client.engine;

import java.io.OutputStream;
import java.io.IOException;

import com.funambol.util.StringUtil;
import com.funambol.sapisync.source.JSONSyncItem;
import com.funambol.sapisync.source.JSONFileObject;
import com.funambol.concurrent.Task;
import com.funambol.sapisync.source.util.HttpDownloader;
import com.funambol.storage.Tuple;
import com.funambol.storage.QueryFilter;
import com.funambol.storage.QueryResult;
import com.funambol.storage.Tuple;
import com.funambol.client.source.MediaMetadata;
import com.funambol.platform.FileAdapter;
import com.funambol.sync.SyncException;
import com.funambol.storage.Table;
import com.funambol.util.Log;

public class ItemDownloadTask implements Task {

    private static final String TAG_LOG = "ItemDownloadTask";

    private String url;
    private String fileName;
    private String id;
    private String thumbSize;
    private int initialDelay;
    private Table metadata;
    private String tableColumnName;

    public ItemDownloadTask(String url, String fileName, String id, String thumbSize,
                            int initialDelay, Table metadata, String tableColumnName) {
        this.url = url;
        this.fileName = fileName;
        this.id = id;
        this.thumbSize = thumbSize;
        this.initialDelay = initialDelay;
        this.metadata = metadata;
        this.tableColumnName = tableColumnName;
    }

    public void run() {
        boolean done = false;
        int delay = 1;
        do {
            try {
                download();
                done = true;
            } catch (IOException ioe) {
                Log.error(TAG_LOG, "Cannot download item", ioe);
                // We try again, after some time
                try {
                    Thread.sleep(delay * 1000);
                } catch (Exception e) {}
                delay *= 2;
            } catch (Exception e) {
                Log.error(TAG_LOG, "Cannot download item, giving up", e);
                done = true;
                generateFailureEvent(id);
                return;
            }
        } while (!done);
        generateSuccessEvent(id);
    }


    private void download() throws Exception {
        Log.info(TAG_LOG, "Start downloading thumbnail at " + url);
        FileAdapter fa = null;
        OutputStream os = null;
        try {
            Thread.sleep(initialDelay);
        } catch (Exception e) {
        }
        try {
            fa = new FileAdapter(fileName);
            os = fa.openOutputStream();
            HttpDownloader downloader = new HttpDownloader();
            downloader.download(url, os, "");

            // Update the table
            metadata.open();
            QueryResult results = null;
            try {
                long keyLong = Long.parseLong(id);
                if (Log.isLoggable(Log.TRACE)) {
                    Log.trace(TAG_LOG, "Searching for metadata with key " + keyLong);
                }
                QueryFilter qf = metadata.createQueryFilter(new Long(keyLong));
                results = metadata.query(qf);
                if (results.hasMoreElements()) {
                    Tuple tuple = results.nextElement();

                    // Save the protocol so that we know this file is local now
                    tuple.setField(metadata.getColIndexOrThrow(tableColumnName), "file://" + fileName);
                    metadata.update(tuple);
                    metadata.save();
                } else {
                    Log.error(TAG_LOG, "Cannot update metadata table, item not found " + id);
                }
            } catch (Exception e) {
                Log.error(TAG_LOG, "Cannot update metadata table, this item has failed and cannot be resumed");
                throw new SyncException(SyncException.CLIENT_ERROR, "Cannot download remote item");
            } finally {
                if (results != null) {
                    results.close();
                }
            }
        } finally {
            if (os != null) {
                try {
                    os.close();
                } catch (Exception e) {}
            }
            if (fa != null) {
                try {
                    fa.close();
                } catch (Exception e) {}
            }
            if (metadata != null) {
                try {
                    metadata.close();
                } catch (Exception e) {}
            }
        }
    }


    private void generateFailureEvent(String id) {
        // TODO FIXME: generate an event to indicate this download has failed
    }

    private void generateSuccessEvent(String id) {
        // TODO FIXME: generate an event to indicate this download has succeeded
    }
}



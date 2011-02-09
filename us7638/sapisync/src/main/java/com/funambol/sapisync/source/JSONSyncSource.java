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

package com.funambol.sapisync.source;

import com.funambol.sync.SyncItem;
import com.funambol.sync.SourceConfig;
import com.funambol.sync.SyncException;
import com.funambol.sync.SyncSource;
import com.funambol.sync.client.ChangesTracker;
import com.funambol.sync.client.TrackableSyncSource;
import com.funambol.sapisync.source.util.HttpDownloader;
import com.funambol.util.Log;

import java.io.OutputStream;

/**
 * Represents a SyncSource which handles JSON file objects as input SyncItems.
 * You should define the getDownloadOutputStream in order to provide the
 * OutputSteam used to download the file.
 */
public abstract class JSONSyncSource extends TrackableSyncSource {

    private static final String TAG_LOG = "JSONSyncSource";

    protected boolean downloadFileObject;
    protected boolean downloadThumbnails;

    private HttpDownloader downloader = null;

    //------------------------------------------------------------- Constructors

    /**
     * JSONSyncSource constructor: initialize source config
     */
    public JSONSyncSource(SourceConfig config, ChangesTracker tracker) {
        super(config, tracker);
        downloadFileObject = true;
        downloadThumbnails = false;
        downloader = new HttpDownloader();
    }

    public int addItem(SyncItem item) throws SyncException {
        super.addItem(item);
        return addUpdateItem(item, false);
    }

    public int updateItem(SyncItem item) throws SyncException {
        super.updateItem(item);
        return addUpdateItem(item, true);
    }

    private int addUpdateItem(SyncItem item, boolean isUpdate) throws SyncException {
        try {
            String itemContent = new String(item.getContent());
            JSONFileObject jsonFile = new JSONFileObject(itemContent);
            if(downloadFileObject) {
                String name    = jsonFile.getName();
                String baseUrl = jsonFile.getUrl();
                long size      = jsonFile.getSize();
                OutputStream fileos = getDownloadOutputStream(jsonFile, isUpdate, false);
                downloader.download(composeUrl(baseUrl, name), fileos, size);
            }
            if(downloadThumbnails) {
                // TODO FIXME download thumbnails
            }
            return SyncSource.STATUS_SUCCESS;
        } catch (Throwable t) {
            Log.error(TAG_LOG, "Cannot save json item", t);
            return SyncSource.STATUS_RECV_ERROR;
        }
    }

    protected OutputStream getDownloadOutputStream(JSONFileObject jsonItem,
            boolean isUpdate, boolean isThumbnail) {
        return getDownloadOutputStream(
                jsonItem.getName(),
                jsonItem.getSize(),
                isUpdate,
                isThumbnail);
    }

    /**
     * Must be implemented in order to provide a proper OutputStream to download
     * the item.
     * 
     * @param name
     * @param size
     * @param isUpdate
     * @param isThumbnail
     * @return
     */
    protected abstract OutputStream getDownloadOutputStream(String name,
            long size, boolean isUpdate, boolean isThumbnail);

    private String composeUrl(String baseUrl, String filename) {
        StringBuffer res = new StringBuffer();
        res.append(baseUrl);
        res.append('/');
        res.append(filename);
        return res.toString();
    }
}

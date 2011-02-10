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

import java.io.IOException;
import java.io.OutputStream;

import com.funambol.sync.SyncItem;
import com.funambol.sync.SourceConfig;
import com.funambol.sync.SyncException;
import com.funambol.sync.ItemDownloadInterruptionException;
import com.funambol.sync.SyncSource;
import com.funambol.sync.client.ChangesTracker;
import com.funambol.sync.client.TrackableSyncSource;
import com.funambol.sapisync.source.util.HttpDownloader;
import com.funambol.sync.SyncConfig;
import com.funambol.util.Log;
import com.funambol.util.StringUtil;

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
    private SyncConfig syncConfig = null;
    private String dataTag = null;

    //------------------------------------------------------------- Constructors

    /**
     * JSONSyncSource constructor: initialize source config
     */
    public JSONSyncSource(SourceConfig config, SyncConfig syncConfig, ChangesTracker tracker) {
        super(config, tracker);
        this.downloadFileObject = true;
        this.downloadThumbnails = false;
        this.downloader = new HttpDownloader();
        this.syncConfig = syncConfig;
    }

    public int addItem(SyncItem item) throws SyncException {
        // Note that the addItem must still download the actual item content, therefore
        // it can get a network error and this must be propagated
        try {
            String itemContent = new String(item.getContent());
            JSONFileObject jsonFile = new JSONFileObject(itemContent);
            int res = addUpdateItem(item, jsonFile, false);
            super.addItem(item);
            return res;
        } catch (IOException ioe) {
            Log.error(TAG_LOG, "Cannot add item, ioe");
            return SyncSource.ERROR_STATUS;
        } catch (SyncException se) {
            // This kind of exception blocks the sync because it is a network error of some kind
            throw se;
        } catch (Throwable t) {
            Log.error(TAG_LOG, "Cannot add item", t);
            return SyncSource.ERROR_STATUS;
        }
    }

    public int updateItem(SyncItem item) throws SyncException {
        // We consider IOException and other generic exception as non
        // blocking exceptions for the sync. Only network exceptions will
        // block it
        try {
            String itemContent = new String(item.getContent());
            JSONFileObject jsonFile = new JSONFileObject(itemContent);
            int res = addUpdateItem(item, jsonFile, true);
            super.addItem(item);
            return res;
        } catch (IOException ioe) {
            Log.error(TAG_LOG, "Cannot add item, ioe");
            return SyncSource.ERROR_STATUS;
        } catch (SyncException se) {
            // This kind of exception blocks the sync because it is a network error of some kind
            throw se;
        } catch (Throwable t) {
            Log.error(TAG_LOG, "Cannot add item", t);
            return SyncSource.ERROR_STATUS;
        }
    }

    public void updateSyncConfig(SyncConfig syncConfig) {
        this.syncConfig = syncConfig;
    }

    // This method returns the tag name in the JSONobject for the specific
    // type of data handled by this source. Refer to the SAPI documentation
    // for more info.
    public String getDataTag() {
        return dataTag;
    }

    public void setDataTag(String dataTag) {
        this.dataTag = dataTag;
    }

    protected int addUpdateItem(SyncItem item, JSONFileObject jsonFile, boolean isUpdate)
    throws SyncException, IOException {
        if(downloadFileObject) {
            String baseUrl = jsonFile.getUrl();
            long size = jsonFile.getSize();
            OutputStream fileos = null;
            fileos = getDownloadOutputStream(jsonFile, isUpdate, false);
            long actualSize = downloader.download(composeUrl(baseUrl), fileos, size);
            if (size != actualSize) {
                // The download was interrupted. We shall keep track of this interrupted download
                // so that it can be resumed
                throw new ItemDownloadInterruptionException(item, actualSize);
            }
        }
        if(downloadThumbnails) {
            // TODO FIXME download thumbnails
        }
        return SyncSource.SUCCESS_STATUS;
    }

    protected OutputStream getDownloadOutputStream(JSONFileObject jsonItem,
            boolean isUpdate, boolean isThumbnail) throws IOException {
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
            long size, boolean isUpdate, boolean isThumbnail) throws IOException;

    /**
     * Composes the url to use for the download operation.
     * 
     * @param baseUrl
     * @param filename
     * @return
     */
    private String composeUrl(String baseUrl) {
        String serverUrl = StringUtil.extractAddressFromUrl(
                syncConfig.getSyncUrl());
        StringBuffer res = new StringBuffer();
        res.append(serverUrl);
        res.append(baseUrl);
        return res.toString();
    }
}

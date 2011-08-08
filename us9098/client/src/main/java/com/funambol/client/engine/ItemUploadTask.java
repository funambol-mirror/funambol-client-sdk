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
import com.funambol.client.configuration.Configuration;
import com.funambol.util.Log;

public class ItemUploadTask implements Task {

    private static final String TAG_LOG = "ItemUploadTask";

    private String url;
    private String fileName;
    private String id;
    private String thumbSize;
    private int initialDelay;
    private Table metadata;
    private Configuration configuration;

    public ItemUploadTask(String url, String fileName, String id, String thumbSize,
                          int initialDelay, Table metadata, Configuration configuration) {
        this.url = url;
        this.fileName = fileName;
        this.id = id;
        this.thumbSize = thumbSize;
        this.initialDelay = initialDelay;
        this.metadata = metadata;
        this.canResume = canResume;

        this.configuration = configuration;
    }

    public void run() {
    }

    /*
    public void run() {
        boolean done = false;
        int delay = 1;

        do {
            // We refresh at every attempt so that if the user changes the
            // configuration we pick the updated one
            sapiHandler = new SapiHandler(configuration.getSyncUrl(), configurarion.getUsername(),
                                          configuration.getPassword());

            try {
                // First of all we check if the item is partially available on
                // the server
                long partialSize = sapiHandler.getMediaPartialUploadLength();
                if (partialSize == 0 || !canResume) {
                    uploadItem();
                } else if (partialSize < size) {
                    resumeItemUpload();
                }
                done = true;
            } catch (IOException ioe) {
                Log.error(TAG_LOG, "Cannot upload item", ioe);
                // We try again, after some time
                try {
                    Thread.sleep(delay * 1000);
                } catch (Exception e) {}
                delay *= 2;
            } catch (Exception e) {
                Log.error(TAG_LOG, "Cannot upload item, giving up", e);
                done = true;
                generateFailureEvent(id);
                return;
            }
        } while (!done);
        generateSuccessEvent(id);
    }

    private ResumeResult resumeItemUpload(JSONSyncItem item, String remoteUri, SyncListener listener)
    throws SapiException {
        if (Log.isLoggable(Log.INFO)) {
            Log.info(TAG_LOG, "Resuming upload for item: " + item.getKey());
        }
        JSONFileObject json = item.getJSONFileObject();

        // First of all we need to query the server to understand where we shall
        // restart from. The item must have a valid guid, otherwise we cannot
        // resume
        String guid = item.getGuid();

        if (guid == null) {
            Log.error(TAG_LOG, "Cannot resume, a complete upload will be performed instead");
            String remoteKey = prepareItemUpload(item, remoteUri);
            item.setGuid(remoteKey);
            String crc = uploadItem(item, remoteUri, listener);
            return new ResumeResult(remoteUri, crc);
        }

        long length = -1;
        try {
            length = sapiHandler.getMediaPartialUploadLength(remoteUri, guid, json.getSize());
        } catch (NotSupportedCallException e) {
            Log.error(TAG_LOG, "Server doesn't support the SAPI call", e);
            throw SapiException.SAPI_EXCEPTION_CALL_NOT_SUPPORTED;
        } catch(IOException ex) {
            Log.error(TAG_LOG, "Failed to upload item", ex);
            throw SapiException.SAPI_EXCEPTION_NO_CONNECTION;
        }

        if (length > 0) {
            if(length == json.getSize()) {
                if (Log.isLoggable(Log.INFO)) {
                    Log.info(TAG_LOG, "No need to resume item " + length);
                }
                return new ResumeResult(guid, null);
            } else {
                long fromByte = length + 1;
                if (Log.isLoggable(Log.INFO)) {
                    Log.info(TAG_LOG, "Upload can be resumed at byte " + fromByte);
                }
                String crc = uploadItem(item, remoteUri, listener, fromByte);
                return new ResumeResult(guid, crc);
            }
        } else {
            if (Log.isLoggable(Log.INFO)) {
                Log.info(TAG_LOG, "Upload cannot be resumed, perform a complete upload");
            }
            guid = prepareItemUpload(item, remoteUri);
            item.setGuid(guid);
            String crc = uploadItem(item, remoteUri, listener);
            return new ResumeResult(guid, crc);
        }
    }

    private String uploadItem(JSONSyncItem item, String remoteUri, SyncListener listener)
    throws SapiException {
        return uploadItem(item, remoteUri, listener, 0);
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
                    tuple.setField(metadata.getColIndexOrThrow(MediaMetadata.METADATA_THUMB1_PATH), fileName);
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

    private String uploadItem(JSONSyncItem item, String remoteUri, SyncListener listener, long fromByte)
    throws SapiException
    {
        //FIXME
        //attempt, what is its use? Does it really need?
        int attempt = 0;
        do {
            try {
                // Get ready to perform twice if we have an authorization failure first time
                return uploadItemHelper(item, remoteUri, listener, fromByte);
            } catch (NotAuthorizedCallException nae) {
                if (attempt < 2) {
                    if (Log.isLoggable(Log.INFO)) {
                        Log.info(TAG_LOG, "Retrying operation after logging in");
                        // No need to refresh the device id
                        login(null);
                    }
                } else {
                    throw SapiException.SAPI_EXCEPTION_UNKNOWN;
                }
            }
        } while(true);
    }

    private String uploadItemHelper(JSONSyncItem item, String remoteUri, SyncListener listener, long fromByte)
    throws SapiException, NotAuthorizedCallException

    {
        if (Log.isLoggable(Log.INFO)) {
            Log.info(TAG_LOG, "Uploading item: " + item.getKey());
        }

        InputStream is = null;
        Hashtable headers = new Hashtable();
        JSONFileObject json = item.getJSONFileObject();
        String remoteKey = item.getGuid();

        try {

            // If this is not a resume, we must perform the upload in two
            // phases. Send the metadata first and then the actual content
            is = item.getInputStream();
            if(is == null) {
                if(Log.isLoggable(Log.DEBUG)) {
                    Log.debug(TAG_LOG, "Upload is not needed, item content is null");
                }
                return "";
            }

            if (fromByte == 0) {
                if (Log.isLoggable(Log.DEBUG)) {
                    Log.debug(TAG_LOG, "Uploading a new item with guid " + remoteKey);
                }
            } else {
                if (Log.isLoggable(Log.DEBUG)) {
                    Log.debug(TAG_LOG, "Resuming an item with guid " + remoteKey);
                }
                StringBuffer contentRangeValue = new StringBuffer();
                contentRangeValue.append("bytes ").append(fromByte)
                        .append("-").append(json.getSize()-1)
                        .append("/").append(json.getSize());
                headers.put("Content-Range", contentRangeValue.toString());
            }
            
            headers.put("x-funambol-id", remoteKey);
            headers.put("x-funambol-file-size", Long.toString(json.getSize()));

            SapiUploadSyncListener sapiListener = new SapiUploadSyncListener(item, listener);
            sapiHandler.setSapiRequestListener(sapiListener);
        } catch (NotSupportedCallException e) {
            Log.error(TAG_LOG, "Server doesn't support the SAPI call", e);
            throw SapiException.SAPI_EXCEPTION_CALL_NOT_SUPPORTED;
        } catch(IOException ex) {
            Log.error(TAG_LOG, "Cannot open media stream", ex);
            throw SapiException.SAPI_EXCEPTION_UNKNOWN;
        }


        // Send the upload request
        JSONObject uploadResponse = null;
        try {
            Vector params = new Vector();
            params.addElement("lastupdate=true");

            uploadResponse = sapiHandler.query("upload/" + remoteUri,
                    "save", params, headers, is, json.getMimetype(),
                    json.getSize(), fromByte, json.getName());

            //original code: process the exception if error is present in the object
            if (SapiResultError.hasError(uploadResponse)) {
                checkForCommonSapiErrorCodesAndThrowSapiException(uploadResponse, null, true);
            }

            sapiHandler.setSapiRequestListener(null);
            return uploadResponse.getString("lastupdate");
        } catch (NotSupportedCallException e) {
            Log.error(TAG_LOG, "Server doesn't support the SAPI call", e);
            throw SapiException.SAPI_EXCEPTION_CALL_NOT_SUPPORTED;
        } catch (NotAuthorizedCallException nae) {
            Log.error(TAG_LOG, "Server authentication failure, try to login again", nae);
            throw nae;
        } catch(JSONException ex) {
            throw SapiException.SAPI_EXCEPTION_UNKNOWN;
        } catch (IOException ioe) {
            // The upload failed and got interrupted. We report this error
            // so that a resume is possible
            throw new SapiException(SapiException.CUS_0001, "Error upload item on server");
        }
    }


    private void generateFailureEvent(String id) {
        // TODO FIXME: generate an event to indicate this download has failed
    }

    private void generateSuccessEvent(String id) {
        // TODO FIXME: generate an event to indicate this download has succeeded
    }

    private class SapiUploadSyncListener implements SapiHandler.SapiQueryListener {

        private SyncListener syncListener = null;
        private SyncItem item = null;

        public SapiUploadSyncListener(SyncItem item, SyncListener syncListener) {
            this.syncListener = syncListener;
            this.item = item;
        }

        public void queryStarted(int totalSize) {      
        }

        public void queryProgress(int size) {
            if(syncListener != null) {
                if (item.getState() == SyncItem.STATE_NEW) {
                    syncListener.itemAddSendingProgress(item.getKey(), item.getParent(), size);
                } else if (item.getState() == SyncItem.STATE_UPDATED) {
                    syncListener.itemReplaceSendingProgress(item.getKey(), item.getParent(), size);
                }
            }
        }

        public void queryEnded() {
        }
    }
    */

}



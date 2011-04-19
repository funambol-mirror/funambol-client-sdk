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

import java.io.ByteArrayInputStream;
import java.io.ByteArrayOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;

import com.funambol.client.source.AppSyncSource;
import com.funambol.client.source.AppSyncSourceManager;
import com.funambol.client.test.BasicScriptRunner;
import com.funambol.client.test.Robot;
import com.funambol.client.test.util.TestFileManager;
import com.funambol.client.test.basic.BasicUserCommands;

import com.funambol.sapisync.SapiSyncHandler;
import com.funambol.sapisync.source.JSONFileObject;
import com.funambol.sapisync.source.JSONSyncItem;
import com.funambol.sapisync.source.JSONSyncSource;
import com.funambol.sync.SyncConfig;
import com.funambol.sync.SyncItem;
import com.funambol.sync.SyncSource;
import com.funambol.util.Log;
import com.funambol.util.StringUtil;
import com.funambol.util.ConnectionManager;

import com.funambol.org.json.me.JSONArray;
import com.funambol.org.json.me.JSONException;
import com.funambol.org.json.me.JSONObject;

public abstract class MediaRobot extends Robot {

    private static final String TAG_LOG = "MediaRobot";

    protected AppSyncSourceManager appSourceManager;
    protected TestFileManager fileManager;

    protected SapiSyncHandler sapiSyncHandler = null;

    public MediaRobot(AppSyncSourceManager appSourceManager,
            TestFileManager fileManager) {
        this.appSourceManager = appSourceManager;
        this.fileManager = fileManager;
    }

    public MediaRobot(TestFileManager fileManager) {
        this.fileManager = fileManager;
    }

    public abstract void addMedia(String type, String filename) throws Throwable;
    public abstract void deleteMedia(String type, String filename) throws Throwable;
    public abstract void deleteAllMedia(String type) throws Throwable;
    public abstract void overrideMediaContent(String type, String targetFileName, String sourceFileName) throws Throwable;

    /**
     * Add a media to the server. Media content is read based on specified
     * file name (a file in application's assets or an online resource).
     * 
     * @param type sync source type
     * @param filename a relative path to the resource. BaseUrl parameter on test
     *                 configuration defines if the resource is a local file or is
     *                 an URL. In both cases, file is copied locally and then
     *                 uploaded to the server.
     * @throws Throwable
     */
    public void addMediaOnServer(String type, String filename) throws Throwable {

        //make file available on local storage
        ByteArrayOutputStream os = new ByteArrayOutputStream();
        String contentType = getMediaFile(filename, os);

        byte[] fileContent = os.toByteArray();
        int size = fileContent.length;
        InputStream is = new ByteArrayInputStream(fileContent);
        
        addMediaOnServerFromStream(type, filename, is, size, contentType, null);
    }

    public void overrideMediaContentOnServer(String type, String targetFileName, String sourceFileName) throws Throwable {
        String itemId = findMediaIdOnServer(type, targetFileName);

        ByteArrayOutputStream os = new ByteArrayOutputStream();
        String contentType = getMediaFile(sourceFileName, os);

        byte[] fileContent = os.toByteArray();
        int size = fileContent.length;
        InputStream is = new ByteArrayInputStream(fileContent);
        
        addMediaOnServerFromStream(type, targetFileName, is, size, contentType, itemId);
    }

    /**
     * Add a media to the server. Media content is read from a stream.
     * 
     * @param type sync source type
     * @param itemName name of the item to add
     * @param contentStream stream to the content of the item
     * @param contentSize size of the item
     * @param contentType mimetype of the content to add
     * 
     * @throws JSONException
     */
    protected void addMediaOnServerFromStream(String type, String itemName, InputStream contentStream,
                                              long contentSize, String contentType, String guid)
    throws JSONException {

        itemName = itemName.substring(itemName.lastIndexOf('/') + 1);
        if (Log.isLoggable(Log.DEBUG)) {
            Log.debug(TAG_LOG,
                    "Adding/updating media on server for source " + getRemoteUri(type) +
                    " with name " + itemName +
                    " of type " + contentType +
                    " and size " + contentSize);
        }
        
        // Prepare json item to upload
        JSONFileObject jsonFileObject = new JSONFileObject();
        jsonFileObject.setName(itemName);
        jsonFileObject.setSize(contentSize);
        jsonFileObject.setCreationdate(System.currentTimeMillis());
        jsonFileObject.setLastModifiedDate(System.currentTimeMillis());
        jsonFileObject.setMimetype(contentType);

        MediaSyncItem item = new MediaSyncItem("fake_key", "fake_type",
                guid != null ? SyncItem.STATE_UPDATED : SyncItem.STATE_NEW, null, jsonFileObject,
                contentStream, contentSize);

        if (guid != null) {
            item.setGuid(guid);
        }

        SapiSyncHandler sapiHandler = getSapiSyncHandler();
        sapiHandler.login(null);
        String remoteKey = sapiHandler.prepareItemUpload(item, getRemoteUri(type));
        item.setGuid(remoteKey);
        sapiHandler.uploadItem(item, getRemoteUri(type), null);
        sapiHandler.logout();
    }

    public void deleteMediaOnServer(String type, String filename)
            throws Throwable {
        SapiSyncHandler sapiHandler = getSapiSyncHandler();
        String itemId = findMediaIdOnServer(type, filename);
        sapiHandler.login(null);
        sapiHandler.deleteItem(itemId, getRemoteUri(type), getDataTag(type));
        sapiHandler.logout();
    }

    private String findMediaIdOnServer(String type, String filename)
            throws Throwable {
        JSONObject item = findMediaJSONObjectOnServer(type, filename);
        if(item != null) {
            return item.getString("id");
        }
        return null;
    }

    private JSONObject findMediaJSONObjectOnServer(String type, String filename)
            throws Throwable {
        SapiSyncHandler sapiHandler = getSapiSyncHandler();
        sapiHandler.login(null);
        try {
            SapiSyncHandler.FullSet itemsSet = sapiHandler.getItems(
                    getRemoteUri(type), getDataTag(type), null, null, null,
                    null);
            JSONArray items = itemsSet.items;
            for (int i = 0; i < items.length(); ++i) {
                JSONObject item = items.getJSONObject(i);
                String aFilename = item.getString("name");
                if (filename.equals(aFilename)) {
                    return item;
                }
            }
        } finally {
            sapiHandler.logout();
        }
        return null;
    }

    public void checkMediaCountOnServer(String type, int count) throws Throwable
    {
        SapiSyncHandler sapiHandler = getSapiSyncHandler();
        sapiHandler.login(null);
        int actualCount = sapiHandler.getItemsCount(getRemoteUri(type), null);
        sapiHandler.logout();
        assertTrue(actualCount == count, "Items count on server mismatch for " + type);
    }

    public void deleteAllMediaOnServer(String type) throws Throwable {
        SapiSyncHandler sapiHandler = getSapiSyncHandler();
        sapiHandler.login(null);
        sapiHandler.deleteAllItems(getRemoteUri(type));
        sapiHandler.logout();
    }

    /**
     * Fills the server with some data in order to leave no more space for a
     * further upload of the same file specified as parameter  
     * 
     * @param type
     * @param fileName name of the file to use as a reference
     * @throws Throwable
     */
    public void leaveNoFreeServerQuota(String type, String fileName)
            throws Throwable {
        // get free quota for the current user
        SapiSyncHandler sapiHandler = getSapiSyncHandler();
        sapiHandler.login(null);
        long availableSpace = sapiHandler
                .getUserAvailableServerQuota(getRemoteUri(type));
        sapiHandler.logout();
        
        //make file available on local storage
        ByteArrayOutputStream os = new ByteArrayOutputStream();
        String contentType = getMediaFile(fileName, os);
        byte[] fileContent = os.toByteArray();
        int pictureSize = fileContent.length;
        long repetitions = availableSpace / pictureSize;
        
        if (repetitions > 0) {
            if (Log.isLoggable(Log.DEBUG)) {
                Log.debug(TAG_LOG, "Available user quota is " + availableSpace + ", while picture size is " + pictureSize);
                Log.debug(TAG_LOG, "Filling the user quota, please wait...");
            }
            for (long i=1; i <= repetitions; i++) {
                String newFileName = i + fileName;
                Log.trace(TAG_LOG, "Upload file " + newFileName + " on server [" + i + "/" + repetitions + "]");
                InputStream is = new ByteArrayInputStream(fileContent);
                addMediaOnServerFromStream(type, i + newFileName, is, pictureSize, contentType, null);
            }
        } else {
            if (Log.isLoggable(Log.DEBUG)) {
                Log.debug(TAG_LOG, "Available user quota is " + availableSpace + ", less that the required. Nothing to do.");
            }
        }
    }

    public void interruptItem(String phase, String itemKey, int pos, int itemIdx) {
        ConnectionManager.getInstance().setBreakInfo(phase, itemKey, pos, itemIdx);
    }

    protected AppSyncSourceManager getAppSyncSourceManager() {
        return appSourceManager;
    }


    /**
     * Returns the AppSyncSource related to the given data type
     * 
     * @param type
     * @return
     */
    protected AppSyncSource getAppSyncSource(String type) {
        if (StringUtil.equalsIgnoreCase(
                BasicUserCommands.SOURCE_NAME_PICTURES, type)) {
            return getAppSyncSourceManager().getSource(
                    AppSyncSourceManager.PICTURES_ID);
        } else if (StringUtil.equalsIgnoreCase(
                BasicUserCommands.SOURCE_NAME_VIDEOS, type)) {
            return getAppSyncSourceManager().getSource(
                    AppSyncSourceManager.VIDEOS_ID);
        } else if (StringUtil.equalsIgnoreCase(
                BasicUserCommands.SOURCE_NAME_FILES, type)) {
            return getAppSyncSourceManager().getSource(
                    AppSyncSourceManager.FILES_ID);
        } else {
            throw new IllegalArgumentException("Invalid type: " + type);
        }
    }

    /**
     * Returns the SAPI data tag related to the given data type.
     * 
     * @param type
     * @return
     */
    private String getRemoteUri(String type) {
        return getAppSyncSource(type).getSyncSource().getConfig()
                .getRemoteUri();
    }

    private String getDataTag(String type) {
        SyncSource src = getAppSyncSource(type).getSyncSource();
        String dataTag = null;
        if (src instanceof JSONSyncSource) {
            JSONSyncSource jsonSyncSource = (JSONSyncSource) src;
            dataTag = jsonSyncSource.getDataTag();
        }
        return dataTag;
    }

    private SapiSyncHandler getSapiSyncHandler() {
        if (sapiSyncHandler == null) {
            SyncConfig syncConfig = getSyncConfig();
            sapiSyncHandler = new SapiSyncHandler(
                    StringUtil.extractAddressFromUrl(syncConfig.getSyncUrl()),
                    syncConfig.getUserName(), syncConfig.getPassword());
        }
        return sapiSyncHandler;
    }

    /**
     * This is used to override the item input stream
     */
    private class MediaSyncItem extends JSONSyncItem {

        private InputStream stream;
        private long size;

        public MediaSyncItem(String key, String type, char state,
                String parent, JSONFileObject jsonFileObject,
                InputStream stream, long size) throws JSONException {
            super(key, type, state, parent, jsonFileObject);
            this.stream = stream;
            this.size = size;
        }

        public InputStream getInputStream() throws IOException {
            return stream;
        }

        public long getObjectSize() {
            return size;
        }
    }

    protected String getMediaFile(String filename, OutputStream output)
            throws Throwable {
        String baseUrl = BasicScriptRunner.getBaseUrl();
        String url = baseUrl + "/" + filename;
        return fileManager.getFile(url, output);
    }

    protected abstract void fillLocalStorage();

    protected abstract void restoreLocalStorage();

    public abstract void checkMediaCount(String type, int count)
            throws Throwable;

    protected abstract SyncConfig getSyncConfig();

    /**
     * Creates a temporary file of specified size
     * 
     * @param byteSize size of the file
     * @param header header of the file
     * @param footer footer of the file
     * 
     * @return name of the file created
     * @throws IOException
     */
    protected abstract void createFileWithSizeOnDevice(long byteSize, String header, String footer)
            throws IOException;


    /**
     * Creates a file in mediahub directory
     * 
     * @param fileName
     * @param fileSize
     * @throws IOException
     */
    public abstract void createFile(String fileName, long fileSize)
            throws Exception;

    /**
     * Renames a file in mediahub directory
     *
     * @param oldFileName
     * @param newFileName
     * @throws IOException
     */
    public abstract void renameFile(String oldFileName, String newFileName)
            throws Exception;

    /**
     * Renames a file in the server
     *
     * @param oldFileName
     * @param newFileName
     * @throws IOException
     */
    public void renameFileOnServer(String oldFileName, String newFileName)
            throws Throwable {
        
        String type = BasicUserCommands.SOURCE_NAME_FILES;
        SapiSyncHandler sapiHandler = getSapiSyncHandler();

        String itemId = findMediaIdOnServer(type, oldFileName);

        sapiHandler.login(null);
        sapiHandler.updateItemName(getRemoteUri(type), itemId, newFileName);
        sapiHandler.logout();
    }
    
    /**
     * Checks the integrity of a file content on both client and server
     * @param filename
     * @throws Throwable
     */
    public void checkFileContentIntegrity(String filename) throws Throwable {
        
        String type = BasicUserCommands.SOURCE_NAME_FILES;

        JSONObject localItem = findMediaJSONObject(filename);
        JSONObject serverItem = findMediaJSONObjectOnServer(type, filename);

        assertTrue(localItem.getString("name"), serverItem.getString("name"),
                "Item name mismatch");
        assertTrue(localItem.getLong("size"), serverItem.getLong("size"),
                "Item size mismatch");
    }

    /**
     * Retrieves a JSONObject which describes the media item, given the file name
     * @param filename
     * @return
     * @throws Exception
     */
    protected abstract JSONObject findMediaJSONObject(String filename) throws Exception;

}

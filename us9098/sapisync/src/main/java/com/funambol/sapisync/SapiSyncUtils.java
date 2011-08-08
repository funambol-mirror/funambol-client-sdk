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

import java.util.Enumeration;
import java.io.OutputStream;
import java.io.IOException;

import com.funambol.org.json.me.JSONArray;
import com.funambol.org.json.me.JSONException;
import com.funambol.org.json.me.JSONObject;
import com.funambol.sync.SyncException;
import com.funambol.sync.SyncSource;
import com.funambol.sync.SyncItem;
import com.funambol.sapisync.source.JSONSyncSource;
import com.funambol.sapisync.source.JSONSyncItem;
import com.funambol.util.StringUtil;
import com.funambol.util.Log;

/**
 * <code>SapiSyncManager</code> represents the synchronization engine performed
 * via SAPI.
 */
class SapiSyncUtils {

    private static final String TAG_LOG = "SapiSyncUtils";

    /**
     * From a {@link SapiException} returns corresponding {@link SyncException}
     * @param sapiException exception to analyze
     * @param newErrorMessage error message for the exception
     * @param throwGenericException thrown a generic exception if a specific one
     *                              is not detected 
     * 
     * @throws SyncException
     */
    public void processCommonSapiExceptions(
            SapiException sapiException,
            String newErrorMessage,
            boolean throwGenericException) throws SyncException {
        
        if (null == sapiException) {
            return;
        }
        
        String genericErrorMessage = null;
        if (StringUtil.isNullOrEmpty(genericErrorMessage)) {
            genericErrorMessage = newErrorMessage;
        }
        if (StringUtil.isNullOrEmpty(genericErrorMessage)) {
            genericErrorMessage = "Generic server error";
        }
        
        //Referring to section 4.1.3 of "Funambol Server API Developers Guide" document
        if (SapiException.NO_CONNECTION.equals(sapiException.getCode()) || 
            SapiException.HTTP_400.equals(sapiException.getCode())) {
            throw new SyncException(
                    SyncException.CONN_NOT_FOUND,
                    StringUtil.isNullOrEmpty(newErrorMessage)
                            ? sapiException.getMessage()
                            : newErrorMessage);
        } else if (SapiException.PAPI_0000.equals(sapiException.getCode())) {
            throw new SyncException(
                    SyncException.SERVER_ERROR,
                    genericErrorMessage);
        } else if (SapiException.SEC_1002.equals(sapiException.getCode())) {
            throw new SyncException(
                    SyncException.AUTH_ERROR,
                    StringUtil.isNullOrEmpty(newErrorMessage)
                            ? sapiException.getMessage()
                            : newErrorMessage);
        } else if (SapiException.SEC_1004.equals(sapiException.getCode())) {
            throw new SyncException(
                    SyncException.AUTH_ERROR,
                    StringUtil.isNullOrEmpty(newErrorMessage)
                            ? sapiException.getMessage()
                            : newErrorMessage);
        } else if (SapiException.SEC_1001.equals(sapiException.getCode())) {
            throw new SyncException(
                    SyncException.AUTH_ERROR,
                    StringUtil.isNullOrEmpty(newErrorMessage)
                            ? sapiException.getMessage()
                            : newErrorMessage);
        } else if (SapiException.SEC_1003.equals(sapiException.getCode())) {
            throw new SyncException(
                    SyncException.AUTH_ERROR,
                    StringUtil.isNullOrEmpty(newErrorMessage)
                            ? sapiException.getMessage()
                            : newErrorMessage);
        } else if (SapiException.CUS_0003.equals(sapiException.getCode())) {
            throw new SyncException(SyncException.NOT_SUPPORTED,
                    sapiException.getMessage());
        }

        if (throwGenericException) {
            throw new SyncException(
                    SyncException.SERVER_ERROR,
                    genericErrorMessage);
        }
        //SAPI specific errors must be handled by calling method
    }

    public void processCustomSapiExceptions(
            SapiException sapiException,
            String newErrorMessage,
            boolean throwGenericException) throws SyncException {

        String genericErrorMessage = null;
        if (StringUtil.isNullOrEmpty(genericErrorMessage)) {
            genericErrorMessage = newErrorMessage;
        }
        if (StringUtil.isNullOrEmpty(genericErrorMessage)) {
            genericErrorMessage = "Generic server error";
        }
        
        if (null == sapiException) {
            return;
        }
        if (SapiException.CUS_0003.equals(sapiException.getCode())) {
            throw new SyncException(SyncException.NOT_SUPPORTED,
                    sapiException.getMessage());
        }

        if (throwGenericException) {
            throw new SyncException(
                    SyncException.SERVER_ERROR,
                    genericErrorMessage);
        }
    }

    public JSONSyncItem createSyncItem(SyncSource src, String luid, char state, 
            long size, JSONObject item, String serverUrl) throws JSONException {

        JSONSyncItem syncItem = null;
        if(src instanceof JSONSyncSource) {
            syncItem = (JSONSyncItem) ((JSONSyncSource)src).createSyncItem(
                    luid, src.getType(), state, null, item, serverUrl);
        } else {
            // A generic sync item needs to be filled with the json item content
            syncItem = (JSONSyncItem)src.createSyncItem(luid, src.getType(), state, null, size);
            OutputStream os = null;
            try {
                os = syncItem.getOutputStream();
                os.write(item.toString().getBytes());
                os.close();
            } catch (IOException ioe) {
                Log.error(TAG_LOG, "Cannot write into sync item stream", ioe);
                // Ignore this item and continue
            } finally {
                try {
                    if (os != null) {
                        os.close();
                    }
                } catch (IOException ioe) {
                }
            }
        }
        return syncItem;
    }

    public String getDataTag(SyncSource src) {
        String dataTag = null;
        if (src instanceof JSONSyncSource) {
            JSONSyncSource jsonSyncSource = (JSONSyncSource)src;
            dataTag = jsonSyncSource.getDataTag();
        }
        if (dataTag == null) {
            // This is the default value
            dataTag = src.getConfig().getRemoteUri() + "s";
        }
        return dataTag;
    }


}



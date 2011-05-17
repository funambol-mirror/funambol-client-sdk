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

package com.funambol.syncml.spds;

import java.io.DataInputStream;
import java.io.DataOutputStream;
import java.io.IOException;
import java.util.Vector;

import com.funambol.syncml.protocol.DataStore;
import com.funambol.sync.SourceConfig;

public class SyncMLSourceConfig extends SourceConfig {

    private static final String TAG_LOG = "SyncMLSourceConfig";

    private DataStore dataStore;
    private Vector devInfExts;

    //------------------------------------------------------------- Constructors

    /** Hash of last DevInf sent to server. */
    private String lastDevInfHash;
    
    /**
     * Creates a new source configuration. This is the configuration of
     * the source, so it holds all of its properties.
     * By default the source encoding is set to base64, use the setEncoding
     * method to override this property.
     *
     * @param name the source name
     * @param type the source mime type
     * @param remoteUri the remote uri
     */
    public SyncMLSourceConfig(String name, String type, String remoteUri) {
        super(name, type, remoteUri);
    }

    /**
     * Creates a new source configuration. This is the configuration of
     * the source, so it holds all of its properties.
     * By default the source encoding is set to base64, use the setEncoding
     * method to override this property.
     *
     * @param name the source name
     * @param type the source mime type
     * @param remoteUri the remote uri
     */
    public SyncMLSourceConfig(String name, String type, String remoteUri, DataStore dataStore) {
        super(name, type, remoteUri);
        this.dataStore = dataStore;
    }
    
    public void serialize(DataOutputStream out) throws IOException {
        super.serialize(out);
        if (lastDevInfHash != null) {
            out.writeUTF(lastDevInfHash);
        } else {
            out.writeUTF("");
        }
    }
    
    public void deserialize(DataInputStream in) throws IOException {
        super.deserialize(in);
        try {
            lastDevInfHash = in.readUTF();
        } catch(IOException e) {
            //previous version of input stream, no problem
            lastDevInfHash = "";
        }
    }
    
    /**
     * This method returns the source DataStore. This store describe the source
     * capabilities from a SyncML standpoint and this info is used to build the
     * client device capabilities. If no DataStore is returned, the API will
     * build a default basic devinf (@see SyncMLFormatter)
     */
    public DataStore getDataStore() {
        return dataStore;
    }

    public void setDevInfExts(Vector exts) {
        this.devInfExts = exts;
    }

    public Vector getDevInfExts() {
        return devInfExts;
    }
    
    /**
     * Gets the value of last DevInf hash sent to server
     * @return
     */
    public String getLastDevInfHash() {
        return lastDevInfHash;
    }
    /**
     * Sets the value of last DevInf hash sent to server
     */
    public void setLastDevInfHash(String newValue) {
        this.lastDevInfHash = newValue;
    }
    
}

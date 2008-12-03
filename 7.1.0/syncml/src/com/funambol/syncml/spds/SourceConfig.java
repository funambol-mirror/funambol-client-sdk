/*
 * Funambol is a mobile platform developed by Funambol, Inc. 
 * Copyright (C) 2003 - 2007 Funambol, Inc.
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

import java.io.DataOutputStream;
import java.io.DataInputStream;
import java.io.IOException;
import com.funambol.storage.Serializable;
import com.funambol.util.Log;

import com.funambol.syncml.protocol.SyncML;

/**
 * Configuration data for the SyncSource. Can be extended by subclasses
 * of SyncSource to add specific data.
 */
public class SourceConfig implements Serializable {

    //--------------------------------------------------------------- Constants

    // Definitions for the names of the sources available in a standard
    // Funambol installation. A client can implement different sources too.
    //
    public static final String MAIL      = "mail";
    public static final String CONTACT   = "contact";
    public static final String TASK      = "task";
    public static final String CALENDAR  = "calendar";
    public static final String NOTE      = "note";
    public static final String BRIEFCASE = "briefcase";

    //Specific SyncSource Related Properties
    /**vCard source name definition */
    public static final String VCARD_NAME = "card";
    /** vCard mime type definition*/
    public static final String VCARD_TYPE = "text/x-vcard";
    /**eMail Object source name definition */
    public static final String EMAIL_OBJECT_NAME = "mail";
    /**eMail Object mime type definition */
    public static final String EMAIL_OBJECT_TYPE = "application/vnd.omads-email+xml";

    
    // This field contains a version number of the configuration data
    protected static final int VERSION = 600 ;
    
    //-------------------------------------------------------------- Attributes
    //Parameter to store the configuration version
    private int version ;

    /** The name of this source. */
    private String name;
    
    /** The mime-type of the items for this source */
    private String type;

    /** 
     * The encoding of the items for this source
     */
    private String encoding;

    /**
     * Sync Mode for this source (it is the initial alert code).
     */
    private int syncMode;

    /** The remote URI on the SyncML server */
    private String remoteUri;

    /** The last anchor for this source */
    private long lastAnchor;
    
    /** The next anchor for this source */
    private long nextAnchor;
    
    //------------------------------------------------------------- Constructors
    
    /**
     * Initializes a new instance of SourceConfig:
     * Sets default configuration values, valid for a generic briefcase source.
     */
    public SourceConfig() {
            //Server Auth. params
            version = VERSION;

            name = BRIEFCASE ;   
            type = "application/*";
            encoding = SyncSource.ENCODING_B64;
            syncMode = SyncML.ALERT_CODE_FAST;
            remoteUri = "briefcase";
            lastAnchor = 0;
            nextAnchor = 0;
    }
    
    /**
     * Initializes a new instance of SourceConfig:
     * Sets default configuration values, valid for a generic briefcase source.
     */
    public SourceConfig(String name, String type, String remoteUri) {
            //Server Auth. params
            version = VERSION;

            this.name = name;   
            this.type = type;
            this.encoding = SyncSource.ENCODING_B64;
            this.syncMode = SyncML.ALERT_CODE_FAST;
            this.remoteUri = remoteUri;
            lastAnchor = 0;
            nextAnchor = 0;
    }
    
    

    //----------------------------------------------------------- Public Methods
    
    /** Return the name of this source */
    public String getName() {
        return name;
    }

    /** Set the name of this source */
    public void setName(String name) {
        this.name = name;
    }

    /** Return the mime-type of this source */
    public String getType() {
        return type;
    }

    /** Set the mime-type of this source */
    public void setType(String type) {
        this.type = type;
    }

    /** Return the encoding of this source */
    public String getEncoding() {
        return encoding;
    }

    /** Set the encoding of this source */
    public void setEncoding(String enc) {
        this.encoding = enc;
    }

    /** Return the sync mode of this source */
    public int getSyncMode() {
        return syncMode;
    }

    /** Set the sync mode of this source */
    public void setSyncMode(int syncMode) {
        this.syncMode = syncMode;
    }
    
    /** Return the remote URI of this source */
    public String getRemoteUri() {
        return remoteUri;
    }

    /** Set the remote URI of this source */
    public void setRemoteUri(String remoteUri) {
        this.remoteUri = remoteUri;
    }

    /** Return the last anchor of this source */
    public long getLastAnchor() {
        return lastAnchor;
    }

    /** Set the last anchor of this source */
    public void setLastAnchor(long anchor) {
        Log.debug("[Sourceconfig] [ANCHOR]" + name + " setting last anchor to " + anchor);
        this.lastAnchor = anchor;
    }

    /** Return the next anchor of this source */
    public long getNextAnchor() {
        return nextAnchor;
    }

    /** Set the next anchor of this source */
    public void setNextAnchor(long anchor) {
        Log.debug("[Sourceconfig] [ANCHOR] "+ name +" setting next anchor to " + anchor);
        this.nextAnchor = anchor;
    }

    /**
     * Write object fields to the output stream.
     * @param out Output stream
     * @throws IOException
     */
    public void serialize(DataOutputStream out) throws IOException {
        Log.debug("[Sourceconfig] [ANCHOR] "+ name +" serializing. " +
                "LastAnchor = " + lastAnchor + 
                " NextAnchor = " + nextAnchor );
            out.writeInt(version);
            out.writeUTF(name);
            out.writeUTF(type);
            out.writeUTF(encoding);
            out.writeInt(syncMode);
            out.writeUTF(remoteUri);
            out.writeLong(lastAnchor);
            out.writeLong(nextAnchor);
    }

    /**
     * Read object field from the input stream.
     * @param in Input stream
     * @throws IOException
     */
    public void deserialize(DataInputStream in) throws IOException {
            int savedVer = in.readInt();
            if (savedVer != VERSION) {
                Log.error("Invalid source config version.");
                // TODO: Handle backward compatibilty
                return;
            }

            version = savedVer;
            name = in.readUTF();
            type = in.readUTF();
            encoding = in.readUTF();
            syncMode = in.readInt();
            remoteUri = in.readUTF();
            lastAnchor = in.readLong();
            nextAnchor = in.readLong();
    }
    
}

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

package com.funambol.syncml.protocol;

/**
 * Common definitions for SyncML protocol.
 */
public class SyncML {
    
    //---------------------------------------------------------------- Constants
    
    // SyncML tags
    public static final String TAG_ALERT            = "Alert"         ;
    public static final String TAG_ADD              = "Add"           ;
    public static final String TAG_CMD              = "Cmd"           ;
    public static final String TAG_CMDID            = "CmdID"         ;
    public static final String TAG_CMDREF           = "CmdRef"        ;
    public static final String TAG_DATA             = "Data"          ;
    public static final String TAG_MORE_DATA        = "MoreData"      ;
    public static final String TAG_DELETE           = "Delete"        ;
    public static final String TAG_FORMAT           = "Format"        ;
    public static final String TAG_ITEM             = "Item"          ;
    public static final String TAG_LOC_URI          = "LocURI"        ;
    public static final String TAG_LOC_NAME         = "LocName"       ;
    public static final String TAG_MSGID            = "MsgID"         ;
    public static final String TAG_MSGREF           = "MsgRef"        ;
    public static final String TAG_REPLACE          = "Replace"       ;
    public static final String TAG_MAP              = "Map"           ;
    public static final String TAG_PUT              = "Put"           ;
    public static final String TAG_SOURCE           = "Source"        ;
    public static final String TAG_SOURCE_PARENT    = "SourceParent"  ;
    public static final String TAG_SOURCEREF        = "SourceRef"     ;
    public static final String TAG_STATUS           = "Status"        ;
    public static final String TAG_SYNC             = "Sync"          ;
    public static final String TAG_SYNCBODY         = "SyncBody"      ;
    public static final String TAG_SYNCHDR          = "SyncHdr"       ;
    public static final String TAG_SYNCML           = "SyncML"        ;
    public static final String TAG_TARGET           = "Target"        ;
    public static final String TAG_TARGETREF        = "TargetRef"     ;
    public static final String TAG_TARGET_PARENT    = "TargetParent"  ;
    public static final String TAG_TYPE             = "Type"          ;
    public static final String TAG_META             = "Meta"          ;
    public static final String TAG_METAINF          = "MetaInf"       ;
    public static final String TAG_LOCURI           = "LocURI"        ;
    public static final String TAG_LOCNAME          = "LocName"       ;
    public static final String TAG_DEVINF           = "DevInf"        ;
    public static final String TAG_VERDTD           = "VerDTD"        ;
    public static final String TAG_DEVINFMAN        = "Man"           ;
    public static final String TAG_DEVINFMOD        = "Mod"           ;
    public static final String TAG_DEVINFOEM        = "OEM"           ;
    public static final String TAG_DEVINFFWV        = "FwV"           ;
    public static final String TAG_DEVINFSWV        = "SwV"           ;
    public static final String TAG_DEVINFHWV        = "HwV"           ;
    public static final String TAG_DEVINFDEVID      = "DevID"         ;
    public static final String TAG_DEVINFDEVTYP     = "DevTyp"        ;
    public static final String TAG_DEVINFUTC        = "UTC"           ;
    public static final String TAG_DEVINFLO         = "SupportLargeObjs";
    public static final String TAG_DEVINFNC         = "SupportNumberOfChanges";
    public static final String TAG_DEVINFDATASTORE  = "DataStore";
    public static final String TAG_DATASTOREHS      = "SupportHierarchicalSync";
    public static final String TAG_DISPLAYNAME      = "DisplayName";
    public static final String TAG_MAXGUIDSIZE      = "MaxGUIDSize";
    public static final String TAG_RX               = "Rx";
    public static final String TAG_RXPREF           = "Rx-Pref";
    public static final String TAG_TX               = "Tx";
    public static final String TAG_TXPREF           = "Tx-Pref";
    public static final String TAG_CTTYPE           = "CTType";
    public static final String TAG_VERCT            = "VerCT";
    public static final String TAG_SYNCCAP          = "SyncCap";
    public static final String TAG_SYNCTYPE         = "SyncType";
    public static final String TAG_EXT              = "Ext";
    public static final String TAG_XNAM             = "XNam";
    public static final String TAG_XVAL             = "XVal";
    public static final String TAG_RESULTS          = "Results";
    public static final String TAG_CTCAP            = "CTCap";
    public static final String TAG_PROPERTY         = "Property";
    public static final String TAG_PROPNAME         = "PropName";
    public static final String TAG_MAXSIZE          = "MaxSize";
    public static final String TAG_MAXOCCUR         = "MaxOccur";
    public static final String TAG_DATATYPE         = "DataType";
    public static final String TAG_VALENUM          = "ValEnum";
    public static final String TAG_PROPPARAM        = "PropParam";
    public static final String TAG_PARAMNAME        = "ParamName";
    public static final String TAG_DSMEM            = "DSMem";
    public static final String TAG_SHAREDMEM        = "SharedMem";
    public static final String TAG_MAXMEM           = "MaxMem";
    public static final String TAG_MAXID            = "MaxID";
    public static final String TAG_VERPROTO         = "VerProto";
    public static final String TAG_SESSIONID        = "SessionID";
    public static final String TAG_LAST             = "Last";
    public static final String TAG_NEXT             = "Next";
    public static final String TAG_ANCHOR           = "Anchor";
    public static final String TAG_NUMBEROFCHANGES  = "NumberOfChanges";
    public static final String TAG_FINAL            = "Final";
    public static final String TAG_NEXTNONCE        = "NextNonce";
    public static final String TAG_CHAL             = "Chal";
    public static final String TAG_RESPURI          = "RespURI";
    public static final String TAG_SIZE             = "Size";
    public static final String TAG_NORESP           = "NoResp";
    public static final String TAG_GET              = "Get";
    public static final String TAG_CRED             = "Cred";
    public static final String TAG_LANG             = "Lang";
    public static final String TAG_MAXMSGSIZE       = "MaxMsgSize";
    public static final String TAG_MAXOBJSIZE       = "MaxObjSize";
    public static final String TAG_MAPITEM          = "MapItem";
    public static final String TAG_NEXT_NONCE       = "NextNonce";
    public static final String TAG_VERSION          = "Version";

    public static final String DEVINF12             = "./devinf12";

    // Authentication types
    public static final String AUTH_TYPE_MD5        = "syncml:auth-md5"  ;
    public static final String AUTH_TYPE_BASIC      = "syncml:auth-basic";
    public static final String AUTH_TYPE_HMAC       = "syncml:auth-MAC"  ;
    public static final String AUTH_NONE            = "none"             ;
    
    //Alert Codes
    public static final int ALERT_CODE_NONE                 = 0       ;
    public static final int ALERT_CODE_FAST                 = 200     ;
    public static final int ALERT_CODE_SLOW                 = 201     ;
    public static final int ALERT_CODE_ONE_WAY_FROM_CLIENT  = 202     ;
    public static final int ALERT_CODE_REFRESH_FROM_CLIENT  = 203     ;
    public static final int ALERT_CODE_ONE_WAY_FROM_SERVER  = 204     ;
    public static final int ALERT_CODE_REFRESH_FROM_SERVER  = 205     ;
    public static final int ALERT_CODE_TWO_WAY_BY_SERVER    = 206     ;
    public static final int ALERT_CODE_ONE_WAY_FROM_CLIENT_BY_SERVER = 207;
    public static final int ALERT_CODE_REFRESH_FROM_CLIENT_BY_SERVER = 208;
    public static final int ALERT_CODE_ONE_WAY_FROM_SERVER_BY_SERVER = 209;
    public static final int ALERT_CODE_REFRESH_FROM_SERVER_BY_SERVER = 210;
    public static final int ALERT_CODE_NEXT_MESSAGE                  = 222;
    public static final int ALERT_CODE_RESUME                        = 225;
    public static final int ALERT_CODE_SUSPEND                       = 224;

    //Extended alert codes defined by Funambol for custom sync
    public static final int ALERT_CODE_ONE_WAY_FROM_CLIENT_NO_SLOW = 250     ;
            
            
    //------------------------------------------------------------- Private Data
    private SyncHdr  header;
    private SyncBody body;
            
    //--------------------------------------------------------------- Properties
            
    //------------------------------------------------------------- Constructors
    public SyncML() {}
            
    /**
     * Creates a new SyncML object from header and body.
     *
     * @param header the SyncML header - NOT NULL
     * @param body the SyncML body - NOT NULL
     *
     */
    public SyncML(final SyncHdr  header,
                  final SyncBody body) {
        setSyncHdr(header);
        setSyncBody(body);
    }

    // ---------------------------------------------------------- Public methods

    /**
     * Returns the SyncML header
     *
     * @return the SyncML header
     *
     */
    public SyncHdr getSyncHdr() {
        return header;
    }

    /**
     * Sets the SyncML header
     *
     * @param header the SyncML header - NOT NULL
     *
     * @throws IllegalArgumentException if header is null
     */
    public void setSyncHdr(SyncHdr header) {
        if (header == null) {
            throw new IllegalArgumentException("header cannot be null");
        }
        this.header = header;
    }

    /**
     * Returns the SyncML body
     *
     * @return the SyncML body
     *
     */
    public SyncBody getSyncBody() {
        return body;
    }

    /**
     * Sets the SyncML body
     *
     * @param body the SyncML body - NOT NULL
     *
     * @throws IllegalArgumentException if body is null
     */
    public void setSyncBody(SyncBody body) {
        if (body == null) {
            throw new IllegalArgumentException("body cannot be null");
        }
        this.body = body;
    }

    /**
     * Is this message the last one of the package?
     *
     * @return lastMessage
     */
    public boolean isLastMessage() {
        return body.isFinalMsg();
    }

    /**
     * Sets lastMessage
     *
     * @param lastMessage the new lastMessage value
     *
     */
    public void setLastMessage() {
        body.setFinalMsg(new Boolean(true));
    }

    
    //-----------------------------------------------------------Private Methods
    
}


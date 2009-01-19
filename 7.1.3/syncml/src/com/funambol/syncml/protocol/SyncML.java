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
    public static final String TAG_DELETE           = "Delete"        ;
    public static final String TAG_FORMAT           = "Format"        ;
    public static final String TAG_ITEM             = "Item"          ;
    public static final String TAG_LOC_URI          = "LocURI"        ;
    public static final String TAG_MSGID            = "MsgID"         ;
    public static final String TAG_MSGREF           = "MsgRef"        ;
    public static final String TAG_SOURCE_PARENT    = "SourceParent"  ;
    public static final String TAG_TARGET_PARENT    = "TargetParent"  ;
    public static final String TAG_REPLACE          = "Replace"       ;
    public static final String TAG_SOURCEREF        = "SourceRef"     ;
    public static final String TAG_STATUS           = "Status"        ;
    public static final String TAG_SYNC             = "Sync"          ;
    public static final String TAG_SYNCBODY         = "SyncBody"      ;
    public static final String TAG_SYNCHDR          = "SyncHdr"       ;
    public static final String TAG_SYNCML           = "SyncML"        ;
    public static final String TAG_TARGET           = "Target"        ;
    public static final String TAG_TARGETREF        = "TargetRef"     ;
    public static final String TAG_TYPE             = "Type"          ;
    
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
            
            
    //------------------------------------------------------------- Private Data
            
    //--------------------------------------------------------------- Properties
            
    //------------------------------------------------------------- Constructors
            
    /**
     * No instances of this class
     */
    private SyncML() {

    }
    
    //----------------------------------------------------------- Public Methods
    
    //-----------------------------------------------------------Private Methods
    
}


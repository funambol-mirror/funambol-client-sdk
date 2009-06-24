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

package com.funambol.syncclient.spds.event;

import com.funambol.syncclient.spds.engine.SyncItemKey;

/**
 * Is used to notify the listeners
 * received a status command.
 *
 *
 *
 * @version $Id: SyncStatusEvent.java,v 1.3 2007-12-22 18:09:18 nichele Exp $
 */
public class SyncStatusEvent {

    // -------------------------------------------------------- Constants

    /** Create status to send to the server */
    public static final int STATUS_TO_SEND      = 0  ;

    /** Received status from the server */
    public static final int STATUS_RECEIVED     = 1  ;


    // -------------------------------------------------------- Private data

    private int          type       = 0    ;
    private String       command    = null ;
    private int          statusCode = 0    ;
    private SyncItemKey  itemKey    = null ;
    private String       sourceUri  = null ;

    // -------------------------------------------------------- Constructors

    /**
     * Creates a SyncStatusEvent
     *
     * @param type the event type
     * @param command the command the status relates to
     * @param statusCode the status code
     * @param itemKey the key of the item this status relates to
     * @param sourceUri the uri of the syncsource
     */
    public SyncStatusEvent(int         type        ,
                           String      command     ,
                           int         statusCode  ,
                           SyncItemKey itemKey     ,
                           String      sourceUri   ) {

        this.type       = type       ;
        this.command    = command    ;
        this.statusCode = statusCode ;
        this.itemKey    = itemKey    ;
        this.sourceUri  = sourceUri  ;

    }

    // -------------------------------------------------------- Public methods

    public int getType            () {
        return type;
    }

    public String getCommand      () {
        return command;
    }

    public SyncItemKey getItemKey () {
        return itemKey;
    }

    public String getSourceUri    () {
        return sourceUri;
    }

    public int getStatusCode      () {
        return statusCode;
    }


    public void setType        (int          type       ) {
        this.type     = type          ;
    }

    public void setCommand     (String       command    ) {
        this.command     = command    ;
    }

    public void setItemKey     (SyncItemKey  itemKey    ) {
        this.itemKey     = itemKey    ;
    }

    public void setSourceUri   (String       sourceUri  ) {
        this.sourceUri   = sourceUri  ;
    }

    public void setStatusCode  (int          statusCode ) {
        this.statusCode  = statusCode ;
    }

    // -------------------------------------------------------- Private methods

}

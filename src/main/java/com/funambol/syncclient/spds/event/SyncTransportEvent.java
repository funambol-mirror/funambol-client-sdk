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

import java.util.Date;

/**
 * Is used to notify the listeners
 * of the sending of data to the server
 * and of the reception of data from the server.
 *
 *
 *
 * @version $Id: SyncTransportEvent.java,v 1.3 2007-12-22 18:09:18 nichele Exp $
 */
public class SyncTransportEvent {


    // -------------------------------------------------------- Constants

    /** Engine started to send data to the server */
    public static final int SEND_DATA_BEGIN                = 0 ;

    /** Engine has sent all data to the server */
    public static final int SEND_DATA_END                  = 1 ;

    /** Engine started to receive data from the server */
    public static final int RECEIVE_DATA_BEGIN             = 2 ;

    /** Engine is receiving data from the server */
    public static final int DATA_RECEIVED                  = 3 ;

    /** Engine has received all data from the server */
    public static final int RECEIVE_DATA_END               = 4 ;

    // -------------------------------------------------------- Private data

    private int data  = 0  ;
    private int type  = 0  ;

    // -------------------------------------------------------- Constructors

    /**
     * Creates a SyncTransportEvent
     *
     * @param type the event type
     * @param data the data length
     */
    public SyncTransportEvent(int type ,
                              int data ) {
        this.type = type ;
        this.data = data ;
    }

    // -------------------------------------------------------- Public methods

    public int getData  () {
        return data;
    }

    public int getType  () {
        return type;
    }

    public void setData     (int   data ) {
        this.data = data ;
    }

    public void setType     (int   type ) {
        this.type = type ;
    }

    // -------------------------------------------------------- Private methods

}

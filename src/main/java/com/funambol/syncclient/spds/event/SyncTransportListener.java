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

/**
 * Is used to notify
 * of the sending of data to the server
 * and of the reception of data from the server.
 *
 *
 *
 * @version $Id: SyncTransportListener.java,v 1.3 2007-12-22 18:09:18 nichele Exp $
 */
public interface SyncTransportListener {

    /**
     * Engine started to send data to the server
     *
     * @param event
     */
    public void sendDataBegin(SyncTransportEvent event) ;

    /**
     * Engine has sent all data to the server
     *
     * @param event
     */
    public void sendDataEnd(SyncTransportEvent event) ;

    /**
     * Engine started to receive data from the server
     *
     * @param event
     */
    public void receiveDataBegin(SyncTransportEvent event) ;

    /**
     * Engine has received all data from the server
     *
     * @param event
     */
    public void dataReceived(SyncTransportEvent event) ;

    /**
     * Engine is receiving data from the server
     *
     * @param event
     */
    public void receiveDataEnd(SyncTransportEvent event) ;

}

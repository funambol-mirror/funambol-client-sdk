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

package com.funambol.util;

/**
 * This class represents a generic Sync Listener, an object that listens to
 * the synchronization of a reporitory.
 * The synchronization is a sequence of events that occour in a certain order.
 * There are three different phases:
 *
 * 1) Connection set up
 * 2) Modification exchange
 * 3) Mapping
 *
 * The phases are encapsulated into a sync session which starts with a
 * startSession and is terminated by an endSession.
 * The phases are described in the SyncML specification and their order is
 * fixed as all of them are mandatory.
 * The Modification exchange phase is the most interesting from the point of
 * view of the SyncListener, because it can be further split into more sub
 * events.
 * The modifications are exchanged with a sequence of send/receive sesssion.
 * Each time modifications are about to be sent to the server a startSending
 * event is generated. Then we can have an arbitrary sequence of itemAddSent,
 * itemReplaceSent and itemDeleteSent. At the end of the sending step a
 * endSending event is generated. Similarly for the receiving phase.
 * The Modification exchange phase can thus be described as:
 *
 * MODIFICATION -> startSyncing (SEND RECEIVE)+ endSyncing
 *
 * SEND -> startSending [itemAddSent | itemReplaceSent | * itemDeleteSent]* endSending
 * RECEIVE -> startReceiving [itemReceived | itemDeleted | itemUpdated |
 *                            dataReceived]* endReceiving
 *
 */
public interface SyncListener {
    
    //-------------------------------------------------------------- Constants
    
   
    /**
     * Used when the messages to send or received is not know to
     * the mail protocol subsystem
     */
    public static final int ITEMS_NUMBER_UNKNOWN = -1;
    
    //--------------------------------------------------------- Public methods
    
    /**
     * Invoked at the beginning of the session, before opening the
     * connection with the server
     */
    public void startSession();

   /**
     * Invoked at the end of a session, before closing the connection with
     * the remote server.
     * @param status gives an indication if the session was error free or if
     *        one or more errors were encountered. The value is encoded as a
     *        bit mask according to the value of the STATUS_* constants (see
     *        above). If no error was encountered then status has the value
     *        STATUS_SUCCESS.
     */
    public void endSession(int status);



    /**
     * Invoked at the beginning of the login phase.
     *
     */
    public void startConnecting();

    /**
     * Invoked at the end of the login phase.
     *
     * @param action describes the action the server requires (this value is
     * repository dependent)
     */
    public void endConnecting(int action);


    /**
     * Invoked at the beginning of the syncing phase
     *
     * @param alertCode is the code returned by the server at the end of the
     * connection phase
     */
    public void startSyncing(int alertCode);

    /**
     * Invoked at the end of the syncing phase
     */
    public void endSyncing();


    /**
     * Invoked at the beginning of the mapping phase
     */
    public void startMapping();

    /**
     * Invoked at the end of the mapping phase
     */
    public void endMapping();


    /**
     * Invoked when items are ready to be received from the server.
     *
     * @param number number of items that will be sent during the
     *               session, if known, or ITEMS_NUMBER_UNKNOWN otherwise.
     */
    public void startReceiving(int number);

    /**
     * Invoked at the end of the receiving phase
     */
    public void endReceiving();
 
    /**
     * Invoked each time a message is received and stored successfully
     * in the client.
     *
     * @param item is the new value received
     */
    public void itemReceived(Object item);

    /**
     * Invoked each time a message is deleted
     *
     * @param itemId is the id of the value being removed
     */
    public void itemDeleted(Object item);

    /**
     * Invoked when an item changes on the other side.
     *
     * @param item is the item that changed
     * @param update is an encoding (client depending) of the update
     */
    public void itemUpdated(Object item, Object update);

    /**
     * Invoked when an item changes on the other side.
     *
     * @param item is the item that changed
     */
    public void itemUpdated(Object item);

    /**
     * Invoked each time data is received from the server, with the timestamp
     * and the size in bytes of the receive data.
     *
     * @param date is the timestamp
     * @param size is the number of bytes received
     */
    public void dataReceived(String date, int size);

    /**
     * Invoked before beginning to send items to the server.
     *
     * @param numNewItems number of new items to be sent
     * @param numUpdItems number of updated items to be sent
     * @param numDelItems number of deleted items to be sent
     */
    public void startSending(int numNewItems, int numUpdItems, int numDelItems);

    /**
     * Invoked each time an item added is sent to the server.
     * The item has type SyncItem, but we do not want this package
     * to depend on anything else, therefore we use a generic Object.
     */
    public void itemAddSent(Object item);

    /**
     * Invoked each time an item replaced is sent to the server.
     * The item has type SyncItem, but we do not want this package
     * to depend on anything else, therefore we use a generic Object.
     */
    public void itemReplaceSent(Object item);

    /**
     * Invoked each time an item deleted is sent to the server.
     * The item has type SyncItem, but we do not want this package
     * to depend on anything else, therefore we use a generic Object.
     */
    public void itemDeleteSent(Object item);
    
    /**
     * Invoked when the mail protocol subsystem has finished to send message.
     *
     */
    public void endSending();
}


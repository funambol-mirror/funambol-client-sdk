/*
 * Funambol is a mobile platform developed by Funambol, Inc.
 * Copyright (C) 2008 Funambol, Inc.
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

import java.io.UnsupportedEncodingException;
import java.io.IOException;
import java.io.OutputStream;
import java.io.InputStream;
import java.util.Hashtable;
import java.util.Vector;
import java.util.Enumeration;

import com.funambol.sync.SyncItem;
import com.funambol.sync.SyncListener;
import com.funambol.sync.SyncSource;
import com.funambol.sync.SourceConfig;
import com.funambol.sync.SyncException;

import com.funambol.syncml.protocol.*;

import com.funambol.util.Base64;
import com.funambol.util.Log;
import com.funambol.util.StringUtil;
import com.funambol.util.XmlUtil;

/**
 * This class is part of the synchronization engine and it is not visible
 * outside of the package as it is not intended to be used externally.
 * Its purpose is to manage the interaction with the sync sources. In particular
 * everything related to the construction, composition and receiving of
 * SyncItem(s) is processed here.
 * These are the main functionalities of this class:
 *
 * 1) create modification messages by appending items provided by sync sources
 * until no more items are available of until the message reaches the max
 * message size
 *
 * 2) handle large objects for both incoming and outgoing items
 *
 */
class SyncSourceLOHandler {

    private static final String TAG_LOG = "SyncSourceLOHandler";

    public static final int DONE  = 0;
    public static final int FLUSH = 1;
    public static final int MORE  = 2;

    private static final int GET_NEXT_ITEM         = 0;
    private static final int GET_NEXT_NEW_ITEM     = 1;
    private static final int GET_NEXT_UPDATED_ITEM = 2;
 
    private static final int ADD_COMMAND           = 0;
    private static final int REPLACE_COMMAND       = 1;
    private static final int DELETE_COMMAND        = 2;

    // This is an approximated size of SyncML commands
    // overhead
    private static final int SYNCML_XML_ITEM_OVERHEAD = 180;
    private static final int SYNCML_WBXML_ITEM_OVERHEAD = 80;

    private SyncSource             source;
    private int                    maxMsgSize;
    private Chunk                  nextAddChunk       = null;
    private Chunk                  nextReplaceChunk   = null;
    private SyncItem               nextDeleteItem     = null;
    private Chunk                  nextChunk          = null;
    private SyncItem               incomingLo         = null;
    private SyncItem               outgoingItem       = null;
    private byte[]                 previousChunk      = null;
    private ItemReader             outgoingItemReader = null;
    private OutputStream           incomingLoStream   = null;
    private boolean                cancel             = false;
    private boolean                wbxml              = false;
    private boolean                resume             = false;
    private boolean                deletesResumed     = false;
    private Enumeration            sentItemKeysForResume = null;

    public SyncSourceLOHandler(SyncSource source, int maxMsgSize, boolean wbxml)
    {
        this.source     = source;
        this.maxMsgSize = maxMsgSize;
        this.wbxml      = wbxml;
    }

    public void cancel() {
        cancel = true;
    }

    public void setResume(boolean resume) {
        this.resume = resume;
    }

    /**
     * This method apply a list of changes to the sync source and notifies the listener about the progress.
     * @param items the list of items to be passed to the sync source
     * @param listener the sync listener to be notified
     * @return a list of SyncItem after the source processed them (the key and the status are updated)
     */
    public Vector applyChanges(ItemsList items, SyncListener listener) {
        // We pass all the items to the sync source, but we need to transform them
        // into SyncItem first
        if (Log.isLoggable(Log.TRACE)) {
            Log.trace(TAG_LOG, "applyChanges");
        }
        Vector syncItems = new Vector();
        for(int i=0;i<items.size();++i) {
            Chunk chunk = (Chunk)items.elementAt(i);
            SyncMLCommand command = items.getItemCommand(chunk);

            char state;
            if (SyncML.TAG_ADD.equals(command.getName())) {
                state = SyncItem.STATE_NEW;
            } else if (SyncML.TAG_REPLACE.equals(command.getName())) {
                state = SyncItem.STATE_UPDATED;
            } else {
                state = SyncItem.STATE_DELETED;
            }

            boolean newItem = (incomingLo == null);

            SyncItem syncItem = getNextIncomingItem(chunk, state);

            long size = chunk.getObjectSize();
            if (listener != null) {
                if (SyncML.TAG_ADD.equals(command.getName())) {
                    if(newItem) {
                        listener.itemAddReceivingStarted(chunk.getKey(),
                                chunk.getParent(), (int)size);
                    } else {
                        listener.itemAddReceivingProgress(chunk.getKey(),
                                chunk.getParent(), chunk.getContent().length);
                    }
                } else if (SyncML.TAG_REPLACE.equals(command.getName())) {
                    if(newItem) {
                        listener.itemAddReceivingStarted(chunk.getKey(),
                                chunk.getParent(), (int)size);
                    } else {
                        listener.itemAddReceivingProgress(chunk.getKey(),
                                chunk.getParent(), chunk.getContent().length);
                    }
                } else {
                    listener.itemDeleted(syncItem);
                }
            }

            // If the received command is a delete, there is no
            // content to be written
            if (state != SyncItem.STATE_DELETED) {
                byte data[] = chunk.getContent();
                OutputStream os = null;

                try {
                    if (incomingLoStream != null) {
                        os = incomingLoStream;
                    } else {
                        os = syncItem.getOutputStream();
                    }

                    os.write(data);
                    os.flush();
                } catch (IOException ioe) {
                    Log.error(TAG_LOG, "Cannot write item content", ioe);
                } finally {
                    if (os != null) {
                        try {
                            os.close();
                        } catch (IOException ioe2) {
                            Log.error(TAG_LOG, "Cannot close output stream", ioe2);
                        }
                    }
                }
            }
            syncItems.addElement(syncItem);
            if (listener != null && !chunk.hasMoreData()) {
                if (SyncML.TAG_ADD.equals(command.getName())) {
                    listener.itemAddReceivingEnded(chunk.getKey(), chunk.getParent());
                } else if (SyncML.TAG_REPLACE.equals(command.getName())) {
                    listener.itemReplaceReceivingEnded(chunk.getKey(), chunk.getParent());
                }
            }
            incomingLo = null;
            incomingLoStream = null;
        }
        source.applyChanges(syncItems);

        // Convert the status codes from SyncSource neutral to SyncML
        for(int i=0;i<syncItems.size();++i) {
            SyncItem item = (SyncItem)syncItems.elementAt(i);
            item.setSyncStatus(getSyncMLStatusCode(item.getSyncStatus()));
        }

        return syncItems;
    }

    public int addUpdateChunk(Chunk chunk, boolean add) throws SyncException {
        if (Log.isLoggable(Log.TRACE)) {
            Log.trace(TAG_LOG, "addUpdateChunk " + chunk.getKey());
        }

        if (isSyncToBeCancelled()) {
            cancelSync();
        }

        char state = add ? SyncItem.STATE_NEW : SyncItem.STATE_UPDATED;
        SyncItem item = getNextIncomingItem(chunk, state);

        // Grab the sync item output stream and append to it
        try {
            if (incomingLoStream == null) {
                incomingLoStream = item.getOutputStream();
            }

            if (incomingLoStream == null) {
                Log.error(TAG_LOG, "addUpdateItem Cannot write to null output stream");
                return SyncMLStatus.GENERIC_ERROR;
            }
            byte data[] = chunk.getContent();
            incomingLoStream.write(data);
            incomingLoStream.flush();
        } catch (IOException ioe) {
            Log.error(TAG_LOG, "addUpdateItem Cannot write to output stream: " + ioe.toString());

            // Close the output stream and finalize the large object
            incomingLo = null;
            try {
                incomingLoStream.close();
            } catch (IOException ioe2) {
                Log.error(TAG_LOG, "Cannot close output stream: " + ioe2.toString());
            } finally {
                incomingLoStream = null;
            }
            return SyncMLStatus.GENERIC_ERROR;
        }

        if (!chunk.hasMoreData()) {
            Log.error(TAG_LOG, "SyncSourceLOHandler bug: the item must be a large object chunk");
        }
        // Return the status to proceed
        return SyncMLStatus.CHUNKED_ITEM_ACCEPTED;
    }




    private void cancelSync() throws SyncException
    {
        if (Log.isLoggable(Log.INFO)) {
            Log.info(TAG_LOG, "Cancelling sync for source ["+source.getName()+"]");
        }
        throw new SyncException(SyncException.CANCELLED, "SyncManager sync got cancelled");
    }

    private boolean isSyncToBeCancelled() {
        return cancel;
    }


    /**
     * This is a utility method that returns the proper item to store the
     * incoming data. For single chunk items or the first chunk of a LO the
     * method asks the sync source to create a SyncItem of the proper type. When
     * a LO is being received this method returns the same item until the LO is
     * being completely received. Other methods (addItem and updateItem) use this
     * utility to fetch the current SyncItem and append the data received from
     * the server.
     *
     * @param chunk is the chunk received from the server (possibly an entire
     * item if not a LO)
     * @param state is the item state (can be NEW or UPDATED here)
     *
     * @return a SyncItem to hold the entire item (the concatenation of all the
     * chunks composing the item)
     */
    private SyncItem getNextIncomingItem(Chunk chunk, char state) {

        String key = chunk.getKey();
        if (Log.isLoggable(Log.TRACE)) {
            Log.trace(TAG_LOG, "getNextIncomingItem " + key);
        }

        if (incomingLo == null) {
            incomingLo = source.createSyncItem(chunk.getKey(),
                                               chunk.getType(),
                                               state,
                                               chunk.getParent(),
                                               chunk.getObjectSize());
            // Set the source parent if the info is available
            if (chunk.getSourceParent() != null) {
                incomingLo.setSourceParent(chunk.getSourceParent());
            }
        }
        return incomingLo;
    }

    /**
     * This method returns and Add command. The method gets an add command at
     * a time until there are items or the message has reached its maximum size.
     * The chunking of large objects is performed by the utility method
     * getNextNewItem which is responsible for returning items of the proper
     * size.
     *
     * @param size is the current size of the message
     * @param listener is the SyncListener
     * @param cmdTag is the string containing syncml tag the method will fill
     * (output parameter)
     * @param cmdId is the CmdId. This is updated after each tag is added
     *
     * @return the status of this message (it can be DONE if no more items are
     * available, FLUSH if the current msg must be flushed or MORE if there are
     * more items but they don't fit in this message). When the method returns
     * FLUSH it is not known if there are new items to be sent. An extra call to
     * getAddCommand is required to check that.
     */
    public int getAddCommand(int size, SyncListener listener,
                             SyncMLCommand command, CmdId cmdId, SyncStatus syncStatus) throws SyncException {

        if (Log.isLoggable(Log.TRACE)) {
            Log.trace(TAG_LOG, "getAddCommand");
        }

        if (isSyncToBeCancelled()) {
            cancelSync();
        }

        Chunk chunk = null;
        if (nextAddChunk == null) {
            chunk = getNextNewItem();
            // No item for this source
            if (chunk == null) {
                return DONE;
            }
        } else {
            chunk = nextAddChunk;
            nextAddChunk = null;
        }

        Item item = prepareItemAddUpdate(chunk);
        int commandSize = computeItemSize(item);

        command.setCmdId(cmdId.next());

        // We allow a certain degree of flexibility in the message size
        // and do not complain if the item is not bigger than 10% of the
        // maxMsgSize
        if (size + commandSize > ((maxMsgSize * 110)/100)) {
            // If the item does not fit in the max msg size then we shall drop
            // it but for backward compatibility we let it go through

            if (Log.isLoggable(Log.INFO)) {
                Log.info(TAG_LOG, source.getName() + 
                                  " returned an item that exceeds max msg size and should be dropped");
            }
        }

        Vector items = new Vector();
        int ret = MORE;
        do {
            items.addElement(item);
            // Notify the listener
            notifyListener(listener, ADD_COMMAND, chunk);

            Chunk previousChunk = chunk;
            // Ask the source for next item
            chunk = getNextNewItem();

            // Last new item found
            if (chunk == null) {
                ret = DONE; 
                break;
            }

            item = prepareItemAddUpdate(chunk);
            commandSize += computeItemSize(item);
        } while (size + commandSize < maxMsgSize);

        command.setItems(items);
        command.setSize(commandSize);

        if (chunk != null) {
            // If we get here then we reached the max msg size, so
            // we store the next msg for the next message
            nextAddChunk = chunk;
        }

        return ret;
    }


    /**
     * This method returns the Replace command tag. The tag is composed by
     * concatenating all the necessary items. The method gets a replace command at
     * a time until there are items or the message has reached its maximum size.
     * The chunking of large objects is performed by the utility method
     * getNextUpdItem which is responsible for returning items of the proper
     * size.
     *
     * @param size is the current size of the message
     * @param listener is the SyncListener
     * @param cmdTag is the string containing syncml tag the method will fill
     * (output parameter)
     * @param cmdId is the CmdId. This is updated after each tag is added
     *
     * @return the status of this message (it can be DONE if no more items are
     * available, FLUSH if the current msg must be flushed or MORE if there are
     * more items but they don't fit in this message). When the method returns
     * FLUSH it is not known if there are new items to be sent. An extra call to
     * getReplaceCommand is required to check that.
     */
    public int getReplaceCommand(int size, SyncListener listener,
                                 SyncMLCommand command, CmdId cmdId) throws SyncException {

        if (Log.isLoggable(Log.TRACE)) {
            Log.trace(TAG_LOG, "getReplaceCommand");
        }

        if (isSyncToBeCancelled()) {
            cancelSync();
        }

        Chunk chunk = null;
        if (nextReplaceChunk == null) {
            chunk = getNextUpdatedItem();
            // No item for this source
            if (chunk == null) {
                return DONE;
            }
        } else {
            chunk = nextReplaceChunk;
            nextReplaceChunk = null;
        }

        Item item = prepareItemAddUpdate(chunk);
        int commandSize = computeItemSize(item);

        command.setCmdId(cmdId.next());

        // We allow a certain degree of flexibility in the message size
        // and do not complain if the item is not bigger than 10% of the
        // maxMsgSize
        if (size + commandSize > ((maxMsgSize * 110)/100)) {
            // If the item does not fit in the max msg size then we shall drop
            // it but for backward compatibility we let it go through
            if (Log.isLoggable(Log.INFO)) {
                Log.info(TAG_LOG, source.getName() + 
                                 " returned an item that exceeds max msg size and should be dropped");
            }
        }

        Vector items = new Vector();
        int ret = MORE;
        do {
            items.addElement(item);
            // Notify the listener
            notifyListener(listener, REPLACE_COMMAND, chunk);

            Chunk previousChunk = chunk;

            // Ask the source for next item
            chunk = getNextUpdatedItem();

            // Last item found
            if (chunk == null) {
                ret = DONE;
                break;
            }

            item = prepareItemAddUpdate(chunk);
            commandSize += computeItemSize(item);
        } while (size + commandSize < maxMsgSize);

        command.setItems(items);
        command.setSize(commandSize);

        if (chunk != null) {
            // If we get here then we reached the max msg size, so
            // we store the next msg for the next message
            nextReplaceChunk = chunk;
        }

        return ret;
    }

    /**
     * This method returns the Delete command tag. The tag is composed by
     * concatenating all the necessary items. The method gets a delete command at
     * a time until there are items or the message has reached its maximum size
     *
     * @param size is the current size of the message
     * @param listener is the SyncListener
     * @param cmdTag is the string containing syncml tag the method will fill
     * (output parameter)
     * @param cmdId is the CmdId. This is updated after each tag is added
     *
     * @return true iff there are no more items to send
     */
    public boolean getDeleteCommand(int size, SyncListener listener,
                                    SyncMLCommand command, CmdId cmdId)
    throws SyncException {

        if (Log.isLoggable(Log.TRACE)) {
            Log.trace(TAG_LOG, "getDeleteCommand]");
        }

        if (isSyncToBeCancelled()) {
            cancelSync();
        }

        SyncItem item = null;

        if (nextDeleteItem == null) {
            item = source.getNextDeletedItem();
            // No item for this source
            if (item == null) {
                return true;
            }
        } else {
            item = nextDeleteItem;
            nextDeleteItem = null;
        }

        command.setCmdId(cmdId.next());
        Item commandItem = prepareItemDelete(item.getKey());
        int commandSize = computeItemSize(commandItem);

        // We allow a certain degree of flexibility in the message size
        // and do not complain if the item is not bigger than 10% of the
        // maxMsgSize
        if (size + commandSize > ((maxMsgSize * 110)/100)) {
            // If the item does not fit in the max msg size then we shall drop
            // it but for backward compatibility we let it go through

            if (Log.isLoggable(Log.INFO)) {
                Log.info(TAG_LOG, source.getName() + 
                                  " returned an item that exceeds max msg size and should be dropped");
            }
        }

        Vector items = new Vector();
        // Build Delete command
        boolean done = false;
        do {
            items.addElement(commandItem);

            // Notify the listener
            listener.itemDeleteSent(item);

            if (isSyncToBeCancelled()) {
                cancelSync();
            }

            // Ask the source for next item
            item = source.getNextDeletedItem();

            // Last item found
            if (item == null) {
                done = true;
                break;
            }
            commandItem = prepareItemDelete(item.getKey());
            commandSize += computeItemSize(commandItem);
        } while (size + commandSize < maxMsgSize);

        command.setItems(items);
        command.setSize(commandSize);

        if (item != null) {
            // If we get here then we reached the max msg size, so
            // we store the next msg for the next message
            nextDeleteItem = item;
        }

        return done;
    }


    /**
     * This method returns the next command tag in a slow sync. The tag is composed by
     * concatenating all the necessary items.
     * The method gets an item at a time and pack it into a "replace" command.
     * The process continues until there are items or the message has reached its maximum size.
     * The chunking of large objects is performed by the utility method
     * getNextItem which is responsible for returning items of the proper
     * size.
     *
     * @param size is the current size of the message
     * @param listener is the SyncListener
     * @param command is the command to be filled (output param)
     * @param cmdId is the CmdId. This is updated after each tag is added
     * @param syncStatus the current syncStatus
     *
     * @param msgStatus the status of this message (it can be DONE if no more items are
     * available, FLUSH if the current msg must be flushed or MORE if there are
     * more items but they don't fit in this message). When the method returns
     * FLUSH it is not known if there are new items to be sent. An extra call to
     * getNextCommand is required to check that.
     */
    public SyncMLCommand getNextCommand(int size, SyncListener listener,
                                        CmdId cmdId, SyncStatus syncStatus, int msgStatus[])
    throws SyncException {

        Chunk chunk = null;

        if (isSyncToBeCancelled()) {
            cancelSync();
        }

        // During a resume we may need to send delete commands for items that
        // were previously sent and got deleted after the suspend
        if (resume && !deletesResumed) {
            // Iterate over the sent items
            if (sentItemKeysForResume == null) {
                sentItemKeysForResume = syncStatus.getSentItems();
            }
            SyncMLCommand delCommand = null;
            Vector delItems = new Vector();
            while(sentItemKeysForResume.hasMoreElements()) {
                String k = (String)sentItemKeysForResume.nextElement();
                // If this has item has been deleted, then we must send a delete
                // command

                if (!source.exists(k)) {
                    if (Log.isLoggable(Log.INFO)) {
                        Log.info(TAG_LOG, "Item " + k + " was sent in suspended sync and no longer exists. Send a delete");
                    }
                    if (delCommand == null) {
                        delCommand = SyncMLCommand.newInstance(SyncML.TAG_DELETE);
                        delCommand.setCmdId(cmdId.next());
                    }
                    SyncItem delSyncItem = new SyncItem(k);
                    listener.itemDeleteSent(delSyncItem);
                    Item item = prepareItemDelete(k);
                    // TODO: fix the size
                    delItems.addElement(item);
                }
            }
            if (delCommand == null) {
                // No deletes to send, this preliminary check is over
                deletesResumed = true;
                sentItemKeysForResume = null;
            } else {
                // Send all the deleted items in the first msg
                msgStatus[0] = MORE;
                delCommand.setItems(delItems);
                return delCommand;
            }
        }

        SyncMLCommand command = SyncMLCommand.newInstance(SyncML.TAG_REPLACE);

        // Here we grab the standard items
        if (nextChunk == null) {
            if (resume) {
                chunk = getNextItemWithResumeFilter(syncStatus);
            } else {
                chunk = getNextItem();
            }
            // No item for this source
            if (chunk == null) {
                msgStatus[0] = DONE;
                return command;
            }
        } else {
            chunk = nextChunk;
            nextChunk = null;
        }

        command.setCmdId(cmdId.next());
        Item item = prepareItemAddUpdate(chunk);
        int commandSize = computeItemSize(item);

        // We allow a certain degree of flexibility in the message size
        // and do not complain if the item is not bigger than 10% of the
        // maxMsgSize
        if (size + commandSize > ((maxMsgSize * 110)/100)) {
            if (Log.isLoggable(Log.INFO)) {
                Log.info(TAG_LOG, source.getName() + 
                                  " returned an item that exceeds max msg size and should be dropped");
            }
        }

        int ret = MORE;
        int itemsCounter = 1;
        SourceConfig srcConfig = source.getConfig();
        int maxItemsPerMessageInSlowSync = srcConfig.getMaxItemsPerMessageInSlowSync();
        if (Log.isLoggable(Log.TRACE)) {
            Log.trace(TAG_LOG, "maxItemsPerMessageInSlowSync=" + maxItemsPerMessageInSlowSync);
        }
        Vector items = new Vector();
        do {
            items.addElement(item);
            // Notify the listener
            notifyListener(listener, REPLACE_COMMAND, chunk);

            Chunk previousChunk = chunk;

            // Ask the source for next item
            if (resume) {
                chunk = getNextItemWithResumeFilter(syncStatus);
            } else {
                chunk = getNextItem();
            }

            // Last item found
            if (chunk == null) {
                ret = DONE;
                break;
            }

            // If we reached the max items count, then we are done
            // with this message
            if (maxItemsPerMessageInSlowSync > 0 && itemsCounter >= maxItemsPerMessageInSlowSync) {
                if (Log.isLoggable(Log.INFO)) {
                    Log.info(TAG_LOG, "Reached max number of items per message in slow sync");
                }
                ret = FLUSH;
                break;
            }

            item = prepareItemAddUpdate(chunk);
            commandSize += computeItemSize(item);
            itemsCounter++;
        } while (size + commandSize < maxMsgSize);

        command.setItems(items);
        command.setSize(commandSize);

        if (chunk != null) {
            // If we get here then we reached the max msg size, so
            // we store the next msg for the next message
            nextChunk = chunk;
        }

        msgStatus[0] = ret;
        return command;
    }

    public void releaseResources() {
        if (outgoingItemReader != null) {
            try {
                outgoingItemReader.close();
            } catch (IOException ioe) {
                Log.error(TAG_LOG, "Cannot close item reader " + ioe.toString());
            }
        }
        if (incomingLoStream != null) {
            try {
                incomingLoStream.close();
            } catch (IOException ioe) {
                Log.error(TAG_LOG, "Cannot close output stream " + ioe.toString());
            }
        }
    }

    private Chunk getNextItemWithResumeFilter(SyncStatus syncStatus) {

        Chunk chunk;
        boolean skip;

        do {
            chunk = getNextItem();
            // No item for this source
            if (chunk == null) {
                return null;
            }

            int status = syncStatus.getSentItemStatus(chunk.getKey());

            if (SyncMLStatus.isSuccess(status)) {
                // This item has already been succesfully sent. We may still
                // need to send it if has changed since the last interrupted
                // sync
                if (source.hasChangedSinceLastSync(chunk.getKey(), syncStatus.getLastSyncStartTime())) {
                    if (Log.isLoggable(Log.INFO)) {
                        Log.info(TAG_LOG, "An item sent during previous sync has changed since then. Resend it " + chunk.getKey());
                    }
                    skip = false;
                } else {
                    // there is no need to resend this item, skip it
                    if (Log.isLoggable(Log.INFO)) {
                        Log.info(TAG_LOG, "Skipping item during resume that was previously sent " + chunk.getKey());
                    }
                    skip = true;
                }
            } else {
                skip = false;
            }
        } while(skip);
        return chunk;
    }

    private void notifyListener(SyncListener listener, int command, Chunk chunk) {

        if (Log.isLoggable(Log.TRACE)) {
            Log.trace(TAG_LOG, "notifying listener");
        }
        if (Log.isLoggable(Log.TRACE)) {
            Log.trace(TAG_LOG, "key=" + chunk.getKey());
        }
        if (Log.isLoggable(Log.TRACE)) {
            Log.trace(TAG_LOG, "chunk number = " + chunk.getChunkNumber());
        }
        if (Log.isLoggable(Log.TRACE)) {
            Log.trace(TAG_LOG, "has more data = " + chunk.hasMoreData());
        }

        if (chunk.getChunkNumber() == 0) {
            // A new chunk is about to begin
            long size = chunk.getObjectSize();
            switch (command) {
                case ADD_COMMAND:
                    listener.itemAddSendingStarted(chunk.getKey(), chunk.getParent(),
                                                   (int)size);
                    break;
                case REPLACE_COMMAND:
                    listener.itemReplaceSendingStarted(chunk.getKey(), chunk.getParent(),
                                                       (int)size);
                    break;
                default:
                    Log.error(TAG_LOG, "Unknown command type " + command);
                    break;
            }
        }
        
        if (chunk.hasMoreData()) {

            // This is an individual chunk
            switch (command) {
                case ADD_COMMAND:
                    listener.itemAddSendingProgress(chunk.getKey(), chunk.getParent(),
                                              chunk.getContent().length);
                    break;
                case REPLACE_COMMAND:
                    listener.itemReplaceSendingProgress(chunk.getKey(), chunk.getParent(),
                                                  chunk.getContent().length);
                    break;
                default:
                    Log.error(TAG_LOG, "Unexpected chunked item in delete command");
                    break;
            }
        } else {
            // This is the last chunk of a multi or single chunked item
            switch (command) {
                case ADD_COMMAND:
                    listener.itemAddSendingEnded(chunk.getKey(), chunk.getParent());
                    break;
                case REPLACE_COMMAND:
                    listener.itemReplaceSendingEnded(chunk.getKey(), chunk.getParent());
                    break;
                default:
                    Log.error(TAG_LOG, "Unknown command type " + command);
                    break;
            }
        }
    }

    private Chunk getNextItem() throws SyncException {
        return getNextItemHelper(GET_NEXT_ITEM);
    }

    private Chunk getNextNewItem() throws SyncException {
        return getNextItemHelper(GET_NEXT_NEW_ITEM);
    }

    private Chunk getNextUpdatedItem() throws SyncException {
        return getNextItemHelper(GET_NEXT_UPDATED_ITEM);
    }

    private Chunk getNextItemHelper(int syncSourceMethod) throws SyncException {
        if (Log.isLoggable(Log.TRACE)) {
            Log.trace(TAG_LOG, "getNextItemHelper");
        }

        if (isSyncToBeCancelled()) {
            cancelSync();
        }

        try {
            SyncItem newItem;
            boolean multiChunks = false;
            if (outgoingItemReader == null) {

                if (syncSourceMethod == GET_NEXT_ITEM) {
                    newItem = source.getNextItem();
                } else if (syncSourceMethod == GET_NEXT_NEW_ITEM) {
                    newItem = source.getNextNewItem();
                } else if (syncSourceMethod == GET_NEXT_UPDATED_ITEM) {
                    newItem = source.getNextUpdatedItem();
                } else {
                    // This is an internal error
                    throw new SyncException(SyncException.CLIENT_ERROR, "Unknown sync source method");
                }

                // If there are no more items to send, just return
                if (newItem == null) {
                    return null;
                }

                InputStream is = newItem.getInputStream();
                outgoingItemReader = new ItemReader(maxMsgSize, is,
                        SyncSource.ENCODING_B64.equals(source.getEncoding()));
                outgoingItem = newItem;
            } else {
                multiChunks = true;
            }
            int size = outgoingItemReader.read();
            if (size <= 0) {
                throw new SyncException(SyncException.CLIENT_ERROR, "Internal error: size is zero");
            }

            byte actualContent[] = outgoingItemReader.getChunkContent();
            Chunk chunk = new Chunk(outgoingItem.getKey(), outgoingItem.getType(),
                    outgoingItem.getParent(),
                    actualContent,
                    !outgoingItemReader.last());
            chunk.setObjectSize(outgoingItem.getObjectSize());
            chunk.setChunkNumber(outgoingItemReader.getChunkNumber());

            if (outgoingItemReader.last()) {
                if (multiChunks) {
                    chunk.setLastChunkOfLO(true);
                }
                try {
                    outgoingItemReader.close();
                } catch (IOException ioe) {
                    Log.error(TAG_LOG, "Cannot close input stream " + ioe.toString());
                }
                outgoingItemReader = null;
            }
            return chunk;
        } catch (SyncException se) {
            throw se;
        } catch (Exception e) {
            throw new SyncException(SyncException.CLIENT_ERROR, e.toString());
        }
    }

    /**
     * Encode the item data according to the format specified by the SyncSource.
     *
     * @param formats the list of requested encodings (des, 3des, b64)
     * @param data the byte array of data to encode
     * @return the encoded byte array, or <code>null</code> in case of error
     */
    private byte[] encodeItemData(String[] formats, byte[] data) {

        if (formats != null && data != null) {
            // If ecryption types are specified, apply them
            for (int count = formats.length - 1; count >= 0; count--) {

                String encoding = formats[count];

                if (encoding.equals("b64")) {
                    data = Base64.encode(data);
                }
            /*
            else if (encoding.equals("des")) {
            // DES not supported now, ignore SyncSource encoding
            }
            else if (currentDecodeType.equals("3des")) {
            // 3DES not supported now, ignore SyncSource encoding
            }
             */
            }
        }
        return data;
    }

    ////////////////////////////// SyncML parser ////////////////////////////

    /**
     * Get an item from the SyncML tag.
     *
     * @param type the mime type of the item
     * @param xmlItem the SyncML tag for this item
     * @param formatList a list of encoding formats
     * @param hierarchy the current inverse mapping table, used to retrieve the SyncItem
     *                  parent, when the SourceParent is specified in the SyncML
     *                  command.
     *
     * @return a Chunk instance corresponding to the SyncML item
     *
     * @throws SyncException if the command parsing failed
     *
     */
    public Chunk getItem(Item item, String type, String[] formatList, Hashtable hierarchy) throws SyncException {

        String key = null;
        String parent = null;
        String sourceParent = null;
        byte[] content = null;

        if (isSyncToBeCancelled()) {
            cancelSync();
        }

        // Get the item key
        Target tgt = item.getTarget();
        if (tgt != null) {
            key = tgt.getLocURI();
        }
        if (key == null) {
            Source src = item.getSource();
            if (src != null) {
                key = src.getLocURI();
            }
        }

        if (key == null) {
            Log.error(TAG_LOG, "Invalid item key from server: ");
            throw new SyncException(
                    SyncException.SERVER_ERROR, "Invalid item key from server.");
        }

        // Get the parent (not mandatory)
        TargetParent tgtParent = item.getTargetParent();
        if (tgtParent != null) {
            parent = tgtParent.getLocURI();
        }
        if (parent == null) {
            SourceParent srcParent = item.getSourceParent();
            if (srcParent != null) {
                sourceParent = srcParent.getLocURI();
                // Lookup the parent key from the mapping table
                if(hierarchy != null) {
                    parent = (String) hierarchy.get(sourceParent);
                }
                if(sourceParent == null) {
                    if (Log.isLoggable(Log.INFO)) {
                        Log.info(TAG_LOG, "Received an item without target parent and source parent: "
                                          + sourceParent);
                    }
                }
            }
        }

        // Check if the item has the MoreData
        boolean hasMoreData = item.isMoreData();

        // Process the data section
        Data dataTag = item.getData();
        if (dataTag != null) {
            String data = dataTag.getData();
            byte binData[] = dataTag.getBinData();
            try {
                // Get item data
                if (formatList != null) {
                    // Format tag from server
                    content = decodeItemData(hasMoreData, formatList, data.getBytes());
                } else if (!source.getEncoding().equals(source.ENCODING_NONE)) {
                    // If the server does not send a format, apply the one
                    // defined for the sync source
                    formatList = new String[1];
                    formatList[0] = source.getEncoding();
                    content = decodeItemData(hasMoreData, formatList, data.getBytes());
                } else {
                    // Else, the data is text/plain,
                    // and the XML special chars are escaped.
                    // The encoding must be set to UTF-8
                    // in order to read the symbols like "euro"
                    if (wbxml) {
                        if (data != null) {
                            content = data.getBytes("UTF-8");
                        } else {
                            content = binData;
                        }
                    } else {
                        content = XmlUtil.unescapeXml(data).getBytes("UTF-8");
                    }
                }
            } catch (UnsupportedEncodingException uee) {
                uee.printStackTrace();
                Log.error(TAG_LOG, "Can't decode content for item: " + key);
                // in case of error, the content is null
                // and this will be reported as an error to the server
                content = null;
            }
        }
        // Create an item in memory. We don't use the sync source item type
        // here, as this is a single chunk. The item handler will ask the source
        // to create the item if necessary
        Chunk chunk = new Chunk(key, type, parent, content, hasMoreData);
        if (parent == null && sourceParent != null) {
            chunk.setSourceParent(sourceParent);
        }
        return chunk;
    }

    private Item prepareItemAddUpdate(Chunk chunk) throws SyncException {

        StringBuffer ret = new StringBuffer();

        Item item = Item.newInstance();
       
        if (Log.isLoggable(Log.INFO)) {
            Log.info(TAG_LOG, "The encoding method is [" + source.getEncoding() + "]");
        }
        String encodedData  = null;
        byte   binaryData[] = null;

        if (!chunk.hasContent()) {
            Log.error(TAG_LOG, "Empty content from SyncSource for chunk:" +
                    chunk.getKey());
            encodedData = "";
        } else if (!source.getEncoding().equals(source.ENCODING_NONE)) {
            String[] formatList = StringUtil.split(
                    source.getEncoding(), ";");
            byte[] data = encodeItemData(formatList, chunk.getContent());

            encodedData = new String(data);
        } else {
            // Else, the data is text/plain,
            // and the XML special chars are escaped.
            if (!wbxml) {
                // XmlPull formatter performs the escaping, so 
                // we don't need to escape here
                encodedData = new String(chunk.getContent());
            } else {
                binaryData = chunk.getContent();
            }
        }

        // Meta information
        Meta meta = Meta.newInstance();

        // type
        String theType = chunk.getType() == null ? source.getType() : chunk.getType();
        meta.setType(theType);

        // format
        String format;

        if (!source.getEncoding().equals(source.ENCODING_NONE)) {
            format = source.getEncoding(); 
        } else {
            format = "";
        }

        /*
        if (wbxml) {
            // In WBXML we always send binary data, so we add the bin format
            if (format.length() > 0) {
                format = format + ";bin";
            } else {
                format = "bin";
            }
        }
        */

        if (format.length() > 0) {
            meta.setFormat(format);
        }
        item.setMeta(meta);

        // If this is the first chunk of a Large Object, and if this
        // item has a declared size then we must specify it in the meta
        // data. If the item must be encoded, the size must reflect
        // that.
        if (Log.isLoggable(Log.TRACE)) {
            Log.trace(TAG_LOG, "objsize: "+ chunk.getObjectSize()
                               + " chunk: "+ chunk.getChunkNumber()
                               + " moredata: " + chunk.hasMoreData());
        }

        if (chunk.getChunkNumber() == 0 && chunk.hasMoreData()) {
            if (chunk.getObjectSize() != -1) {
                long realObjSize = getRealSize(chunk.getObjectSize());
                meta.setSize(new Long(realObjSize));
            } else {
                Log.error(TAG_LOG, "Cannot format a LO with unknown size");
                throw new SyncException(SyncException.CLIENT_ERROR, "LO with unknwon size");
            }
        }
        item.setMeta(meta);

        Source source = Source.newInstance();
        source.setLocURI(chunk.getKey());
        item.setSource(source);

        //parent
        if (chunk.getParent() != null) {
            SourceParent sourceParent = SourceParent.newInstance();
            sourceParent.setLocURI(chunk.getParent());
            item.setSourceParent(sourceParent);
        }

        //item data
        if (binaryData != null) {
            if (Log.isLoggable(Log.DEBUG)) {
                Log.debug(TAG_LOG, "BinaryDataSize: " + binaryData.length);
            }
        } else {
            if (Log.isLoggable(Log.DEBUG)) {
                Log.debug(TAG_LOG, "EncodedDataSize: " + encodedData.length());
            }
        }
        Data itemData;
        if (binaryData != null) {
            itemData = Data.newInstance(binaryData);
        } else {
            itemData = Data.newInstance(encodedData);
        }
        item.setData(itemData);

        // More data flag
        if (chunk.hasMoreData()) {
            item.setMoreData(new Boolean(true));
        }

        return item;
    }

    private Item prepareItemDelete(String key) {
        Item item = Item.newInstance();
        Source itemSource = Source.newInstance();
        itemSource.setLocURI(key);
        item.setSource(itemSource);
        return item;
    }

    /*
     * If the source has b64 encoding, compute the size of the item that will be sent.
     * B64 size is 4/3 of the original size.
     */
    private long getRealSize(long origSize) {
        if(source.getEncoding() == SyncSource.ENCODING_B64) {
            long rem  = origSize % 3;
            long size;
            if (rem == 0) {
                size = 4 * (origSize / 3);
            } else {
                size = 4 * ((origSize / 3) + 1);
            }
            return size;
        }
        else {
            return origSize;
        }
    }

    private byte[] decodeChunk(boolean hasMoreData, byte[] content) {

        int extra = 0;
        if (previousChunk != null) {
            extra = previousChunk.length;
        }
        int size = content.length + extra;
        int rem  = (3 * size) % 4;
        byte data[];

        if (rem != 0 && hasMoreData) {
            // We have a remainder, so we must truncate and keep the
            // extra bytes for the next chunk, unless this is the last
            // one
            int chunkableSize = (size / 4) * 4;
            rem = extra + content.length - chunkableSize;
            data = new byte[chunkableSize];
            int i;
            for(i=0;i<extra;++i) {
                data[i] = previousChunk[i];
            }
            for(i=0;i<content.length - rem;++i) {
                data[i+extra] = content[i];
            }
            previousChunk = new byte[rem];
            for(int j=0;j<rem;j++) {
                previousChunk[j] = content[i+j];
            }
        } else {
            int realSize = extra + content.length;
            // Copy everything
            data = new byte[realSize];
            int i;
            for(i=0;i<extra;++i) {
                data[i] = previousChunk[i];
            }
            for(i=0;i<content.length;++i) {
                data[i+extra] = content[i];
            }
            previousChunk = null;
        }
        if (data.length > 0) {
            data = Base64.decode(data);
        }
        return data;
    }

    /**
     * Decode the item data according to the format specified by the server.
     *
     * @param formats the list of requested decodings (des, 3des, b64)
     * @param data the byte array of data to decode
     * @return the decode byte array, or <code>null</code> in case of error
     *
     * @throws UnsupportedEncodingException
     */
    private byte[] decodeItemData(boolean hasMoreData, String[] formats, byte[] data)
            throws UnsupportedEncodingException {

        if (formats != null && data != null) {
            // If ecryption types are specified, apply them
            for (int count = formats.length - 1; count >= 0; count--) {

                String currentDecodeType = formats[count];

                if (currentDecodeType.equals("b64")) {
                    data = decodeChunk(hasMoreData, data);
                } else if (currentDecodeType.equals("des")) {
                    // Error, DES not supported now, send error to the server
                    return null;
                /*
                desCrypto = new Sync4jDesCrypto(Base64.encode(login.getBytes()));
                data = desCrypto.decryptData(data);
                 */
                } else if (currentDecodeType.equals("3des")) {
                    // Error, 3DES not supported now, send error to the server
                    return null;
                /*
                sync3desCrypto = new Sync4j3DesCrypto(Base64.encode(login.getBytes()));
                data = sync3desCrypto.decryptData(data);
                 */
                }
            }
        }
        return data;
    }

    /**
     * This method computes an approximated msg size. It is heuristic for all
     * the meta data part as the real length is known only after the actual
     * formatting is performed.
     *
     * @param item the item whose size must be estimated
     * @return the estimated size
     */
    private int computeItemSize(Item item) {
        int itemSize = wbxml ? SYNCML_WBXML_ITEM_OVERHEAD : SYNCML_XML_ITEM_OVERHEAD;
        Data data = item.getData();
        if (data != null) {
            itemSize += data.getSize();
        }

        return itemSize;
    }

    private int getSyncMLStatusCode(int syncMLStatusCode) {

        int ret;

        switch (syncMLStatusCode) {
            case SyncSource.SUCCESS_STATUS:
                ret = SyncMLStatus.SUCCESS;
                break;
            case SyncSource.CHUNK_SUCCESS_STATUS:
                ret = SyncMLStatus.CHUNKED_ITEM_ACCEPTED;
                break;
            case SyncSource.SERVER_FULL_ERROR_STATUS:
            case SyncSource.DEVICE_FULL_ERROR_STATUS:
                ret = SyncMLStatus.DEVICE_FULL;
                break;
            default:
                ret = SyncMLStatus.GENERIC_ERROR;
        }
        return ret;
    }

}


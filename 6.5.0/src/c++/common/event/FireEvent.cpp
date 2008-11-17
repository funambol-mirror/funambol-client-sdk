/*
 * Copyright (C) 2003-2007 Funambol, Inc.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY, TITLE, NONINFRINGEMENT or FITNESS FOR A PARTICULAR
 * PURPOSE.  See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA
 * 02111-1307  USA
 */

#include "base/fscapi.h"
#include "event/FireEvent.h"
#include "event/ManageListener.h"


//
// Fire a SyncEvent
//
bool fireSyncEvent(const char* msg, int type) {

    ManageListener& manage = ManageListener::getInstance();
    SyncListener* listener = manage.getSyncListener();
    if(listener == NULL) {
        return FALSE;
    }

    unsigned long timestamp = (unsigned long)time(NULL);
    // Create event (object alive in the scope of this function)
    SyncEvent event(type, timestamp);
    if(msg) {
        event.setMessage(msg);
    }

    switch(type) {
      case SYNC_BEGIN:
          listener->syncBegin(event);
          break;
      case SYNC_END:
          listener->syncEnd(event);
          break;
      case SEND_INITIALIZATION:
          listener->sendInitialization(event);
          break;
      case SEND_MODIFICATION:
          listener->sendModifications(event);
          break;
      case SEND_FINALIZATION:
          listener->sendFinalization(event);
          break;
      case SYNC_ERROR:
          listener->syncError(event);
          break;
      default:
          return FALSE;
    }

    return TRUE;
}


//
// Fire a Transport Event
//
bool fireTransportEvent(unsigned long size, int type) {

    ManageListener& manage = ManageListener::getInstance();
    TransportListener* listener = manage.getTransportListener();
    if(listener == NULL) {
        return FALSE;
    }

    unsigned long timestamp = (unsigned long)time(NULL);
    // Create event (object alive in the scope of this function)
    TransportEvent event(size, type, timestamp);

    switch(type) {
        case SEND_DATA_BEGIN:
          listener->sendDataBegin(event);
          break;
        case SEND_DATA_END:
          listener->syncDataEnd(event);
          break;
        case RECEIVE_DATA_BEGIN:
          listener->receiveDataBegin(event);
          break;
        case RECEIVE_DATA_END:
          listener->receiveDataEnd(event);
          break;
        case DATA_RECEIVED:
          listener->receivingData(event);
          break;
        default:
          return FALSE;
    }

    return TRUE;
}


//
// Fire a SyncSourceEvent
//
bool fireSyncSourceEvent(const char* sourceURI, const char* sourceName, SyncMode mode, int data, int type) {

    ManageListener& manage = ManageListener::getInstance();
    SyncSourceListener* listener = manage.getSyncSourceListener();
    if(listener == NULL) {
        return FALSE;
    }

    unsigned long timestamp = (unsigned long)time(NULL);
    // Create event (object alive in the scope of this function)
    SyncSourceEvent event(sourceURI, sourceName, mode, data, type, timestamp);

    switch(type) {
      case SYNC_SOURCE_BEGIN:
          listener->syncSourceBegin(event);
          break;
      case SYNC_SOURCE_END:
          listener->syncSourceEnd(event);
      case SYNC_SOURCE_SYNCMODE_REQUESTED:
          listener->syncSourceSyncModeRequested(event);
          break;
      case SYNC_SOURCE_TOTAL_CLIENT_ITEMS:
          listener->syncSourceTotalClientItems(event);
          break;
      case SYNC_SOURCE_TOTAL_SERVER_ITEMS:
          listener->syncSourceTotalServerItems(event);
          break;
      default:
          return FALSE;
    }

    return TRUE;
}


//
// Fire a SyncItemEvent
//
bool fireSyncItemEvent(const char* sourceURI, const char* sourcename, const WCHAR* itemKey, int type) {

    ManageListener& manage = ManageListener::getInstance();
    SyncItemListener* listener = manage.getSyncItemListener();
    if(listener == NULL) {
        return FALSE;
    }

    unsigned long timestamp = (unsigned long)time(NULL);
    // Create event (object alive in the scope of this function)
    SyncItemEvent event(itemKey, sourcename, sourceURI, type, timestamp);

    switch(type) {
        case ITEM_ADDED_BY_SERVER:
          listener->itemAddedByServer(event);
          break;
        case ITEM_DELETED_BY_SERVER:
          listener->itemDeletedByServer(event);
          break;
        case ITEM_UPDATED_BY_SERVER:
          listener->itemUpdatedByServer(event);
          break;
        case ITEM_ADDED_BY_CLIENT:
          listener->itemAddedByClient(event);
          break;
        case ITEM_DELETED_BY_CLIENT:
          listener->itemDeletedByClient(event);
          break;
        case ITEM_UPDATED_BY_CLIENT:
          listener->itemUpdatedByClient(event);
          break;
        default:
          return FALSE;
    }

    return TRUE;
}


//
// Fire a SyncStatusEvent
//
bool fireSyncStatusEvent(const char* command, int statusCode, const char* name, const char* uri, const WCHAR* itemKey, int type) {

    ManageListener& manage = ManageListener::getInstance();
    SyncStatusListener* listener = manage.getSyncStatusListener();
    if(listener == NULL) {
        return FALSE;
    }

    unsigned long timestamp = (unsigned long)time(NULL);
    // Create event (object alive in the scope of this function)
    SyncStatusEvent event(statusCode, command, itemKey, name, uri, type, timestamp);

    switch(type) {
        case CLIENT_STATUS:
            listener->statusSending(event);
            break;
        case SERVER_STATUS:
            listener->statusReceived(event);
            break;
        default:
            return FALSE;
    }

    return TRUE;
}


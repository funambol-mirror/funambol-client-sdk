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

#include "base/fscapi.h"
#include "event/FireEvent.h"
#include "event/ManageListener.h"
#include "base/globalsdef.h"

BEGIN_NAMESPACE

//
// Fire a SyncEvent
//
bool fireSyncEvent(const char* msg, int type) {

    ManageListener& m = ManageListener::getInstance();
    int n = m.countSyncListeners();
    if(n == 0) {
        return false;
    }

    SyncEvent event(type, (unsigned long)time(NULL));
    if(msg) {
        event.setMessage(msg);
    }

    for(int i=0; i<n; i++) {
        SyncListener* listener = m.getSyncListener(i);

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
              return false;
        }
    }

    return true;
}


//
// Fire a Transport Event
//
bool fireTransportEvent(unsigned long size, int type) {

    ManageListener& m = ManageListener::getInstance();
    int n = m.countTransportListeners();
    if(n == 0) {
        return false;
    }

    TransportEvent event(size, type, (unsigned long)time(NULL));

    for(int i=0; i<n; i++) {
        TransportListener* listener = m.getTransportListener(i);

        switch(type) {
            case SEND_DATA_BEGIN:
              listener->sendDataBegin(event);
              break;
            case SEND_DATA_END:
              listener->sendDataEnd(event);
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
              return false;
        }
    }

    return true;
}


//
// Fire a SyncSourceEvent
//
bool fireSyncSourceEvent(const char* sourceURI, const char* sourceName, SyncMode mode, int data, int type) {

    ManageListener& m = ManageListener::getInstance();
    int n = m.countSyncSourceListeners();
    if(n == 0) {
        return false;
    }

    SyncSourceEvent event(sourceURI, sourceName, mode, data, type, (unsigned long)time(NULL));

    for(int i=0; i<n; i++) {
        SyncSourceListener *listener = m.getSyncSourceListener(i);

        switch(type) {
          case SYNC_SOURCE_BEGIN:
              listener->syncSourceBegin(event);
              break;
          case SYNC_SOURCE_END:
              listener->syncSourceEnd(event);
              break;
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
              return false;
        }
    }

    return true;
}


//
// Fire a SyncItemEvent
//
bool fireSyncItemEvent(const char* sourceURI, const char* sourcename, const WCHAR* itemKey, int type) {

    ManageListener& m = ManageListener::getInstance();
    int n = m.countSyncItemListeners();
    if(n == 0) {
        return false;
    }

    SyncItemEvent event(itemKey, sourcename, sourceURI, type, (unsigned long)time(NULL));

    for(int i=0; i<n; i++) {
        SyncItemListener *listener = m.getSyncItemListener(i);

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
              return false;
        }
    }

    return true;
}


//
// Fire a SyncStatusEvent
//
bool fireSyncStatusEvent(const char* command, int statusCode, const char* name, const char* uri, const WCHAR* itemKey, int type) {

    ManageListener& m = ManageListener::getInstance();
    int n = m.countSyncStatusListeners();
    if(n == 0) {
        return false;
    }

    unsigned long timestamp = (unsigned long)time(NULL);
    // Create event (object alive in the scope of this function)
    SyncStatusEvent event(statusCode, command, itemKey, name, uri, type, timestamp);

    for(int i=0; i<n; i++) {
        SyncStatusListener *listener = m.getSyncStatusListener(i);

        switch(type) {
            case CLIENT_STATUS:
                listener->statusSending(event);
                break;
            case SERVER_STATUS:
                listener->statusReceived(event);
                break;
            default:
                return false;
        }
    }

    return true;
}

END_NAMESPACE

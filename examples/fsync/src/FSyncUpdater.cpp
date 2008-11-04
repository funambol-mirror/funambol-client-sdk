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

#include "FSyncUpdater.h"
#include <stdio.h>

#include "event/SyncItemListener.h"
#include "event/SyncListener.h"
#include "event/SyncSourceListener.h"
#include "event/TransportListener.h"
#include "event/ManageListener.h"
#include "spds/spdsutils.h"
#include "base/Log.h"

USE_NAMESPACE

/**
 * This class implements the SyncItemListener intrface in order to be notified
 * of the SyncItemEvents. It simply displays the sync messages on the standard
 * output.
 */
class FSyncItemListener : public SyncItemListener
{
    public:

        FSyncItemListener() { 
            itemsAddedByServerCount   = 0;
            itemsUpdatedByServerCount = 0;
            itemsDeletedByServerCount = 0;
            itemsAddedByClientCount   = 0;
            itemsUpdatedByClientCount = 0;
            itemsDeletedByClientCount = 0;
        }

        void itemAddedByServer (SyncItemEvent& event) {
            printf("new file received %d\n", ++itemsAddedByServerCount);
        }

        void itemDeletedByServer (SyncItemEvent& event) {
            printf("file \"%ls\" deleted from server\n", event.getItemKey());
        }

        void itemUpdatedByServer (SyncItemEvent& event) {
            printf("updated file received \"%ls\"\n", event.getItemKey());
        }

        void itemAddedByClient (SyncItemEvent& event) {
            printf("new file sent: \"%ls\"\n", event.getItemKey());
        }

        void itemDeletedByClient (SyncItemEvent& event) {
            printf("deleted file sent \"%ls\"\n", event.getItemKey());
        }

        void itemUpdatedByClient (SyncItemEvent& event) {
            printf("updated file sent \"%ls\"\n", event.getItemKey());
        }

    private:

        unsigned int itemsAddedByServerCount;
        unsigned int itemsUpdatedByServerCount;
        unsigned int itemsDeletedByServerCount;
        unsigned int itemsAddedByClientCount;
        unsigned int itemsUpdatedByClientCount;
        unsigned int itemsDeletedByClientCount;

};

/**
 * This class implements the SyncListener intrface in order to be notified of
 * the SyncEvents. It simply displays the sync messages on the standard
 * output.
 */
class FSyncListener : public SyncListener {
    public:

        void syncBegin (SyncEvent& event) {
            printf("\nBegin synchronization");
        }
        void syncEnd (SyncEvent& event) {
            printf("\nSynchronization done.\n");
        }
        void sendInitialization (SyncEvent& event) {
            printf("\nConnecting to the server");
        }
        void sendModifications (SyncEvent& event) {
            //printf("\nSending modifications");
        }
        void sendFinalization (SyncEvent& event) {
            printf("\nSending finalization");
        }
        void syncError (SyncEvent& event) {
            printf("\nSynchronization error: %s", event.getMessage());
        }
};

/**
 * This class implements the SyncSourceListener interface in order to be
 * notified of the SyncSourceEvents. It simply displays the sync messages on
 * the standard output.
 */
class FSyncSourceListener : public SyncSourceListener
{
    public:
  
        void syncSourceBegin (SyncSourceEvent& event) {
            //printf("Begin sync of files\n");
        }
        void syncSourceEnd (SyncSourceEvent& event) {
            //printf("End sync of files\n");
        }
        void syncSourceSyncModeRequested (SyncSourceEvent& ev) {
            SyncMode mode = (SyncMode)ev.getSyncMode();
            printf("\nPerforming a %s sync.\n", syncModeKeyword(mode));
        }
        void syncSourceTotalClientItems (SyncSourceEvent& ev) {
            // not used yet
            //printf("Syncing %d items from the client\n", ev.getData());
        }
        void syncSourceTotalServerItems (SyncSourceEvent& ev) {
            // not used yet
            //printf("Syncing %d items from the server\n", ev.getData());
        }

};

class FSyncTransportListener : public TransportListener
{
    public:
  
        void sendDataBegin    (TransportEvent& event) { putchar('.'); };
        void syncDataEnd      (TransportEvent& event) { putchar('.'); };
        void receiveDataBegin (TransportEvent& event) { putchar('.'); };
        void receivingData    (TransportEvent& event) { putchar('.'); };
        void receiveDataEnd   (TransportEvent& event) { putchar('.'); };

};

//----------------------------------------------------------------------- 

FSyncUpdater::FSyncUpdater() {
    
}

FSyncUpdater::~FSyncUpdater() {
    unsetListeners();
}

void FSyncUpdater::setListeners(VerboseLevel verbose) {
    ManageListener& lman = ManageListener::getInstance();
    
    LOG.debug("Set listeners.");
    if (verbose >= NORMAL) {
        lman.setSyncListener(new FSyncListener());
        if (verbose >= VERBOSE) {
            lman.setSyncItemListener(new FSyncItemListener());
            lman.setSyncSourceListener(new FSyncSourceListener());
            lman.setTransportListener(new FSyncTransportListener());
        }
    } 
}

void FSyncUpdater::unsetListeners() {
    ManageListener& lman = ManageListener::getInstance();

    LOG.debug("Unset listeners.");
    lman.unsetSyncListener();
    lman.unsetSyncItemListener();
    lman.unsetSyncSourceListener();
    lman.unsetTransportListener();

    ManageListener::dispose();
}



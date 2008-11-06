/*
 * Copyright (C) 2003-2007 Funambol, Inc
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


#ifndef INCL_SYNC_LISTENER
#define INCL_SYNC_LISTENER
/** @cond DEV */

#include "event/SyncEvent.h"
#include "event/Listener.h"


/*
 * Set Listeners for each event in SyncEvent.
*/

class SyncListener : public Listener {

public:

    //Contructor
    SyncListener();

    // listen for the Sync Begin Event
    virtual void syncBegin(SyncEvent& event);

    // listen for the Sync End Event
    virtual void syncEnd(SyncEvent& event);

    // listen for the Send Initialization Event
    virtual void sendInitialization(SyncEvent& event);

    // listen for the Send Modifications Event
    virtual void sendModifications(SyncEvent& event);

    // listen for the Sync Finalization Event
    virtual void sendFinalization(SyncEvent& event);

    // listen for the Sync Error Event
    virtual void syncError(SyncEvent& event);
};

/** @endcond */
#endif


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


#ifndef INCL_MANAGE_LISTENER
#define INCL_MANAGE_LISTENER
/** @cond DEV */

#include "event/SyncListener.h"
#include "event/SyncItemListener.h"
#include "event/SyncStatusListener.h"
#include "event/SyncSourceListener.h"
#include "event/TransportListener.h"

/* This is the ManageListener class - which keeps track of the various registered
 * listeners. This is implemented as an singleton pattern to make sure only instance is
 * active at any point of time.
 */
class ManageListener {

private:
    static ManageListener *instance;

    //Registered Listeners : At present only one Listener per event family
    SyncListener*       synclistener;
    TransportListener*  transportlistener;
    SyncStatusListener* syncstatuslistener;
    SyncItemListener*   syncitemlistener;
    SyncSourceListener* syncsourcelistener;

    //private constructor & destructor
    ManageListener();
    ~ManageListener();


public:
    //get and release singleton instance
    static ManageListener & getInstance();
    static void dispose();

    SyncListener*       getSyncListener();
    TransportListener*  getTransportListener();
    SyncSourceListener* getSyncSourceListener();
    SyncItemListener*   getSyncItemListener();
    SyncStatusListener* getSyncStatusListener();

    void setSyncListener      (SyncListener* listener);
    void setTransportListener (TransportListener* listener);
    void setSyncSourceListener(SyncSourceListener* listener);
    void setSyncItemListener  (SyncItemListener* listener);
    void setSyncStatusListener(SyncStatusListener* listener);

    void unsetSyncListener();
    void unsetTransportListener();
    void unsetSyncSourceListener();
    void unsetSyncItemListener();
    void unsetSyncStatusListener();

};

/** @endcond */
#endif

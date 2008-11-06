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

#include "event/ManageListener.h"

/* Static Variables */

ManageListener * ManageListener::instance = 0;

// Private Methods
//Contructor and Destructor

ManageListener::ManageListener() {

	synclistener       = NULL;
	transportlistener  = NULL;
	syncitemlistener   = NULL;
	syncstatuslistener = NULL;
	syncsourcelistener = NULL;
}

ManageListener::~ManageListener() {

	if(synclistener) {
		delete synclistener;
        synclistener = NULL;
	}
	if(transportlistener) {
		delete transportlistener;
        transportlistener = NULL;
	}
	if(syncitemlistener) {
		delete syncitemlistener;
        syncitemlistener = NULL;
	}
	if(syncsourcelistener) {
		delete syncsourcelistener;
        syncsourcelistener = NULL;
	}
	if(syncstatuslistener) {
		delete syncstatuslistener;
        syncstatuslistener = NULL;
	}
}


//--------------------- Public Methods ----------------------

/*
 * Get, or create, ManageListener instance
 */
ManageListener& ManageListener::getInstance() {

	if(instance == NULL) {
		instance = new ManageListener();
	}
	return *instance;
}

void ManageListener::dispose() {

	if(instance) {
		delete instance;
	}
	instance = NULL;
}


//
// Get listeners (return internal pointer):
//
SyncListener* ManageListener::getSyncListener() {
    return synclistener;
}
TransportListener* ManageListener::getTransportListener() {
    return transportlistener;
}
SyncSourceListener* ManageListener::getSyncSourceListener() {
    return syncsourcelistener;
}
SyncItemListener* ManageListener::getSyncItemListener() {
    return syncitemlistener;
}
SyncStatusListener* ManageListener::getSyncStatusListener() {
    return syncstatuslistener;
}



//
// Set listeners:
//
void ManageListener::setSyncListener(SyncListener* listener) {
    if(synclistener) {
        delete synclistener;
    }
    synclistener = listener;
}

void ManageListener::setTransportListener(TransportListener* listener) {
    if(transportlistener) {
        delete transportlistener;
    }
    transportlistener = listener;
}

void ManageListener::setSyncSourceListener(SyncSourceListener* listener) {
    if(syncsourcelistener) {
        delete syncsourcelistener;
    }
    syncsourcelistener = listener;
}

void ManageListener::setSyncItemListener(SyncItemListener* listener) {
    if(syncitemlistener) {
        delete syncitemlistener;
    }
    syncitemlistener = listener;
}

void ManageListener::setSyncStatusListener(SyncStatusListener* listener) {
    if(syncstatuslistener) {
        delete syncstatuslistener;
    }
    syncstatuslistener = listener;
}

//
// Unset listeners:
//
void ManageListener::unsetSyncListener() {
    if(synclistener) {
        delete synclistener;
        synclistener = NULL;
    }
}

void ManageListener::unsetTransportListener() {
    if(transportlistener) {
        delete transportlistener;
        transportlistener = NULL;
    }
}

void ManageListener::unsetSyncSourceListener() {
    if(syncsourcelistener) {
        delete syncsourcelistener;
        syncsourcelistener = NULL;
    }
}

void ManageListener::unsetSyncItemListener() {
    if(syncitemlistener) {
        delete syncitemlistener;
        syncitemlistener = NULL;
    }
}

void ManageListener::unsetSyncStatusListener() {
    if(syncstatuslistener) {
        delete syncstatuslistener;
        syncstatuslistener = NULL;
    }
}


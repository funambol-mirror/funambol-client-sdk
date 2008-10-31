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

#include "event/ManageListener.h"
#include "base/globalsdef.h"

USE_NAMESPACE

class DestroyManageListener{
public:
    DestroyManageListener() {           };
    ~DestroyManageListener(){ delete &(ManageListener::getInstance());}
};

DestroyManageListener destroyManageListener;

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


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

#ifndef INCL_SET_LISTENER
#define INCL_SET_LISTENER
/** @cond DEV */

#include "event/SyncListener.h"
#include "event/SyncSourceListener.h"
#include "event/SyncItemListener.h"
#include "event/SyncStatusListener.h"
#include "event/TransportListener.h"

/*
 * A set of global functions to either set or unset Listeners
 * for various events.
 */

// Set the SyncEvent Listener
void setSyncListener(SyncListener* listener);

// Set the TransportEvent Listener
void setTransportListener(TransportListener* listener);

// Set the SyncSourceEvent Listener
void setSyncSourceListener(SyncSourceListener* listener);

// Set the SyncItemEvent Listener
void setSyncItemListener(SyncItemListener* listener);

// Set the SyncStatusEvent Listener
void setSyncStatusListener(SyncStatusListener* listener);


//removes the SyncEvent Listener
void unsetSyncListener();

// Removes the TransportEvent Listener
void unsetTransportListener();

// Removes the SyncSourceEvent Listener
void unsetSyncSourceListener();

// Removes the SyncItemEvent Listener
void unsetSyncItemListener();

// Removes the SyncStatusEvent Listener
void unsetSyncStatusListener();


/** @endcond */
#endif

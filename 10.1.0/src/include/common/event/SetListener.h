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

#ifndef INCL_SET_LISTENER
#define INCL_SET_LISTENER
/** @cond DEV */

#include "event/SyncListener.h"
#include "event/SyncSourceListener.h"
#include "event/SyncItemListener.h"
#include "event/SyncStatusListener.h"
#include "event/TransportListener.h"

BEGIN_NAMESPACE

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
void unsetSyncListener(const char* name="");

// Removes the TransportEvent Listener
void unsetTransportListener(const char* name="");

// Removes the SyncSourceEvent Listener
void unsetSyncSourceListener(const char* name="");

// Removes the SyncItemEvent Listener
void unsetSyncItemListener(const char* name="");

// Removes the SyncStatusEvent Listener
void unsetSyncStatusListener(const char* name="");

END_NAMESPACE

/** @endcond */
#endif

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


#ifndef INCL_MANAGE_LISTENER
#define INCL_MANAGE_LISTENER
/** @cond DEV */

#include "event/SyncListener.h"
#include "event/SyncItemListener.h"
#include "event/SyncStatusListener.h"
#include "event/SyncSourceListener.h"
#include "event/TransportListener.h"
#include "base/globalsdef.h"

BEGIN_NAMESPACE

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


public:
    ~ManageListener();
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


END_NAMESPACE

/** @endcond */
#endif

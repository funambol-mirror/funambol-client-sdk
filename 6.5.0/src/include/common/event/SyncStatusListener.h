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


#ifndef INCL_SYNC_STATUS_LISTENER
#define INCL_SYNC_STATUS_LISTENER
/** @cond DEV */

#include "event/SyncStatusEvent.h"
#include "event/Listener.h"

/*
Set Listeners for each event in SyncStatusEvent
*/

class SyncStatusListener : public Listener {

public:
    // Constructor
    SyncStatusListener();

    // listen for the Status Received Event
    virtual void statusReceived(SyncStatusEvent& event);

    // listen for the Status Sending Event
    virtual void statusSending(SyncStatusEvent& event);
};

/** @endcond */
#endif


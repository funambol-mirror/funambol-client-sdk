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


#ifndef INCL_TRANSPORT_LISTENER
#define INCL_TRANSPORT_LISTENER
/** @cond DEV */

#include "event/TransportEvent.h"
#include "event/Listener.h"


/*
 * Set Listeners for each event in TransportEvent
 */

class TransportListener : public Listener{

public:

    // Constructor
    TransportListener();

    // listen for the Send Data Begin Event
    virtual void sendDataBegin(TransportEvent& event);

    // listen for the Sync Data End Event
    virtual void syncDataEnd(TransportEvent& event);

    // listen for the Receive Data Begin Event
    virtual void receiveDataBegin(TransportEvent& event);

    // listen for the Receiving Data Event
    virtual void receivingData(TransportEvent& event);

    // listen for the Receive Data End Event
    virtual void receiveDataEnd(TransportEvent& event);
};

/** @endcond */
#endif


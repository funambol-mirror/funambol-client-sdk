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


#include "event/TransportListener.h"
#include "event/TransportEvent.h"
#include "event/constants.h"

/*
 * Empty TransportListener methods.
 * Application developers override the methods corresponding to the
 * events they are listening for.
*/

TransportListener::TransportListener() : Listener() {}

void TransportListener::sendDataBegin   (TransportEvent& event) {}
void TransportListener::syncDataEnd     (TransportEvent& event) {}
void TransportListener::receiveDataBegin(TransportEvent& event) {}
void TransportListener::receivingData   (TransportEvent& event) {}
void TransportListener::receiveDataEnd  (TransportEvent& event) {}

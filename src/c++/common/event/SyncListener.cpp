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


#include "event/SyncListener.h"
#include "event/SyncEvent.h"
#include "event/constants.h"

/*
 * Empty SyncListener methods.
 * Application developers override the methods corresponding to the
 * events they are listening for.
*/

SyncListener::SyncListener() : Listener() {}

void SyncListener::syncBegin         (SyncEvent& event) {}
void SyncListener::syncEnd           (SyncEvent& event) {}
void SyncListener::syncError         (SyncEvent& event) {}
void SyncListener::sendInitialization(SyncEvent& event) {}
void SyncListener::sendModifications (SyncEvent& event) {}
void SyncListener::sendFinalization  (SyncEvent& event) {}

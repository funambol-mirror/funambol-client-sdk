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


#include "event/SetListener.h"
#include "event/ManageListener.h"

//
// Set listeners:
//
void setSyncListener(SyncListener* listener) {
    ManageListener& manage = ManageListener::getInstance();
    manage.setSyncListener(listener);
}

void setTransportListener(TransportListener* listener) {
    ManageListener& manage = ManageListener::getInstance();
    manage.setTransportListener(listener);
}

void setSyncSourceListener(SyncSourceListener* listener) {
    ManageListener& manage = ManageListener::getInstance();
    manage.setSyncSourceListener(listener);
}

void setSyncItemListener(SyncItemListener* listener) {
    ManageListener& manage = ManageListener::getInstance();
    manage.setSyncItemListener(listener);
}

void setSyncStatusListener(SyncStatusListener* listener) {
    ManageListener& manage = ManageListener::getInstance();
    manage.setSyncStatusListener(listener);
}

//
// Unset listeners:
//
void unsetSyncListener() {
    ManageListener& manage = ManageListener::getInstance();
    manage.unsetSyncListener();
}

void unsetTransportListener() {
    ManageListener& manage = ManageListener::getInstance();
    manage.unsetTransportListener();
}

void unsetSyncSourceListener() {
    ManageListener& manage = ManageListener::getInstance();
    manage.unsetSyncSourceListener();
}

void unsetSyncItemListener() {
    ManageListener& manage = ManageListener::getInstance();
    manage.unsetSyncItemListener();
}

void unsetSyncStatusListener() {
    ManageListener& manage = ManageListener::getInstance();
    manage.unsetSyncStatusListener();
}


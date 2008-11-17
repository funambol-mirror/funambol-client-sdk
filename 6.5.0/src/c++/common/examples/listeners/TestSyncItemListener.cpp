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


#include "examples/listeners/TestSyncItemListener.h"

void TestSyncItemListener::itemAddedByServer(SyncItemEvent &event) {
    printf("SyncItemEvent occurred.\n");
    printf("SyncItem %ls from sourceURI %s added by server, detected at %ld.\n\n",event.getItemKey(), event.getSourceURI(), event.getDate());
}

void TestSyncItemListener::itemDeletedByServer(SyncItemEvent &event) {
    printf("SyncItemEvent occurred.\n");
    printf("SyncItem %ls from sourceURI %s deleted by server, detected at %ld.\n\n",event.getItemKey(), event.getSourceURI(), event.getDate());
}

void TestSyncItemListener::itemUpdatedByServer(SyncItemEvent &event) {
    printf("SyncItemEvent occurred.\n");
    printf("SyncItem %ls from sourceURI %s updated by server, detected at %ld.\n\n",event.getItemKey(), event.getSourceURI(), event.getDate());
}

void TestSyncItemListener::itemAddedByClient(SyncItemEvent &event) {
    printf("SyncItemEvent occurred.\n");
    printf("SyncItem %ls from sourceURI %s added by client at %ld.\n\n",event.getItemKey(), event.getSourceURI(), event.getDate());
}

void TestSyncItemListener::itemUpdatedByClient(SyncItemEvent &event) {
    printf("SyncItemEvent occurred.\n");
    printf("SyncItem %ls from sourceURI %s updated by client at %ld.\n\n",event.getItemKey(), event.getSourceURI(), event.getDate());
}

void TestSyncItemListener::itemDeletedByClient(SyncItemEvent &event) {
    printf("SyncItemEvent occurred.\n");
    printf("SyncItem %ls from sourceURI %s deleted by client at %ld.\n\n",event.getItemKey(), event.getSourceURI(), event.getDate());
}



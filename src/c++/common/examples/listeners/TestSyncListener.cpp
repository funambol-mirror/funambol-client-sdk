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


#include "examples/listeners/TestSyncListener.h"

void TestSyncListener::syncBegin(SyncEvent &event) {
    printf("SyncEvent occurred.\n");
    printf("Syncing successfully began at %ld.\n\n", event.getDate());
}

void TestSyncListener::syncEnd(SyncEvent &event) {
    printf("SyncEvent occurred.\n");
    printf("Syncing successfully ended at %ld.\n\n", event.getDate());
}

void TestSyncListener::sendInitialization( SyncEvent &event) {
    printf("SyncEvent occurred.\n");
    printf("Initializations done at %ld.\n\n", event.getDate());
}

void TestSyncListener::sendModifications(SyncEvent &event) {
    printf("SyncEvent occurred.\n");
    printf("Modifications processed at %ld.\n\n", event.getDate());
}

void TestSyncListener::sendFinalization(SyncEvent &event) {
    printf("SyncEvent occurred.\n");
    printf("final package set & processed at %ld.\n\n", event.getDate());
}

void TestSyncListener::syncError(SyncEvent &event) {
    printf("SyncEvent occurred.\n");
    printf("Sync Error %s occured began at %ld.\n\n", event.getMessage(), event.getDate());
}



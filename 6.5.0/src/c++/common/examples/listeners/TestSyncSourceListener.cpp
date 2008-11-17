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

#include <stdio.h>
#include "examples/listeners/TestSyncSourceListener.h"

void TestSyncSourceListener::syncSourceBegin(SyncSourceEvent &event) {
    printf("SyncSourceEvent occurred.\n");
    printf("Syncing Source %s (uri = %s) in syncmode %d successfully began at %ld.\n\n", event.getSourceName(), event.getSourceURI(), event.getSyncMode(), event.getDate());
}

void TestSyncSourceListener::syncSourceEnd(SyncSourceEvent &event) {
    printf("SyncSourceEvent occurred.\n");
    printf("Syncing Source %s (uri = %s) in syncmode %d successfully ended at %ld.\n\n", event.getSourceName(), event.getSourceURI(), event.getSyncMode(), event.getDate());
}

void TestSyncSourceListener::syncSourceSyncModeRequested(SyncSourceEvent &event) {
    printf("SyncSourceEvent occurred.\n");
    printf("Syncing Source %s (uri = %s) requested for syncmode %d at %ld.\n\n", event.getSourceName(), event.getSourceURI(), event.getSyncMode(), event.getDate());
}

void TestSyncSourceListener::syncSourceTotalClientItems(SyncSourceEvent &event) {
    printf("SyncSourceEvent occurred.\n");
    printf("Syncing Source %s (uri = %s): total client items = %d at %ld.\n\n", event.getSourceName(), event.getSourceURI(), event.getData(), event.getDate());
}

void TestSyncSourceListener::syncSourceTotalServerItems(SyncSourceEvent &event) {
    printf("SyncSourceEvent occurred.\n");
    if (event.getData() != -1)
        printf("Syncing Source %s (uri = %s): total server items = %d at %ld.\n\n", event.getSourceName(), event.getSourceURI(), event.getData(), event.getDate());
    else
        printf("Syncing Source %s (uri = %s): total server items not specified by server.\n\n", event.getSourceName(), event.getSourceURI());

}
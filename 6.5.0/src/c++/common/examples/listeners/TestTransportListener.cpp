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

#include "stdio.h"
#include "examples/listeners/TestTransportListener.h"

void TestTransportListener::sendDataBegin(TransportEvent &event) {
    printf("TransportEvent occurred.\n");
    printf("Sending Data of size %d began at %ld.\n\n",event.getDataSize(), event.getDate());
}

void TestTransportListener::syncDataEnd(TransportEvent &event) {
    printf("TransportEvent occurred.\n");
    printf("Sync Data of size %d finished at %ld.\n\n",event.getDataSize(), event.getDate());
}

void TestTransportListener::receiveDataBegin(TransportEvent &event) {
    printf("TransportEvent occurred.\n");
    printf("Begun Receiving Data of size %d began at %ld.\n\n",event.getDataSize(), event.getDate());
}

void TestTransportListener::receivingData(TransportEvent &event) {
    printf("TransportEvent occurred.\n");
    printf("Receiving Data of size %d began at %ld.\n\n",event.getDataSize(), event.getDate());
}

void TestTransportListener::receiveDataEnd(TransportEvent &event) {
    printf("TransportEvent occurred.\n");
    printf("Receiving Data done of size %d began at %ld.\n\n",event.getDataSize(), event.getDate());
}


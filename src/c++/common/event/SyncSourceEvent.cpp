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


#include "event/SyncSourceEvent.h"
#include "base/util/utils.h"

SyncSourceEvent::SyncSourceEvent(const char* uri, const char* sourcename, int mode, int data, int type, unsigned long date) : BaseEvent(type, date) {

    sourceURI = stringdup(uri);
    syncMode  = mode;
    name = stringdup(sourcename);
    this->data = data;
}

SyncSourceEvent::~SyncSourceEvent() {

    if(sourceURI) {
        delete [] sourceURI;
        sourceURI = NULL;
    }
    if(name) {
        delete [] name;
        name = NULL;
    }
}

const char* SyncSourceEvent::getSourceURI() const{
    return sourceURI;
}

int SyncSourceEvent::getSyncMode() {
    return syncMode;
}

const char* SyncSourceEvent::getSourceName() const{
    return name;
}

int SyncSourceEvent::getData() {
    return data;
}

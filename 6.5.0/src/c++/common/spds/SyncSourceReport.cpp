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


#include "spds/SyncSourceReport.h"
#include "spds/SyncReport.h"
#include "spds/ItemReport.h"

const char* const SyncSourceReport::targets[] = {
    CLIENT,
    SERVER,
    NULL
};

const char* const SyncSourceReport::commands[] = {
    COMMAND_ADD,
    COMMAND_REPLACE,
    COMMAND_DELETE,
    NULL
};



//--------------------------------------------------- Constructor & Destructor
SyncSourceReport::SyncSourceReport(const char* name) {

    initialize();

    if (name) {
        setSourceName(name);
    }

    clientAddItems = new ArrayList();
    clientModItems = new ArrayList();
    clientDelItems = new ArrayList();

    serverAddItems = new ArrayList();
    serverModItems = new ArrayList();
    serverDelItems = new ArrayList();
}

SyncSourceReport::SyncSourceReport(SyncSourceReport& ssr) {
    initialize();
    assign(ssr);
}

SyncSourceReport::~SyncSourceReport() {

    if (lastErrorMsg) {
        delete [] lastErrorMsg;
        lastErrorMsg = NULL;
    }
    if (sourceName) {
        delete [] sourceName;
        sourceName = NULL;
    }

    clientAddItems->clear();
    clientModItems->clear();
    clientDelItems->clear();

    serverAddItems->clear();
    serverModItems->clear();
    serverDelItems->clear();
}



//------------------------------------------------------------- Public Methods

const int SyncSourceReport::getLastErrorCode() const {
    return lastErrorCode;
}
void SyncSourceReport::setLastErrorCode(const int code) {
    lastErrorCode = code;
}

const SourceState SyncSourceReport::getState() const {
    return state;
}
void SyncSourceReport::setState(const SourceState s) {
    state = s;
}

const char* SyncSourceReport::getLastErrorMsg() const {
    return lastErrorMsg;
}
void SyncSourceReport::setLastErrorMsg(const char* msg) {
    if (lastErrorMsg) {
        delete [] lastErrorMsg;
        lastErrorMsg = NULL;
    }
    lastErrorMsg = stringdup(msg);
}

const char* SyncSourceReport::getSourceName() const {
    return sourceName;
}
void SyncSourceReport::setSourceName(const char* name) {
    if (sourceName) {
        delete [] sourceName;
        sourceName = NULL;
    }
    sourceName = stringdup(name);
}


bool SyncSourceReport::checkState() {
    if (state == SOURCE_ACTIVE) {
        return true;
    }
    return false;
}


ItemReport* SyncSourceReport::getItemReport(const char* target, const char* command, int index) {

    ArrayList* list = getList(target, command);

    if (index<0 || index >= list->size()) {
        return NULL;
    }
    return (ItemReport*)list->get(index);
}


void SyncSourceReport::addItem(const char* target, const char* command, const WCHAR* ID,
                               const int status, const WCHAR* statusMessage) {

    // Create the ItemReport element
    ItemReport element(ID, status, statusMessage);

    // Add element in the corresponding list
    ArrayList* list = getList(target, command);


    // If the element is already present -> no add, only replace status with the new one.
  /*  ItemReport* ie = NULL;
    for (int i=0; i<list->size(); i++) {
        ie = getItemReport(target, command, i);
        if ( !wcscmp(element.getId(), ie->getId()) ) {
            ie->setStatus(status);
            return;
        }
    }*/

    // If here, element is new -> add.
    list->add(element);
}


int SyncSourceReport::getItemReportCount(const char* target, const char* command) {
    ArrayList* list = getList(target, command);
    return list->size();
}

int SyncSourceReport::getItemReportSuccessfulCount(const char* target, const char* command) {

    ArrayList* list = getList(target, command);
    ItemReport* e;

    // Scan for succesful codes
    int good = 0;
    if (list->size() > 0) {
        e = (ItemReport*)list->front();
        if ( isSuccessful(e->getStatus()) ) good++;
        for (int i=1; i<list->size(); i++) {
            e = (ItemReport*)list->next();
            if ( isSuccessful(e->getStatus()) ) good++;
        }
    }
    return good;
}


int SyncSourceReport::getItemReportFailedCount(const char* target, const char* command) {

    ArrayList* list = getList(target, command);
    if (list->size() == 0) {
        return 0;
    }
    int good = getItemReportSuccessfulCount(target, command);
    return (list->size() - good);
}


int SyncSourceReport::getItemReportAlreadyExistCount(const char* target, const char* command) {

    ArrayList* list = getList(target, command);
    ItemReport* e;

    // Scan for code 418 = ALREADY_EXISTS
    int found = 0;
    if (list->size() > 0) {
        e = (ItemReport*)list->front();
        if (e->getStatus() == ALREADY_EXISTS) found++;
        for (int i=1; i<list->size(); i++) {
            e = (ItemReport*)list->next();
            if (e->getStatus() == ALREADY_EXISTS) found++;
        }
    }
    return found;
}



ArrayList* SyncSourceReport::getList(const char* target, const char* command) const {

    ArrayList* ret = NULL;

    if (!strcmp(target, CLIENT)) {
        if (!strcmp(command, COMMAND_ADD)) {
            ret = clientAddItems;
        }
        else if (!strcmp(command, COMMAND_REPLACE)) {
            ret = clientModItems;
        }
        else if (!strcmp(command, COMMAND_DELETE)) {
            ret = clientDelItems;
        }
        else {
            // error
        }
    }
    else if (!strcmp(target, SERVER)) {
        if (!strcmp(command, COMMAND_ADD)) {
            ret = serverAddItems;
        }
        else if (!strcmp(command, COMMAND_REPLACE)) {
            ret = serverModItems;
        }
        else if (!strcmp(command, COMMAND_DELETE)) {
            ret = serverDelItems;
        }
        else {
            // error
        }
    }
    else {
        // error
    }

    return ret;
}


//------------------------------------------------------------- Private Methods

bool SyncSourceReport::isSuccessful(const int status) {
    if (status >= 200 && status < 500)
        return true;
    else
        return false;
}

void SyncSourceReport::initialize() {
    lastErrorCode  = ERR_NONE;
    lastErrorMsg   = NULL;
    sourceName     = NULL;
    state          = SOURCE_INACTIVE;
    clientAddItems = NULL;
    clientModItems = NULL;
    clientDelItems = NULL;
    serverAddItems = NULL;
    serverModItems = NULL;
    serverDelItems = NULL;
}

void SyncSourceReport::assign(const SyncSourceReport& ssr) {

    setLastErrorCode(ssr.getLastErrorCode());
    setLastErrorMsg (ssr.getLastErrorMsg ());
    setSourceName   (ssr.getSourceName   ());
    setState        (ssr.getState        ());

    clientAddItems = ssr.getList(CLIENT, COMMAND_ADD)->clone();
    clientModItems = ssr.getList(CLIENT, COMMAND_REPLACE)->clone();
    clientDelItems = ssr.getList(CLIENT, COMMAND_DELETE)->clone();

    serverAddItems = ssr.getList(SERVER, COMMAND_ADD)->clone();
    serverModItems = ssr.getList(SERVER, COMMAND_REPLACE)->clone();
    serverDelItems = ssr.getList(SERVER, COMMAND_DELETE)->clone();
}

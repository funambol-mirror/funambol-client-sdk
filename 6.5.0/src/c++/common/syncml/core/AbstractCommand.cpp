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


#include "syncml/core/AbstractCommand.h"

AbstractCommand::AbstractCommand() {
    initialize();
}

AbstractCommand::AbstractCommand(CmdID* cmdID, BOOL noResp) {
    initialize();
    set(cmdID, noResp);
}

/**
 * Create a new AbstractCommand object with the given commandIdentifier
 *
 * @param cmdID the command identifier - NOT NULL
 *
 */
 AbstractCommand::AbstractCommand(CmdID* cmdID) {
    initialize();
    set(cmdID, FALSE);
}

void AbstractCommand::set(CmdID* cmdID, BOOL noResp) {
    setCmdID(cmdID);
    if (noResp != NULL) {
        this->noResp  = (noResp == TRUE) ? TRUE : FALSE;
    } else {
        this->noResp  = NULL;
    }
}

AbstractCommand::AbstractCommand(CmdID* cmdID,
                                 BOOL noResp,
                                 Meta* meta) {
        initialize();

        setCmdID(cmdID);
        if (noResp != NULL) {
            this->noResp  = (noResp == TRUE) ? TRUE : FALSE;
        } else {
            this->noResp  = NULL;
        }
        setMeta(meta);
    }

void AbstractCommand::initialize() {
     cmdID  = NULL;
     noResp = FALSE;
     meta   = NULL;
     credential   = NULL;
 }

AbstractCommand::~AbstractCommand() {

    if (cmdID)      {delete cmdID; cmdID = NULL; }
    if (meta)       {delete meta; meta = NULL; }
    if (credential) {delete credential; credential = NULL; }

    noResp = FALSE;
}

/**
 * Get CommandIdentifier property
 *
 * @return the command identifier - NOT NULL
 */
 CmdID* AbstractCommand::getCmdID() {
    return this->cmdID;
}

/**
 * Sets the CommandIdentifier property
 *
 * @param cmdID the command identifier
 *
 */
 void AbstractCommand::setCmdID(CmdID* cmdID) {
    if (this->cmdID) {
        delete this->cmdID; this->cmdID = NULL;
    }
    if (cmdID) {
        this->cmdID = cmdID->clone();
    }
}

/**
 * Gets noResp property
 *
 * @return true if the command doesn't require a response, false otherwise
 */
 BOOL AbstractCommand::isNoResp() {
    return (noResp != NULL);
}


 BOOL AbstractCommand::getNoResp() {
    return noResp;
}

/**
 * Sets noResp true if no response is required
 *
 * @param noResp is true if no response is required
 *
 */
 void AbstractCommand::setNoResp(BOOL noResp) {
     if ((noResp == NULL) || (noResp != TRUE && noResp != FALSE)) {
        this->noResp = NULL;
     } else {
        this->noResp = noResp;
     }
}


/**
* Gets Credential object
*
* @return the Credential object
*/
Cred* AbstractCommand::getCred() {
    return credential;

}

/**
* Sets authentication credential
*
* @param cred the authentication credential
*
*/
void AbstractCommand::setCred(Cred* cred) {

    if (credential) {
        delete credential; credential = NULL;
    }
    if (cred) {
        credential = cred->clone();
    } else {
        credential = NULL;
    }
}

/**
* Gets an Meta object
*
* @return an Meta object
*/
Meta* AbstractCommand::getMeta() {
    return meta;
}

/**
* Sets Meta object
*
* @param meta the meta object
*
*/
void AbstractCommand::setMeta(Meta* meta) {

    if (this->meta) {
        delete this->meta; this->meta = NULL;
    }
    if (meta) {
        this->meta = meta->clone();
    }

}

/*
 * Funambol is a mobile platform developed by Funambol, Inc. 
 * Copyright (C) 2003 - 2007 Funambol, Inc.
 * 
 * This program is free software; you can redistribute it and/or modify it under
 * the terms of the GNU Affero General Public License version 3 as published by
 * the Free Software Foundation with the addition of the following permission 
 * added to Section 15 as permitted in Section 7(a): FOR ANY PART OF THE COVERED
 * WORK IN WHICH THE COPYRIGHT IS OWNED BY FUNAMBOL, FUNAMBOL DISCLAIMS THE 
 * WARRANTY OF NON INFRINGEMENT  OF THIRD PARTY RIGHTS.
 * 
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
 * details.
 * 
 * You should have received a copy of the GNU Affero General Public License 
 * along with this program; if not, see http://www.gnu.org/licenses or write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 * MA 02110-1301 USA.
 * 
 * You can contact Funambol, Inc. headquarters at 643 Bair Island Road, Suite 
 * 305, Redwood City, CA 94063, USA, or at email address info@funambol.com.
 * 
 * The interactive user interfaces in modified source and object code versions
 * of this program must display Appropriate Legal Notices, as required under
 * Section 5 of the GNU Affero General Public License version 3.
 * 
 * In accordance with Section 7(b) of the GNU Affero General Public License
 * version 3, these Appropriate Legal Notices must retain the display of the
 * "Powered by Funambol" logo. If the display of the logo is not reasonably 
 * feasible for technical reasons, the Appropriate Legal Notices must display
 * the words "Powered by Funambol".
 */


#include "syncml/core/AbstractCommand.h"
#include "base/globalsdef.h"

USE_NAMESPACE

AbstractCommand::AbstractCommand() {
    initialize();
}

AbstractCommand::AbstractCommand(CmdID* cmdID, bool noResp) {
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
    set(cmdID, false);
}

void AbstractCommand::set(CmdID* cmdID, bool noResp) {
    setCmdID(cmdID);
    this->noResp  = noResp;
}

AbstractCommand::AbstractCommand(CmdID* cmdID,
                                 bool noResp,
                                 Meta* meta) {
    initialize();

    setCmdID(cmdID);
    this->noResp  = noResp;
    setMeta(meta);
}

void AbstractCommand::initialize() {
     cmdID  = NULL;
     noResp = false;
     meta   = NULL;
     credential   = NULL;
 }

AbstractCommand::~AbstractCommand() {

    if (cmdID)      {delete cmdID; cmdID = NULL; }
    if (meta)       {delete meta; meta = NULL; }
    if (credential) {delete credential; credential = NULL; }

    noResp = false;
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
 bool AbstractCommand::isNoResp() {
    return noResp;
}


 bool AbstractCommand::getNoResp() {
    return noResp;
}

/**
 * Sets noResp true if no response is required
 *
 * @param noResp is true if no response is required
 *
 */
 void AbstractCommand::setNoResp(bool noResp) {
     this->noResp = noResp;
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

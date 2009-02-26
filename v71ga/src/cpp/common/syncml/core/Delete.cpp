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


#include "syncml/core/Delete.h"
#include "base/globalsdef.h"

USE_NAMESPACE

Delete::Delete() {
    COMMAND_NAME = new char[strlen(DELETE_COMMAND_NAME) + 1];
    sprintf(COMMAND_NAME, DELETE_COMMAND_NAME);


}
Delete::~Delete() {
    if (COMMAND_NAME) {
        delete [] COMMAND_NAME; COMMAND_NAME = NULL;
    }
    archive = false;
    sftDel  = false;
}

/**
* Creates a new Delete object with the given command identifier,
* noResponse, archiveData, softDelete, credential, meta and array of item
*
* @param cmdID the command identifier - NOT NULL
* @param noResp true if no response is required
* @param archive true if the deleted data should be archived
* @param sftDel true if this is a "soft delete". If set to false, then
*                   this delete command is a "hard delete"
* @param cred the authentication credential
* @param meta the meta data
* @param items the array of item - NOT NULL
*
*/
Delete::Delete(CmdID* cmdID,
               bool noResp,
               bool archive,
               bool sftDel,
               Cred* cred,
               Meta* meta,
               ArrayList* items) : ModificationCommand(cmdID, meta, items) {

    setCred(cred);
    setNoResp(noResp);
    setArchive(archive);
    setSftDel(sftDel);

    COMMAND_NAME = new char[strlen(DELETE_COMMAND_NAME) + 1];
    sprintf(COMMAND_NAME, DELETE_COMMAND_NAME);
}

/**
* Gets the command name property
*
* @return the command name property
*/
const char* Delete::getName() {
    return COMMAND_NAME;
}

/**
* Gets the Archive property
*
* @return true if the deleted data should be archived
*/
bool Delete::isArchive() {
     return archive;
}

/**
* Gets the Boolean archive property
*
* @return archive the Boolean archive property
*/
bool Delete::getArchive() {
    return archive;
}

/**
* Sets the archive property
*
* @param archive the Boolean archive object
*/
void Delete::setArchive(bool archive) {
    this->archive = archive;
}

/**
* Gets the SftDel property
*
* @return <b>true</b>  if this is a "Soft delete"
*         <b>false</b> if this is a "hard delete"
*/
bool Delete::isSftDel() {
    return sftDel;
}

bool Delete::getSftDel() {
    return sftDel;
}


void Delete::setSftDel(bool sftDel) {
    this->sftDel = sftDel;
}

ArrayElement* Delete::clone() {
    Delete* ret = new Delete(getCmdID(), getNoResp(), archive, sftDel, getCred(), getMeta(), getItems());
    return ret;
}

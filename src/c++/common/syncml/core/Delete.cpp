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


#include "syncml/core/Delete.h"

Delete::Delete() {
    COMMAND_NAME = new char[strlen(DELETE_COMMAND_NAME) + 1];
    sprintf(COMMAND_NAME, DELETE_COMMAND_NAME);


}
Delete::~Delete() {
    if (COMMAND_NAME) {
        delete [] COMMAND_NAME; COMMAND_NAME = NULL;
    }
    archive = FALSE;
    sftDel  = FALSE;
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
               BOOL noResp,
               BOOL archive,
               BOOL sftDel,
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
BOOL Delete::isArchive() {
     return (archive != NULL);
}

/**
* Gets the Boolean archive property
*
* @return archive the Boolean archive property
*/
BOOL Delete::getArchive() {
    return archive;
}

/**
* Sets the archive property
*
* @param archive the Boolean archive object
*/
void Delete::setArchive(BOOL archive) {
    if ((archive == NULL) || (archive != TRUE && archive != FALSE)) {
        this->archive = NULL;
    } else {
        this->archive = archive;
    }

}

/**
* Gets the SftDel property
*
* @return <b>true</b>  if this is a "Soft delete"
*         <b>false</b> if this is a "hard delete"
*/
BOOL Delete::isSftDel() {
    return (sftDel != NULL);
}

BOOL Delete::getSftDel() {
    return sftDel;
}


void Delete::setSftDel(BOOL sftDel) {
    if ((sftDel == NULL) || (sftDel != TRUE && sftDel != FALSE)) {
        this->sftDel = NULL;
    } else {
        this->sftDel = sftDel;
    }
}

ArrayElement* Delete::clone() {
    Delete* ret = new Delete(getCmdID(), getNoResp(), archive, sftDel, getCred(), getMeta(), getItems());
    return ret;
}

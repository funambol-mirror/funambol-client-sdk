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


#include "syncml/core/Status.h"
#include "base/globalsdef.h"

USE_NAMESPACE

Status::Status() {

   initialize();
}
Status::~Status() {
    if (COMMAND_NAME)   { delete [] COMMAND_NAME; COMMAND_NAME = NULL; }
    if (chal)           { delete    chal;         chal         = NULL; }
    if (data)           { delete    data;         data         = NULL; }
    if (credential)     { delete    credential;   credential   = NULL; }
    if (cmd)            { delete [] cmd;          cmd          = NULL; }
}

/**
* Creates a new Status object.
*
* @param cmdID command identifier - NOT NULL
* @param msgRef message reference
* @param cmdRef command reference - NOT NULL
* @param cmd command - NOT NULL
* @param targetRefs target references. If null
* @param sourceRefs source references. If null
* @param cred authentication credentials
* @param chal authentication challenge
* @param data status data - NOT NULL
* @param items command items - NOT NULL
*
*
*/
Status::Status(CmdID*        cmdID     ,
               const char*  msgRef    ,
               const char*  cmdRef    ,
               const char*  cmd       ,
               ArrayList*    targetRefs,
               ArrayList*    sourceRefs,
               Cred*         cred      ,
               Chal*         chal      ,
               Data*         data      ,
               ArrayList*    items     ) : ResponseCommand(cmdID,
                                                           msgRef,
                                                           cmdRef,
                                                           targetRefs,
                                                           sourceRefs,
                                                           items)  {

    initialize();

    setCred(cred);
    setData(data);
    setCmd(cmd);
    setChal(chal);
}

void Status::initialize() {

    COMMAND_NAME = new char[strlen(STATUS_COMMAND_NAME) + 1];
    sprintf(COMMAND_NAME, STATUS_COMMAND_NAME);

    chal        = NULL;
    data        = NULL;
    cmd         = NULL;
    credential  = NULL;
}

/**
* Returns the chal element
*
* @return the chal element
*
*/
Chal* Status::getChal() {
    return chal;
}

/**
* Sets the chal element
*
* @param chal the new chal
*/
void Status::setChal(Chal* chal) {
    if (this->chal) {
        delete this->chal; this->chal = NULL;
    }
    if (chal) {
        this->chal = chal->clone();
    }
}

/**
* Returns the status data
*
* @return the status data
*
*/
Data* Status::getData() {
    return data;
}

/**
* Sets the status data
*
* @param data the new data
*
*/
void Status::setData(Data* data) {
    if (data == NULL) {
            //
    } else {
        if (this->data) {
            delete this->data; this->data = NULL;
        }
        this->data = data->clone();
    }
}

/**
* Returns the cmd element
*
* @return the cmd element
*/
const char* Status::getCmd() {
    return cmd;
}

/**
* Sets the cmd element
*
* @param cmd the new cmd element - NOT NULL
*
*/
void Status::setCmd(const char* cmd) {
    if (cmd == NULL) {
        // TBD
    } else {
        if (this->cmd) {
            delete [] this->cmd; this->cmd = NULL;
        }
        this->cmd = stringdup(cmd);
    }
}

/**
* Returns the status code as int
*
* @return the status code as int
*/
int Status::getStatusCode() {
    return strtol(data->getData(), NULL, 10);

}

/**
* Returns the command name
*
* @return the command name
*/
const char* Status::getName() {
    return COMMAND_NAME;
}

ArrayElement* Status::clone() {
    Status* ret = new Status(getCmdID(), getMsgRef(), getCmdRef(), getCmd(), getTargetRef(),
                             getSourceRef(), getCred(), chal, data, getItems());
    return ret;
}

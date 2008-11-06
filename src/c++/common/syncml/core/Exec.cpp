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


#include "syncml/core/Exec.h"

Exec::Exec() {
    COMMAND_NAME = new char[strlen(EXEC_COMMAND_NAME) + 1];
    sprintf(COMMAND_NAME, EXEC_COMMAND_NAME);
}

Exec::~Exec() {
    if (COMMAND_NAME) {
        delete [] COMMAND_NAME; COMMAND_NAME = NULL;
    }
}

/**
 * Creates a new Exec object with the given command identifier,
 * noResponse, credential and item
 *
 * @param cmdID the command identifier - NOT NULL
 * @param noResp true if no response is required
 * @param cred the authentication credential
 * @param item the item - NOT NULL
 *
 */
Exec::Exec(CmdID*   cmdID,
           BOOL   noResp,
           Cred*  cred,
           ArrayList*  items) : ModificationCommand(cmdID, items) {

    COMMAND_NAME = new char[strlen(EXEC_COMMAND_NAME) + 1];
    sprintf(COMMAND_NAME, EXEC_COMMAND_NAME);

    setNoResp(noResp);
    setCred(cred);


}

/**
 * Gets the command name property
 *
 * @return the command name property
 */
const char* Exec::getName() {
    return COMMAND_NAME;
}

/**
 * Gets an Item object
 *
 * @return an Item object
 */
Item* Exec::getItem() {
    return (Item*)items->get(0);
}

ArrayElement* Exec::clone() {
    Exec* ret = new Exec(getCmdID(), getNoResp(), getCred(), getItems());
    return ret;
}

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


#include "syncml/core/CmdID.h"
#include "base/util/utils.h"


CmdID::CmdID() {
    this->cmdID = NULL;
}

/**
 * Creates a new CmdID object with the given String cmdID
 *
 * @param cmdID the cmdID of CmdID - NOT NULL
 *
 */
CmdID::CmdID(const char* cmdID) {

    this->cmdID = NULL;
    if ((cmdID == NULL) || (strlen(cmdID) == 0)) {
        // tbd
    }
    this->cmdID = stringdup(cmdID);
}

CmdID::~CmdID() {
    if (cmdID) {
        delete [] cmdID; cmdID = NULL;
    }

}

/**
 * Creates a new CmdID object with the given numeric cmdID
 *
 * @param cmdID the cmdID of CmdID
 *
 */
CmdID::CmdID(long cmdID) {
    char t[64];
    sprintf(t, "%i", cmdID);
    this->cmdID = stringdup(t);
}

/**
 * Gets cmdID properties
 *
 * @return cmdID properties
 */
const char* CmdID::getCmdID() {
    return cmdID;
}

CmdID* CmdID::clone() {
    CmdID* ret = new CmdID(cmdID);
    return ret;

}

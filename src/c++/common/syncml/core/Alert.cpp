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

#include "syncml/core/Alert.h"

Alert::Alert() {
    initialize();
}
Alert::~Alert() {
    if (COMMAND_NAME) {
        delete [] COMMAND_NAME; COMMAND_NAME = NULL;
    }
}

/**
* Creates a new Alert object with the given command identifier,
* noResponse, authentication credential, alert code and array of item
*
* @param cmdID command identifier - NOT NULL
* @param noResp is true if no response is required
* @param cred the authentication credential
* @param data the code of Alert
* @param items the array of item - NOT NULL
*
*/
Alert::Alert( CmdID* cmdID,
              BOOL noResp,
              Cred* cred,
              int data,
              ArrayList* items) : ItemizedCommand(cmdID, items) {

        initialize();
        setNoResp(noResp);
        setCred(cred);
        setData(data);
}

void Alert::initialize() {
    data = 0;
    COMMAND_NAME = new char[strlen(ALERT_COMMAND_NAME) + 1];
    sprintf(COMMAND_NAME, ALERT_COMMAND_NAME);
}

/**
* Gets the alert code
*
* @return the alert code
*/
int Alert::getData() {
    return data;
}

/**
* Sets the alert code
*
* @param data the alert code
*/
void Alert::setData(int data) {
    this->data = data;
}

/**
* Gets the command name property
*
* @return the command name property
*/
const char* Alert::getName() {
    return COMMAND_NAME;
}

ArrayElement* Alert::clone() {
    Alert* ret = new Alert(getCmdID(), getNoResp(), getCred(), data, getItems());
    ret->setMeta(getMeta());
    return ret;

}

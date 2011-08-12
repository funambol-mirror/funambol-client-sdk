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

#include "syncml/core/Alert.h"
#include "base/globalsdef.h"

USE_NAMESPACE

Alert::Alert() {
    initialize();
}
Alert::~Alert() {
    if (COMMAND_NAME) {
        delete [] COMMAND_NAME; COMMAND_NAME = NULL;
    }

	if (correlator) {
		delete [] correlator; correlator = NULL;
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
              bool noResp,
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
    ret->setCorrelator(getCorrelator());
    return ret;
}

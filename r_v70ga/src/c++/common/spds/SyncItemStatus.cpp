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


#include "spds/SyncItemStatus.h"
#include "base/globalsdef.h"

USE_NAMESPACE

/*
 * Default constructor
 */
SyncItemStatus::SyncItemStatus() {
	key = NULL;
}

/*
 * Constructs a new SyncItemStatus identified by the given key. The key must
 * not be longer than DIM_KEY_SYNC_ITEM_STATUS (see SPDS Constants).
 *
 * @param key - the key
 */
SyncItemStatus::SyncItemStatus(char* itemStatusKey){

    key = new char[strlen(itemStatusKey)+1];
    strcpy(key, itemStatusKey);

    data    = 0;
    cmdID   = 0;
    msgRef  = 0;
    cmdRef  = 0;

    cmd = NULL;

}

SyncItemStatus::~SyncItemStatus() {
	if (key) {
		delete [] key;
	}

	if (cmd) {
		delete [] cmd;
	}

}

/*
 * Returns the SyncItemStatus's key. If key is NULL, the internal buffer is
 * returned; if key is not NULL, the value is copied in the caller
 * allocated buffer and the given buffer pointer is returned.
 *
 * @param key - buffer where the key will be stored
 */
const char* SyncItemStatus::getKey() {
    return key;
}

/*
 * Changes the SyncItemStatus key. The key must not be longer than DIM_KEY_SYNC_ITEM_STATUS
 * (see SPDS Constants).
 *
 * @param key - the key
 */
void SyncItemStatus::setKey(const char*itemStatusKey) {
	if (key) {
		delete [] key;
	}
	key = new char[strlen(itemStatusKey)+1];
    strcpy(key, itemStatusKey);
}

 /*
 * Returns the SyncItemStatus's command name. If cmd is NULL, the internal buffer is
 * returned; if cmd is not NULL, the value is copied in the caller
 * allocated buffer and the given buffer pointer is returned.
 *
 * @param itemStatusCmd - buffer where the itemStatusCmd will be stored
 */
const char* SyncItemStatus::getCmd() {
    return cmd;
}


/*
 * Changes the SyncItemStatus cmd. The cmd must not be longer than DIM_COMMAND_SYNC_ITEM_STATUS
 * (see SPDS Constants).
 *
 * @param itemStatusCmd - the itemStatusCmd
 */
void SyncItemStatus::setCmd(const char*itemStatusCmd) {
    if (cmd) {
		delete [] cmd;
	}
	cmd = new char[strlen(itemStatusCmd)+1];
    strcpy(cmd, itemStatusCmd);

}


/*
 * Sets the SyncItemStatus data. The passed data are copied into an
 * internal variable.
 */
void SyncItemStatus::setData(int itemStatusData) {
    data = itemStatusData;
}

/*
 * Returns the SyncItemStatus data variable.
 */
int SyncItemStatus::getData() {
    return data;
}


/*
 * Sets the SyncItemStatus command ID. The passed data are copied into an
 * internal variable.
 */
void SyncItemStatus::setCmdID(int itemStatusCmdID) {
    cmdID = itemStatusCmdID;
}

/*
 * Returns the SyncItemStatus command ID variable.
 */
int SyncItemStatus::getCmdID() {
    return cmdID;
}

/*
 * Sets the SyncItemStatus message referring. The passed data are copied into an
 * internal variable.
 */
void SyncItemStatus::setMsgRef(int itemStatusMsgRef) {
    msgRef = itemStatusMsgRef;
}

/*
 * Returns the SyncItemStatus message referring variable.
 */
int SyncItemStatus::getMsgRef() {
    return msgRef;
}

/*
 * Sets the SyncItemStatus command referring. The passed data are copied into an
 * internal variable.
 */
void SyncItemStatus::setCmdRef(int itemStatusCmdRef) {
    cmdRef = itemStatusCmdRef;
}

/*
 * Returns the SyncItemStatus command referring variable.
 */
int SyncItemStatus::getCmdRef() {
    return cmdRef;
}

ArrayElement* SyncItemStatus::clone() {
	SyncItemStatus* ret = new SyncItemStatus(key);

	ret->setCmd(cmd);
	ret->setData(data);
	ret->setCmdRef(cmdRef);
	ret->setMsgRef(msgRef);
	ret->setCmdID(cmdID);

	return ret;
}

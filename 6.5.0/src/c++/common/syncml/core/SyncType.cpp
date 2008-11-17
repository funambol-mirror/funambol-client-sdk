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


#include "syncml/core/SyncType.h"


SyncType::SyncType() {
    syncType = 0;
}
SyncType::~SyncType() {
    syncType = 0;
}

/**
 * Creates a new SyncType object with syncType value
 *
 * @param syncType the value of SyncType - NOT NULL
 *
 */
 SyncType::SyncType(int syncType) {
    this->syncType = syncType;
 }

// ---------------------------------------------------------- Public methods

/**
 * Gets the synchronization type
 *
 * @return syncType the synchronization type
 */
 int SyncType::getType() {
    return syncType;
 }


/**
 * Sets the synchronization type
 *
 * @param syncType the synchronization type
 */
 void SyncType::setType(int syncType) {
    this->syncType = syncType;
 }

 ArrayElement* SyncType::clone() {
    SyncType* ret = new SyncType(syncType);
    return ret;

 }

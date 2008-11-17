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


#include "syncml/core/SyncTypeArray.h"

SyncTypeArray::SyncTypeArray(){

    syncTypeArray = new ArrayList();

    SyncType TWO_WAY             = SyncType(1);
    SyncType SLOW                = SyncType(2);
    SyncType ONE_WAY_FROM_CLIENT = SyncType(3);
    SyncType REFRESH_FROM_CLIENT = SyncType(4);
    SyncType ONE_WAY_FROM_SERVER = SyncType(5);
    SyncType REFRESH_FROM_SERVER = SyncType(6);
    SyncType SERVER_ALERTED      = SyncType(7);

    syncTypeArray->add(TWO_WAY);
    syncTypeArray->add(SLOW);
    syncTypeArray->add(ONE_WAY_FROM_CLIENT);
    syncTypeArray->add(REFRESH_FROM_CLIENT);
    syncTypeArray->add(ONE_WAY_FROM_SERVER);
    syncTypeArray->add(REFRESH_FROM_SERVER);
    syncTypeArray->add(SERVER_ALERTED);

}

SyncTypeArray::~SyncTypeArray() {
    if (syncTypeArray) {
        syncTypeArray->clear();  //delete syncTypeArray; syncTypeArray = NULL;
    }
}

ArrayList* SyncTypeArray::getSyncTypeArray() {
    return syncTypeArray;
}

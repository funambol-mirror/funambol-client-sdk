/*
 * Copyright (C) 2003-2007 Funambol, Inc
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


#ifndef INCL_TEST_LISTENER
#define INCL_TEST_LISTENER
/** @cond DEV */

#include "event/SyncListener.h"

class TestSyncListener : public SyncListener {

    void syncBegin         (SyncEvent& event);
    void syncEnd           (SyncEvent& event);
    void sendInitialization(SyncEvent& event);
    void sendModifications (SyncEvent& event);
    void sendFinalization  (SyncEvent& event);
    void syncError         (SyncEvent& event);

};

/** @endcond */
#endif

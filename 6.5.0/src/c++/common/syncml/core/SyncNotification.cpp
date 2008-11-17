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


#include "base/fscapi.h"
#include "base/Log.h"
#include "base/util/utils.h"
#include "syncml/core/SyncNotification.h"

/*
 * This class represent a Server Alerted Notification for DS message (Pkg0).
 * After a call to method parse(), the object contains the message data in
 * a usable form.
 */

////////////////////// Support datatypes
#pragma pack(1)
// Notification header
union notification_hdr {
    struct {
        unsigned version:10;
        unsigned ui_mode:2;
        unsigned initiator:1;
        unsigned reserved1:19;
        unsigned reserved2:8;
        unsigned session_id:16;
        unsigned server_id_len:8;
    } s;
    struct {
        unsigned long w1;
        unsigned long w2;
    } r;
};

struct sync_hdr {
    unsigned num_sync: 4;
    unsigned reserved: 4;
};

union sync_body {
    struct {
        unsigned reserved: 4;
        unsigned sync_type: 4;
        unsigned content_type: 24;
    } s;
    unsigned long r;
};


/*
 * Constructor
 */
SyncNotification::SyncNotification()
{
    reset(false);
}

/*
 * Destructor
 */
SyncNotification::~SyncNotification() {
    reset(true);
}

// Reset attributes, freing memory if needed
void SyncNotification::reset(bool free) {
    if(free){
        if (syncAlerts)
            delete [] syncAlerts;
        if (serverId)
            delete [] serverId;
    }
    version=0;
    uiMode=UI_NotSpecified;
    initiator=ServerInitiated;
    sessionId=0;
    serverId=0;
    numSyncs=0;
    syncAlerts=0;
}

/**
 * @brief Processes the PKG0 message, populating the
 *        object's attributes.
 *
 * @param msg the PKG0 message from the server
 *
 * @returns 0 on success, an error code defined by
 *          NotifProcErrors otherwise.
 */
SN_Errors SyncNotification::parse (const char *msg, int len)
{
// This macro increments local pointer p or return error if out of bounds.
#define IncP(Num) if((p+(Num))<=end) p+=(Num); else return SNErr_Incomplete

    const char *digest;
    notification_hdr *hdr;
    int server_id_len=10;

    const char *p=msg, *end=msg+len;          // message cursor and boundary

    //LOG.setLevel(LOG_LEVEL_DEBUG);
    //LOG.debug("SyncNotification::parse START....");

    if(!msg){
        return SNErr_NullValue;
    }
    reset(true);

    // TODO: check MD5!
    digest=msg; IncP(16);
    // Cast message to header
    hdr = (notification_hdr *)p;

    // copy header values
    version = hdr->s.version;
    uiMode = (SN_UIMode) hdr->s.ui_mode;
    initiator = (SN_Initiator) hdr->s.initiator;
    sessionId = hdr->s.session_id;
    server_id_len = hdr->s.server_id_len;
    IncP(8);
    //LOG.debug("After header.");

    // get server identifier
    serverId = stringdup(p, server_id_len);
    IncP(server_id_len);

    //char dbg[256];
    //sprintf(dbg, "serverId: %s", serverId);
    //LOG.debug(dbg);

    // get num sync
    numSyncs = (*p)>>4;
    IncP(1);
    //sprintf(dbg, "numSyncs: %d", numSyncs);
    //LOG.debug(dbg);

    // allocate syncalert objects
    syncAlerts = new SyncAlert[numSyncs];


    // Init all sync items
    for(int i=0; i<numSyncs; i++){
        char *tmps;
        sync_body *sync;
        unsigned char server_URI_len;
        int ret;

        sync = (sync_body *)p;
        IncP(sizeof(sync_body));

        // set syncAlert attributes
        server_URI_len = *p;
        IncP(1);
        tmps = stringdup(p, server_URI_len);
        ret = syncAlerts[i].set(sync->s.sync_type, sync->s.content_type, tmps);
        delete [] tmps;
        // Check errors
        if (ret){
            return SNErr_InvSync;
        }
        IncP(server_URI_len);
    }
    return SNErr_Ok;
}

// Accessor methods
int SyncNotification::getVersion() {
    return version;
}

SN_UIMode SyncNotification::getUIMode() {
    return uiMode;
}

SN_Initiator SyncNotification::getInitiator() {
    return initiator;
}

int SyncNotification::getSessionId() {
    return sessionId;
}

const char *SyncNotification::getServerId() {
    return serverId;
}

int SyncNotification::getNumSyncs() {
    return numSyncs;
}

SyncAlert* SyncNotification::getSyncAlert ( int idx ) {
    if (idx < 0 || idx > numSyncs)
        return 0;
    return &syncAlerts[idx];
}



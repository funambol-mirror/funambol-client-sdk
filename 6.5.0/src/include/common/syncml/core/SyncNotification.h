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

/*
 * This class represent a Server Alerted Notification for DS message (Pkg0).
 * After a call to method parse(), the object contains the message data in
 * a usable form.
 */

#ifndef INCL_NOTIF_PROCESSOR
#define INCL_NOTIF_PROCESSOR
/** @cond DEV */

#include "syncml/core/SyncAlert.h"

// Define User Interaction Modes
enum SN_UIMode {
    UI_NotSpecified,
    UI_Background,
    UI_Info,
    UI_Interactive
};

// Define Sync Initiator
enum SN_Initiator {
    UserInitiated,
    ServerInitiated
};

// Sync Notification Error Codes
enum SN_Errors {
    SNErr_Ok,
    SNErr_BadReq,
    SNErr_InvSync,
    SNErr_Incomplete,
    SNErr_NullValue,
    SNErr_Undefined = 255         // Generic error
};


class SyncNotification {

    public:

        // Constructor
        SyncNotification();
        // Destructor
        ~SyncNotification();

        /*
         * Parse a PKG0 message
         */
        SN_Errors parse(const char *msg, int len);

        /*
         * Accessor methods
         */
        int getVersion();
        SN_UIMode getUIMode();
        SN_Initiator getInitiator();
        int getSessionId();
        const char*  getServerId();
        int getNumSyncs();
        SyncAlert* getSyncAlert ( int idx );

    private:
        // Notification Header
        int version;
        SN_UIMode uiMode;
        SN_Initiator initiator;
        int sessionId;
        char*  serverId;
        // Notification Body
        int numSyncs;
        SyncAlert *syncAlerts;

        void reset(bool free);
};

/** @endcond */
#endif


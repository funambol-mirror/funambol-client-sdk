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


#ifndef INCL_SYNC_HDR
#define INCL_SYNC_HDR
/** @cond DEV */

#include "base/fscapi.h"
#include "syncml/core/VerDTD.h"
#include "syncml/core/VerProto.h"
#include "syncml/core/SessionID.h"
#include "syncml/core/Target.h"
#include "syncml/core/Source.h"
#include "syncml/core/Cred.h"
#include "syncml/core/Meta.h"

#define SYNCHDR_COMMAND_NAME "SyncHdr"

class SyncHdr {

     // ------------------------------------------------------------ Private data
    private:
        char*     COMMAND_NAME;
        VerDTD*     verDTD   ;
        VerProto*   verProto ;
        SessionID*  sessionID;
        char*     msgID    ;
        Target*     target   ;
        Source*     source   ;
        char*     respURI  ;
        BOOL        noResp   ;
        Cred*       cred     ;
        Meta*       meta     ;

    // ---------------------------------------------------------- Public data
    public:

        SyncHdr();
        ~SyncHdr();

        /**
         * Creates a nee SyncHdr object
         *
         * @param verDTD SyncML DTD version - NOT NULL
         * @param verProto SyncML protocol version - NOT NULL
         * @param sessionID sync session identifier - NOT NULL
         * @param msgID message ID - NOT NULL
         * @param target target URI - NOT NULL
         * @param source source URI - NOT NULL
         * @param respURI may be null.
         * @param noResp true if no response is required
         * @param cred credentials. May be null.
         * @param meta may be null.
         *
         */
        SyncHdr(VerDTD*      verDTD,
                VerProto*    verProto,
                SessionID*   sessionID,
                char*      msgID,
                Target*      target,
                Source*      source,
                char*      respURI,
                BOOL         noResp,
                Cred*        cred,
                Meta*        meta);

        /**
         * Gets the DTD version
         *
         * @return verDTD the DTD version
         */
        VerDTD* getVerDTD();

        /**
         * Sets the DTD version
         *
         * @param verDTD the DTD version
         *
         */
        void setVerDTD(VerDTD* verDTD);

        /**
         * Gets the protocol version
         *
         * @return verProto the protocol version
         */
        VerProto* getVerProto();

        /**
         * Sets the protocol version
         *
         * @param verProto the protocol version
         */
        void setVerProto(VerProto* verProto);

        /**
         * Gets the session identifier
         *
         * @return sessionID the session identifier
         */
        SessionID* getSessionID();

        /**
         * Sets the session identifier
         *
         * @param sessionID the session identifier
         *
         */
        void setSessionID(SessionID* sessionID);

        /**
         * Gets the message identifier
         *
         * @return msgID the message identifier
         */
        const char* getMsgID();

        /**
         * Sets the message identifier
         *
         * @param msgID the message identifier
         */
        void setMsgID(const char* msgID);

        /**
         * Gets the Target object
         *
         * @return target the Target object
         */
        Target* getTarget();

        /**
         * Sets the Target object
         *
         * @param target the Target object
         */
        void setTarget(Target* target);

        /**
         * Gets the Source object
         *
         * @return source the Source object
         */
        Source* getSource();

        /**
         * Sets the Source object
         *
         * @param source the Source object
         */
        void setSource(Source* source);

        /**
         * Gets the response URI
         *
         * @return respURI the response URI
         */
        const char* getRespURI();

        /**
         * Sets the response URI.
         *
         * @param uri the new response URI; NOT NULL
         */
        void setRespURI(const char* uri);

        /**
         * Gets noResp property
         *
         * @return true if the command doesn't require a response, false otherwise
         */
        BOOL isNoResp();

        /**
         * Gets the Boolean value of noResp
         *
         * @return true if the command doesn't require a response, null otherwise
         */
        BOOL getNoResp();

        /**
         * Sets the noResponse property
         *
         * @param noResp the noResponse property
         */
        void setNoResp(BOOL noResp);

        /**
         * Gets the Credential property
         *
         * @return cred the Credential property
         */
        Cred* getCred();

        /**
         * Sets the Credential property
         *
         * @param cred the Credential property
         */
        void setCred(Cred* cred);

        /**
         * Gets the Meta property
         *
         * @return meta the Meta property
         */
        Meta* getMeta();

        /**
         * Sets the Meta property
         *
         * @param meta the Meta property
         */
        void setMeta(Meta* meta);

        const char* getName();

        SyncHdr* clone();
};

/** @endcond */
#endif

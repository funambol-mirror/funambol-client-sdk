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
#include "base/globalsdef.h"

BEGIN_NAMESPACE

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
        bool        noResp   ;
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
                const char*  msgID,
                Target*      target,
                Source*      source,
                const char*  respURI,
                bool         noResp,
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
         * Gets the Boolean value of noResp
         *
         * @return true if the command doesn't require a response, null otherwise
         */
        bool getNoResp();

        /**
         * Sets the noResponse property
         *
         * @param noResp the noResponse property
         */
        void setNoResp(bool noResp);

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


END_NAMESPACE

/** @endcond */
#endif

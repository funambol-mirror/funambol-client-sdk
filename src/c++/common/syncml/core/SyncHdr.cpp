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


#include "syncml/core/SyncHdr.h"
#include "base/globalsdef.h"

USE_NAMESPACE


SyncHdr::SyncHdr() {

    COMMAND_NAME = new char[strlen(SYNCHDR_COMMAND_NAME) + 1];
    sprintf(COMMAND_NAME, SYNCHDR_COMMAND_NAME);

    verDTD      = NULL;
    verProto    = NULL;
    sessionID   = NULL;
    msgID       = NULL;
    target      = NULL;
    source      = NULL;
    respURI     = NULL;
    cred        = NULL;
    meta        = NULL;
    noResp      = false;
}

SyncHdr::~SyncHdr() {
    if (COMMAND_NAME){ delete [] COMMAND_NAME;     COMMAND_NAME      = NULL; }
    if (verDTD)     { delete    verDTD;     verDTD      = NULL; }
    if (verProto)   { delete    verProto;   verProto    = NULL; }
    if (sessionID)  { delete    sessionID;  sessionID   = NULL; }
    if (msgID)      { delete [] msgID;      msgID       = NULL; }
    if (target)     { delete    target;     target      = NULL; }
    if (source)     { delete    source;     source      = NULL; }
    if (respURI)    { delete [] respURI;    respURI     = NULL; }
    if (cred)       { delete    cred;       cred        = NULL; }
    if (meta)       { delete    meta;       meta        = NULL; }

    noResp = false   ;
}

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
SyncHdr::SyncHdr(VerDTD*      verDTD,
                VerProto*    verProto,
                SessionID*   sessionID,
                char*     msgID,
                Target*      target,
                Source*      source,
                char*     respURI,
                bool         noResp,
                Cred*        cred,
                Meta*        meta) {

    this->verDTD      = NULL;
    this->verProto    = NULL;
    this->sessionID   = NULL;
    this->msgID       = NULL;
    this->target      = NULL;
    this->source      = NULL;
    this->respURI     = NULL;
    this->cred        = NULL;
    this->meta        = NULL;
    this->noResp      = false;

    COMMAND_NAME = new char[strlen(SYNCHDR_COMMAND_NAME) + 1];
    sprintf(COMMAND_NAME, SYNCHDR_COMMAND_NAME);
    setMsgID(msgID);
    setVerDTD(verDTD);
    setVerProto(verProto);
    setSessionID(sessionID);
    setTarget(target);
    setSource(source);

    setNoResp(noResp);
    setRespURI(respURI);

    setCred(cred);
    setMeta(meta);
}

/**
* Gets the DTD version
*
* @return verDTD the DTD version
*/
VerDTD* SyncHdr::getVerDTD() {
    return verDTD;
}

/**
* Sets the DTD version
*
* @param verDTD the DTD version
*
*/
void SyncHdr::setVerDTD(VerDTD* verDTD) {
    if (verDTD == NULL) {
        // TBD
    } else {
        if (this->verDTD) {
            delete this->verDTD; this->verDTD = NULL;
        }
        this->verDTD = verDTD->clone();
    }
}

/**
* Gets the protocol version
*
* @return verProto the protocol version
*/
VerProto* SyncHdr::getVerProto() {
    return verProto;
}

/**
* Sets the protocol version
*
* @param verProto the protocol version
*/
void SyncHdr::setVerProto(VerProto* verProto) {
    if (verProto == NULL) {
        // TBD
    } else {
        if (this->verProto) {
            delete this->verProto; this->verProto = NULL;
        }
        this->verProto = verProto->clone();
    }
}
/**
* Gets the session identifier
*
* @return sessionID the session identifier
*/
SessionID* SyncHdr::getSessionID() {
    return sessionID;
}

/**
* Sets the session identifier
*
* @param sessionID the session identifier
*
*/
void SyncHdr::setSessionID(SessionID* sessionID) {
    if (sessionID == NULL) {
        // TBD
    } else {
        if (this->sessionID) {
            delete this->sessionID; this->sessionID = NULL;
        }
        this->sessionID = sessionID->clone();
    }
}


/**
* Gets the message identifier
*
* @return msgID the message identifier
*/
const char* SyncHdr::getMsgID() {
    return msgID;
}


/**
* Sets the message identifier
*
* @param msgID the message identifier
*/
void SyncHdr::setMsgID(const char*msgID) {
    if (this->msgID) {
        delete [] this->msgID; this->msgID = NULL;
    }
    this->msgID = stringdup(msgID);
}

/**
* Gets the Target object
*
* @return target the Target object
*/
Target* SyncHdr::getTarget() {
    return target;
}

/**
* Sets the Target object
*
* @param target the Target object
*/
void SyncHdr::setTarget(Target* target) {
    if (target == NULL) {
            // TBD
    } else {
        if (this->target) {
            delete this->target; this->target = NULL;
        }
        this->target = target->clone();
    }
}

/**
* Gets the Source object
*
* @return source the Source object
*/
Source* SyncHdr::getSource() {
    return source;
}

/**
* Sets the Source object
*
* @param source the Source object
*/
void SyncHdr::setSource(Source* source) {
    if (source == NULL) {
            // TBD
    } else {
        if (this->source) {
            delete this->source; this->source = NULL;
        }
        this->source = source->clone();
    }
}

/**
* Gets the response URI
*
* @return respURI the response URI
*/
const char* SyncHdr::getRespURI() {
    return respURI;
}


/**
* Sets the response URI.
*
* @param uri the new response URI; NOT NULL
*/
void SyncHdr::setRespURI(const char*uri) {
    if (this->respURI) {
        delete [] this->respURI; this->respURI = NULL;
    }
    this->respURI = stringdup(uri);
}


/**
* Gets the Boolean value of noResp
*
* @return true if the command doesn't require a response, null otherwise
*/
bool SyncHdr::getNoResp() {
    return noResp;
}

/**
* Sets the noResponse property
*
* @param noResp the noResponse property
*/
void SyncHdr::setNoResp(bool noResp) {
    this->noResp = noResp;
}

/**
* Gets the Credential property
*
* @return cred the Credential property
*/
Cred* SyncHdr::getCred() {
    return cred;
}

/**
* Sets the Credential property
*
* @param cred the Credential property
*/
void SyncHdr::setCred(Cred* cred) {
    if (this->cred) {
        delete this->cred; this->cred = NULL;
    }
    if (cred) {
        this->cred = cred->clone();
    }
}

/**
* Gets the Meta property
*
* @return meta the Meta property
*/
Meta* SyncHdr::getMeta() {
    return meta;
}

/**
* Sets the Meta property
*
* @param meta the Meta property
*/
void SyncHdr::setMeta(Meta* meta) {
    if (this->meta) {
        delete this->meta; this->meta = NULL;
    }
    if (meta) {
        this->meta = meta->clone();
    }
}

const char* SyncHdr::getName() {
    return COMMAND_NAME;

}

SyncHdr* SyncHdr::clone() {
    SyncHdr* ret = new SyncHdr(verDTD, verProto, sessionID, msgID, target, source, respURI, noResp, cred, meta);
    return ret;

}

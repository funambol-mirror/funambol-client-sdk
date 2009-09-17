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
 * FOR A PARTICULAR PURPOSE. See the GNU General Public License for more 
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

#include "push/CTPMessage.h"
#include "base/util/utils.h"
#include "base/globalsdef.h"

USE_NAMESPACE

CTPMessage::CTPMessage() {
    initialize();

}

CTPMessage::CTPMessage(const char* package, int maxPackageLen) {
   initialize();
   parse(package, maxPackageLen);
}

CTPMessage::~CTPMessage() {
    delete [] buffer;
    delete np;
    delete  [] from;
}

void CTPMessage::initialize() {
    buffer          = NULL;
    from            = NULL;
    bufferLength    = 0;
    np              = NULL;
    packageLength   = 0;
    protocolVersion = 0;
    fromLength      = 0;
}


int32_t CTPMessage::parse(const char* package, int32_t maxPackageLen) {
    
// This macro increments local pointer p or return error if out of bounds.
#define IncP(Num) ((p+(Num)<end) ? (p+=(Num)) : (p = NULL))
    
    LOG.debug("Parsing msg...");
    int32_t ret = 0;
    setBufferLength(0);
    if (buffer) { delete [] buffer; buffer = NULL; }
    if (from) { delete [] from; from = NULL; } 

    //get the length of the message (first 2 bytes)    
    int messageLen  = (int)((unsigned char)package[0]);
    int messageLen1 = (int)((unsigned char)package[1]);
    
    messageLen <<= 8;
    messageLen = messageLen | messageLen1;
    //LOG.debug("messageLen = %d", messageLen);

    if (maxPackageLen) {
        // Safe check on message length
        if (messageLen+2 > maxPackageLen) {
            messageLen = maxPackageLen - 2;
            LOG.info("Warning: recv received only %d bytes: set messageLength to %d", maxPackageLen, messageLen);
        }
        else if (messageLen+2 < maxPackageLen) {
            LOG.info("Warning: recv received more bytes: %d", maxPackageLen);
        }
    }
    setPackageLength(messageLen + 2);
    
    // variables at the beginning and the end of the message
    const char *p = &package[2], *end = p + messageLen;

    ////////// Save the msg received to file
    //char name[32];
    //sprintf(name, "/dump/%d.dmp", time((void*)NULL));
    //saveFile(name, package, messageLen+2, true);
    //const char* start = p;
    //////////
    
    // Protocol version
    if (p == NULL) {
        LOG.debug("Error in parsing ctp message: protocol version not found");
        ret = -1;
        goto finally;
    }
    setProtocolVersion(*p);

    // command or status
    IncP(1);
    if (p == NULL) {
        LOG.debug("Error in parsing ctp message: command or status not found");
        ret = -1;
        goto finally;
    }
    setGenericCommand(*p);

    // now could be or not params
    while (IncP(1)) {                       // param-code
        CTPParam param;
        param.setParamCode(*p);
        
        IncP(1); //value-length
        if (p == NULL) {
            LOG.debug("Error in parsing ctp message: Param-value-length not found");
            ret = -1;
            goto finally;   
        }
        int valLen = (int)((unsigned char)(*p));

        // Safe check on value-length:
        if (p+valLen-1 >= end) {
            LOG.debug("Warning! value length too big (%d), using: %d", valLen, end-p);
            valLen = end-p;
        }
        
        IncP(1);
        param.setValue((const void*)p, valLen);

        IncP(valLen - 1);
        params.add(param);
                
        // The max params for the status sent by the server are only 2.        
        if (params.size() == 1) {
            if (ST_SYNC == getGenericCommand()) { // the server is sending the <san>
                LOG.debug("SAN param found");
                np = new SyncNotification();
                np->parse((const char*)param.getValue(), valLen);
            } else if (ST_JUMP == getGenericCommand()) {
                LOG.debug("FROM param found");
                from = new char[valLen];
                memcpy(from, param.getValue(), valLen);
                setFromLength(valLen);
            }            
            else {
                LOG.debug("param found");
                buffer = new char[valLen];
                memcpy(buffer, param.getValue(), valLen);
                setBufferLength(valLen);
            }

        } else if (params.size() == 2) {  
            if (ST_JUMP == getGenericCommand()) {
                LOG.debug("TO param found");
                buffer = new char[valLen];
                memcpy(buffer, param.getValue(), valLen);
                setBufferLength(valLen);
            }
        }
        if (!p) goto finally;
    }
    
finally:
    return ret;
}

char* CTPMessage::toByte() {
    
    int totalMessageSize = 2; // version and genericCommand
    int totalPackageSize = 0, i = 0;
   
    CTPParam* param = (CTPParam*)params.front();
    while(param) {
        
        totalMessageSize += 2;
        totalMessageSize += param->getValueLength();

        param = (CTPParam*) params.next();
    }
    totalPackageSize = totalMessageSize +2;
    
    setPackageLength(totalPackageSize); // the whole package
    setBufferLength(totalPackageSize);  // the same package that is in the buffer

    buffer = new char[totalPackageSize];

    if (totalMessageSize < 255) {
        buffer[0] = 0x00;
        buffer[1] = (unsigned char)totalMessageSize;        
    } else {
        int upper = 0, down = 0; 
        upper = totalMessageSize >> 8 ;
        buffer[0] = (unsigned char)(upper);
        down = totalMessageSize;
        upper <<= 8; 
        down = down ^ upper;
        buffer[1] = (unsigned char)down;    

    }
    
    buffer[2] = getProtocolVersion();
    buffer[3] = getGenericCommand();
    
    char* p;
    if (totalPackageSize == 4) {
        goto finally;
    }
    p = &buffer[4];
   
    param = (CTPParam*)params.front();
    while(param) {

        p[i++] = param->getParamCode();
        p[i++] = param->getValueLength();
        memcpy(&p[i], param->getValue(), param->getValueLength());
        i += param->getValueLength();
        
        param = (CTPParam*) params.next();
    }

finally:
    return buffer;
}

void CTPMessage::addParam(CTPParam* p) {
    params.add(*p);
}


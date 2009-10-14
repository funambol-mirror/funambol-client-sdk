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

#ifndef INCL_CTP_MESSAGE
#define INCL_CTP_MESSAGE
/** @cond DEV */

//#include <iostream>

#include "base/fscapi.h"
#include "syncml/core/SyncNotification.h" 
#include "base/util/ArrayList.h"

#include "push/CTPParam.h"

BEGIN_NAMESPACE

/**
* Commands
*/
#define CM_AUTH                 0x01
#define CM_READY                0x02
#define CM_BYE                  0x03

/**
* Status
*/
#define ST_OK                   0x20
#define ST_JUMP                 0x37
#define ST_ERROR                0x50
#define ST_NOT_AUTHENTICATED    0x41            // Server auth failed, nonce param received
#define ST_UNAUTHORIZED         0x42            // Server auth failed at 1st try, nonce param received
#define ST_FORBIDDEN            0x43            // No Server auth, no nonce param
#define ST_SYNC                 0x29
#define ST_RETRY                0x53


#define MAX_MESSAGE_SIZE        256
#include "base/globalsdef.h"
    
class CTPMessage {

private:
    
    /**
    * It represents the notification <san> message from the server.
    * It is no used if the CTPMessage is created from the client to 
    * communicate with the server
    */
    SyncNotification* np;

    
    /**
    * The length of the package to be sent to the server. Used together
    * to the toByte method.
    * For the parse method this value should be set at the beginning
    * with the value of the messageLength + 2.
    */
    int32_t packageLength;
    
    /**
    * The lenght of the message inside the package. This value is retrieved
    * from the first 2 bytes of the package.
    */
    int32_t bufferLength;
    
    /**
    * The buffer field has a different meaning based on the action it is performing.        
    * Parse action: The field contains the value of the status associated.
    * The same could be retrieved from the Params list.
    * if genericCommand is OK  : buffer is the <nonce> (if exists)
    * if genericCommand is JUMP: buffer is the <to> 
    * if genericCommand is ERROR: buffer is the <description> 
    * if genericCommand is UNAUTHORIZED: buffer is the <nonce> 
    * if genericCommand is FORBIDDEN: buffer is the NULL 
    * if genericCommand is SYNC: buffer is the NULL (SyncNotification is populated)
    * if genericCommand is RETRY: buffer is the <sleep> (not yet implemented)
    
    * Format action: to create the package to be returned after a toByte action
    */
    char* buffer;
    /**
    * the length of from property
    */
    int32_t fromLength;
    void setFromLength(int v) { fromLength = v; }

    /**
    * Used only when the status is JUMP. It represent the <from> Param sent by the
    * server. The <to> is in the buffer property
    */
    char* from;
    

    /**
    * the version of the protocol
    * to set in exadecimal mode 0x10 for 1.0 version...
    */ 
    int8_t protocolVersion;
    
    /**
    * the generic command could be a command or a status.
    * The commands are 
    *       AUTH, 
    *       READY, 
    *       BYE 
    * sent from client to server
    * The status are 
    *       OK,
            JUMP,
            ERROR,
            UNAUTHORIZED,
            FORBIDDEN,
            SYNC,
            RETRY (not yet implemented)

    * Command and status are in exadecimal representation
    */ 
    int8_t genericCommand;
    
    /**
    * init the variables
    */
    void initialize();

public:        
    

    /**
    * List containing the param(s) to be exchanged in the message.
    * It is public to permit to have access to all list methods
    * (to be considered if making it private makes more sense)
    */
    ArrayList params;

    /**
    * Used to create an empty class to be populated by the client.
    */
    CTPMessage();
    
    /**
    * Used to create a filled class populated with the server package.
    * @param package the package sent by the server
    * @param maxPackageLen  optional, the length of the package sent (if already known)
    */
    CTPMessage(const char* package, int maxPackageLen = 0);
    
    /**
    * Destructor
    */
    ~CTPMessage();
    
    /**
    * Used to populated all the fields with the server properties.
    * @param package the package sent by the server to be parsed
    * @param maxPackageLen  optional, the length of the package sent (if already known)
    * @return 0 if the parse action works properly, -1 there is an error.
    */
    int32_t parse(const char* package, int32_t maxPackageLen = 0);
    
    /**
    * Used to create the byte array of the package using all the not-NULL fields .
    * @return the pointer to the internally allocated package
    * NOTE: the length of the byte array is in the packageLength field
    */
    char* toByte();
    
    /**
    * set the package length of the just created byte array to be sent
    * to the server
    */
    void setPackageLength(int32_t v) { packageLength = v; }
    
    /**
    * return the package length of the just created byte array 
    */
    int getPackageLength() { return packageLength; }
    
    /**
    * set the message length
    */
    void setBufferLength(int32_t v) { bufferLength = v; }
    
    /**
    * get the message length
    */
    int32_t getBufferLength()      { return bufferLength; }

    /**
    * get the allocated buffer internally. It needs to get the action
    * due to the parting method
    */
    char* getBuffer() { return buffer; }
   
    /**
    * Used to set the command from client to server
    */
    void setGenericCommand(int8_t c) { genericCommand = c; }

    /**
    * Used to get the status from server to client
    */
    int8_t getGenericCommand() { return genericCommand; }
    
    /**
    * Used to set the protocol version 
    */
    void setProtocolVersion(int8_t c) { protocolVersion = c; }

    /**
    * Used to get the protocol version
    */
    int8_t getProtocolVersion() { return protocolVersion; }
    
    /**
    * Return the internal SyncNotification
    */
    SyncNotification* getSyncNotification() { return np; }
    
    /**
    * get the from field length
    */
    int32_t getFromLength()      { return fromLength; }

    /**
    * get the allocated buffer internally. It needs to get the action
    * due to the parting method
    */
    char* getFrom() { return from; }
        
    /**
    * add CTPParam to the internal public params list member. This method is
    * only to avoid to add directly 
    */
    void addParam(CTPParam* p);
};

    

END_NAMESPACE

/** @endcond */
#endif

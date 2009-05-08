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

#ifndef INCL_CTP_SERVICE
#define INCL_CTP_SERVICE

/** @cond DEV */

#include "base/globalsdef.h"
#include "base/fscapi.h"

#include "push/FThread.h"
#include "push/FSocket.h"
#include "push/PushListener.h"
#include "push/CTPMessage.h"
#include "push/CTPConfig.h"
#include "push/CTPThreadPool.h"

/**< CTP Protocol version = 1.0 */
#define CTP_PROTOCOL_VERSION            0x10
/**< Each time the CTP connection is broken, we double the ctpRetry time */
#define CTP_RETRY_INCREASE_FACTOR       2

#define CTP_SEND_BYE_MSG                0   /** Set to 1 to send the [BYE] msg before closing connection */


BEGIN_NAMESPACE


// Private Threads
class CTPThread : public FThread {

public:
    CTPThread();
    ~CTPThread();
    void run();
    int32_t getErrorCode() { return errorCode; }

private:
    int32_t errorCode;
    bool saveNonceParam(CTPMessage* authStatusMsg);
};

class ReceiverThread : public FThread {
public:
    ReceiverThread();
    ~ReceiverThread();
    void run();
    int32_t getErrorCode() { return errorCode; }

private:
    int32_t errorCode;
};

class HeartbeatThread : public FThread {
public:
    HeartbeatThread();
    ~HeartbeatThread();
    void run();
    int32_t getErrorCode() { return errorCode; }
    void softTerminate();

private:
    int32_t errorCode;

};

class CmdTimeoutThread : public FThread {

public:
    CmdTimeoutThread();
    ~CmdTimeoutThread();
    void run();
    void softTerminate();
};


/**
 * The class to manage the CTP connection, singleton.
 */
class CTPService {

public:
    /**
     * The state of CTP connection.
     * State CTP_STATE_WAITING_RESPONSE is used to check the timeout (ctpCmdTimeout)
     * for which the client shall wait for the server response, after sending a command.
     * CTP Service is not active only when state is DISCONNECTED. 
     */
    typedef enum {
        CTP_STATE_DISCONNECTED          = 0, 
        CTP_STATE_SLEEPING              = 1,
        CTP_STATE_CONNECTING            = 2, 
        CTP_STATE_CONNECTED             = 3,
        CTP_STATE_AUTHENTICATING        = 4, 
        CTP_STATE_READY                 = 5, 
        CTP_STATE_WAITING_RESPONSE      = 6, 
        CTP_STATE_CLOSING               = 7
    } CtpState;
    
    /**
     * Possible errors of CTP service.
     */
    typedef enum {
        CTP_ERROR_NOT_AUTHENTICATED         = 1,
        CTP_ERROR_UNAUTHORIZED              = 2, 
        CTP_ERROR_AUTH_FORBIDDEN            = 3, 
        CTP_ERROR_RECEIVED_UNKNOWN_COMMAND  = 4,
        CTP_ERROR_RECEIVED_STATUS_ERROR     = 5, 
        CTP_ERROR_RECEIVED_WRONG_COMMAND    = 6, 
        CTP_ERROR_ANOTHER_INSTANCE          = 7,
        CTP_ERROR_SENDING_READY             = 8,
        CTP_ERROR_RECEIVING_STATUS          = 9,
        CTP_ERROR_RECEIVE_TIMOUT            = 10,
        CTP_ERROR_CONNECTION_FAILED         = 11
    } CtpError;

private:

    /// Pointer to CTPService instance
    static CTPService* pinstance;


    /// The configuration of CTP
    CTPConfig config;

    /// The state of CTP connection.
    CtpState ctpState;

    /// Used by threads to exit in a soft way.
    bool leaving;

    /// The socket used
    FSocket* ctpSocket;
    
    /**
     * The listener for push notifications.
     * Once the Client has registered itself calling registerPushListener()
     * every push notification will be delivered to this object.
     * TODO: will be moved in PushManager
     */
    PushListener* pushListener;
    

    /**< Handle of main CTP thread, which implements the CTP process */
    CTPThread* ctpThread;                   
    /**< Handle of thread used to receive msg from Server */
    ReceiverThread* receiverThread;
    /**< Handle of thread used to send ready msg to Server */
    HeartbeatThread* heartbeatThread;
    /**< Handle of thread used to check if a response arrived in ctpCmdTimeout seconds */
    CmdTimeoutThread* cmdTimeoutThread;

    /// Store the received message from Server
    CTPMessage* receivedMsg;

    // For debugging
    int32_t totalBytesSent;
    int32_t totalBytesReceived;

    /// CTP Thread Pool Manager
    CTPThreadPool threadPool;

private:

    // Private methods:
    int32_t sendMsg(CTPMessage* message);
    StringBuffer createMD5Credentials();
    StringBuffer createErrorMsg(uint32_t errorCode = 0);

    
protected:

    // Constructor
    CTPService();


public:

    // Method to get the sole instance of CTPService
    static CTPService* getInstance();
    
    // Method to delete the sole instance
    static void dispose();

    ~CTPService();

    FThread* startCTP();
    int32_t stopCTP();
    int32_t openConnection();
    int32_t closeConnection();
    int32_t receive();

    // Create and send messages through the socket.
    int32_t sendReadyMsg();
    int32_t sendAuthMsg();
    int32_t sendByeMsg();

    CTPMessage* receiveStatusMsg();

    /**
     * Returns pointer to (internally owned) CTPConfig
     */
    CTPConfig* getConfig() { return &config; }

    /// Get the ctpState member.
    CtpState getCtpState()    { return ctpState; }

    /// Set the ctpState member.
    void setCtpState(CtpState v) { ctpState = v; }

    /// Returns true if CTP is leaving.
    bool isLeaving() { return leaving; }

    /// Forces CTP to go in the "leaving" state. 
    /// It hallows to exit threads and close correctly CTP.
    void setLeaving(bool value) { leaving = value; }
    
    
    /**
     * Register the passed object as the listener for push notifications.
     * Clients should derivate a class from PushListener and register 
     * it with this method, then implement PushListener::onNotificationReceived() 
     * to execute actions when a push message arrives.
     * TODO: will be moved in PushManager
     * 
     * @note  Only one listener can be registered at the time.
     *        This would discard a listener previously registered.
     * @param listener  the notification listener object
     */
    void registerPushListener(PushListener& listener) { pushListener = &listener; }
    
    /**
     * Method called when a sync notification has been received.
     * It's called by the receive thread, after a CTP push.
     * TODO: will be moved in PushManager, also used for STP
     * Will notify the pushListener (if registered) with a list of serverURI.
     * 
     * @param sn  the SyncNotification object received
     */
    void syncNotificationReceived(SyncNotification* sn);
    
    /**
     * Method called when a CTP error occurs.
     * Will notify the pushListener (if registered) with the passed
     * error code and (optional) addinitional info.
     * @param errorCode       the CTP error code (one of CtpError)
     * @param additionalInfo  [optional] further information about the error
     */
    void notifyError(const int errorCode, const int additionalInfo = 0);
    
    
    /// Stops the heartbeatThread and sets the pointer to NULL.
    void stopHeartbeatThread();
    
    /// Stops the cmdTimeoutThread and sets the pointer to NULL.
    void stopCmdTimeoutThread();
    
    /// Stops the receiverThread and sets the pointer to NULL.
    void stopReceiverThread();
    
    /// Stops the ctpThread and sets the pointer to NULL.
    void stopCtpThread();
    
    
    /**
     * Extracts the list of ServerURI names inside the SyncNotification.
     * Each ServerURI corresponds to a source that we've been notified for,
     * but it's the Client that knows the right correspondence with the source name.
     * TODO: will be moved in PushManager
     * 
     * @param sn  pointer to the SyncNotification object to read
     * @return    an ArrayList of ServerURI names (StringBuffers)
     */
    ArrayList getUriListFromSAN(SyncNotification* sn);

    
private:
    void hexDump(char *buf, int len);
    int extractMsgLength(const char* package, int packageLen);
    bool saveNonceParam(CTPMessage* authStatusMsg);
    
    /**
     * Utility to terminate a desired thread, setting its HANDLE to NULL.
     * @param thread   the HANDLE of the thread to be stopped
     * @return         true if the thread has been effectively terminated
     */
    bool stopThread(FThread* thread);
};



END_NAMESPACE

/** @endcond */
#endif


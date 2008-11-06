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

#include "base/globalsdef.h"
#include "base/fscapi.h"
#include "base/Log.h"
#include "base/util/utils.h"
#include "base/util/StringBuffer.h"
#include "base/util/ArrayList.h"

#include "push/CTPParam.h"
#include "push/FThread.h"
#include "push/FSocket.h"
#include "push/CTPThreadPool.h"

#include "push/CTPService.h"

USE_NAMESPACE

// Init static pointer.
CTPService* CTPService::pinstance = NULL;


/**
 * Method to create the sole instance of CTPService
 */
CTPService* CTPService::getInstance() {

    if (pinstance == NULL) {
        pinstance = new CTPService;
    }
    return pinstance;
}


/**
 * Constructor: reads the CTPConfig from registry and init members.
 */
CTPService::CTPService() : config(APPLICATION_URI) {

    // Read config from registry
    config.readCTPConfig();
    LOG.debug("CTP config read");
    

    ctpSocket        = NULL;
    ctpThread        = NULL;
    receiverThread   = NULL;
    heartbeatThread  = NULL;
    cmdTimeoutThread = NULL;
    receivedMsg      = NULL;
    ctpState         = CTP_STATE_DISCONNECTED;
    leaving          = false;
    pushListener     = NULL;

    totalBytesSent     = 0;
    totalBytesReceived = 0;
}


/**
 * Denstructor: stop any active thread, and close socket
 * connection if still active.
 */
CTPService::~CTPService() {
    
    stopCtpThread();

    stopReceiverThread();

    stopHeartbeatThread();

    stopCmdTimeoutThread();

    closeConnection();
    delete receivedMsg;
    // Cleanup any running thread
    threadPool.cleanup();
}


/**
 * Starts the CTP process.
 * Creates the main CTP thread, passing handle stpThread (NULL if not created)
 * @param stpThread  handle of STPThread: the CTPThread started here must
 *                   wait until the STPThread has finished
 * @return           handle of the ctpThread started
 */
FThread* CTPService::startCTP() {
    setCtpState(CTP_STATE_DISCONNECTED);
    leaving = false;
    totalBytesSent     = 0;
    totalBytesReceived = 0;

    LOG.debug("Starting ctp thread");
    ctpThread = new CTPThread();
    ctpThread->start();
    return ctpThread;
}



/**
 * Stops the CTP process.
 * 1. terminates the heartbeat thread if running, to avoid sending
 *    more READY msg at this point. 
 * 2. starts the receiving thread if not already running, to catch 
 *    the last OK msg from the Server
 * 3. sends the BYE msg
 * 4. waits for the OK msg, in case of timeout or error the 
 *    receive thread is terminated
 *
 * @return  0 if ctp closed successfully
 *          1 if ctpThread not running
 *          2 if socket connection not available
 *          3 if OK msg from the Server is not received (connection closed after a timeout)
 *         -1 if errors occurred creating the receive thread
 *         -2 if errors occurred waiting on receiverThread
 *          
 */
int32_t CTPService::stopCTP() {

    // Set leaving to true, so all the threads can check this flag and exit.
    leaving = true;
    setCtpState(CTP_STATE_CLOSING);

    if (!ctpThread) {
        LOG.debug("No CTP thread available -> exiting.");
        return 1;
    }
    if (!ctpSocket) {
        LOG.debug("No socket connection -> exiting.");
        return 2;
    }

    int ret = 0;
    LOG.info("Closing CTP connection...");


    int32_t timeout;
    // bool terminated;

    //
    // Send the BYE message if the receiverThread is running (if not we assume
    // the connection is unavailable)
    //
    if (receiverThread) {
        LOG.info("Sending [BYE] message...");
        if (sendByeMsg()) {
            LOG.error("Error sending the BYE message");
            goto finally;
        }
    }

    //
    // Wait for OK msg: receive thread should exit after the last OK
    // message sent by Server - timeout = ctpCmdTimeout (60sec).
    //
    timeout = config.getCtpCmdTimeout();
    if (!timeout) {
        timeout = 60;
    }

#if 0
    // Because of the bug in the FThread it is unsafe to invoke
    // wait(timeout)
    terminated = receiverThread->wait(timeout * 1000);

    if (terminated) {
        LOG.debug("receiverThread terminated");
        ret = 0;
    } else {
        // Timeout: kill thread -> out.
        LOG.debug("Timeout - receiverThread will now be terminated");
        ret = 3;
    }
#endif


finally:

    // Stop the heartbeat & timeout threads
    stopHeartbeatThread();
    stopCmdTimeoutThread();

    //
    // Close socket connection
    //
    closeConnection();

    return ret;
}


/**
 * Opens the socket connection.
 * 1. Initialize Winsock
 * 2. Get the address of the host (use getaddrinfo())
 * 3. Create a TCP/IP stream socket
 * 4. Connect to the server
 *
 * @return  0 if no errors
 */
int32_t CTPService::openConnection() {

    int32_t ret = 0;

    if (ctpSocket) {
        // If an old CTP session is up, here we close threads and bring 
        // down socket: we want to keep the latest CTP session
        closeConnection();
    }

    LOG.info("--- Starting a new SOCKET connection ---");
    ctpState = CTP_STATE_CONNECTING;
    leaving  = false;
    totalBytesSent     = 0;
    totalBytesReceived = 0;

    //
    // Find the server
    //
    LOG.debug("Find the server address...");
    LOG.info("HOSTNAME = '%s'  PORT = '%d'", config.getUrlTo().c_str(), config.getCtpPort());

    //
    // Create a TCP/IP stream socket and connect to the server
    //
    LOG.debug("Create SOCKET connection...");
    StringBuffer url(config.getUrlTo().c_str());
    ctpSocket = FSocket::createSocket(url, config.getCtpPort());
    LOG.debug("after socket created");

    if (ctpSocket == NULL) {
        LOG.error("Cannot create FSocket");
        ret = -3;
    } else {
        LOG.info("Succesfully connected to %s!", url.c_str());
        ctpState = CTP_STATE_CONNECTED;
    }

    if (ret != 0) {
        ctpState = CTP_STATE_DISCONNECTED;
    }
    return ret;
}



/**
 * Closes the socket connection.
 * @return 0  if no errors
 */
int32_t CTPService::closeConnection() {

    int ret = 0;
    if (ctpSocket) {
        ctpSocket->close();
        ctpSocket = NULL;
        LOG.info("Socket connection closed");
    }

    LOG.debug("Total number of bytes sent = %d",     totalBytesSent);
    LOG.debug("Total number of bytes received = %d", totalBytesReceived);
    totalBytesSent     = 0;
    totalBytesReceived = 0;
    
    ctpState = CTP_STATE_DISCONNECTED;
    return ret;
}



/**
 * Sends an AUTH message to the Server.
 * A CTPMessage is filled with parameters from CTPConfig.
 * 'sendMsg()' method is used to send the message with the ctpSocket.
 * @return 0 if no errors
 */
int32_t CTPService::sendAuthMsg(){

    LOG.debug("Creating AUTH msg...");
    ctpState = CTP_STATE_AUTHENTICATING;

    // Fill CTPMessage members
    CTPMessage authMsg;
    authMsg.setGenericCommand(CM_AUTH);
    authMsg.setProtocolVersion(CTP_PROTOCOL_VERSION);

    // Fill parameters (read values from config)
    CTPParam devId;
    devId.setParamCode(P_DEVID);
    devId.setValue(config.getDevID(), strlen(config.getDevID()));
    authMsg.addParam(&devId);

    CTPParam username;
    username.setParamCode(P_USERNAME);
    username.setValue(config.getUsername(), strlen(config.getUsername()));
    authMsg.addParam(&username);

    CTPParam cred;
    cred.setParamCode(P_CRED);
    // Create credentials from config props
    StringBuffer credentials = createMD5Credentials();
    cred.setValue(credentials.c_str(), credentials.length());
    authMsg.addParam(&cred);

    StringBuffer& fromValue = config.getUrlFrom();
    if (fromValue.length() > 0) {
        // FROM is used only after a JUMP status
        CTPParam from;
        from.setParamCode(P_FROM);
        from.setValue(fromValue.c_str(), fromValue.length());
        authMsg.addParam(&from);
    }

    LOG.info ("AUTH: devId='%s', user='%s', cred='%s'", config.getDevID(), 
                                                        config.getUsername(),
                                                        credentials.c_str() );

    // Send message
    return sendMsg(&authMsg);
}


/**
 * Sends a READY message to the Server.
 * A CTPMessage is filled with parameters from CTPConfig.
 * 'sendMsg()' method is used to send the message with the ctpSocket.
 * @return 0 if no errors
 */
int32_t CTPService::sendReadyMsg() { 

    // Fill CTPMessage members
    CTPMessage readyMsg;
    readyMsg.setGenericCommand(CM_READY);
    readyMsg.setProtocolVersion(CTP_PROTOCOL_VERSION);

    // Send message
    return sendMsg(&readyMsg);
}


/**
 * Sends a BYE message to the Server.
 * A CTPMessage is filled with parameters from CTPConfig.
 * 'sendMsg()' method is used to send the message with the ctpSocket.
 * @return 0 if no errors
 */
int32_t CTPService::sendByeMsg(){

    // Fill CTPMessage members
    CTPMessage byeMsg;
    byeMsg.setGenericCommand(CM_BYE);
    byeMsg.setProtocolVersion(CTP_PROTOCOL_VERSION);

    // Send message
    return sendMsg(&byeMsg);
}


/**
 * Sends a generic CTPMessage to the Server.
 * The socket must be already opened calling 'openConnection()'.
 * The CTPMessage passed must be already filled with all 
 * desired members, so a byte-array is formatted and sent to
 * the Server.
 * The 'ctpState' is set to CTP_STATE_WAITING_RESPONSE after the message 
 * is sent, as we always wait for a Server response for each sent msg.
 *
 * @param message  the CTPMessage ready to be sent
 * @return         0 if no errors
 * @note           no timeout is set for the socket 'send' operation
 */
int32_t CTPService::sendMsg(CTPMessage* message) {

    if (!message) {
        return 1;
    }

    if (!ctpSocket) {
        LOG.error("sendMsg error: socket not initialized.");
        return 2;
    }

    int ret = 0;
    // The msg is owned by the CTPMessage and there is no need to delete it
    char* msg = message->toByte();
    int msgLength = message->getPackageLength();

    // Debug the message to send.
    LOG.debug("Sending %d bytes:", msgLength);
    hexDump(msg, msgLength);

    ret = ctpSocket->writeBuffer((const int8_t* const) msg, msgLength);
    if (ret != msgLength) {
        LOG.error("CTPService::sendMsg - send() error (%d bytes sent)", ret);
        return -1;
    }
    else {
        LOG.debug("sendMsg - %d bytes sent", ret);
        ctpState = CTP_STATE_WAITING_RESPONSE;          // We wait for a Server response every msg sent!
        totalBytesSent += ret;
        LOG.debug("Total bytes sent since beginning: %d", totalBytesSent);

        // Will restore connection if no response in 60sec
        // Declare the old timeout done
        stopCmdTimeoutThread();

        threadPool.cleanup();
        // Create a new timeout thread
        cmdTimeoutThread = threadPool.createCmdTimeoutThread();
        cmdTimeoutThread->start();
    }
    return 0;
}




/**
 * Receive a CTP message through the socket connection.
 * The message is parsed, a CTPMessage is filled and returned (the
 * CTPMessage is internally owned by CTPService).
 * The message could be split into more packages, so we read the first 2 bytes
 * and keep receiving until the message is complete.
 * The ctpState is set to CTP_STATE_READY after the msg is received successfully.
 * 
 * @return  the received CTPMessage (pointer to internally owned object)
 * @note  this method calls winsock 'recv' function which is blocking
 */
CTPMessage* CTPService::receiveStatusMsg() {

    char buffer[MAX_MESSAGE_SIZE], msg[MAX_MESSAGE_SIZE];
    int totalBytes     = 0;
    int expectedLength = 0;

    delete receivedMsg;
    receivedMsg = NULL;

    //
    // Receive socket message: could be split into more pkg
    //
    while (1) {
        LOG.debug("Waiting for Server message...");
        int pkgLen = ctpSocket->readBuffer((int8_t*)buffer, sizeof(buffer));

        if (pkgLen <= 0) {
            // Socket error -> exit
            LOG.error("SOCKET recv() error");
            goto finally;
        } else {
            if (totalBytes == 0) {      // first time
                expectedLength = extractMsgLength(buffer, pkgLen);
                if (!expectedLength) {
                    goto finally;
                }
                expectedLength += 2;    // the first 2 bytes are the msg length
            }
            LOG.debug("Package received: %d bytes read (total = %d, expected = %d)",
                      pkgLen, totalBytes+pkgLen, expectedLength);
            // Check if msg too big
            if (totalBytes+pkgLen >= MAX_MESSAGE_SIZE) {
                LOG.error("Message larger than %d bytes!", MAX_MESSAGE_SIZE);
                goto finally;
            }

            // Append bytes to the 'msg' array
            memcpy(&msg[totalBytes], buffer, pkgLen);
            totalBytes += pkgLen;

            // Check if msg is complete
            if (totalBytes < expectedLength) {
                LOG.debug("Message incomplete -> back to receive");
                continue;
            }
            else {
                LOG.debug("Message complete");
                ctpState = CTP_STATE_READY;             // ctpState back to 'ready'
                totalBytesReceived += totalBytes;
                
                // Debug the message received.
                LOG.debug("Received %d bytes:", totalBytes);
                hexDump(msg, totalBytes);
                LOG.debug("Total bytes received since beginning: %d", totalBytesReceived);
                break;
            }
        }
    }

    // Parse the message, receivedMsg is internally owned
    receivedMsg = new CTPMessage(msg, totalBytes);
    LOG.debug("status = 0x%02x", receivedMsg->getGenericCommand());

finally:
    // Msg received or error, anyway kill the cmdTimeoutThread.
    stopCmdTimeoutThread();

    return receivedMsg;
}



/**
 * This method is called when the CTP process is connected and ready to receive
 * notifications from the Server. Two threads are started here:
 *
 * 1. heartbeatWorker: used to send a READY message every 'ctpReady' seconds,
 *                     as an heartbeat for the CTP connection.
 * 2. receiveWorker  : keep listening to Server messages on the same socket.
 *                     Starts the sync when a notification is received.
 * Handles of both threads are internally owned by CTPService, so threads
 * can be eventually terminated in case of errors.
 *
 * After the 2 threads have been started, we wait on the receiverThread 
 * (timeout = ctpConnTimeout, default = INFINITE) to be able to terminate the
 * CTP connection and restore it from scratch every 'ctpConnTimeout' seconds.
 * 
 * @return 0 if no errors
 * @note this method is blocked on the receiverThread (which is blocked on socket recv())
 *       and will exit only in case of:
 *       - socket errors
 *       - Server sends an error state
 *       - CTP in 'leaving state' (Client is closing CTP)
 */
int32_t CTPService::receive() {

    // Safe checks...
    if (!ctpSocket) {
        LOG.error("CTPService::receive() error: no socket connection available");
        return -3;
    }
    
    //
    // Start thread to send 'ready' messages
    //
    heartbeatThread = threadPool.createHeartbeatThread();
    heartbeatThread->start();
    
    //
    // Start thread to receive messages from Server
    //
    receiverThread = threadPool.createReceiverThread();
    receiverThread->start();

    //
    // Wait for receiverThread: it ends only in case of errors.
    // Use 'ctpConnTimeout' as timeout on this thread.
    //
    int32_t ret = 0;
    uint32_t timeout = getConfig()->getCtpConnTimeout() * 1000;

    LOG.debug("Waiting for the receive thread to finish (timeout = %d sec)...",
              getConfig()->getCtpConnTimeout());
    bool receiveTerminated;
    if (timeout == 0) {
        receiverThread->wait();
        receiveTerminated = true;
    } else {
        receiveTerminated = receiverThread->wait(timeout);
    }

    if (receiveTerminated) {
        LOG.debug("receiverThread terminated");
        ret = 0;
    } else {
        LOG.debug("Timeout - receiverThread will now be terminated");
        stopReceiverThread();
        ret = 1;
    }

    // We are terminating receiving, therefore we must stop the heartbeat
    stopHeartbeatThread();

    return ret;
}


void CTPService::stopHeartbeatThread() {
    stopThread(heartbeatThread); 
    heartbeatThread = NULL;
}
void CTPService::stopCmdTimeoutThread() {
    stopThread(cmdTimeoutThread); 
    cmdTimeoutThread = NULL;
}
void CTPService::stopReceiverThread() {
    stopThread(receiverThread); 
    receiverThread = NULL;
}
void CTPService::stopCtpThread() {
    stopThread(ctpThread); 
    ctpThread = NULL;
}

bool CTPService::stopThread(FThread *thread) {

    bool terminated = false;
    if (thread) {
        thread->softTerminate();
        terminated = thread->finished();
    }

    return terminated;
}


/**
 * Formats a string for the 'cred' CTP param.
 * User credentials are encoded using MD5schema: 
 *   B64(MD5( B64(MD5("username":"password")):"clientNonce" ))
 * User parameters are retrieved from CTPConfig.
 * @return the credential string in b64 format.
 */
StringBuffer CTPService::createMD5Credentials() {

    char* credential = NULL;

    const char*  username    = config.getAccessConfig().getUsername();
    const char*  password    = config.getAccessConfig().getPassword();
    StringBuffer clientNonce = config.getCtpNonce();

    //LOG.debug("Creating cred from: username = '%s', pwd = '%s', clientNonce = '%s'", 
    //          username, password, clientNonce.c_str());
    
    credential = MD5CredentialData(username, password, clientNonce.c_str());
    if (credential) {
        StringBuffer ret(credential);
        delete [] credential;
        return ret;
    }

    StringBuffer emptyRes;
    return emptyRes;
}


///// TODO FIXME where are these functions from????
bool checkStartSync(void);
bool checkStartSync() {
    return false;
}


// TODO where should this go???
/**
 * Print the message written in exadecimal code.
 */
void CTPService::hexDump(char *buf, int len) {

    if (LOG.getLevel() < LOG_LEVEL_DEBUG) {
        return;
    }

    char* tmp = new char[len*8 + 3];
    tmp[0] = '[';
    int pos = 1;
    for (int i=0; i<len; i++) {
        sprintf(&tmp[pos], "%02x ", buf[i]);
        pos += 3;
    }
    tmp[pos-1] = ']';
    tmp[pos] = 0;
    LOG.debug("%s", tmp);
    delete [] tmp;
}

/**
 * Utility to extract the message length from a CTP package.
 * The message length is stored in the first 2 bytes of the CTP package.
 * @param  package    the input package
 * @param  packageLen the length of the passed package
 * @return            the message length
 */
int CTPService::extractMsgLength(const char* package, int packageLen) {

    if (packageLen < 2) {
        LOG.error("Unable to read the package length: not enough bytes received (%d)", packageLen);
        return 0;
    }

    int messageLen  = (int)((unsigned char)package[0]);
    int messageLen1 = (int)((unsigned char)package[1]);

    messageLen <<= 8;
    messageLen = messageLen | messageLen1;
    return messageLen;
}


void CTPService::syncNotificationReceived(SyncNotification* sn) {
    
    if (pushListener) {
        // Forward the notification to the registered listener
        ArrayList uriList = getUriListFromSAN(sn);
        pushListener->onNotificationReceived(uriList);
    }
    else {
        LOG.debug("No pushListener registered, push message lost.");
    }
}

void CTPService::notifyError(const int errorCode, const int additionalInfo) {
    
    if (pushListener) {
        // Forward the error to the registered listener
        pushListener->onCTPError(errorCode);
    }
}



//////////////////////////////////////////////////////////////////////////////
// CmdTimeoutThread
//////////////////////////////////////////////////////////////////////////////
CmdTimeoutThread::CmdTimeoutThread() {
}

CmdTimeoutThread::~CmdTimeoutThread() {
}

/**
 * Thread used to check if a response arrived in ctpCmdTimeout seconds.
 * If not, the CTP connection will be pulled down so that ctpThread
 * will restore the whole CTP connection.
 * This thread is started every time a message is sent.
 * @param lpv  unused
 */
void CmdTimeoutThread::run() {
    LOG.debug("Starting cmdTimeoutWorker thread");

    // TODO PowerPolicyNotify(PPN_UNATTENDEDMODE, TRUE);

    // Load the timeout (ctpCmdTimeout)
    CTPService* ctpService = CTPService::getInstance();
    int32_t timeout = ctpService->getConfig()->getCtpCmdTimeout();
    if (!timeout) {
        timeout = 180;      // 3 minutes max
    }

    FThread::sleep(timeout * 1000);

    // Check if we were killed, then there is nothing to do
    if (terminate) {
        goto finally;
    }

    if ( (ctpService->isLeaving() == false) &&
         (ctpService->getCtpState() == CTPService::CTP_STATE_WAITING_RESPONSE) ) {
        // Response not received -> close ctp connection so that
        // the receiveThread will exit with error, so ctpThread will restore ctp.
        LOG.info("No response received from Server after %d seconds: closing CTP", timeout);
        ctpService->notifyError(CTPService::CTP_ERROR_RECEIVE_TIMOUT);
        
        ctpService->closeConnection();
        
        // Heartbeat thread can be in sleep mode, we must terminate it.
        ctpService->stopHeartbeatThread();
    }

finally:

    // TODO PowerPolicyNotify(PPN_UNATTENDEDMODE, FALSE);
    LOG.debug("Exiting cmdTimeoutWorker thread");
}

void CmdTimeoutThread::softTerminate() {
    terminate = true;
}

//////////////////////////////////////////////////////////////////////////////
// CTPThread
//////////////////////////////////////////////////////////////////////////////
CTPThread::CTPThread() : FThread(),
                         errorCode(0)
{
}

CTPThread::~CTPThread() {
}

/**
 * This is the main CTP thread. Manages the CTP connection using CTPService:
 * 1. waits until the stpThread is finished, if successfully we exit this thread
 *    (STP is working), otherwise continue with CTP (STP is not working)
 * 2. Opens the CTP socket connection
 * 3. Authenticate the client
 * 4. Starts threads to receive messages and to send 'ready' msg.
 * 
 * @param lpv  the HANDLE of the stpThread running (NULL if not started)
 * @return a value >= 0 if no errors:
 *         0 if CTP closed correctly (Client closed CTP)
 *         1 if STP is working correctly, CTP not started
 *         2 if CTP client not authenticated (at the 2nd AUTH msg)
 *         3 if CTP client unauthorized
 *         4 if CTP client authentication forbidden
 *         5 if STP not working but CTP not started either
 *         6 if CTP already active in another instance
 *         <0 if errors
 *
 * @note This thread exits only in case of:
 *   - STPThread ended succesfully (STP is working, CTP not started)
 *   - CTP client not authenticated
 *   - CTP in 'leaving state' (Client is closing CTP)
 *   - CTP started from another instance (code 6)
 * In case of socket/Server errors, the whole CTP connection is pulled down
 * and restored again in a while (after 'ctpRetry' seconds).
 */
void CTPThread::run() {

    LOG.debug("Starting ctpWorker thread");

    // Get the unique instance of CTPService.
    CTPService* ctpService = CTPService::getInstance();

    //TODO PowerPolicyNotify(PPN_UNATTENDEDMODE, TRUE);

    // Refresh configuration, save the ctpRetry original value in a buffer
    ctpService->getConfig()->readCTPConfig();

    LOG.debug("Reading CtpRetry");
    int32_t defaultCtpRetry = ctpService->getConfig()->getCtpRetry();

    // Start the CTP connection process.
    // *********************************
    // Infinite cycle: always restore the connection if it's lost 
    // or in case of errors. Exit only if leavingState flag is up.
    //
    bool restore = false;
    bool jump    = false;
    while (ctpService->isLeaving() == false) {

        if (restore) {
            // Restoring from a broken connection: close socket and wait some seconds.
            LOG.debug("Restoring CTP connection...");
            ctpService->closeConnection();
            ctpService->setCtpState(CTPService::CTP_STATE_SLEEPING);

            int32_t ctpRetry = ctpService->getConfig()->getCtpRetry();
            int32_t maxCtpRetry = ctpService->getConfig()->getMaxCtpRetry();
            int32_t sleepTime = ctpRetry < maxCtpRetry ? ctpRetry : maxCtpRetry;
            
            LOG.info("CTP will be restored in %d seconds...", sleepTime);
            if (sleepTime == maxCtpRetry) {
                // In case the max retry time is reached
                ctpService->notifyError(CTPService::CTP_ERROR_CONNECTION_FAILED, sleepTime * 1000);
            }
            FThread::sleep(sleepTime * 1000);
            
            // Calling StopCTP() sets the leaving flag.
            if (ctpService->isLeaving()) {
                LOG.debug("CTP state is 'leaving' -> exit CTP");
                goto finally;
            }

            // CTP could have been restarted during the sleep time!
            // So exit if the ctp is active.
            if (ctpService->getCtpState() > CTPService::CTP_STATE_SLEEPING) {
                LOG.debug("CTP already active -> don't restore ctp");
                ctpService->notifyError(CTPService::CTP_ERROR_ANOTHER_INSTANCE);
                errorCode = 6;
                goto finally;
            }

            // Save the new value to config
            sleepTime *= CTP_RETRY_INCREASE_FACTOR;     // Double the retry time
            ctpService->getConfig()->setCtpRetry(sleepTime);
            restore = false;
        }
        if (jump) {
            // Restoring from a JUMP status: close socket and reconnect immediately.
            LOG.debug("Restoring CTP connection from a JUMP...");
            ctpService->closeConnection();
            jump = false;
        }


        while (checkStartSync()) {
            // Wait for sync to finish: CTP AUTH phase uses the same clientNonce
            // used by sync authentication, so avoid interferences.
            LOG.debug("Waiting for sync to finish...");
            FThread::sleep(5000);
        }

        //
        // Open socket connection
        // ----------------------
        LOG.debug("Open CTP connection...");
        if (ctpService->openConnection()) {
            restore = true;
            continue;
        }

        //
        // Authentication
        // --------------
        LOG.debug("Sending [AUTH] message...");
        if (ctpService->sendAuthMsg()) {
            restore = true;
            continue;
        }
        
        // Receiving AUTH status message
        CTPMessage* authStatusMsg = ctpService->receiveStatusMsg();
        if (!authStatusMsg) {
            LOG.error("Error receiving AUTH status msg.");
            restore = true;
            continue;
        }
        char authStatus = authStatusMsg->getGenericCommand();
        CTPParam* param;
        switch (authStatus) {

            case ST_NOT_AUTHENTICATED:
                //
                // Retry with new nonce received
                //
                LOG.info("Client not authenticated: retry with new nonce");
                if (saveNonceParam(authStatusMsg) == false) {
                    // NONCE not found -> restore connection
                    LOG.error("Error receiving NON_AUTHENTICATED Status message: NONCE param is missing");
                    restore = true;
                    continue;
                }

                // Send 2nd auth msg
                LOG.info("Sending CTP authentication message...");
                if (ctpService->sendAuthMsg()) {
                    restore = true;
                    continue;
                }

                // Check 2nd status received, only OK allowed
                authStatusMsg = ctpService->receiveStatusMsg();
                authStatus = authStatusMsg->getGenericCommand();
                if (authStatus == ST_OK) {
                    // *** Authentication OK! *** 
                    // Save nonce if any (go to case ST_OK)
                }
                else if (authStatus == ST_NOT_AUTHENTICATED) {
                    LOG.info("CTP error: Client not authenticated. Please check your credentials.");
                    //showInvalidCredentialsMsgBox(INVALID_CREDENTIALS, 10);      // code 401
                    errorCode = 2;
                    ctpService->notifyError(CTPService::CTP_ERROR_NOT_AUTHENTICATED);
                    goto error;
                }
                else if (authStatus == ST_UNAUTHORIZED) {
                    LOG.info("CTP error: Client unauthorized by the Server. Please check your credentials.");
                    //showInvalidCredentialsMsgBox(PAYMENT_REQUIRED, 10);         // code 402
                    errorCode = 2;
                    ctpService->notifyError(CTPService::CTP_ERROR_UNAUTHORIZED);
                    goto error;
                }
                else {
                    LOG.info("CTP error: received status '0x%02x'.", authStatus);
                    errorCode = 2;
                    ctpService->notifyError(CTPService::CTP_ERROR_RECEIVED_UNKNOWN_COMMAND);
                    goto error;
                }
                // no 'break': need to enter into case ST_OK...


            case ST_OK:
                // *** Authentication OK! *** 
                LOG.info("Client authenticated successfully!");
                ctpService->setCtpState(CTPService::CTP_STATE_READY);
                ctpService->getConfig()->setCtpRetry(defaultCtpRetry);     // Restore the original ctpRetry time
                // Save nonce if any
                if (saveNonceParam(authStatusMsg) == false) {
                    LOG.info("No new nonce received.");
                }
                break;


            // --- note: JUMP not implemented Server side ----
            case ST_JUMP:
                //
                // Jump to desired server 'to' and save the 'from' value
                //
                LOG.info("Server requested a JUMP");
                if (authStatusMsg->params.size() < 1) {
                    // Expected FROM and TO params -> restore connection
                    LOG.error("Error receiving JUMP Status message: some parameter is missing");
                    restore = true;
                    continue;
                }

                // Read FROM and TO parameters and update CTPConfig
                param = (CTPParam*)authStatusMsg->params.front();
                while (param) {
                    int valueLen = param->getValueLength();
                    void* value  = param->getValue();
                    if (param->getParamCode() == P_FROM) {
                        char* from = stringdup((char*)value, valueLen);
                        ctpService->getConfig()->setUrlFrom(from);
                        delete [] from;
                    }
                    else if (param->getParamCode() == P_TO) {
                        char* url = stringdup((char*)value, valueLen);
                        StringBuffer to = ctpService->getConfig()->getHostName(url);
                        int port  = ctpService->getConfig()->getHostPort(url);
                        ctpService->getConfig()->setUrlTo(to);
                        ctpService->getConfig()->setCtpPort(port);
                        delete [] url;
                    }
                    else {
                        // Unexpected Status param -> restore connection
                        LOG.error("Error receiving JUMP Status message: unexpected \
                                   param '0x%02x'", (*param).getParamCode());
                        restore = true;
                        continue;
                    }
                    param = (CTPParam*)authStatusMsg->params.next();
                }

                // Now restore the socket connection to the new Server address...
                LOG.debug("JUMP status received: FROM %s TO %s:%d",
                                 ctpService->getConfig()->getUrlFrom().c_str(), 
                                 ctpService->getConfig()->getUrlTo().c_str(), 
                                 ctpService->getConfig()->getCtpPort() );
                jump = true;
                continue;


            case ST_UNAUTHORIZED:
                // Not authorized -> save nonce if any, exit thread
                LOG.info("Unauthorized by the Server, please check your credentials.");
                //showInvalidCredentialsMsgBox(PAYMENT_REQUIRED, 10);         // code 402
                if (saveNonceParam(authStatusMsg) == false) {
                    LOG.debug("No new nonce received.");
                }
                ctpService->notifyError(CTPService::CTP_ERROR_UNAUTHORIZED);
                errorCode = 3;
                goto error;


            case ST_FORBIDDEN:
                // Authentication forbidden -> exit thread
                LOG.info("Authentication forbidden by the Server, please check your credentials.");
                //showInvalidCredentialsMsgBox(FORBIDDEN, 10);                // code 403
                errorCode = 4;
                ctpService->notifyError(CTPService::CTP_ERROR_AUTH_FORBIDDEN);
                goto error;

            case ST_ERROR:
                // Error -> restore connection
                LOG.info("Received ERROR status from Server: restore ctp connection");
                //printErrorStatus(authStatusMsg);
                // TODO: parse the error message and pass to the notification function
                ctpService->notifyError(CTPService::CTP_ERROR_RECEIVED_STATUS_ERROR);
                restore = true;
                continue;

            default:
                // Unexpected status -> restore connection
                LOG.error("Unexpected status received '0x%02x' -> restore ctp connection", authStatus);
                ctpService->notifyError(CTPService::CTP_ERROR_RECEIVED_WRONG_COMMAND);
                restore = true;
                continue;
        }

        //
        // Creates the thread that will be blocked waiting for server msg
        // and waits until it ends (errors or ctpConnTimeout).
        //
        ctpService->receive();

        // If here, connection was broken or ctpConnTimeout -> restore connection
        restore = true;
    }
    goto finally;


error:
    // Restore the original ctpRetry time
    ctpService->getConfig()->setCtpRetry(defaultCtpRetry);
    ctpService->closeConnection();

finally:
    // TODO PowerPolicyNotify(PPN_UNATTENDEDMODE, FALSE);
    LOG.debug("Exiting ctpWorker thread");
}

/**
 * Utility to extract the nonce parameter from CTPMessage passed and
 * save it to 'CTPConfig::ctpNonce'.
 * The configuration (CTPConfig) is also saved to registry.
 * The nonce is expected to be the first parameter of the message.
 * @param authStatusMsg  the CTPMessage to analyze
 * @return               true if the nonce has been saved, 
 *                       false if nonce not found
 */
bool CTPThread::saveNonceParam(CTPMessage* authStatusMsg) {

    if (authStatusMsg->params.size() == 0) {
        return false;
    }

    // Get nonce param
    int nonceLen = 0;
    void* nonce  = NULL;
    CTPParam* param = (CTPParam*)authStatusMsg->params.front();
    if (param && param->getParamCode() == P_NONCE) {
        nonceLen = (*param).getValueLength();
        nonce    = (*param).getValue();
    }
    else {
        return false;
    }
    if (!nonce || nonceLen==0) {
        return false;
    }


    // Nonce is encoded in b64.
    char* b64Nonce = new char[((nonceLen/3+1)<<2) + 32];
    int len = b64_encode(b64Nonce, nonce, nonceLen);
    b64Nonce[len] = 0;

    //LOG.debug("New nonce received:");
    //hexDump((char*)nonce, nonceLen);
    LOG.debug("New nonce received: '%s'", b64Nonce);

    // Save new nonce to config, and save config!
    CTPService* ctpService = CTPService::getInstance();
    ctpService->getConfig()->setCtpNonce(b64Nonce);
    ctpService->getConfig()->saveCTPConfig();

    LOG.debug("Done");

    delete [] b64Nonce;
    return true;
}



//////////////////////////////////////////////////////////////////////////////
// ReceiverThread
//////////////////////////////////////////////////////////////////////////////
ReceiverThread::ReceiverThread() : FThread(),
                                   errorCode(0)
{
}

ReceiverThread::~ReceiverThread() {
}

void ReceiverThread::run() {
 
    LOG.debug("Starting receiveWorker thread");
    errorCode = 0;
    CTPService* ctpService = CTPService::getInstance();

    //TODO PowerPolicyNotify(PPN_UNATTENDEDMODE, TRUE);

    // Keep the socket open, always in 'receive' state.
    // Exit only in case of errors or if we're leaving CTP.
    while (ctpService->isLeaving() == false) {

        // Receive msg from Server
        CTPMessage* statusMsg = ctpService->receiveStatusMsg();
        if (!statusMsg) {
            // Error on receiving -> exit thread
            errorCode = -1;
            ctpService->notifyError(CTPService::CTP_ERROR_RECEIVING_STATUS);
            goto finally;
        }

        char status = statusMsg->getGenericCommand();
        SyncNotification* sn = NULL;
        switch (status) {

            case ST_OK:
                // 'OK' to our 'READY' command -> back to recv
                LOG.debug("[OK] received -> back to receive state");
                break;

            case ST_SYNC:
                //
                // Start the sync!
                // ---------------
                LOG.info("[SYNC] notification received! Starting the sync");
                sn = statusMsg->getSyncNotification();
                ctpService->syncNotificationReceived(sn);

                // Back to recv
                LOG.debug("Back to receive state");
                break;

            case ST_ERROR:
                LOG.debug("[ERROR] message received");
                ctpService->notifyError(CTPService::CTP_ERROR_RECEIVED_STATUS_ERROR);
                //printErrorStatus(statusMsg);
            default:
                // Error from server -> exit thread (will try restoring the socket from scratch)
                LOG.debug("Bad status received (code 0x%02x), exiting thread", status);
                errorCode = -2;
                ctpService->notifyError(CTPService::CTP_ERROR_RECEIVED_UNKNOWN_COMMAND);
                goto finally;
        }
    }

finally:
    //TODO PowerPolicyNotify(PPN_UNATTENDEDMODE, FALSE);
    LOG.debug("Exiting receiveWorker thread");
}

//////////////////////////////////////////////////////////////////////////////
// HeartbeatThread
//////////////////////////////////////////////////////////////////////////////
/**
 * Thread used to send 'READY' messages as a heartbeat, every 'ctpReady' seconds.
 * It ends only if CTP is in 'leaving state' (Client stops CTP).
 * @param lpv  unused
 * @return 0 if no errors
 */
HeartbeatThread::HeartbeatThread() : FThread()
{
}

HeartbeatThread::~HeartbeatThread() {
}

void HeartbeatThread::softTerminate() {
    terminate = true;
}


void HeartbeatThread::run() {
    LOG.debug("Starting Heartbeat thread");

    errorCode = 0;

    //TODO PowerPolicyNotify(PPN_UNATTENDEDMODE, TRUE);

    // Load the sleep interval (ctpReady)
    CTPService* ctpService = CTPService::getInstance();
    int32_t sleepInterval = ctpService->getConfig()->getCtpReady();

    // Send 'ready' message to Server and sleep ctpReady seconds
    while (terminate == false) {

        LOG.debug("Sending [READY] message...");
        if (ctpService->sendReadyMsg()) {
            LOG.debug("Error sending READY msg");
            errorCode = 1;
            ctpService->notifyError(CTPService::CTP_ERROR_SENDING_READY);
            // By closing the connection we force the CTP to restart
            ctpService->closeConnection();
            break;
        }
        //LOG.debug("Next ready msg will be sent in %d seconds...", sleepInterval);
        FThread::sleep(sleepInterval * 1000);
    }

    //TODO PowerPolicyNotify(PPN_UNATTENDEDMODE, FALSE);
    LOG.debug("Exiting heartbeatWorker thread");
}



// TODO: will be moved in PushManager
ArrayList CTPService::getUriListFromSAN(SyncNotification* sn) 
{
    ArrayList list;
    int n = 0;
    
    if (!sn) {
        LOG.error("CTP notification error: SyncNotification is NULL");
        return list;
    }

    // Get number of sources to sync
    n = sn->getNumSyncs();
    if (!n) {
        LOG.error("CTP notification error: no sources to sync from server");
        return list;
    }
    
    // Compose the array of ServerURI names
    for (int i=0; i<n; i++) {
        SyncAlert* sync = sn->getSyncAlert(i);
        if(!sync) {
            LOG.error("CTP notification error: no SyncAlert in SyncNotification");
            continue;
        }
        if (sync->getServerURI()) {
            StringBuffer uri(sync->getServerURI());
            list.add(uri);
            LOG.debug("uri pushed: '%s'", uri.c_str());
        } 
        else {
            LOG.error("CTP notification error: no source found from server "
                      "notification request: %s", sync->getServerURI());
            continue;
        }
    }

    if (list.size() == 0) {
        // 0 sources to sync -> out
        LOG.info("No sources to sync");
    }
    return list;
}


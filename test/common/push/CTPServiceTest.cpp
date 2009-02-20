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

#include "base/globalsdef.h"
#include "base/fscapi.h"
#include "base/Log.h"
#include "base/adapter/PlatformAdapter.h"
#include "spds/DefaultConfigFactory.h"

#include "push/CTPService.h"
#include "push/FThread.h"
#include "push/FSocket.h"

#include "cppunit/extensions/TestFactoryRegistry.h"
#include "cppunit/extensions/HelperMacros.h"

#define CTP_TEST_APPLICATION_URI "FunambolTest/CTPService"

// Test default config
#define TEST_USERNAME      "testuser"
#define TEST_PASSWORD      "testpassword"
#define TEST_DEVID         "fts-00000000"

// Test CTPConfig
#define CTP_URL_TO         "fake.url.com"
#define CTP_CMD_TIMEOUT    60
#define CTP_CONN_TIMEOUT   0
#define CTP_PORT           4745
#define CTP_READY          300
#define CTP_RETRY          5
#define MAX_CTP_RETRY      900
#define CTP_NOTIFY_TIMEOUT 120

#define CTP_SERVICE CTPService::getInstance()

// Ctp assertions defs
#define CTPASSERT_COMMAND(command)            assertOutputCommand(command)
#define CTPASSERT_STATE(state)                assertCTPState(state)
#define CTPASSERT_NOTIFICATION()              assertNotification()
#define CTPASSERT_ERROR_NOTIFICATION(error)   assertErrorNotification(error)

USE_NAMESPACE

/**
 * Define a fake socket used to simulate a client socket.
 * It simply responds to the write and read methods, depending
 * on the CTPService state.
 */
class FakeSocket : public FSocket {

public:
    
/***************************** FSOCKET METHODS ********************************/
    
    FakeSocket() { }
    ~FakeSocket() { }

    /** FSocket createSocket */
    static FSocket* createSocket(const StringBuffer& peer, int32_t port) {
        return new FakeSocket();
    }

    /** FSocket readBuffer */
    int32_t readBuffer(int8_t* buffer, int32_t maxLen) {
        memset ( buffer, 0, maxLen );
        
        // Wait until the input message is available
        int timeout = 20000;
        while(!inputPacketAvailable()) {
            // Check if the connection is closed closed
            if(getConnectionClosed()) return -1;
            FThread::sleep(100);
            timeout -= 100;
            if(timeout < 0) { 
                CPPUNIT_ASSERT(false);
            }
        }

        // Get the read buffer size
        int readBytes = input_socket_buffer_len > maxLen ? 
                        maxLen : input_socket_buffer_len;

        // Copy the input message buffer
        memcpy(buffer, input_socket_buffer, readBytes);

        // Update socket buffer/length
        input_socket_buffer_len -= readBytes;
        if(input_socket_buffer_len > maxLen) {
            // there is still package segments to read
            input_socket_buffer += readBytes;
        }
        LOG.debug("FakeSocket - reading buffer data");
        hexDump((char*)buffer, readBytes);

        return readBytes;
    }

    /** FSocket writeBuffer */
    int32_t writeBuffer(const int8_t* const buffer, int32_t len) {
        
        LOG.debug("FakeSocket - writing buffer");
        hexDump((char*)buffer, len);

        /******* Catch the heartbeat [READY] messages ********/
        CTPMessage msg; msg.parse((char*)buffer, len);
        if(msg.getGenericCommand() == CM_READY) {
            // Reply OK -> it may override a push message
            CTPMessage reply;
            reply.setGenericCommand(ST_OK);
            reply.setProtocolVersion(CTP_PROTOCOL_VERSION);
            char* msgbuffer = reply.toByte(); 
            int   msgbuffer_len = reply.getBufferLength(); 
            setInputSocketBuffer(msgbuffer, msgbuffer_len);
            return len;
        }
        /*****************************************************/

        setOutputSocketBuffer((char*)buffer, len);
        return len;
    }

    void close() { 
        dispose();
    }

/***************************  END FSOCKET METHODS *******************************/
    
    /** Set the input socket buffer */
    static void setInputSocketBuffer(char* buffer, int len) {
        freeBuffer(input_socket_buffer, input_socket_buffer_len);
        input_socket_buffer = createBuffer(buffer, len);
        input_socket_buffer_len = len;
    }

    /** Set the output socket buffer */
    static void setOutputSocketBuffer(char* buffer, int len) {
        freeBuffer(output_socket_buffer, output_socket_buffer_len);        
        output_socket_buffer = createBuffer(buffer, len);
        output_socket_buffer_len = len;
    }

    /** Get the output socket buffer, return the buffer length */
    static int getOutputSocketBuffer(int8_t* buffer, int maxLen) {
        
        // wait for output packet available
        int timeout = 20000;
        while(!FakeSocket::outputPacketAvailable()) { 
            FThread::sleep(100);
            timeout -= 100;
            if(timeout < 0) { 
                CPPUNIT_ASSERT(false);
            }
        }

        memset ( buffer, 0, maxLen );
        int returnLength = output_socket_buffer_len > maxLen ? 
                                 maxLen : output_socket_buffer_len;
       
        memcpy(buffer, output_socket_buffer, returnLength);
        setOutputSocketBuffer(NULL, 0);
        
        return returnLength;
    }
    
    static bool getConnectionClosed() {
        return connectionClosed;
    }
    
    static void setConnectionClosed(bool closed) {
        connectionClosed = closed;
    }

    /* Free buffers */
    static void dispose() { 
        freeBuffer(input_socket_buffer, input_socket_buffer_len);
        freeBuffer(output_socket_buffer, output_socket_buffer_len);
        input_socket_buffer_len = 0;
        output_socket_buffer_len = 0;
        connectionClosed = false;
    }

private:

    /** Check whether an input package is available */
    bool inputPacketAvailable() {
        return input_socket_buffer_len > 0;
    }
    /** Check whether an output package is available */
    static bool outputPacketAvailable() {
        return output_socket_buffer_len > 0;
    }

    /** Create a string buffer */
    static char* createBuffer(char* buffer, int len) {
        char* s = 0;
        s = (char *)realloc(s, len * sizeof(char) );
        memcpy(s, buffer, len);
        return s;
    }

    /** Delete a string buffer */
    static void freeBuffer(char* buffer, int len) {
        if(len>0) {
            free(buffer);
        }
    }

    /** Print the message written in exadecimal code. */
    static void hexDump(char *buf, int len) {
        char* tmp = new char[len*8 + 3];
        tmp[0] = '[';
        int pos = 1;
        for (int i=0; i<len; i++) {
            sprintf(&tmp[pos], "%02x ", buf[i]);
            pos += 3;
        }
        tmp[pos-1] = ']';
        tmp[pos] = 0;
        LOG.debug("FakeSocket - %s", tmp);
        delete [] tmp;
    }

    /** Input/Output buffers */
    static char* input_socket_buffer;
    static int   input_socket_buffer_len;

    static char* output_socket_buffer;
    static int   output_socket_buffer_len;
    
    static bool  connectionClosed;
};

char*   FakeSocket::input_socket_buffer;
int     FakeSocket::input_socket_buffer_len;
char*   FakeSocket::output_socket_buffer;
int     FakeSocket::output_socket_buffer_len;

bool    FakeSocket::connectionClosed;


/******************************************************
 * Test suite for the CTPService class. It involves:  *
 *  - FThread                                         *
 *  - FSocket                                         *
 *  - CTPConfig                                       *
 *  - CTPMessage                                      *
 ******************************************************/
class CTPServiceTest : public CppUnit::TestFixture, public PushListener {
    
    CPPUNIT_TEST_SUITE(CTPServiceTest);
    CPPUNIT_TEST(CTPConfigTest);
    CPPUNIT_TEST(CTPReadyMessageTest);
    CPPUNIT_TEST(CTPByeMessageTest);
    CPPUNIT_TEST(CTPConnectionTest);
    CPPUNIT_TEST(CTPAuthenticationTest);    
    CPPUNIT_TEST(CTPStartStopServiceTest);
    CPPUNIT_TEST(CTPAuthenticationFailTest1);
    CPPUNIT_TEST(CTPAuthenticationFailTest2);
    CPPUNIT_TEST(CTPTestPush);
    CPPUNIT_TEST_SUITE_END();

public:

    /**
     * Initialize a test client configuration used by the CTPService while
     * connecting to the server.
     * Register the PushListener, to be notified of the CTP events
     */
    void setUp() {
        
        LOG.setLevel(LOG_LEVEL_DEBUG);
        
        //Initialize test configuration
        initConfig();

        //Register push listener
        LOG.debug("CTPServiceTest - Register the push listener");
        CTP_SERVICE->registerPushListener(*this);

        //Set the custom socket as a fake socket
        LOG.debug("CTPServiceTest - Set a fake FSocket");
        FSocket::setSocket(FakeSocket::createSocket(CTP_SERVICE->getConfig()->getUrlTo(), 
                                                    CTP_SERVICE->getConfig()->getCtpPort()));
        //Reset socket buffers
        FakeSocket::dispose();

        notificationReceived = false;
        errorReceived = false;
    }

    void tearDown() { 
        //Reset socket buffers
        FakeSocket::dispose();
    }

    /* Initialize the CTP test configuration */
    void initConfig() {
        // Forcing PlatformAdapter initialization
        PlatformAdapter::init(CTP_TEST_APPLICATION_URI, true);

        // Set up a default test configuration
        LOG.debug("CTPServiceTest - Initializing the client configuration");
        DMTClientConfig config;
        config.setClientDefaults();
        
        // Set username/password
        config.getAccessConfig().setUsername(TEST_USERNAME);
        config.getAccessConfig().setPassword(TEST_PASSWORD);
        config.getAccessConfig().setSyncURL(CTP_URL_TO);

        // Set deviceid
        config.getDeviceConfig().setDevID(TEST_DEVID);
        
        // Save default config
        config.save();
        
        // Initialize test CTPConfig
        LOG.debug("CTPServiceTest - Initializing the ctp configuration");
        CTPConfig* ctpConfig = CTP_SERVICE->getConfig();
        ctpConfig->setUrlTo         (CTP_URL_TO);
        ctpConfig->setCtpCmdTimeout (CTP_CMD_TIMEOUT);
        ctpConfig->setCtpConnTimeout(CTP_CONN_TIMEOUT);
        ctpConfig->setCtpPort       (CTP_PORT);
        ctpConfig->setCtpReady      (CTP_READY);
        ctpConfig->setCtpRetry      (CTP_RETRY);
        ctpConfig->setMaxCtpRetry   (MAX_CTP_RETRY);
        ctpConfig->setNotifyTimeout (CTP_NOTIFY_TIMEOUT);
        ctpConfig->saveCTPConfig();
    }

/********************************** TEST CASES *********************************/

    void CTPConfigTest() {
        LOG.debug("######################### CTPConfigTest #########################");
        CPPUNIT_ASSERT(strcmp(CTP_SERVICE->getConfig()->getUsername(), TEST_USERNAME) == 0);
        CPPUNIT_ASSERT(strcmp(CTP_SERVICE->getConfig()->getPassword(), TEST_PASSWORD) == 0);
        CPPUNIT_ASSERT(strcmp(CTP_SERVICE->getConfig()->getDevID(), TEST_DEVID) == 0);
        CPPUNIT_ASSERT(strcmp(CTP_SERVICE->getConfig()->getUrlTo(), CTP_URL_TO) == 0);
    }

    void CTPConnectionTest() {
        LOG.debug("######################### CTPConnectionTest #########################");
        // Open connection with the server
        CTP_SERVICE->openConnection();
        CPPUNIT_ASSERT_EQUAL(CTP_SERVICE->getCtpState(), CTPService::CTP_STATE_CONNECTED);

        // Close connection
        CTP_SERVICE->closeConnection();
        CPPUNIT_ASSERT_EQUAL(CTP_SERVICE->getCtpState(), CTPService::CTP_STATE_DISCONNECTED);
    }

    void CTPReadyMessageTest() {
        LOG.debug("######################### CTPReadyMessageTest #########################");
        // Ensure there's an opened connection with the server
        CTP_SERVICE->openConnection();
        
        // Send ready message 
        CTP_SERVICE->sendReadyMsg();
        
        // The READY command is catched by the Socket (see FakeSocket writeBuffer method)
        //CTPASSERT_COMMAND(CM_READY);
    }

    void CTPByeMessageTest() {
        LOG.debug("######################### CTPByeMessageTest #########################");
        // Ensure there's an opened connection with the server
        CTP_SERVICE->openConnection();

        // Send bye message 
        CTP_SERVICE->sendByeMsg();
        CTPASSERT_COMMAND(CM_BYE);
    }

    void CTPAuthenticationTest() {
        LOG.debug("######################### CTPAuthenticationTest #########################");
        // Ensure there's an opened connection with the server
        CTP_SERVICE->openConnection();

        // Send auth message 
        CTP_SERVICE->sendAuthMsg();
        CTPASSERT_COMMAND(CM_AUTH);
        CTPASSERT_STATE(CTPService::CTP_STATE_WAITING_RESPONSE);

        // Wait for server response
        simulateServerResponse(ST_OK);
        CTP_SERVICE->receiveStatusMsg();
        CTPASSERT_STATE(CTPService::CTP_STATE_READY);
    }

    void CTPStartStopServiceTest() {
        LOG.debug("######################### CTPStartStopServiceTest #########################");
        CTP_SERVICE->startCTP();
        CTPASSERT_STATE(CTPService::CTP_STATE_WAITING_RESPONSE);
        CTP_SERVICE->stopCTP();
        
        //Used to kill the receiver thread
        FakeSocket::setConnectionClosed(true);
        CTPASSERT_STATE(CTPService::CTP_STATE_DISCONNECTED);
        FThread::sleep(2000);
    }

    /* Simulate auth error only one time */
    void CTPAuthenticationFailTest1() {
        LOG.debug("######################### CTPAuthenticationFailTest1 #########################");
        CTP_SERVICE->startCTP();
        CTPASSERT_COMMAND(CM_AUTH);
        CTPASSERT_STATE(CTPService::CTP_STATE_WAITING_RESPONSE);
        simulateServerResponse(ST_NOT_AUTHENTICATED);
        CTPASSERT_COMMAND(CM_AUTH);
        CTPASSERT_STATE(CTPService::CTP_STATE_WAITING_RESPONSE);
        simulateServerResponse(ST_OK);
        CTPASSERT_STATE(CTPService::CTP_STATE_READY);
        CTP_SERVICE->stopCTP();

        //Used to kill the receiver thread
        FakeSocket::setConnectionClosed(true);
        CTPASSERT_STATE(CTPService::CTP_STATE_DISCONNECTED);
        FThread::sleep(2000);
    }

    /* Simulate auth error two times */
    void CTPAuthenticationFailTest2() {
        LOG.debug("######################### CTPAuthenticationFailTest2 #########################");
        CTP_SERVICE->startCTP();
        CTPASSERT_COMMAND(CM_AUTH);
        CTPASSERT_STATE(CTPService::CTP_STATE_WAITING_RESPONSE);
        simulateServerResponse(ST_NOT_AUTHENTICATED);
        CTPASSERT_COMMAND(CM_AUTH);
        simulateServerResponse(ST_NOT_AUTHENTICATED);
        CTPASSERT_ERROR_NOTIFICATION(CTPService::CTP_ERROR_NOT_AUTHENTICATED);
        CTP_SERVICE->stopCTP();

        //Used to kill the receiver thread
        FakeSocket::setConnectionClosed(true);
        FThread::sleep(2000);
    }

    void CTPTestPush() {
        
        LOG.debug("######################### CTPTestPush #########################");
        CTP_SERVICE->startCTP();
        CTPASSERT_COMMAND(CM_AUTH);
        CTPASSERT_STATE(CTPService::CTP_STATE_WAITING_RESPONSE);
        simulateServerResponse(ST_OK);
        
        //Wait CTP_STATE_READY state after auth [OK]
        CTPASSERT_STATE(CTPService::CTP_STATE_READY);
        
        //Wait CTP_STATE_WAITING_RESPONSE for heartbeat [READY]
        int timeout = 3000;
        while(CTP_SERVICE->getCtpState() != CTPService::CTP_STATE_WAITING_RESPONSE ) {
            FThread::sleep(100); timeout -= 100;
            if(timeout < 0)  break;
        }
        
        //Wait CTP_STATE_READY state after [OK] msg
        timeout = 3000;
        while(CTP_SERVICE->getCtpState() != CTPService::CTP_STATE_READY ) {
            FThread::sleep(100); timeout -= 100;
            if(timeout < 0)  break;
        }
        simulateServerPush();
        CTPASSERT_NOTIFICATION();
        CTPASSERT_STATE(CTPService::CTP_STATE_READY);
        CTP_SERVICE->stopCTP();
        
        //Used to kill the receiver thread
        FakeSocket::setConnectionClosed(true);
        CTPASSERT_STATE(CTPService::CTP_STATE_DISCONNECTED);
        FThread::sleep(2000);
    }


/********************************* END TEST CASES *********************************/

    /** 
     * Wait for the specified command from the output socket buffer 
     * If it's n authentication command, then assert also the credentials
     */
    void assertOutputCommand(int8_t command) {
        
        // Get the output packet
        char buffer[MAX_MESSAGE_SIZE];
        int len = FakeSocket::getOutputSocketBuffer((int8_t*)buffer, MAX_MESSAGE_SIZE);
        
        CTPMessage msg; msg.parse((char*)buffer, len);
        CPPUNIT_ASSERT_EQUAL(msg.getGenericCommand(), command);

        if(command == CM_AUTH) {
            //
            // assert credentials
            //
            // Create the expected auth message
            CTPMessage authMsg = getAuthMessage();
            char* authbuffer = authMsg.toByte(); 
            int   authbuffer_len = authMsg.getBufferLength();
            CPPUNIT_ASSERT_EQUAL(len, authbuffer_len);
            for(int i=0; i<len; i++) {
                CPPUNIT_ASSERT_EQUAL(authbuffer[i], buffer[i]);
            }
        }
    }

    /** Wait for the specified ctp state to be verified */
    void assertCTPState(int state) {
        int timeout = 5000;
        while(CTP_SERVICE->getCtpState() != state ) {
            FThread::sleep(100);
            timeout -= 100;
            // check timeout
            if(timeout < 0) { 
                CPPUNIT_ASSERT(false);
                return;
            }
        }
        CPPUNIT_ASSERT(true);
    }

    /** Wait for a sync notification */
    void assertNotification() {
        int timeout = 5000;
        while(!notificationReceived) {
            FThread::sleep(100);
            timeout -= 100;
            // check timeout
            if(timeout < 0) { 
                CPPUNIT_ASSERT(false);
                return;
            }
        }
        CPPUNIT_ASSERT(true);
    }

    /** Wait for an error notification */
    void assertErrorNotification(const int error) {
        int timeout = 5000;
        while(!errorReceived) {
            FThread::sleep(100);
            timeout -= 100;
            // check timeout
            if(timeout < 0) { 
                CPPUNIT_ASSERT(false);
                return;
            }
        }
        CPPUNIT_ASSERT_EQUAL(getError(), error);
    }

    /* Simulate a server response */
    void simulateServerResponse(int8_t resp) {
        
        CTPMessage msg;
        msg.setGenericCommand(resp);
        msg.setProtocolVersion(CTP_PROTOCOL_VERSION);
        
        if(resp == ST_NOT_AUTHENTICATED) {
            // add server nonce
            CTPParam nonce;
            nonce.setParamCode(P_NONCE);
            nonce.setValue("fakenonce", 9);
            msg.addParam(&nonce);
        }
        char* msgbuffer = msg.toByte(); 
        int   msgbuffer_len = msg.getBufferLength(); 
        FakeSocket::setInputSocketBuffer(msgbuffer, msgbuffer_len);
    }

    /* Simulate a server push message */
    void simulateServerPush() {
        CTPMessage msg;
        msg.setGenericCommand(ST_SYNC);
        msg.setProtocolVersion(CTP_PROTOCOL_VERSION);
        char* msgbuffer = msg.toByte(); 
        int   msgbuffer_len = msg.getBufferLength(); 
        FakeSocket::setInputSocketBuffer(msgbuffer, msgbuffer_len);
    }

    /** PushListener notification methods */
    void onNotificationReceived(const ArrayList& serverURIList ) {
        //syncElementUri = ((StringBuffer*)serverURIList.get(0))->c_str();
        notificationReceived = true;
    }
    void onCTPError(const int errorCode, const int additionalInfo = 0) {
        error = errorCode;
        errorReceived = true;
    }

    const char* getSyncElementUri() {
        notificationReceived = false;
        return syncElementUri.c_str();
    }

    int getError() {
        errorReceived = false;
        return error;
    }

public:

    bool notificationReceived;
    bool errorReceived;

private:

    /* Format the authentication message from client defaults */
    CTPMessage getAuthMessage() {
        
        CTPMessage authMsg;
        authMsg.setGenericCommand(CM_AUTH);
        authMsg.setProtocolVersion(CTP_PROTOCOL_VERSION);

        CTPParam devId;
        devId.setParamCode(P_DEVID);
        devId.setValue(TEST_DEVID, (int32_t)strlen(TEST_DEVID));
        authMsg.addParam(&devId);

        CTPParam username;
        username.setParamCode(P_USERNAME);
        username.setValue(TEST_USERNAME, (int32_t)strlen(TEST_USERNAME));
        authMsg.addParam(&username);

        CTPParam cred;
        cred.setParamCode(P_CRED);
        StringBuffer credentials = MD5CredentialData(TEST_USERNAME, 
            TEST_PASSWORD, CTP_SERVICE->getConfig()->getCtpNonce());
        cred.setValue(credentials.c_str(), credentials.length());
        authMsg.addParam(&cred);

        return authMsg;
    }

    StringBuffer syncElementUri;
    int error;

};
    
CPPUNIT_TEST_SUITE_REGISTRATION( CTPServiceTest );

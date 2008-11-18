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

package com.funambol.push;

import com.funambol.util.Log;
import com.funambol.util.MD5;
import com.funambol.util.Base64;
import com.funambol.util.ThreadPool;

import javax.microedition.io.Connector; 
import javax.microedition.io.SocketConnection; 

import java.io.InputStream;
import java.io.OutputStream;
import java.io.DataInputStream;
import java.io.DataOutputStream;
import java.io.IOException;
import java.util.Vector;
import java.util.Timer;
import java.util.TimerTask;

//#ifdef isBlackberry
//# import com.funambol.util.BlackberryHelper;
//#endif

/**
 * This class implements the CTPService.
 * A service which opens a CTP connection and communicates with the server
 * via the CTP protocol (see CTP design document).
 * The service is executed in a separate thread, and it can be stopped at
 * any time. The service uses socket connection and therefore requires
 * network access that may result in user questions.
 * The service is implemented by two threads plus a Timer, and all threads
 * can be created within a thread pool.
 * The service shall never throw any exception (even runtime ones) and on
 * errors it is implemented to retry connecting. If too many failures occurs
 * then the service is stopped (at the moment no notification is sent).
 *
 * The class performs connection and MD5 authentication on startup and if this
 * is succesfull then the listening/heartbeat phase starts.
 * 
 * The main thread is controlled by the following FSM:
 *
  *                   open socket                 socket opened
 *    DISCONNECTED  ---------------> CONNECTING ----------------> CONNECTED
 *         ^                            | fail                       |
 *         |                            V                            |
 *         ---------------------------------------------             |
 *         |                                           |             | send MD5 
 *         |                            ok             |             | auth
 *    LISTENING <--- AUTHENTICATED  <---------> AUTHENTICATING <-----|
 *                                      fail
 */
public class CTPService extends Thread {

    /** CTP Protocol version = 1.0 */
    private static final int PROTOCOL_VERSION      = 0x10;

    /**
    * Commands
    */
    private static final int CM_AUTH               = 0x01;
    private static final int CM_READY              = 0x02;
    private static final int CM_BYE                = 0x03;
    
    /**
    * Status
    */
    private static final int ST_OK                 = 0x20;
    private static final int ST_JUMP               = 0x37;
    private static final int ST_ERROR              = 0x50;
    private static final int ST_NOT_AUTHENTICATED  = 0x41;
    // Server auth failed, nonce param received
    private static final int ST_UNAUTHORIZED       = 0x42;
    // Server auth failed at 1st try, nonce param received
    private static final int ST_FORBIDDEN          = 0x43;
    // No Server auth, no nonce param
    private static final int ST_SYNC               = 0x29;
    private static final int ST_RETRY              = 0x53;

    /**
     * Parameters
     */
    private static final int P_DEVID               = 0x01;
    private static final int P_USERNAME            = 0x02;
    private static final int P_CRED                = 0x03;
    private static final int P_FROM                = 0x04;
    private static final int P_TO                  = 0x05;
    private static final int P_NONCE               = 0x06;
    private static final int P_SAN                 = 0x07;   
    private static final int P_SLEEP               = 0x09;   


    private static final int MAX_MESSAGE_SIZE      = 4096;
   


    /**
     * CTP server thread state
     */
    private static final int DISCONNECTED    = 0;
    private static final int CONNECTING      = 1;
    private static final int CONNECTED       = 2;
    private static final int AUTHENTICATING  = 3;
    private static final int AUTHENTICATED   = 4;
    private static final int LISTENING       = 5;

    /** Specifies if the CTP service should terminate */
    private boolean done                     = false;

    /** Thread pool to be used to create new threads.
     *  Can be null if no pool is to be used.
     */
    private ThreadPool threadPool            = null;

    /** Socket connection, with input and output stream */
    private SocketConnection sc              = null;
    private OutputStream os                  = null;
    private InputStream is                   = null;

    /** CTP push notification listener */
    private PushNotificationListener pushListener = null;

    /** Specifies if the server sent an OK status to our last command */
    private boolean okReceived               = false;

    /** Push configuration */
    private PushConfig config                = null;

    /** CTPService status */
    private int state                        = DISCONNECTED;

    /** Timer used to monitor connection timeouts */
    private Timer timer                      = new Timer();

    private HeartbeatGenerator heartbeatGenerator = null;

    /**
     * This class is used (with a Timer) to monitor a connection and
     * interrupt it if it hangs for more than command timeout.
     * For each IO operation to be monitored, one such object must be created.
     * The client is responsible for notifying when the operation is terminated.
     * If by the time the alarm is triggered, the operation is not terminated,
     * then such an operation is considered timeout and closeConnection is
     * invoked. This will cause exceptions in any hanging read/write, allowing
     * each thread to resume execution.
     */
    private class ConnectionTimer extends TimerTask {

        /** IO timeout (timer delay) */
        private int delay  = -1;

        /** Specifies whether the IO operation has actually terminated */
        private boolean terminated = false;

        /** Constructor. The delay is specified in the Configuration */
        public ConnectionTimer() {

            delay = config.getCtpCmdTimeout() * 1000;
        }

        /** Notifies the ConnectionTimer that the IO operation has terminated.
         * Whenever the alarm will be triggered it won't cause a timeout because
         * the operation is finished.
         **/
        public void endOperation() {
            terminated = true;
        }

        /** Returns the delay for this task */
        public int getDelay() {
            return delay;
        }

        /** This method is invoked when the alarm expires.
         * If the operation this task is monitoring has not finished yet, then
         * we force the entire connection to shut down. This will cause
         * exceptions for all the pending read/write operations
         **/
        public void run() {
            // We were monitoring an operation whose idx is nextTimed
            // check if it terminated
            if (terminated == false) {
                // The operation did not terminate
                // We force an exception to wake up the thread
                Log.error("[CTPService] An IO operation did not complete before"
                        + " maximum allowed time. Restart the CTPService");
                disconnect();
            }
        }
    }

    /**
     * This class implements an hearbeat generator. A thread which is in charge
     * of generating messages for the CTP server to signal that the client is
     * alive. The interval between successive messages is specified in the
     * configuration.
     * The thread sends a message and wait for an answer. The send operation is
     * monitored for timeout. The answer is caught by the main listener thread.
     * The heartbeat generator only checks whether an ok has been received. In
     * this case it consider everything is OK. This is a trick as we do not
     * really monitor the reading with a timeout. We rather expects that the OK
     * arrives before we have to generate a new READY message. This way we
     * simplify the implementation and the behavior seems still reasonable.
     */
    private class HeartbeatGenerator extends Thread {

        /** Indicates if the heart is beating (alive) */
        private boolean isRunning = false;

        /** This is the heart beat main loop. It generates a beat every ready
         *  period of time (as specified in the PushConfig.
         *  The task runs until the termination flag (done) gets set.
         *  If at the moment of generating a new beat, the previous has not
         *  received an OK status, then a ctp restart is forced by closing the
         *  connection.
         **/
        public void run() {

            Log.debug("[CTPService] starting heartbeat generator");

            isRunning = true;

            // Load the sleep interval (ctpReady)
            int sleepTime = config.getCtpReady();

            // Prepare the CTP message
            CTPMessage readyMsg = new CTPMessage();
            readyMsg.setCommand(CM_READY);

            // Send 'ready' message to Server and sleep ctpReady seconds
            try {
                while (!done) {
                    okReceived = false;
                    sendMessage(readyMsg);
                    Log.debug("[CTPService] next ready msg will be sent in " + sleepTime
                              + " seconds...");
                    sleepSecs(sleepTime);
                    if (!okReceived) {
                        // We consider this as a timeout
                        throw new IOException("OK not received");
                    }
                }
            } catch (IOException ioe) {
                Log.error("[CTPService] error sending the heartbeat");
                disconnect();
                return;
            } finally {
                Log.debug("[CTPService] exiting heartbeat generator");
                isRunning = false;
            }
        }

        /** Returns true if the heart is beating. */
        public boolean isRunning() {
            return isRunning;
        }
    }

    /**
     * This class represents a CTP message. Messages format is described in the
     * CTP design document.
     * This class purpose is to allow users to easily create a CTP message,
     * setting all its properties and then to generate the byte sequence that
     * represent such a message. The second goal of the class is to allow the
     * opposite transformation. A byte stream received from the server can be
     * parsed and translated into a CTPMessage object.
     */
    private class CTPMessage {

        /** Parameters. Codes and values (must be kept in sync) */
        private Vector paramsValue = new Vector();
        private Vector paramsCode  = new Vector();

        /** protocol version */
        private int    protocolVersion = PROTOCOL_VERSION;

        /** Total message length */
        private int    length          = -1;

        /** command or status code*/
        private int    commandCode     = -1;

        /** Last received nonce. This is just a parameter but it is cached in
         * this field for performance reasons
         **/
        private byte[] nonce           = null;

        /** Build an empty message */
        public CTPMessage() {
        }

        /**
         * Build a message decoding the given byte stream
         * @param rawMessage is the byte stream representing the CTP message */
        public CTPMessage(byte[] rawMessage) {
            parsePacket(rawMessage);
        }

        /**
         * Sets the command or status code
         * @param the new code
         * */
        public void setCommand(int commandCode) {
            this.commandCode = commandCode;
        }

        /** Returns the command or status code */
        public int getCommand() {
            return commandCode;
        }

        /**
         * Add one parameter
         * @param code parameter code
         * @param value parameter value
         **/
        public void addParameter(int code, byte[] value) {

            paramsCode.addElement(new Integer(code));
            paramsValue.addElement(value);
        }

        /**
         * Returns the number of parameters.
         */
        public int getParametersNumber() {
 
            return paramsCode.size();
        }

        /**
         * Get the parameter code of a given parameter.
         * An exception maybe thrown if the index is out of bounds.
         * @param idx parameter index
         * @return the parameter code
         */
        public int getParameterCode(int idx) {

            Integer code = (Integer)paramsCode.elementAt(idx);
            return code.intValue();
        }

        /**
         * Get the parameter value of a given parameter.
         * An exception maybe thrown if the index is out of bounds.
         */
        public byte[] getParameterValue(int idx) {

            byte[] value = (byte[])paramsValue.elementAt(idx);
            return value;
        }


        /**
         * Get a byte representation of this message. This byte representation
         * is conformant to the CTP protocol specification (see the CTP design
         * document).
         *
         * @return a byte array representing this message in CTP protocol format
         */
        public byte[] getBytes() {
            // Compute the total length (header + parameters size)
            int msgLength = 4; // header size (Protocol version, command)
            int numParams = paramsValue.size();
            for(int i=0;i<numParams;++i) {
                byte[] param = (byte[]) paramsValue.elementAt(i);
                msgLength += (1 + 1 + param.length); // param-type + param-length + value
            }
            int idx = 0;
            byte[] bytes = new byte[msgLength];
            // Msg length (does not include this field)
            bytes[idx++] = (byte)((msgLength-2) >> 8);
            bytes[idx++] = (byte)((msgLength-2) & 0xFF);
            // Protocol version
            bytes[idx++] = (byte)PROTOCOL_VERSION;
            // Command
            bytes[idx++] = (byte)commandCode;
            // Parameters
            for(int i=0;i<numParams;++i) {
                Integer codeInt = (Integer) paramsCode.elementAt(i);
                byte   code   = codeInt.byteValue();
                byte[] param  = (byte[]) paramsValue.elementAt(i);
                byte   length = (byte)param.length;

                bytes[idx++] = code;
                bytes[idx++] = length;
                System.arraycopy(param, 0, bytes, idx, param.length);
                idx += param.length;
            }

            return bytes;
        }


        /** 
         * Return the last nonce that has been parsed.
         * This method is not generic, it does not return the nonce for any CTP
         * message. But only for messages that have been created from a byte
         * stream and which contains a nonce parameter.
         *
         * @return the nonce value or null if not defined
         */
        public byte[] getNonce() {
            return nonce;
        }

        // All the methods below are the implementation of a recursive parser in
        // charge of parsing an incoming message. The message (byte stream) is
        // translated into a CTPMessage object.
        // 
        // CTP message grammar:
        //
        // PACKET  -> MSGLEN MSG
        // MSG     -> VERSION COMSTAT
        // MSGLEN  -> short
        // VERSION -> byte
        // COMMAND -> CODE PARAM
        // CODE    -> byte
        // PARAM   -> CODE SLEN VALUE
        // PARAM   -> eps
        // SLEN    -> byte
        // VALUE   -> byte VALUE
        // VALUE   -> eps

        private void parsePacket(byte[] rawMessage) {
            // Length is big endian
            length = ((int)rawMessage[0]) * 10 + (int)rawMessage[1];
            parseMessage(rawMessage, 2);
        }

        private int parseMessage(byte[] rawMessage, int offset) {
            protocolVersion = (int)rawMessage[offset];
            offset = parseCommand(rawMessage, offset + 1);
            return offset;
        }

        private int parseCommand(byte[] rawMessage, int offset) {
            commandCode = (int)rawMessage[offset];
            offset = parseParam(rawMessage, offset + 1);
            return offset;
        }

        private int parseParam(byte[] rawMessage, int offset) {

            int i;

            // Handle the stream end
            if (offset >= rawMessage.length) {
                return offset;
            }

            // Ok, we have another param to consume
            int code = (int)rawMessage[offset];
            int len  = (int)rawMessage[offset+1];

            paramsCode.addElement(new Integer(code));
            byte[] value = new byte[len];
            System.arraycopy(rawMessage, offset+2, value, 0, len);
            paramsValue.addElement(value);

            // For convenience we save the nonce which needs to be retrieved
            // very often
            if (code == P_NONCE) {
                nonce = value;
            }

            offset = parseParam(rawMessage, offset + 2 + len);

            return offset;
        }

    }

    /** Builds a service.
     *
     * @param threadPool thread pool for created threads
     * @param pushListener listener that is notified of relevant push events
     * @param config the global push configuration
     */
    public CTPService(ThreadPool threadPool,
                      PushNotificationListener pushListener,
                      PushConfig config) {

        this.threadPool   = threadPool;
        this.pushListener = pushListener;
        this.config       = config;
    }

    /** Builds a service (threads are created in the global java pool)
     *
     * @param pushListener listener that is notified of relevant push events
     * @param config the global push configuration
     */
    public CTPService(PushNotificationListener pushListener,
                      PushConfig config) {

        this.pushListener = pushListener;
        this.config       = config;
    }

    /**
     * Start the complete CTP service. This includes the listener and the
     * heartbeat generator. Since this method fires a thread, the actual
     * startService implementation is in the run method.
     */
    public void startService() {

        Log.debug("[CTPService]: Starting CTPService in a thread pool");
        done = false;
        if (threadPool != null) {
            threadPool.startThread(this);
        } else {
            this.start();
        }
    }

    /**
     * Set the push events listener.
     *
     * @param pushListener the listener or null to remove it.
     */
    public void setPushNotificationListener(PushNotificationListener pushListener) {
        this.pushListener = pushListener;
    }

    /**
     * Stops the service.
     * If a connection is active the server is notified by sending a bye
     * command. If IO operations are pending, they are terminated by closing the
     * socket connection. This will result in exceptions that will resume the
     * threads. They all will stop as the "done" flag is set to true.
     */
    public void stopService() {

        Log.debug("[CTPService] asked to stop");

        // This will gently stop the threads of the CTP service
        done = true;

        try {
            if (state >= CONNECTED) {
                // Send BYE command
                CTPMessage byeMessage = new CTPMessage();
                byeMessage.setCommand(CM_BYE);
                sendMessage(byeMessage);
            }
            //CTPMessage response = receiveMessageWithTimeout();
            // Cancel any pending timer (so the timer thread can be stopped)
            timer.cancel();
            // If we have any IO operation pending we must awake the the
            // corresponding thread. We do this by closing the connection which will
            // cause some exceptions.
            disconnect();
        } catch (IOException e) {
            Log.error("[CTPService] Send of BYE command failed");
        }
    }


    /**
     * This is the thread entry point. This method implements the real service
     * startup sequence. Such a sequence is described in the CTP design
     * document.
     * Beside performing the service startup, this method activates the
     * heartbeat generator and if authentication is OK it invokes the listen
     * method that waits for server messages.
     */
    public void run() {

        Log.debug("[CTPService]: Starting CTPService thread");
        state = DISCONNECTED;

        int ctpRetry = config.getCtpRetry();
        int connectionTentatives = 0;
        done = false;

        // TODO FIXME: this is temporary. At the moment we do not store the
        // nonce across sessions. Therefore the first authentication attempt
        // always fails. We do not want to wait between the first and second
        // attempt, thus we need to track the fact this is the first attempt
        boolean first = true;

        while (!done) {

            try {
                connectionTentatives++;
                Log.debug("[CTPService]: Attempting Connection # " + connectionTentatives);
                if (state != CONNECTED) {
                    connect();
                }
                if (state == CONNECTED) {
                    // Authenticate
                    int authStatus = authenticate();

                    if ((authStatus == ST_UNAUTHORIZED) ||
                        (authStatus == ST_FORBIDDEN)) {
                        // No point in trying again
                        done = true;
                    } else if (authStatus == ST_OK) {
                        // Reset the ctpRetry time
                        ctpRetry = config.getCtpRetry();

                        // Start the heartbeat generator
                        if (heartbeatGenerator == null) {
                            heartbeatGenerator = new HeartbeatGenerator();
                        }
                        if (!heartbeatGenerator.isRunning()) {
                            if (threadPool != null) {
                                Log.debug("[CTPService] running heartbeat generator in a pool thread");
                                threadPool.startThread(heartbeatGenerator);
                            } else {
                                Log.debug("[CTPService] running heartbeat generator in a thread");
                                heartbeatGenerator.start();
                            }
                        }
                        // Start the responses listener
                        listenCTPMessages();
                    }
                }
            } catch (Throwable e) {

                // If we are done, then this is not a real error, just forced
                // the connection to go down
                if (!done) {
                    Log.error("[CTPService] Exception reading stream" + e.toString());
                    e.printStackTrace();
                }
            }

            if (!done && !first) {
                Log.debug("[CTPService] Suspending for " + ctpRetry + " seconds");
                sleepSecs(ctpRetry);

                // The retry time is doubled at each tentative to
                // minimize the impact of failures. We keep increasing
                // up to the configured limit. On success the ctpRetry
                // is reinitialized.
                if (ctpRetry * 2 < config.getCtpMaxRetry()) {
                    ctpRetry *= 2;
                }
            }
            first = false;
        }
    }

    //////////////////////////////////////////////// Private methods

    /**
     * Close the connection, forcing exceptions if there are pending network IO
     * operations.
     */
    private void closeConnection() {
        try {
            if (os != null) {
                Log.debug("[CTPService] Closing output stream");
                os.close();
            }
            if (is != null) {
                Log.debug("[CTPService] Closing input stream");
                is.close();
            }
            if (sc != null) {
                Log.debug("[CTPService] Closing socket connection");
                sc.close();
            }
        } catch (IOException e) {
            Log.error("[CTPService] Cannot force socket closure");
        } finally {
            os = null;
            is = null;
            sc = null;
        }
    }

    /**
     * Sleeps the given number of seconds. If the sleep gets interrupted, the
     * method simply returns and does not wait till the sleeping time is really
     * elapsed.
     *
     * @param secs number of seconds
     */
    private void sleepSecs(int secs) {
        try {
            Thread.sleep(secs * 1000);
        } catch (InterruptedException e) {
            // Ignore it
        }
    }

    /**
     * Perform the CTP connecting phase. The main purpose of the connecting
     * phase is to open the socket and the IO streams. The server addres is
     * retrived from the push configuration.
     */
    private void connect() throws IOException {

        String uri = "socket://" + config.getCtpServer() + ":" + config.getCtpPort();
        Log.debug("[CTPService]: connecting to " + uri);

        // Start connecting
        state = CONNECTING;

        try {
            //#ifdef isBlackberry
            //# Log.info( "Using TCP Options: " + BlackberryHelper.getTcpOptions() );
            //# sc = (SocketConnection) Connector.open( uri + BlackberryHelper.getTcpOptions() );
            //#else
            sc = (SocketConnection) Connector.open(uri);
            //#endif
            sc.setSocketOption(SocketConnection.LINGER, 5);

            os = sc.openOutputStream();
            is = sc.openInputStream();

            state = CONNECTED;
            Log.debug("[CTPService]: Connected");
        } catch (IOException ioe) {
            Log.error("CTPService]: Cannot open CTP connection to: " + uri);
            state = DISCONNECTED;
            throw ioe;
        }
    }

    /**
     * Disconnect from server. This is not the CTP protocol disconnection, as we
     * do not send the BYE command. It is rather a socket disconnect where we
     * close the socket and its streams.
     */
    private void disconnect() {

        state = DISCONNECTED;
        closeConnection();
    }

    /**
     * This method is for debugging purpose only. It prints a byte array into a
     * string. Each byte is dumped as hex.
     */
    private String byteArrayToString(byte[] array) {
        StringBuffer res = new StringBuffer();
        for (int i=0;i<array.length;++i) {
            String hexString = Integer.toHexString(array[i] & 0xFF);
            res.append(hexString + " ");
        }
        return res.toString();
    }

    /**
     * This method creates the authentication message. The credentials are
     * grabbed from the push config and the message is encrypted using the MD5
     * authentication algorithm.
     * The authentication information is packed into a valid CTP message which
     * is returned.
     *
     * @return the CTP message to be used for authentication.
     */
    private CTPMessage createAuthMessage() {

        CTPMessage authMessage = new CTPMessage();
        authMessage.setCommand(CM_AUTH);

        String username = config.getCtpUsername();
        String password = config.getCtpPassword();
        byte[] nonce    = config.getCtpNonce();

        Log.debug("[CTPService]: create credentials for " + username + ","
                  + password);
        Log.debug("[CTPService]: nonce is " + new String(Base64.encode(nonce))
                  + " ---- " + byteArrayToString(nonce));

        authMessage.addParameter(P_DEVID,    config.getDeviceId().getBytes());
        authMessage.addParameter(P_USERNAME, username.getBytes());

        MD5 md5         = new MD5();
        byte[] credentials = md5.computeMD5Credentials(username, password, nonce);

        Log.debug("[CTPService]: credentials " + new String(Base64.encode(credentials))
                  + " ---- " + byteArrayToString(credentials));

        authMessage.addParameter(P_CRED, credentials);

        // TODO handle the FROM here
        return authMessage;
    }

    /**
     * This method performs the CTP authentication. The authentication process
     * is described in the CTP design document. Basically we build an
     * authentication message using the last nonce saved in the configuration.
     * If the server authenticates us, then the method terminates, otherwise it
     * grabs the new nonce and retry authentication. The server may responds in
     * several different ways to the authentication request. Depending on such a
     * response we decide if re-authenticating or aborting the authentication
     * process.
     *
     * @return the last authentication status sent by the server.
     */
    private int authenticate() throws IOException {

        Log.debug("[CTPService] Start CTP authentication");

        state = AUTHENTICATING;

        CTPMessage authMessage = createAuthMessage();
        Log.debug("[CTPService] Sending CTP authentication message");
        sendMessage(authMessage);
        Log.debug("[CTPService] Waiting CTP response");
        CTPMessage response = receiveMessageWithTimeout();

        int authStatus = response.getCommand();
        switch (authStatus) {

            case ST_NOT_AUTHENTICATED:
                //
                // Retry with new nonce received
                //
                Log.debug("[CTPService] Client not authenticated: retry with new nonce");
                config.setCtpNonce(response.getNonce());

                // Send 2nd auth msg
                Log.debug("[CTPService] Re-Sending CTP authentication message");
                authMessage = createAuthMessage();
                sendMessage(authMessage);
                
                // Check 2nd status received, only OK allowed
                Log.debug("[CTPService] Waiting CTP response");
                response = receiveMessageWithTimeout();
                authStatus = response.getCommand();
                if (authStatus == ST_OK) {
                    // *** Authentication OK! *** 
                    // Save nonce
                    Log.info("[CTPService] authentication OK!");
                    config.setCtpNonce(response.getNonce());
                    state = AUTHENTICATED;
                }
                else {
                    Log.info("CTP error: Client not authenticated. Please check your credentials.");
                }
                break;

            case ST_OK:
                // *** Authentication OK! *** 
                Log.info("CTP client authenticated successfully!");
                // Save nonce if any
                config.setCtpNonce(response.getNonce());
                state = AUTHENTICATED;
                break;

            // --- note: JUMP not implemented Server side ----
            case ST_JUMP:
                //
                // Jump to desired server 'to' and save the 'from' value
                //
                Log.error("Server requested a JUMP. Not supported yet.");
                break;

            case ST_UNAUTHORIZED:
                // Not authorized -> save nonce if any, exit thread
                Log.info("Unauthorized by the Server, please check your credentials.");
                config.setCtpNonce(response.getNonce());
                break;

            case ST_FORBIDDEN:
                // Authentication forbidden -> exit thread
                Log.info("Authentication forbidden by the Server, please check your credentials.");
                break;

            case ST_ERROR:
                // Error -> restore connection
                Log.info("Received ERROR status from Server: restore ctp connection");
                break;

            default:
                // Unexpected status -> restore connection
                Log.error("Unexpected status received " + authStatus);
        }
        return authStatus;
    }

    /**
     * Sends a message to the server and monitor the operation via a timer. If
     * the operation does not terminate after the timeout, then the
     * ConnectionTimer will reset the connection, causing the method to trap an
     * exception that will be re-thrown.
     *
     * @param message the CTP message to be sent
     * @throws IOException if the socket writing fails
     */
    private void sendMessage(CTPMessage message) throws IOException {
        byte[] msgBytes = message.getBytes();
        Log.debug("[CTPService]: Sending message " + byteArrayToString(msgBytes));
        // Set the timer
        ConnectionTimer connTimer = new ConnectionTimer();
        Log.debug("[CTPService] Programming alarm in " + connTimer.getDelay() + " msec");
        timer.schedule(connTimer, connTimer.getDelay());
        // Perform the I/O operation
        try {
            os.write(msgBytes);
            os.flush();
            connTimer.endOperation();
        } catch (IOException e) {
            connTimer.endOperation();
            throw e;
        }
    }

    /**
     * Receives a message from the server. The read operation is guarded by a
     * timeout. If nothing is received within the timeout then ConnectionTimer
     * will reset the connection,  causing the method to trap an
     * exception that will be re-thrown.
     *
     * @return the CTP message received
     * @throws IOException if the socket writing fails
     */
    private CTPMessage receiveMessageWithTimeout() throws IOException {
        ConnectionTimer connTimer = new ConnectionTimer();
        Log.debug("[CTPService] Programming alarm in " + connTimer.getDelay() + " msec");
        timer.schedule(connTimer, connTimer.getDelay());
        try {
            CTPMessage res = receiveMessage();
            connTimer.endOperation();
            return res;
        } catch (IOException e) {
            connTimer.endOperation();
            throw e;
        }
    }

    /**
     * Receives a message from the server (wait till a message is received or an
     * error is encountered).
     *
     * @return the CTP message received
     * @throws IOException if the socket writing fails
     */
    private CTPMessage receiveMessage() throws IOException {

        byte[] sizeBytes = new byte[2];
        int bytesRead = is.read(sizeBytes, 0, 2);
        if (bytesRead != 2) {
            Log.debug("[CTPService]: Read " + bytesRead + " bytes");
            throw new IOException("Cannot read message size");
        } else {
            int size = (int)sizeBytes[0] * 10 + (int)sizeBytes[1];

            // The protocol specifies the max message length. If the received
            // message exceeds it, then we throw an exception
            if (size + 2 > MAX_MESSAGE_SIZE) {
                // This is not really an IOException, but we do not want to
                // introduce too many exceptions as each class takes some space
                // in the jar. And anyway the handling is the same.
                throw new IOException("Message length exceeds MAX_MESSAGE_SIZE");
            }

            byte[] rawMessage = new byte[2 + size];
            bytesRead = is.read(rawMessage, 2, size);
            if (bytesRead != size) {
                throw new IOException("Cannot read message content");
            } else {
                rawMessage[0] = sizeBytes[0];
                rawMessage[1] = sizeBytes[1];
                CTPMessage message = new CTPMessage(rawMessage);
                Log.debug("[CTPService]: Received message " + byteArrayToString(rawMessage));
                return message;
            }
        }
    }

    /**
     * This method waits for messages from the server, till the service is
     * active. It aborts on error throwing an IOException, or if the server
     * communicates an error.
     * If the received message is a SYNC, then the listener (if any) is
     * notified with the proper SAN message.
     *
     * @throws IOException if there is an error during stream reading
     *
     */
    private void listenCTPMessages() throws IOException {

        state = LISTENING;
        Log.debug("[CTPService]: listening messages mode");

        SANMessageParser smp = new SANMessageParser();

        while (!done) {
            CTPMessage message = null;
            try {
                message = receiveMessage();
            } catch (IOException ioe) {
                Log.error("[CTPService] Exception while reading server message");
                throw ioe;
            }
            int status = message.getCommand();

            switch (status) {

                case ST_OK:
                    // 'OK' to our 'READY' command -> back to recv
                    okReceived = true;
                    Log.debug("[CTPService] OK received");
                    break;

                case ST_SYNC:
                    //
                    // Start the sync!
                    // ---------------
                    Log.debug("[CTPService] notification received.");
                
                    // We could assert: number of params == 1, first param
                    // code is SAN. If something is wrong we go to the
                    // exception code, because the handling is the same
                    if (pushListener != null) {
                        byte[] sanBytes = message.getParameterValue(0);
                        try {
                            SANMessage sanMessage = smp.parseMessage(sanBytes,
                                                                     false);
                            Log.debug("[CTPService] handle SAN message");
                            pushListener.handleMessage(sanMessage);
                        } catch (MessageParserException mpe) {
                            Log.error("[CTPService] error parsing SAN message");
                            Log.error("[CTPService] sync cannot start");
                        }
                    }
                    // Back to recv
                    Log.debug("[CTPService] Sync started");
                    break;

                case ST_ERROR:
                    Log.error("[CTPService] ERROR message received");
                    disconnect();
                    return;
                default:
                    // Error from server -> exit thread (will try restoring the socket from scratch)
                    Log.error("[CTPService] Bad status received " + status);
                    disconnect();
                    return;
            }
        }
    }
}


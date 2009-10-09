/*
 * Funambol is a mobile platform developed by Funambol, Inc. 
 * Copyright (C) 2003 - 2009 Funambol, Inc.
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
package com.funambol.client.ipc.rpc;

import com.funambol.util.ConnectionManager;

import java.io.IOException;
import java.io.EOFException;

import javax.microedition.io.SocketConnection;

import com.funambol.util.Log;
import java.io.InputStream;
import java.io.OutputStream;

/**
 * This class manages the Inter-Process-Communication between the Email Client 
 * and other application on the system.
 */
class RPCServer implements Runnable {

    /** Definition of Disconnected status */
    protected static final int DISCONNECTED = 0;
    /** Definition of Connected status */
    protected static final int CONNECTED = 1;
    /** Definition of Listening status */
    protected int LISTENING = 2;
    /** The system socket connection port*/
    public static final int SYSTEM_SOCKET_PORT = 50005;
    /** The system socket connection port*/
    public static final String SYSTEM_SOCKET_SERVER = "127.0.0.1";

    /** System Socket connection used for IPC*/
    private SocketConnection sc = null;
    /** OutputStream for the connection*/
    protected OutputStream os = null;
    /** InputStream for the connection*/
    protected InputStream is = null;
    /** Service Status */
    protected int status = DISCONNECTED;

    /** The listener for this class instance*/
    protected RPCServerListener listener = null;

    /** Boolean to handle the message receiver engine */
    protected boolean done = false;

    /** The service port */
    private int serviceRemotePort = SYSTEM_SOCKET_PORT;

    /** The service remote host */
    private String serviceRemoteHost = SYSTEM_SOCKET_SERVER;

    /**
     * Carriage return + Line feed (0x0D 0x0A). In the RFC 2822 this is used as
     * line separator between different headers with relative values
     */
    private static final String EOL = "\r\n";

    private static final String CONTENT_LENGTH = "Content-Length";

    public RPCServer() {
    }

    public RPCServer(String host, int port) {
        serviceRemoteHost = host;
        serviceRemotePort = port;
    }

    /**
     * The run method for this thread
     */
    public void run() {

        do {
            try {
                Log.info("[RPCServer.run]Connecting...");
                connect();
                Log.info("[RPCServer.run]Start Listening");
                listenMessage();
            } catch (IOException ioe) {
                Log.error("IOException while receiving RPC data " + ioe.toString());
                stopService();
                return;
            } catch (SecurityException se) {
                Log.error("SecurityException while receiving RPC data " + se.toString());
                stopService();
                return;
            } catch (Exception e) {
                Log.error("Error in RPCServer, keep listening " + e.toString());
                // On any other error we restart the service
            }
        } while (!done);
    }

    /**
     * Start the IPC channel listening
     */
    public void startService() {
        Log.debug("[RPCServer.startService]Starting Service in a thread pool");

        Thread startService = new Thread(this);
        startService.start();
    }

    /**
     * Stop the IPC channel listening
     */
    public void stopService() {
        Log.debug("[RPCServer.stopService]Stopping Service...");
        disconnect();
        listener.serviceStopped();
    }

    /**
     * Send a message to the IPC channel
     * @param ipcmsg te message to be sent
     * @throws IOException if the message cannot be sent
     */
    public void sendMessage(String message) throws IOException {
        message = addHeaders(message);
        os.write(message.getBytes());
        os.flush();
        listener.messageSent();
        Log.debug("[RPCServer.sendIpcMessage]Message sent: " + message);
    }


    /**
     * Get the current service status
     * @return int the status of the service
     */
    public int getStatus() {
        return status;
    }

    /**
     * Get the running status
     * @return boolean the value true if the service is listening for messages,
     * false otherwise
     */
    public boolean isRunning() {
        return status == LISTENING;
    }

    /**
     * Set the Listener for the IPC service instance
     * @param ipcListener the BasicIpcListener to be set
     */

    public void setListener(RPCServerListener listener) {
        this.listener = listener;
    }

    protected void connect() throws IOException {
        //Open the connection to the System port and initializes the streams
        Log.debug("[RPCServer.connect]Opening Socket Connection");

        String url = "socket://" + serviceRemoteHost + ":" + serviceRemotePort;
        sc = (SocketConnection) ConnectionManager.getInstance().open(url);

        is = sc.openInputStream();
        os = sc.openOutputStream();

        done=false;
        status = CONNECTED;
        listener.serviceConnected();
        Log.debug("[RPCServer.connect]Connected to the local service at: "
                + sc.getAddress() + "; port: " + sc.getPort());
    }

    /**
     * Close the connection, forcing exceptions if there are pending network IO
     * operations.
     */
    protected void disconnect() {
        done=true;
        Log.trace("[RPCServer.disconnect]Disconnecting...");
        closeOutputStream();
        closeInputStream();
        closeSocketConnection();
        status = DISCONNECTED;
        listener.serviceDisconnected();
        Log.debug("[RPCServer.disconnect]Status DISCONNECTED");
    }

    private void closeOutputStream() {
        if (os != null) {
            try {
                os.close();
            } catch (IOException ioe) {
                Log.debug("[RPCServer.closeOutputStream]Exception closing the stream: " + ioe);
            } finally {
                os = null;
            }
        }
    }

    private void closeInputStream() {
        if (is != null) {
            try {
                is.close();
            } catch (IOException ioe) {
                Log.debug("[RPCServer.closeInputStream]Exception closing the stream: " + ioe);
            } finally {
                is = null;
            }
        }
    }

    private void closeSocketConnection() {
        if (sc != null) {

            try {
                sc.close();
            } catch (IOException e) {
                Log.error("[RPCServer.closeSockectConnection]Cannot force socket closure");
            } finally {
                sc = null;
            }
        } else {
            Log.debug("[RPCServer.closeSockectConnection]No need to close socket...");
        }
    }

    protected void listenMessage() throws IOException {

        status = LISTENING;
        listener.serviceListening();
        while (!done) {
            int messageLength = -1;
            int consecutiveEol = 0;
            // Now we must read until we find two EOL in a row (and discard
            // everything else)
            int ch = -1;
            StringBuffer line = new StringBuffer();
            String       currentLine = null;
            while (consecutiveEol < 2) {
                ch = is.read();
                if (ch == '\r') {
                    consecutiveEol++;
                    ch = is.read();
                    if (ch == '\n') {
                        ch = -1;
                    }
                    currentLine = line.toString();
                    line = new StringBuffer();
                } else if (ch == '\n') {
                    consecutiveEol++;
                    currentLine = line.toString();
                    line = new StringBuffer();
                } else {
                    consecutiveEol = 0;
                    line.append((char)ch);
                }

                if (currentLine != null) {
                    currentLine = currentLine.toLowerCase();

                    if (currentLine.startsWith(CONTENT_LENGTH.toLowerCase())) {
                        // line is the content length
                        int pos = currentLine.indexOf(':');
                        String size  = currentLine.substring(pos + 1).trim();
                        messageLength = Integer.parseInt(size);
                    }

                    currentLine = null;
                }
            }
            if (messageLength == -1) {
                throw new IllegalArgumentException("Missing XmlRpc content length header");
            }
            // Ok, this is the beginning of the real content. We may have read a
            // character already, looking ahead for the CR
            byte[] ret = new byte[messageLength];
            if (ch != -1) {
                ret[0] = (byte)ch;
                is.read(ret, 1, messageLength-1);
            } else {
                is.read(ret);
            }
            String content=new String(ret);
            Log.debug("[RPCServer.listenMessage] received message: "+content);
            listener.messageReceived(content);
        }
    }

    private String addHeaders(String msg){
        int msgLength = msg.length();
        StringBuffer buf = new StringBuffer(CONTENT_LENGTH);
        buf.append(": ").append(msgLength).append(EOL).append(EOL).append(msg);
        return buf.toString();
    }
}

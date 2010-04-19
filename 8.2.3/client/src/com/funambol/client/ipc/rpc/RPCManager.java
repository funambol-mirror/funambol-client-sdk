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

import java.util.Vector;
import java.util.Hashtable;
import java.io.ByteArrayInputStream;
import java.io.ByteArrayOutputStream;

import com.funambol.util.Log;

/**
 * This class represents the main entry point to access Remote Procedure Call
 * services. There are two basic functionalities:
 *
 * <UL>
 *   <LI> call a remote method
 *   <LI> publish a method that can be invoked from remote
 * </UL>
 *
 * The class provides methods that implement these functionalities. The protocol
 * used to perform the remote calls is hidden to the user. The current
 * implementation uses Xml Rpc over a socket connection.
 *
 * The following example shows how a remote method can be invoked:
 * <pre>
 * RPCManager manager = RPCManager.getInstance();
 * // Invoke TestMethod with no arguments
 * RPCParam res = manager.invoke("TestMethod", null);
 * // We expect a string in return
 * String retState = res.getStringValue();
 * </pre>
 *
 * It is also possible to publish a method that can be invoked from remote
 * parties. The following example shows an example:
 * <pre>
 *
 *  private class GetStateInfoMethod extends RPCMethod {
 *
 *      public GetStateInfoMethod() {
 *          super("getStateInfo");
 *      }
 *
 *      public RPCParam execute(RPCParam[] params) throws Exception {
 *          // We expect one parameter (the state name)
 *          if (params.length != 1) {
 *              throw new IllegalArgumentException("Expected one parameter");
 *          }
 *          RPCParam param = params[0];
 *          if (param.getType() != RPCParam.TYPE_STRING) {
 *              throw new IllegalArgumentException("Expected a string parameter");
 *          }
 *
 *          // Perform the operation
 *          String res = "No info available";
 *          RPCParam ret = new RPCParam();
 *          ret.setStringValue(res);
 *          return ret;
 *      }
 *  }
 *
 *
 *
 *  RPCManager manager = RPCManager.getInstance();
 *  GetStateInfoMethod method1 = new GetStateInfoMethod(this);
 *  manager.register(method1);
 *
 * </pre>
 * The class GetStateInfoMethod is an extension of {@link RPCMethod} and it is used
 * to represent the method <i>getStateInfo</i> which is the method being
 * published. Its method <i>execute</i> is invoked on remote invokation. The
 * list of input parameters is in the params parameter and the method can return
 * one value (or null if its type is void).
 * The rest of the example shows the mechanism in the RPCManager to register (or
 * publish) a method).
 */
public class RPCManager implements RPCServerListener {
    
    private static RPCManager instance = null;
    private XmlRpcMessage     result = null;
    private RPCServer service = null;

    private Hashtable registered = new Hashtable();
    
    private Object lock = new Object();
    private Object startLock = new Object();
    
    
    /**
     * Default private constructor
     */
    private RPCManager() {
        service = new RPCServer();
        service.setListener(this);
    }

    /**
     * Construct an instance using the given RPCserver
     */
    private RPCManager(RPCServer service) {
        this.service = service;
        service.setListener(this);
    }
    
    /**
     * Default singleton instance access method
     * @return RPCManager the only available instance of this class
     */
    public static RPCManager getInstance() {
        if (instance==null) {
            instance = new RPCManager();
        } 
        return instance;
    }

    /**
     * This method allows the user to set its own RPCServer. This is normally
     * not intended for normal uses, but it is required to mock the RPCServer in
     * unit tests.
     * @param service the RPCServer
     */
    public void setRPCServer(RPCServer service) {
        this.service = service;
        service.setListener(this);
    }

    /**
     * Invoke a remote method. The method is synchronous, it stops the calling
     * thread execution until a response is received from the remote server (or
     * an exception is thrown).
     * This method is synchronized, as it serves one request at a time.
     *
     * @param methodName the name of the method
     * @param params the list of parameters (can be null if no parameters are
     * given)
     * @return a RPCParam representing the return value (it can be null if the
     * method is void)
     * @throws RPCException if an exception occurs during the remote invokation
     */
    public RPCParam invoke(String methodName, RPCParam[] params) throws RPCException {

        if (!service.isRunning()) {
            startRPCServer();
        }

        if (!service.isRunning()) {
            throw new RPCException("Cannot start RPCServer");
        }

        XmlRpcFormatter formatter = new XmlRpcFormatter();
        XmlRpcMessage msg = new XmlRpcMessage();
        msg.setMethodCall(true);
        msg.setMethodName(methodName);
        if (params != null) {
            msg.setParams(params);
        }
        ByteArrayOutputStream os = new ByteArrayOutputStream();
        try {
            formatter.format(msg, os);
        } catch (XmlRpcFormatterException e) {
            throw new RPCException(e.toString());
        }

        // All the RPCs are serialized
        synchronized(lock) {
            result = null;
            // Now wait for the answer
            RPCParam res = null;
            try {
                // Now we can ask the server to send the message
                service.sendMessage(os.toString());
                // If we have already received a response, there is no need to
                // wait for anything
                if (result == null) {
                    lock.wait();
                }
                // Once we get here, we expect the result to have returned via the
                // listener methods
                if (result != null) {
                    Vector retValues = result.getParams();
                    // We expect one or zero values here
                    if (retValues.size() == 1) {
                        res = (RPCParam)retValues.elementAt(0);
                    } else if (retValues.size() > 1) {
                        Log.error("Multiple return values not supported");
                        res = (RPCParam)retValues.elementAt(0);
                    }
                } else {
                    throw new RPCException("Invalid or no response from the RPC server");
                }
            } catch (Exception e) {
                String emsg = "Error while waiting for RPC response: " + e.toString();
                Log.error(emsg);
                throw new RPCException(emsg);
            }
            return res;
        }
    }

    /**
     * Register a method so that is is made available externally.
     * @param method the method descriptor (@see RPCMethod)
     */
    public void register(RPCMethod method) {
        registered.put(method.getName(), method);
    }

    /**
     * Stops the service. This operation may take some time, and the call is
     * asynchronous. It may return before the service is actually stopped.
     */
    public void stop() {
        service.stopService();
    }

    /**
     * Start the service if it is not running already. The call is synchronous
     * and the method returns when the underlying service is actually running or
     * an error stopped it. After this call returns, it is possible to check if
     * the service is running by invoking isRunning().
     */
    public void start() {
        if (!service.isRunning()) {
            startRPCServer();
        }
    }
   
    /**
     * This method is called by the RPCServer on incoming messages. It is not
     * meant for the service user, but it is the implementation of the
     * RPCServerListener interface.
     * @param msg the received message
     */
    public void messageReceived(String msg) {
        synchronized (lock) {
            // The message can be the answer to our method invocation, or a new
            // request. We must parse here to make the distinction
            XmlRpcParser parser = new XmlRpcParser();
            ByteArrayInputStream is = new ByteArrayInputStream(msg.getBytes());
            try {
                XmlRpcMessage resMsg = parser.parse(is);

                if (resMsg.isMethodCall()) {
                    // This is a method call, we shall start a thread to run the
                    // invoked method
                    RPCMethod method = (RPCMethod)registered.get(resMsg.getMethodName());
                    if (method == null) {
                        Log.error("Invoked an unregistered method: " + resMsg.getMethodName());
                        // Send back a failure message response. We send it in a
                        XmlRpcMessage fault = new XmlRpcMessage();
                        fault.setMethodCall(false);
                        fault.setFault(0, "Unknown method");
                        ByteArrayOutputStream os = new ByteArrayOutputStream();
                        XmlRpcFormatter formatter = new XmlRpcFormatter();
                        try {
                            formatter.format(fault, os);
                            service.sendMessage(os.toString());
                        } catch (Exception e) {
                            Log.error("Cannot send method response back" + e.toString());
                        }
                    } else {
                        // The method was found
                        MethodInvoker invoker = new MethodInvoker(method,
                                                                  resMsg.getParams());
                        invoker.start();
                    }
                } else {
                    result = resMsg;
                }
            } catch (XmlRpcParserException e) {
                throw new RPCException(e.toString());
            } finally {
                // In any case we shall unlock the thread waiting
                lock.notify();
            }
        }
    }

    /**
     * This method is called by the RPCServer on outgoing messages that are sent. It is not
     * meant for the service user, but it is the implementation of the
     * RPCServerListener interface.
     */
    public void messageSent() {
    }

    /**
     * This method is called by the RPCServer on service connection. When the
     * connection is established, this method is called. It is part of the implementation of the
     * RPCServerListener interface.
     */
    public void serviceConnected() {
        Log.info("RPC Connected");
    }

    /**
     * This method is called by the RPCServer on service disconnection. When the
     * connection is dropped, this method is called. It is part of the implementation of the
     * RPCServerListener interface.
     */
    public void serviceDisconnected() {
        Log.info("RPC Disconnected");
        synchronized(startLock) {
            startLock.notify();
        }
    }

    /**
     * This method is called by the RPCServer when the service starts listening
     * for incoming messages (after the connection phase).
     */
    public void serviceListening() {
        Log.info("RPC Listening");
        synchronized(startLock) {
            startLock.notify();
        }
    }



    /**
     * This method is called by the RPCServer on service connection termination. When the
     * connection is established, this method is called. It is part of the implementation of the
     * RPCServerListener interface.
     */
    public void serviceStopped() {
    }
 

    private class MethodInvoker extends Thread {

        private RPCMethod method;
        private Vector    params;

        public MethodInvoker(RPCMethod method, Vector params) {
            this.method = method;
            this.params = params;
        }

        public void run() {
            try {
                RPCParam p[] = new RPCParam[params.size()];
                for(int i=0;i<params.size();++i) {
                    p[i] = (RPCParam)params.elementAt(i);
                }
                RPCParam res = method.execute(p);
                XmlRpcMessage msg = new XmlRpcMessage();
                msg.setMethodCall(false);
                msg.addParam(res);
                XmlRpcFormatter formatter = new XmlRpcFormatter();
                ByteArrayOutputStream os = new ByteArrayOutputStream();
                formatter.format(msg, os);
                // Send the result back to the caller
                try {
                    service.sendMessage(os.toString());
                } catch (Exception e) {
                    Log.error("Cannot send method response back" + e.toString());
                }
            } catch (Exception e) {
                // Send back a failure message response. We send it in a
                Log.error("Remote Method invocation generated an exception: " + e.toString());
                XmlRpcMessage fault = new XmlRpcMessage();
                fault.setMethodCall(false);
                fault.setFault(0, e.toString());
                ByteArrayOutputStream os = new ByteArrayOutputStream();
                XmlRpcFormatter formatter = new XmlRpcFormatter();
                try {
                    formatter.format(fault, os);
                    service.sendMessage(os.toString());
                } catch (Exception e1) {
                    Log.error("Cannot send method response back" + e1.toString());
                }
            }
        }
    }

   
    private void startRPCServer() {
        synchronized(startLock) {
            service.startService();
            try {
                startLock.wait();
            } catch (InterruptedException ie) {
            }
        }
    }

}

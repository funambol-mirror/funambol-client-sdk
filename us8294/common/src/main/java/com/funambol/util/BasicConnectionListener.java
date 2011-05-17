/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package com.funambol.util;


/**
 * the simplest conneection handler ever. just save the config
 * 
 */
public class BasicConnectionListener implements ConnectionListener {

    private static final String TAG_LOG = "BasicConnectionListener";

    /**
     * Check if the connection configuration is allowed
     * @param config is the configuration to be checked
     * @return true in the basic implementation because no real check is 
     * performed on the configuration permission
     */
    public boolean isConnectionConfigurationAllowed(String config) {
        if (Log.isLoggable(Log.TRACE)) {
            Log.trace(TAG_LOG, "Configuration is always allowed in this implementation");
        }
        return true;
    }

    /**
     * Notify that a connection was succesfully opened
     */
    public void connectionOpened() {
        if (Log.isLoggable(Log.TRACE)) {
            Log.trace(TAG_LOG, "Connection Opened");
        }
    }

    /**
     * Notify that a data request was succesfully written on the connection 
     * stream
     */
    public void requestWritten() {
        if (Log.isLoggable(Log.TRACE)) {
            Log.trace(TAG_LOG, "Request written");
        }
    }

    /**
     * Notify that a response was received after the request was sent
     */
    public void responseReceived() {
        if (Log.isLoggable(Log.TRACE)) {
            Log.trace(TAG_LOG, "response received");
        }
    }

    /**
     * Notify that a previously opened connection has been closed
     */
    public void connectionClosed() {
        if (Log.isLoggable(Log.TRACE)) {
            Log.trace(TAG_LOG, "Connection closed");
        }
    }

    public void connectionConfigurationChanged() {
        if (Log.isLoggable(Log.TRACE)) {
            Log.trace(TAG_LOG, "Connection Configuration changed");
        }
    }
}

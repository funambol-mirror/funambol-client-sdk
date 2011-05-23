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
package com.funambol.syncclient.spds;

import java.io.InputStream;
import java.io.IOException;
import java.io.OutputStream;

import java.net.URL;
import java.net.URLConnection;

import java.util.Properties;
import java.util.Vector;

import com.funambol.syncclient.common.logging.Logger;
import com.funambol.syncclient.spds.event.SyncTransportEvent;
import com.funambol.syncclient.spds.event.SyncTransportListener;

/**
 * Implement send / receive message by HttpConnection
 *
 * The charSet is specified with the system property
 * <code>spds.charset</code>;
 * if <code>DEFAULT</code> value, the default charSet value is taken.<br>
 * if no value, the default charSet API <code>UTF-8</vode> value is taken.
 *
 *
 * @version $Id: BaseSyncMLClient.java,v 1.3 2007-12-22 18:09:17 nichele Exp $
 *
 */
abstract class BaseSyncMLClient implements SyncMLClient {

    // --------------------------------------------------------------- Constants

    private static final String API_CHARSET         = "UTF-8"          ;
    private static final String PROP_CHARSET        = "spds.charset"   ;
    private static final String KEY_DEFAULT_CHARSET = "DEFAULT"        ;

    private static final String PROP_CONTENT_TYPE   = "Content-Type"   ;
    private static final String PROP_CONTENT_LENGTH = "Content-Length" ;

    private static final String PROP_PROXY_HOST      = "http.proxyHost";
    private static final String PROP_PROXY_PORT      = "http.proxyPort";

    // ------------------------------------------------------------ Private data

    private URL                requestURL              = null         ;
    private Logger             logger                  = new Logger() ;
    private Vector             syncTransportListeners  = null         ;
    private SyncTransportEvent syncTransportEvent      = null         ;

    // -------------------------------------------------------------- Properties

    /**
     * The charSet (encoding)
     */
    private String charSet = null;

    /**
     * Getter for property charSet.
     * @return Value of property charSet.
     */
    public String getCharSet() {
        return charSet;
    }

    /**
     * Setter for property charSet
     * @param charSet New value of property charSet.
    */
    public void setCharSet(String charSet) {
        this.charSet = charSet;
    }

    /**
     * The content-type (mime-type)
     */
    private String contentType = null;

    /**
     * Setter for property contentType
     * @param contentType
     *        New value of property contentType.
     */
    public void setContentType(String contentType) {
        this.contentType = contentType;
    }

    /**
     * The timeout (in milliseconds) for the request
     * (it defaults to 1 minute)
     */
    private int timeout = 60000;

    /**
     * Getter for property timeout.
     * @return Value of property timeout.
     */
    public int getTimeout() {
        return timeout;
    }

    /**
     * Setter for property timeout.
     * Default: 60 sec. 0 means forever
     * @param timeout New value of property timeout.
    */
    public void setTimeout(int timeout) {
        this.timeout = timeout;
    }

    /**
     * MSISDN properties
     */
    private boolean useMsisdn = false;
    private String msisdnKey = null;
    private String msisdnVal = null;
    /** Setter for property 'useMsisdn'
     * @param useMsisdn The value whether the MSISDN header must be used.
     */
    public void setUseMsisdn(boolean useMsisdn) {
        this.useMsisdn = useMsisdn;
    }
    /** Setter for property MSISDN header
     * @param msisdnKey The MSISDN header.
     */
    public void setMsisdnKey(String msisdnKey) {
        this.msisdnKey = msisdnKey;
    }
    /** Setter for property of the value of the MSISDN header.
     * @param msisdnVal The value of the MSISDN header.
     */
    public void setMsisdnVal(String msisdnVal) {
        this.msisdnVal = msisdnVal;
    }

    /**
     * The proxy server. If null no proxy are used.
     */
    private String proxyHost = null;

    /** Getter for property proxyHost.
     * @return Value of property proxyHost.
     */
    public String getProxyHost() {
        return proxyHost;
    }

    /** Setter for property proxyHost.
     * @param proxyHost New value of property proxyHost.
     */
    public void setProxyHost(String proxyHost) {
        this.proxyHost = proxyHost;
    }

    /**
     * The proxy server port.
     * It defaults to 8080.
     */
    private int proxyPort = 8080;

    /** Getter for property proxyPort.
     * @return Value of property proxyPort.
     */
    public int getProxyPort() {
        return proxyPort;
    }

    /** Setter for property proxyPort.
     * @param proxyPort New value of property proxyPort.
     */
    public void setProxyPort(int proxyPort) {
        this.proxyPort = proxyPort;
    }

    /**
     * The proxy enable / disable setting
     * It defaults to false.
     */
    private boolean useProxy = false;

    /** Getter for property useProxy.
     * @return Value of property useProxy.
     */
    public boolean getUseProxy() {
        return useProxy;
    }

    /** Setter for property userProxy.
     * @param useProxy New value of property userProxy.
     */
    public void setUseProxy(boolean useProxy) {
        this.useProxy = useProxy;
    }

    /** Setter for property url.
     * @param url New value of property url.
     */
    public void setUrl(String url) throws IOException {

        if (url == null) {
            throw new NullPointerException("requestURL parameter is null");
        }

        try {
            this.requestURL = new URL(url);
        } catch (Exception e) {
            throw new IOException (e.getMessage());
        }
    }

    private String userAgent = null;
    public String getUserAgent() {
        return userAgent;
    }
    public void setUserAgent(String userAgent) {
        this.userAgent = userAgent;
    }
    // ------------------------------------------------------------ Constructors

    public BaseSyncMLClient()  {
        syncTransportListeners = new Vector();
    }

    // ---------------------------------------------------------- Public methods

    /**
     * Register a new SyncTransportListener.
     *
     * @param listener
     */
    public void addSyncTransportListener (SyncTransportListener listener) {
        syncTransportListeners.addElement(listener);
    }

    /**
     * Remove the specified SyncTransportListener.
     *
     * @param listener
     */
    public void removeSyncTransportListener (SyncTransportListener listener) {
        syncTransportListeners.removeElement(listener);
    }

    public String toString() {
        return requestURL.toString();
    }

    // ------------------------------------------------------- Protected methods

    protected String sendMessage(byte[] bytes)
    throws IOException {

        URLConnection conn = null;
        OutputStream  os   = null;
        InputStream   is   = null;
        Properties    prop = null;

        setCharSet();

        prop = System.getProperties();

        try {

            if (this.useProxy) {
                prop.put(PROP_PROXY_HOST, proxyHost                 ) ;
                prop.put(PROP_PROXY_PORT, String.valueOf(proxyPort) ) ;
            }

            conn = requestURL.openConnection();
            conn.setDoInput (true) ;
            conn.setDoOutput(true) ;
            conn.setRequestProperty (PROP_CONTENT_TYPE,
                                     contentType                  );
            conn.setRequestProperty (PROP_CONTENT_LENGTH,
                                     String.valueOf(bytes.length) );

            if (userAgent != null) {
                conn.setRequestProperty("user-agent", userAgent);
            }

            if (useMsisdn) {
                conn.addRequestProperty(msisdnKey, msisdnVal);
            }

            if (logger.isLoggable(Logger.DEBUG)) {
                logger.debug("Connected to " + requestURL);
            }

            os = conn.getOutputStream();

            syncTransportEvent =
                new SyncTransportEvent(SyncTransportEvent.SEND_DATA_BEGIN ,
                                       bytes.length                       );
            fireSyncTransportEvent(syncTransportEvent);

            os.write(bytes); os.flush();

            syncTransportEvent =
                new SyncTransportEvent(SyncTransportEvent.SEND_DATA_END ,
                                       bytes.length                       );
            fireSyncTransportEvent(syncTransportEvent);

            if (logger.isLoggable(Logger.DEBUG)) {
                logger.debug("Message sent"        ) ;
                logger.debug("Reading response..." ) ;
            }

            is = conn.getInputStream();

            int    contentLength = conn.getContentLength () ;
            String contentType   = conn.getContentType   () ;

            syncTransportEvent =
                new SyncTransportEvent(SyncTransportEvent.RECEIVE_DATA_BEGIN ,
                                       contentLength                        );
            fireSyncTransportEvent(syncTransportEvent);

            if (logger.isLoggable(Logger.DEBUG)) {
                logger.debug(PROP_CONTENT_TYPE   +  ": " + contentType   ) ;
                logger.debug(PROP_CONTENT_LENGTH +  ": " + contentLength ) ;
            }

            if (contentType == null) {
                throw new IOException("Content type is null");
            }

            byte[] buf = new byte[contentLength];

            int c = 0 ;
            int b = 0 ;

            while ((c < buf.length) && (b = is.read(buf, c, buf.length-c)) >= 0) {
                c+=b;
                syncTransportEvent =
                    new SyncTransportEvent(SyncTransportEvent.DATA_RECEIVED ,
                                           b                                );
                fireSyncTransportEvent(syncTransportEvent);
            }

            if (logger.isLoggable(Logger.DEBUG)) {
                logger.debug("Response read") ;
            }

            syncTransportEvent =
                new SyncTransportEvent(SyncTransportEvent.RECEIVE_DATA_END ,
                           contentLength                                   );
            fireSyncTransportEvent(syncTransportEvent);

            if (!KEY_DEFAULT_CHARSET.equals(charSet)) {
                return new String(buf, charSet);
            } else {
                return new String(buf);
            }

        } catch (IOException e) {
            e.printStackTrace();
            throw e;
        } finally {

            //
            // Shut down the connection
            //

            if (is != null) {
                is.close();
                is = null ;
            }
            if (os != null) {
                os.close();
                os = null ;
            }
            if (conn != null) {
                conn = null ;
            }
        }

    }

    // --------------------------------------------------------- Private methods

    /**
     * set charSet
     */
    private void setCharSet() {
        this.charSet = System.getProperty(PROP_CHARSET, API_CHARSET);
    }

    /**
     * Fire SyncTransportEvent to syncTransportListeners.
     *
     * @param syncTransportEvent
     */
    private void fireSyncTransportEvent(SyncTransportEvent syncTransportEvent) {

        SyncTransportListener syncTransportListener = null;
        for (int i = 0, l = syncTransportListeners.size(); i < l; i++) {
            syncTransportListener =
                (SyncTransportListener)syncTransportListeners.elementAt(i);

            switch(syncTransportEvent.getType()) {
                case SyncTransportEvent.SEND_DATA_BEGIN:
                    syncTransportListener.sendDataBegin(syncTransportEvent);
                    break;
                case SyncTransportEvent.SEND_DATA_END:
                    syncTransportListener.sendDataEnd(syncTransportEvent);
                    break;
                case SyncTransportEvent.RECEIVE_DATA_BEGIN:
                    syncTransportListener.receiveDataBegin(syncTransportEvent);
                    break;
                case SyncTransportEvent.DATA_RECEIVED:
                    syncTransportListener.dataReceived(syncTransportEvent);
                    break;
                case SyncTransportEvent.RECEIVE_DATA_END:
                    syncTransportListener.receiveDataEnd(syncTransportEvent);
                    break;
            }
        }
    }

}

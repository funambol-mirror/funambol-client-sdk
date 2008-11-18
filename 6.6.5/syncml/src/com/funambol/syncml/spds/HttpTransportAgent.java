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
package com.funambol.syncml.spds;

//import com.funambol.mailclient.ui.controller.UIController;
import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.io.UnsupportedEncodingException;

import javax.microedition.io.ConnectionNotFoundException;
import javax.microedition.io.Connector;
import javax.microedition.io.HttpConnection;
import com.funambol.util.Log;
import com.funambol.util.StreamReaderFactory;

//Commented out so IDE will not remove when organizing imports
//#ifdef isBlackberry
//# import com.funambol.util.BlackberryHelper;
//#endif

/**
 *  Represents a HTTP client implementation
 **/
public final class HttpTransportAgent implements TransportAgent {
    
    // --------------------------------------------------------------- Constants
    private static final String PROP_MICROEDITION_CONFIGURATION =
            "microedition.configuration";
    private static final String PROP_CONTENT_LANGUAGE           =
            "Content-Language";
    private static final String PROP_CONTENT_LENGTH             =
            "Content-Length";
    private static final String PROP_UNCOMPR_LENGHT             =
            "Uncompressed-Content-Length";
    private static final String PROP_CONTENT_TYPE               =
            "Content-Type";
    private static final String PROP_MICROEDITION_LOCALE        =
            "microedition.locale";
    private static final String PROP_MICROEDITION_PROFILES      =
            "microedition.profiles";
    private static final String PROP_USER_AGENT                 =
            "User-Agent";
    
    // specific property to send device identity to the server
    private static final String PROP_DEVICE_AGENT               =
            "Device-Agent";
    
    // Proprietary http header to avoid bugs of Nokia S60 3ed. FP1
    // It forces the server to set a 'Set-Cookie' header not empty
    private static final String PROP_FORCE_COOKIES              =
            "x-funambol-force-cookies";
    private static final String PROP_ACCEPT_ENCODING = "Accept-Encoding";
    private static final String PROP_CONTENT_ENCODING = "Content-Encoding";
    private static final String PROP_DATE = "Date";
    private static final String PROP_SIZE_THRESHOLD =
            "Size-Threshold";
    private static final String COMPRESSION_TYPE_GZIP = "gzip";
    private static final String COMPRESSION_TYPE_ZLIB = "deflate";
    
    // ------------------------------------------------------------ Private Data
    private final String userAgent;
    private String requestURL;
    private final String charset;
    private String contentType;
    
    // Compression parameters
    private int sizeThreshold;
    private boolean enableCompression;
    
    private String responseDate;
    
    private boolean forceCookies;
    
    private int NUM_RETRY = 3;
    
    private int uncompressedLength;
    
    
    // ------------------------------------------------------------ Constructors
    
    /**
     * Initialize a new HttpTransportAgent with a URL only
     * The default userAgent and charset will be used.
     *
     * @param requestURL must be non-null.
     *
     */
    public HttpTransportAgent(String requestURL, boolean compress, boolean forceCookies) {
        this(requestURL, null, null, compress, forceCookies);
    }
    
    /**
     * Initialize a new HttpTransportAgent with a URL and a userAgent string.
     * The default charset will be used.
     *
     * @param requestURL must be non-null
     * @param userAgent a string to be used as userAgent.
     */
    public HttpTransportAgent(String requestURL, final String userAgent,
            boolean compress, boolean forceCookies) {
        this(requestURL, userAgent, null, compress, forceCookies);
    }
    
    /**
     * Initialize a new HttpTransportAgent with a URL and a charset to use.
     *
     * @param requestURL must be non-null
     * @param userAgent a string to be used as userAgent.
     * @param charset a valid charset, the device charset is used by default.
     *
     */
    public HttpTransportAgent(String requestURL,
            final String userAgent,
            final String charset, boolean compress, boolean forceCookies) {
        
        if (requestURL == null) {
            throw new NullPointerException(
                    "TransportAgent: request URL parameter is null");
        }
        this.userAgent = userAgent;
        this.requestURL = requestURL ;
        this.charset = charset ;
        this.sizeThreshold = 0;
        this.enableCompression = compress;
        this.responseDate = null;
        this.forceCookies = forceCookies;
        Log.info("HttpTransportAgent - enableCompression: " + enableCompression);
        Log.info("HttpTransportAgent - forceCookies: " + forceCookies);
    }
    
    
    // ---------------------------------------------------------- Public methods
    
    /**
     * Send a message using the default charset.
     *
     * @param requestURL must be non-null
     * @param charset a valid charset, UTF-8 is used by default.
     *
     */
    public String sendMessage(String request) throws SyncException {
        return sendMessage(request, this.charset);
    }
    
    public String sendMessage(String request, String charset)
    throws SyncException {
        byte[] indata = null;
        byte[] outdata = null;
        
        if(charset != null) {
            try {
                indata = request.getBytes(charset);
            } catch(UnsupportedEncodingException uee){
                Log.error("Charset "+charset+" not supported. Using default");
                charset = null;
                indata = request.getBytes();
            }
        } else {
            indata = request.getBytes();
        }
        
        request = null;
        outdata = sendMessage(indata);
        
        indata = null;
        
        if (outdata==null) {
            String msg = "Response data null";
            Log.error("[sendMessage] " + msg);
            //throw new SyncException(SyncException.ACCESS_ERROR, msg);
            throw new SyncException(SyncException.DATA_NULL, msg);
        } else {
            
            if(charset != null) {
                try {
                    return new String(outdata, charset);
                } catch(UnsupportedEncodingException uee){
                    Log.error("Charset "+charset+" not supported. Using default");
                    charset = null;
                    return new String(outdata);
                }
            } else {
                return new String(outdata);
            }
        }
        
        
    }
    
    /**
     *
     * @param request HTTP request
     * @return HTTP response
     */
    public byte[] sendMessage(byte[] request) throws SyncException {
        HttpConnection c  = null;
        InputStream    is = null;
        OutputStream   os = null;
        
        byte[] data = null;
        for (int i=0; i<NUM_RETRY; i++) {
            try {
                //#ifdef low_mem
//#                 System.gc();        //XXX
//#                 try { Thread.sleep(1); } catch (InterruptedException ex) {
//#                     Log.error(this, "interruptedException in sendMessage");
//#                     ex.printStackTrace();
//#                 }
                //#endif
                
                Log.info("[sendMessage] requestURL: [" + requestURL +"]");
                //#ifdef isBlackberry
                //# Log.debug("[sendMessage] Using Blackberry Options: " + BlackberryHelper.getHttpOptions());
                //# c = (HttpConnection)Connector.open(requestURL+BlackberryHelper.getHttpOptions(),Connector.READ_WRITE, true);
                //#else
                c = (HttpConnection)Connector.open(requestURL,Connector.READ_WRITE, true);
                //#endif
                
                //Request Configuration: Added ACCEPT ENCODING Parameter
                setConfig(c, request.length);
                
                writeRequest(c, request);
                
                Log.info("[sendMessage] message sent, waiting for response.");
                
                //Receive response on is Inputstream
                is = c.openInputStream();
                
                logHeaders(c);
                
                long len = c.getLength();
                Log.info("HttpResponse length: " + len);
                
                // Check http error
                int httpCode = c.getResponseCode();
                Log.info("[sendMessage] Http Code: " + httpCode);
                if (httpCode != c.HTTP_OK) {
                    //throw new SyncException(SyncException.SERVER_ERROR,
                    Log.error("Attempt n." + (i+1) +
                            " - Http error: code=[" + httpCode + "] msg=["
                            + c.getResponseMessage() + "]");
                    if (i == NUM_RETRY-1) {
                        String msg = " Http error: code=[" + httpCode + "] msg=["
                                + c.getResponseMessage() + "]";
                        throw new SyncException(SyncException.CONN_NOT_FOUND,msg );
                    }
                } else {
                    responseDate = c.getHeaderField(PROP_DATE);
                    Log.info("[sendMessage] Date from server: " + responseDate);
                    
                    contentType = c.getHeaderField(PROP_CONTENT_ENCODING);
                    Log.info("[sendMessage] Encoding Response Type from server: " + contentType);
                    
                    uncompressedLength = c.getHeaderFieldInt(PROP_UNCOMPR_LENGHT, -1);
                    Log.info("[sendMessage] Uncompressed Content Lenght: " + uncompressedLength);
                    
                    if ((len == -1) && (responseDate == null) && (contentType == null)) {
                        Log.error("Attempt n." + (i+1) +
                                " - Http error: httpCode=[" + httpCode + "] msg=["
                                + c.getResponseMessage() + "] len=[" + len + "] date=["
                                + responseDate + "] contentType=[" + contentType + "]");
                        Log.error("[sendMessage] Error in http response, not reading stream...");
                    } else {
                        data = StreamReaderFactory.getStreamReader(contentType).readStream(is, (int)len);
                        Log.debug("data.length: " + data.length);
                        
                        if ((uncompressedLength!=-1) && (data.length!=uncompressedLength)) {
                            // try again but use uncompressed mode
                            //enableCompression = false;
                            //continue;
                            
                            Log.error("[sendMessage] Error reading compressed response");
                            Log.error("[sendMessage] Trying with uncompressed.");
                            throw new CompressedSyncException("Error reading compressed response");
                            
                        }
                        
                        Log.info("[sendMessage] Stream correctly processed.");
                        /*
                        if (UIController.WAP_SIMULATION) {
                            Log.debug("##### WAP SIMULATION with compression error#######");
                            UIController.WAP_SIMULATION = false;
                            throw new CompressedSyncException("Error reading compressed response");
                        }*/
                        break;
                    }
                }
            } catch (ConnectionNotFoundException e) {
                
                String msg = "HttpTransportAgent: can't open connection -->"
                        + e.toString() ;
                os=null;
                Log.error(msg);
                if (i == NUM_RETRY-1) {
                    //throw new SyncException(SyncException.ACCESS_ERROR, msg);
                    throw new SyncException(SyncException.CONN_NOT_FOUND, msg);
                }
            } catch (IOException e) {
                Log.error("[sendMessage] Attempt n." + (i+1) + " failed. Retrying...");
                String msg = "HttpTransportAgent: connection broken --> "
                        + e.toString() ;
                Log.error(msg);
                if (i == NUM_RETRY-1) {
                    throw new SyncException(SyncException.ACCESS_ERROR, msg);
                }
            } catch (IllegalArgumentException e) {
                String msg = "HttpTransportAgent: invalid argument for connection --> "
                        + e.toString() ;
                Log.error(msg);
                //throw new SyncException(SyncException.ACCESS_ERROR, msg);
                throw new SyncException(SyncException.ILLEGAL_ARGUMENT, msg);
            } finally {
                if (is != null) {
                    try {
                        is.close();
                        is=null;
                    } catch(IOException ioe){
                        ioe.printStackTrace();
                        Log.error("sendmessage: can't close input stream.");
                    }
                }
                if (os != null) {
                    try {
                        os.close();
                        os=null;
                    } catch(IOException ioe){
                        ioe.printStackTrace();
                        Log.error("sendmessage: can't close output stream.");
                    }
                }
                if (c != null) {
                    try {
                        c.close();
                        c=null;
                    } catch(IOException ioe){
                        ioe.printStackTrace();
                        Log.error("sendmessage: can't close connection.");
                    }
                }
                
            }
        }
        return data;
    }
    
    private void writeRequest(final HttpConnection c, final byte[] request) throws IOException {
        OutputStream os = null;
        os = c.openOutputStream();
        os.write(request);
        os.close();
        Log.info("[writeRequest] Request written.");
    }
    
    // ---------------------------------------------------------- Private methods
    
    /**
     * Add request properties for the configuration, profiles,
     * and locale of this system.
     * @param c current HttpConnection to receive user agent header
     */
    private void setConfig(HttpConnection c, int length) throws IOException {
        String ua = this.userAgent;
        
        String locale = System.getProperty(PROP_MICROEDITION_LOCALE);
        
        if (ua == null) {
            // Build the default user agent
            String conf = System.getProperty(PROP_MICROEDITION_CONFIGURATION);
            String prof = System.getProperty(PROP_MICROEDITION_PROFILES);
            
            ua = "Profile/" + prof + " Configuration/" + conf ;
        }
        
        c.setRequestMethod(HttpConnection.POST);
        c.setRequestProperty("Connection", "Close");
        c.setRequestProperty(PROP_CONTENT_TYPE, "application/vnd.syncml+xml");
        
        
        c.setRequestProperty(PROP_CONTENT_LENGTH, String.valueOf(length));
        
        if (length == 0) {
            
            Log.error("content length has been set to 0 !");
            
        }
        
        // workaround to avoid errors on server
        // the client user agent must not be sent with User-Agent header
        c.setRequestProperty(PROP_USER_AGENT, ua );
        
        // we use Device-Agent for such an issue
        c.setRequestProperty(PROP_DEVICE_AGENT, createDeviceAgent() );
        
        // If Set-Cookie header is set to empty value in http
        // server response, in Nokia S60 3ed. FP1 devices the Application crashes
        // It's due to a Symbian KVM bug.
        // A specific workaround has been implemented: the client sends a specific header
        // 'x-funambol-force-cookies' to force the server to set a 'Set-Cookie' header not empty.
        if (forceCookies) {
            c.setRequestProperty(PROP_FORCE_COOKIES, "true" );
        }
        
        //Set Encoding and accepted properties: inflater or Gzip input Stream
        if (enableCompression){
            c.setRequestProperty(PROP_ACCEPT_ENCODING, COMPRESSION_TYPE_GZIP);
            Log.debug("Encoding Response Required from Client: " + COMPRESSION_TYPE_GZIP);
        }
        
        if (this.sizeThreshold != 0) {
            c.setRequestProperty(PROP_SIZE_THRESHOLD,
                    String.valueOf(this.sizeThreshold));
        }
        
        if (locale != null) {
            c.setRequestProperty(PROP_CONTENT_LANGUAGE, locale);
        }
    }
    
    public void enableCompression(boolean enable) {
        this.enableCompression = enable;
    }
    
    public void setThreshold(int threshold) {
        this.sizeThreshold = threshold;
    }
    
    public void setRequestURL(String requestURL) {
        this.requestURL = requestURL;
    }
    
    /**
     * Returns the last response date, if available, or null.
     */
    public String getResponseDate() {
        return responseDate;
    }
    
    private void logHeaders(final HttpConnection c) throws IOException {
        
        // debug section for reading http response headers
        if (Log.getLogLevel() >= Log.INFO) {
            StringBuffer sbh = new StringBuffer();
            String tmp = "";
            for (int h = 0; h<12; h++) {
                tmp = c.getHeaderFieldKey(h);
                sbh.append("[").append(h).append("] - ").
                        append(tmp);
                if ((tmp!=null)&&(!tmp.equals(""))) {
                    sbh.append(": ").append(c.getHeaderField(tmp)).append("\n");
                } else {
                    sbh.append("\n");
                }
            }
            Log.info("header: \n" + sbh.toString());
        }
    }
    
    private String createDeviceAgent() {
        StringBuffer sbAgent = new StringBuffer();
        sbAgent.append(System.getProperty("microedition.platform"));
        sbAgent.append(" ").append(System.getProperty("microedition.profiles"));
        sbAgent.append(" ").append(System.getProperty(
                "microedition.configuration"));
        return sbAgent.toString();
    }
    
}


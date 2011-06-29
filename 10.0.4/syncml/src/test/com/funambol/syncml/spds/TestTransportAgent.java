/**
 * Funambol is a mobile platform developed by Funambol, Inc.
 * Copyright (C) 2010 Funambol, Inc.
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

import java.util.Date;
import java.util.Hashtable;

import com.funambol.util.CodedException;
import com.funambol.util.TransportAgent;

public class TestTransportAgent implements TransportAgent {

    /** Cache for requests from client to server */
    private final StringBuffer outgoingStream;
    /** Cache for requests from server to client */
    private final StringBuffer incomingStream;
    private boolean cacheRequests;
    private TestMessageHandler handler;

    public TestTransportAgent(TestMessageHandler h) {
        this(h, false);
    }
    public TestTransportAgent(TestMessageHandler h, boolean cacheRequests) {
        handler = h;
        this.cacheRequests = cacheRequests;
        
        if (cacheRequests) {
            outgoingStream = new StringBuffer();
            incomingStream = new StringBuffer();
        } else {
            outgoingStream = null;
            incomingStream = null;
        }
    }
    
    
    public String sendMessage(String request, String charset) throws CodedException {
        try {
            if (cacheRequests) outgoingStream.append(request);
            String response = handler.handleMessage(request);
            if (cacheRequests) incomingStream.append(response);
            return response;
        } catch (Exception e) {
            throw new CodedException(-1, e.toString());
        }
    }

    public String sendMessage(String request) throws CodedException {
        return sendMessage(request, null);
    }

    public byte[] sendMessage(byte[] request) throws CodedException {
        try {
            if (cacheRequests) outgoingStream.append(new String(request));
            byte[] response = handler.handleMessage(request);
            if (cacheRequests) incomingStream.append(new String(response));
            return response;
        } catch (Exception e) {
            throw new CodedException(-1, e.toString());
        }
    }

    public void setRetryOnWrite(int retries) { }
    public void setRequestURL(String requestUrl) { }
    public String getResponseDate() { return new Date().toString(); }

    public void setRequestContentType(String contentType) {
    }

    public void setCustomHeaders(Hashtable headers) {
    }
    
    public String getOutgoingStream() {
        return outgoingStream != null 
            ? outgoingStream.toString()
            : null;
    }
    public void flushOutgoingStream() {
        if (null != outgoingStream) outgoingStream.delete(0, outgoingStream.length());
    }
    
    
    public String getIncomingStream() {
        return incomingStream != null
            ? incomingStream.toString()
            : null;
    }
    public void flushIncomingStream() {
        if (null != incomingStream) incomingStream.delete(0, incomingStream.length());
    }
    
}

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

package com.funambol.mail;

import com.funambol.util.StringUtil;
import java.util.Enumeration;
import java.util.Hashtable;

import com.funambol.util.Log;

/**
 * The common base class for Messages and BodyParts. It models a MIME <i>entity</i>
 * (see {@link http://www.ietf.org/rfc/rfc2045.txt}, par. 2.4) that defines header
 * attributes (beginning with "content-") and content body. An entity that is
 * also a Message has the standard non-MIME header fields whose meanings are
 * defined by RFC 2822 <p>

 */
public abstract class Part {
    
    // -------------------------------------------------------------- Constants
    
    /**
     * The MIME "Content-Type" header name
     */
    public static final String CONTENT_TYPE = "Content-Type";
    
    /**
     * The MIME "multipart" content type
     */
    public static final String MULTIPART = "multipart";
    
    /**
     * The MIME "multipart/mixed" content type
     */
    public static final String MULTIPART_MIXED = "multipart/mixed";
    
    /**
     * The MIME "multipart/mixed" content type
     */
    public static final String MULTIPART_ALTERNATIVE = "multipart/alternative";
    
    /**
     * The MIME "text/plain" content type
     */
    public static final String TEXT_PLAIN = "text/plain";

    /** RFC 2822 */
    public static final String CONTENT_TRANSFER_ENCODING =
                                        "Content-Transfer-Encoding";

    /**
     * The MIME "7bit" content transfer encoding
     */
    public static final String ENC_7BIT = "7bit";
    
    /**
     * The MIME "8bit" content transfer encoding
     */
    public static final String ENC_8BIT = "8bit";
    
    /**
     * The MIME "base64" content transfer encoding
     */
    public static final String ENC_B64 = "base64";
    
    /**
     * The MIME "quoted-printable" content transfer encoding
     */
    public static final String ENC_QP = "quoted-printable";
    
    // ------------------------------------------------------------- Attributes
    
    /**
     * The headers of this <code>Part</code>
     */
    protected Hashtable headers;
    
    /**
     * The MIME type of this <code>Part</code>
     */
    protected String contentType;
    
    
    /**
     * The size of this <code>Part</code>
     */
    protected String size;
    
    
    
    
    // ----------------------------------------------------------- Constructors
    
    /**
     * The default constructor <p>
     *
     * Used only by subclasses for common initialization.
     */
    protected Part() {
        headers = null;
        contentType = TEXT_PLAIN;
    }
    
    
    /**
     * Adds a new header to the list of headers.<p>
     *
     * NOTE: This method is not yet implemented and must not be called.<p>
     *
     * TODO: support multiple headers?
     *
     * @param name
     *            The name of the header attribute as described in the RFC 2822
     * @param value
     *            The content for the attribute
     */
    public void addHeader(String name, String value) {
        if(headers == null){
            createHeaders();
        }
        if (name != null && value != null) {
            headers.put(name, value);
        }
       
    }
    
    
    /**
     * Returns the header <code>name</code>
     *
     * @return The requested header, or <code>null</code> if not found
     */
    public String getHeader(String name) {
        if (headers == null) {
            return null;
        }
        return (String)headers.get(name);
    }
    
    
    /**
     * Returns the list of headers stored in this Message object, as an array of
     * String
     *
     * @return The list of headers <code>headers</code>
     */
    public String[] getAllHeaders() {
        String ret[] = new String[headers.size()];
        int i;
        Enumeration e;
        for (i = 0, e = headers.keys(); e.hasMoreElements(); i++) {
            String key = (String)e.nextElement();
            ret[i] = key + ": " + (String)headers.get(key);
        }
        return ret;
    }
    
    
    /**
     * Get the content of this Part.
     *
     * @return the content of this Part
     */
    public abstract Object getContent();
    
    
    /**
     * Gets the value of the Content-Type header attribute of this
     * <code>Message</code>
     *
     * @return The MIME media type defined in the RFC 2046
     */
    public String getContentType() {
        return contentType;
    }
    
    
    /**
     * Sets the content of this Part.
     */
    public abstract void setContent(Object content) throws MailException;
    
    
    /**
     * Sets the content and the Content-type of this Part.
     */
    public abstract void setContent(Object content, String type)
    throws MailException;
    
    
    /**
     * Set the MIME-type of this Part.
     *
     * @param type
     *            The value for the MIME type of this Part,
     *            without the Content-type header attributes.
     */
    public void setContentType(String type) {
        contentType = type;
    }
    
    
    /**
     * Set the value of an header, replacing the old value, if any.
     *
     * @param name
     *            The name of the header attribute as described in the RFC 2822
     * @param val
     *            The new content for the attribute
     */
    public void setHeader(String name, String val) {
        if(headers == null){
            createHeaders();
        }
        if (name == null) {
            Log.error("setHeader: invalid header");
            return;
        }
        if (val == null) {
            Log.error("setHeader: invalid header - "+name);
            return;
        }
        headers.put(name, val);
    }
    
    
    /**
     * Remove the header <code>name</code> from this message
     *
     * @param name the name of the header to be removed 
     */
    public void removeHeader(String name) {
        headers.remove(name);
    }
    
    
    /**
     * Check if the MIME type of this Part is multipart/*
     *
     * @return <code>true</code> if the media type is <code>multipart</code>
     */
    public boolean isMultipart() {
        return (contentType.startsWith("multipart/"));
    }
    
    
    /**
     * Check if the MIME type of this Part is text/*
     *
     * @return <code>true</code> if the media type is <code>text</code>
     */
    public boolean isText() {
        return (contentType.startsWith("text/"));
    }
    
    
    /**
     * Check if the MIME type of this Part is text/plain
     *
     * @return <code>true</code> if the media type is <code>text</code> and
     *         the subtype <code>plain</code>
     */
    public boolean isTextPlain() {
        return (contentType.startsWith("text/plain"));
    }
    
    
    /**
     * Check if the MIME type of this Part is text/html
     *
     * @return <code>true</code> if the media type is <code>text</code> and
     *         the subtype <code>html</code>
     */
    public boolean isTextHtml() {
        return (contentType.startsWith("text/html"));
    }
    
    public void setSize(String size){
        this.size = size;
    }
    
    public String getSize(){
        return size;
    }
    
    private void createHeaders(){
        headers = new Hashtable();
    }
    
}

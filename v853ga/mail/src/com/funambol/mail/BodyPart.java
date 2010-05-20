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

import java.io.DataInputStream;
import java.io.DataOutputStream;
import java.io.IOException;

import com.funambol.storage.Serializable;
import com.funambol.storage.ComplexSerializer;

/**
 * A BodyPart object is foreseen to be inserted into a Multipart container,
 * itself inserted in a multi-part Message with MIME Content-Type equal to
 * "multipart"
 */
public class BodyPart extends Part implements Serializable {

    // -------------------------------------------------------------- Constants

    /**
     * The name of the MIME Content-Disposition header attribute
     */
    public static final String CONTENT_DISPOSITION = "Content-Disposition";

    /**
     * A possible value for the the "Content-Disposition" header field
     */
    public static final String CD_ATTACHMENT = "attachment";

    /**
     * A possible value for the the "Content-Disposition" header field
     */
    public static final String CD_INLINE = "inline";

    private static final String CD_FILENAME = "filename";

    // ------------------------------------------------------------- Attributes

    /**
     * A Multipart can contain one or more BodyParts, and each BodyPart has a
     * reference to its container, if present
     */
    private Multipart container;

    /** The content disposition (inline/attachment) */
    private String disposition;

    /** The filename for this bodypart, if specified */
    private String filename;

    /** The filename for this bodypart, if specified */
    private String attachUrl;

    /** The content of this bodypart */
    private Object content;


    // ----------------------------------------------------------- Constructors

    public BodyPart() {
        // super(); // init Part

        // container = null;
        // disposition = INLINE;
        // filename = null;
        // content = null;
        this(null);
    }


    public BodyPart(Multipart container) {

        super(); // init Part

        this.container = container;
        disposition = CD_INLINE;
        filename = null;
        content = null;
        attachUrl = null;

    }


    // --------------------------------------------------------- Public Methods

    /**
     * Returns the value of the "Content-Disposition" header field. This
     * represents the disposition of this BodyPart. The disposition describes
     * how the part should be presented to the user.
     * 
     * @return The value of the Content-Disposition header field of this part or
     *         <code>null</code> if not present.
     */
    public String getDisposition() {
        return disposition;
    }


    /**
     * Returns the value of the filename attribute of the "Content-Disposition"
     * header field. This represents the filename for this BodyPart (suggests to
     * the receiving mail client how to name it).
     * 
     * @return The value of the filename of this BodyPart, or <code>null</code>
     *         if not present.
     */
    public String getFileName() {
        return filename;
    }
    
    /**
     * Returns the value of the attachment url 
     * This represents the attachment file url for this BodyPart 
     * 
     * @return The value of the attachment url of this BodyPart, or <code>null</code>
     *         if not present.
     */
    public String getAttachUrl() {
        return attachUrl;
    }


    /**
     * Returns the container Multipart of this BodyPart.
     * 
     * @return A reference to the Multipart object this part is contained in, if
     *         the message is a multi-part message
     */
    public Multipart getContainer() {
        return container;
    }


    /**
     * Returns the content of of this BodyPart.
     * 
     * @return A reference to the Object in this BodyPart, or null if empty.
     */
    public Object getContent() {
        return content;
    }


    /**
     * 
     * @return The textual content of this BodyPart, if it is a String, or null
     *         otherwise.
     */
    public String getTextContent() {
        if (content instanceof String) {
            return (String)content;
        } else {
            return null;
        }
    }


    /**
     * Sets the container Multipart of this BodyPart.
     * 
     * @param container
     *            A reference to the Multipart object to set as container
     */
    public void setContainer(Multipart container) {
        this.container = container;
    }


    /**
     * Sets the content of this BodyPart. It can be a Multipart or text (a
     * String) or a byte array (e.g. a jpeg attachment). 
     * 
     */
    public void setContent(Object content) throws MailException {
        if (content instanceof Multipart) {
            ((Multipart)content).setContainer(this);
            contentType = "multipart/mixed";
        } else if (content instanceof String) {
            contentType = "text/plain";
        } else if (content instanceof Message) {
            contentType = "application/eml"; 
        } else if (content instanceof byte[]) {
            contentType = "application/octet-stream";
        } else {
            throw (new MailException(
                            MailException.INVALID_CONTENT,
                            "Invalid content: " + content.getClass().getName()));
        }

        this.content = content;
    }


    /**
     * Sets the content of this BodyPart. This can be another Multipart (for
     * instance, a multipart/mixed message can contains a multipart/alternative
     * in one bodypart), String (in case of a text content) or a byte array for
     * binary data.
     * 
     * @param content
     *            The content object
     * @param type
     *            The MIME type of the content
     */
    public void setContent(Object content, String type) throws MailException {
        setContentType(type);
        setContent(content);
    }


    /**
     * Set the content disposition of this BodyPart
     * 
     * @param disp
     *            One of ATTACHMENT or INLINE
     */
    public void setDisposition(String disp) {
        disposition = disp;
    }


    /**
     * Sets the file name of this BodyPart
     * 
     * @param fn
     *            The file name of the attachment
     */
    public void setFileName(String fn) {
        filename = fn;
    }
    
     /**
     * Sets the attachment file url of this BodyPart
     * 
     * @param au
     *            The url of the attachment
     */
    public void setAttachUrl(String au) {
        attachUrl = au;
    }


    // ---------------------------------- interface Serializable implementation

    public void serialize(DataOutputStream dout) throws IOException {
        
        // Write Part attributes
        dout.writeUTF(contentType);
        ComplexSerializer.serializeHashTable(dout, headers);
        
        if (disposition != null) {
            dout.writeBoolean(true);
            dout.writeUTF(disposition);
        } else {
            dout.writeBoolean(false);
        }
            
        if (filename != null) {
            dout.writeBoolean(true);
            dout.writeUTF(filename);
        } else {
            dout.writeBoolean(false);
        }
        
        if (size != null) {
            dout.writeBoolean(true);
            dout.writeUTF(size);
        } else {
            dout.writeBoolean(false);
        }
        
         if (attachUrl != null) {
            dout.writeBoolean(true);
            dout.writeUTF(attachUrl);
        } else {
            dout.writeBoolean(false);
        }
        
        /*
         * the container is not stored: the container will call setContainer on
         * deserialize.
         */
         ComplexSerializer.serializeObject(dout, content);
    }


    public void deserialize(DataInputStream din) throws IOException {

        contentType = din.readUTF();
        headers = ComplexSerializer.deserializeHashTable(din);
        
        if (din.readBoolean())
            disposition = din.readUTF();
        if (din.readBoolean())
            filename = din.readUTF();
        
        if (din.readBoolean()){
            size = din.readUTF();
        }
        
        if (din.readBoolean()){
            attachUrl = din.readUTF();
        }
        
        content = ComplexSerializer.deserializeObject(din);
        // Set the container reference for Multipart
        if (content instanceof Multipart) {
            ((Multipart)content).setContainer(this);
        }

    }
    
}

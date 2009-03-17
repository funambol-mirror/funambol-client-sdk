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

import java.util.Vector;
import java.util.Enumeration;
import java.io.DataInputStream;
import java.io.DataOutputStream;
import java.io.IOException;

import com.funambol.storage.Serializable;
import com.funambol.storage.ComplexSerializer;

/**
 * Represents a container for <code>BodyPart</code>s in multi-part
 * <code>Message</code>s as per RFC 2046
 */
public class Multipart implements Serializable {

    // ------------------------------------------------------------- Attributes

    /** The list of the BodyParts contained in this Multipart */
    private Vector parts;

    /** The Message or BodyPart eventually containing this Multipart. */
    private Part container;

    // ----------------------------------------------------------- Constructors

    /**
     * Constructs a Multipart object of the given content type.
     * 
     * A unique boundary string is generated and this string is setup as the
     * "boundary" string to separate BodyParts in the formatted message. <p>
     * 
     * BodyParts can be added later
     * 
     */
    public Multipart() {

        // Init the BodyPart list with an empty vector
        parts = new Vector(0, 5);
        
        container = null;
    }


    // --------------------------------------------------------- Public Methods

    /**
     * Return the container object for this Multipart.
     * 
     * @return A <code>Message</code> or a <code>BodyPart</code>
     */
    public Part getContainer() {
        return container;
    }


    /**
     * Set the container object for this Multipart.
     * 
     * Normally called by the Message or BodyPart setContent(Multipart) method
     * 
     * @param container
     */
    public void setContainer(Part container) {
        this.container = container;
    }


    /**
     * Insert the part at the end of the MultiPart.
     * 
     * @param part
     *            The BodyPart to be added to this Multipart container
     */
    public void addBodyPart(BodyPart part) {
        // Set the container reference to this Multipart
        part.setContainer(this);
        parts.addElement(part);
    }


    /**
     * Insert the part at the specified index.
     * 
     * @param part
     *            The BodyPart to be added to this Multipart container
     * @param index
     *            The position
     */
    public void addBodyPart(BodyPart part, int index) {
        // Set the container reference to this Multipart
        part.setContainer(this);
        parts.insertElementAt(part, index);
    }


    /**
     * Get the BodyPart at the specified index.
     * 
     * @param index
     *            the index of the BodyPart
     * @return the requested BodyPart
     */
    public BodyPart getBodyPart(int index) {
        return (BodyPart)parts.elementAt(index);
    }


    /**
     * Removes the BodyPart at the specified index.
     * 
     * @param index
     *            the index of the BodyPart
     */
    public void removeBodyPart(int index) {
        // Invalid the container reference
        ((BodyPart)parts.elementAt(index)).setContainer(null);
        // Remove the BodyPart from the list
        parts.removeElementAt(index);
    }


    /**
     * Retrieves the number of body parts contained in this multi-part
     */
    public int getCount() {
        return parts.size();
    }


    // ---------------------------------- interface Serializable implementation

    public void serialize(DataOutputStream dout) throws IOException {

        ComplexSerializer.serializeVector(dout, parts);

    }


    public void deserialize(DataInputStream din) throws IOException {

        parts = ComplexSerializer.deserializeVector(din);

        // Restore the container reference for all the BodyParts
        for (Enumeration e = parts.elements(); e.hasMoreElements();) {
            BodyPart p = (BodyPart)e.nextElement();
            p.setContainer(this);
        }
    }

}

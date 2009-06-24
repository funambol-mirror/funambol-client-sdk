/*
 * Funambol is a mobile platform developed by Funambol, Inc. 
 * Copyright (C) 2005 - 2007 Funambol, Inc.
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

import java.io.*;
import java.text.*;
import java.util.*;
import java.util.Enumeration;

import com.funambol.syncclient.common.*;
import com.funambol.syncclient.common.logging.*;
import com.funambol.syncclient.spdm.*;
import com.funambol.syncclient.spds.engine.*;
import com.funambol.syncclient.spds.event.*;

/**
 */
public class SyncMessages {

    //---------------------------------------------------------------- Constants

    private static final String XML_ALERT          = "/xml/alert.xml";
    private static final String XML_INITIALIZATION = "/xml/init.xml";
    private static final String XML_MODIFICATIONS  = "/xml/mod.xml";
    private static final String XML_MAPPING        = "/xml/map.xml";

    //---------------------------------------------------------------------- Private data

    private String alertXML         = null;
    private String clientInitXML    = null;
    private String modificationsXML = null;
    private String mappingXML       = null;

    //------------------------------------------------------------- Constructors

    /**
     * Creates the SyncMessages.
     * @throws IOException
     **/
    public SyncMessages() throws IOException {
        Class c = this.getClass();

        alertXML         = read(c.getResourceAsStream(XML_ALERT         ));
        clientInitXML    = read(c.getResourceAsStream(XML_INITIALIZATION));
        modificationsXML = read(c.getResourceAsStream(XML_MODIFICATIONS ));
        mappingXML       = read(c.getResourceAsStream(XML_MAPPING       ));
    }

    /**
     * Reads the content of the given input stream.
     *
     * @param is the input stream
     **/
    private String read(InputStream is) throws IOException {
        StringBuffer sb = new StringBuffer();

        try {
            byte[] buf = new byte[1024];

            int nbyte = -1;
            while ((nbyte = is.read(buf)) >= 0) {
                sb.append(new String(buf, 0, nbyte));
            }
        } finally {
            is.close();
        }
        return sb.toString();
    }
    /**
     * Getter for the alert XML message.
     * @return Alert XML message, but it is a copy that might be changed by the
     * caller.
     */
    public String getAlertXML(Object[] objs) {
        return MessageFormat.format(alertXML, objs);
    }
    /**
     * Getter for the client init XML message.
     * @return Client init XML message, but it is a copy that might be changed by the
     * caller.
     */
    public String getClientInitXML(Object[] objs) {
        return MessageFormat.format(clientInitXML, objs);
    }
    /**
     * Getter for the modification XML message.
     * @return Modification XML message, but it is a copy that might be changed by the
     * caller.
     */
    public String getModificationsXML(Object[] objs) {
        return MessageFormat.format(modificationsXML, objs);
    }
    /**
     * Getter for the mapping XML message.
     * @return Mapping XML message, but it is a copy that might be changed by the
     * caller.
     */
    public String getMappingXML(Object[] objs) {
        return MessageFormat.format(mappingXML, objs);
    }
}

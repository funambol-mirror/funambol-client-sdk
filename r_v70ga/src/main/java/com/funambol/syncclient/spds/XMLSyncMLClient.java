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

import java.io.IOException;

/**
 * Implement send / receive message in XML format by HttpConnection.
 *
 * The charSet is specified with the system property
 * <code>spds.charset</code>;
 * if <code>DEFAULT</code> value, the default charSet value is taken.<br>
 * if no value, the default charSet API <code>UTF-8</vode> value is taken.
 *
 *
 *
 * @version $Id: XMLSyncMLClient.java,v 1.3 2007-12-22 18:09:17 nichele Exp $
 *
 */
public class XMLSyncMLClient
extends BaseSyncMLClient
implements SyncMLClient {

    // --------------------------------------------------------------- Constants

    // --------------------------------------------------------------- Private data

    // --------------------------------------------------------------- Properties

    // --------------------------------------------------------------- Public methods

    public String sendMessage(String msg)
    throws IOException {
        return sendMessage(msg, "UTF-8");
    }

    public String sendMessage(String msg, String charSet)
    throws IOException {
        return new String(sendMessage(msg.getBytes(charSet)));
    }

}

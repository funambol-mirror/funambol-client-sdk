/*
 * Funambol is a mobile platform developed by Funambol, Inc. 
 * Copyright (C) 2009 Funambol, Inc.
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

import java.io.ByteArrayOutputStream;
import java.io.InputStream;

import com.funambol.syncml.protocol.SyncML;
import com.funambol.util.ConsoleAppender;
import com.funambol.util.StringUtil;
import com.funambol.util.Log;

import junit.framework.*;

public class SyncMLParserTest extends TestCase {

    private static final String TAG_LOG = "SyncMLParserTest";

    public SyncMLParserTest(String name) {
        super(name);
        Log.initLog(new ConsoleAppender(), Log.TRACE);
    }

    public void testRoundTripXml() throws Exception {
        roundTrip(false);
    }

    public void testSkipUnknownSubTree() throws Exception {
        StringBuffer msg = new StringBuffer();
        msg.append("<SyncML>").append("<SyncHdr>")
           .append("<UnknownTag>").append("<UnknownSubTag>")
           .append("</UnknownSubTag>").append("</UnknownTag>")
           .append("</SyncHdr>").append("</SyncML>");

        SyncMLParser parser = new SyncMLParser(false);
        SyncML syncMLTree = parser.parse(msg.toString().getBytes("UTF-8"));

        assertTrue(syncMLTree.getSyncHdr() != null);
    }

    private void roundTrip(boolean wbxml) throws Exception {
        // Load all the round trip files and do the converion
        int idx = 0;

        SyncMLParser    parser    = new SyncMLParser(wbxml);
        SyncMLFormatter formatter = new SyncMLFormatter(wbxml);

        do {
            String fileName = "SyncMLParserRT" + idx + ".txt";
            InputStream fileStream = getClass().getResourceAsStream("/res/" + fileName);

            if (fileStream != null) {

                if (Log.isLoggable(Log.INFO)) {
                    Log.info(TAG_LOG, "Starting SyncMLParser/Formatter round trip for: " + fileName);
                }

                byte[] origMessage = readFile(fileStream);

                // Now we do the parsing/formatting
                SyncML msg = parser.parse(origMessage);
                ByteArrayOutputStream os = new ByteArrayOutputStream();
                formatter.format(msg, os, "UTF-8");
                byte message[] = os.toByteArray();

                // Now must normalize the two messages and compare
                String origMessageStr = normalize(new String(origMessage));
                String messageStr     = normalize(new String(message));

                assertEquals(origMessageStr, messageStr);
            } else {
                break;
            }
            idx++;
        } while(true);
    }

    private byte[] readFile(InputStream is) throws Exception {
        ByteArrayOutputStream os = new ByteArrayOutputStream();
        int b;
        while((b = is.read()) != -1) {
            os.write(b);
        }
        return os.toByteArray();
    }

    private String normalize(String msg) {

        // Replace all the \r\n into empty
        String res = StringUtil.replaceAll(msg, "\r\n", "");
        // Replace all the \n into empty
        res = StringUtil.replaceAll(res, "\n", "");

        // Our formatter opens/closes tags even when they are empty
        res = StringUtil.replaceAll(res, "<Final/>", "<Final></Final>");

        // Our formatter generates double quotes around attributes
        res = StringUtil.replaceAll(res, "'syncml:metinf'", "\"syncml:metinf\"");

        // Replace all CDATA sections
        res = StringUtil.replaceAll(res, "<![CDATA[","");
        res = StringUtil.replaceAll(res, "]]>", "");

        return res;
    }

}



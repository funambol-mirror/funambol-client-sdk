/*
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

import java.io.ByteArrayOutputStream;
import java.io.InputStream;

import java.util.Vector;
import junit.framework.*;

import com.funambol.sync.SyncException;
import com.funambol.sync.SyncConfig;
import com.funambol.sync.SourceConfig;
import com.funambol.sync.SyncSource;

import com.funambol.syncml.protocol.CTCap;
import com.funambol.syncml.protocol.CTInfo;
import com.funambol.syncml.protocol.DataStore;
import com.funambol.syncml.protocol.DevInf;
import com.funambol.syncml.protocol.PropParam;
import com.funambol.syncml.protocol.Property;
import com.funambol.syncml.protocol.SourceRef;
import com.funambol.syncml.protocol.SyncCap;
import com.funambol.syncml.protocol.SyncML;
import com.funambol.syncml.protocol.SyncType;
import com.funambol.syncml.client.TestSyncSource;
import com.funambol.util.StringUtil;

public class DevInfExchangeTest extends TestCase {

    TestSyncSource tss;
    SyncMLParser parser;
    SyncMLFormatter formatter;
    TestSyncManager sm;

    public DevInfExchangeTest(String name) {
        super(name);
    }

    /**
     * Set up all of the tests
     */
    public void setUp() {
        tss = new TestSyncSource(new SourceConfig("briefcase", SourceConfig.BRIEFCASE_TYPE, "briefcase"));
        tss.setConfig(new SyncMLSourceConfig(tss.getName(), tss.getType(),
                tss.getSourceUri(),
                createTestDataStore("contacts", "test/x-vcard", "2.1")));
        SyncMLAnchor anchor = new SyncMLAnchor();
        tss.setSyncAnchor(anchor);
        
        parser = new SyncMLParser(false);
        formatter = new SyncMLFormatter(false);

        DeviceConfig dc = new DeviceConfig();
        dc.setMan("test manufacturer");
        dc.setMod("test model");
        dc.setSwV("1.0");
        dc.setFwV("1.0");
        dc.setHwV("1.0");
        dc.setDevID("test-device-id");
        dc.setMaxMsgSize(64 * 1024);
        dc.setLoSupport(true);
        dc.setUtc(true);
        dc.setNocSupport(true);

        SyncConfig sc = new SyncConfig();
        sc.syncUrl = "http://my.funambol.com/sync";
        sc.userName = "test";
        sc.password = "test";
        sc.preferredAuthType = SyncConfig.AUTH_TYPE_BASIC;
        sc.userAgent = "Test UA";
        sc.forceCookies = false;

        sm = new TestSyncManager(sc, dc);
    }

    /**
     * Tear down all of the tests
     */
    public void tearDown() {

    }

    //----------------------------------------------- Authentication phase tests

    public void testPutDevInf() throws Throwable {

        // Tells the sync manager to send devinf
        sm.setFlagSendDevInf();

        byte[] msg = sm.prepareInitializationMessage_T(200, false, false);

        String normMsg = normalize(new String(msg));
        String normExpMsg = normalize(new String(readResourceFile("SyncML_putDevInf.txt")));
        assertEquals(normMsg, normExpMsg);
    }
    
    public void testGetDevInf() throws Throwable {

        SyncML getDevinf = parser.parse(readResourceFile(
                "SyncML_getDevInf.txt"));
        sm.processInitMessage_T(getDevinf, tss);

        byte[] msg = sm.prepareModificationMessage_T();

        String normMsg = normalize(new String(msg));
        String normExpMsg = normalize(new String(readResourceFile("SyncML_getDevInf_Results.txt")));
        assertEquals(normMsg, normExpMsg);
    }

    private byte[] readResourceFile(String fileName) throws Exception {
        InputStream syncmlStream = getClass().getResourceAsStream(
                "/res/" + fileName);
        assertTrue(syncmlStream != null);

        ByteArrayOutputStream os = new ByteArrayOutputStream();
        int b;
        while((b = syncmlStream.read()) != -1) {
            os.write(b);
        }
        return os.toByteArray();
    }

    private DataStore createTestDataStore(String name, String type, String version) {

        DataStore ds = new DataStore();
        SourceRef sr = SourceRef.newInstance();
        sr.setValue(name);
        ds.setSourceRef(sr);

        CTInfo rxPref = new CTInfo();
        rxPref.setCTType(type);
        rxPref.setVerCT(version);
        ds.setRxPref(rxPref);

        CTInfo txPref = new CTInfo();
        txPref.setCTType(type);
        txPref.setVerCT(version);
        ds.setTxPref(txPref);

        SyncCap syncCap = new SyncCap();
        Vector types = new Vector();
        types.addElement(SyncType.TWO_WAY);
        types.addElement(SyncType.SLOW);
        types.addElement(SyncType.SERVER_ALERTED);
        syncCap.setSyncType(types);
        ds.setSyncCap(syncCap);

        ds.setMaxGUIDSize(2);

        Vector properties = new Vector();
        properties.addElement(new Property("BEGIN", null, 0, 0, false,
                new String[] {"VCARD"}, null, new PropParam[0]));
        properties.addElement(new Property("END", null, 0, 0, false,
                new String[] {"VCARD"}, null, new PropParam[0]));
        properties.addElement(new Property("VERISON", null, 0, 0, false,
                new String[] {"2.1"}, null, new PropParam[0]));

        properties.addElement(new Property("TEL", null, 0, 0, false,
                new String[0], null, new PropParam[] {
            new PropParam("FAX", null, new String[0], null),
            new PropParam("HOME", null, new String[0], null)
        }));

        properties.addElement(new Property("TEL", null, 0, 0, false,
                new String[0], null, new PropParam[] {
            new PropParam("FAX", null, new String[0], null),
            new PropParam("WORK", null, new String[0], null)
        }));

        Vector ctCaps = new Vector();
        CTCap ctCap = new CTCap();
        ctCap.setCTInfo(new CTInfo(type, version));
        ctCap.setProperties(properties);
        ctCaps.addElement(ctCap);
        ds.setCTCaps(ctCaps);

        return ds;
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



    private class TestSyncManager extends SyncManager {

        public TestSyncManager(SyncConfig sc, DeviceConfig dc) {
            super(sc, dc);

            source = tss;

            serverUrl = "http://my.funambol.com/sync";
            sessionID = "1234567890";

            syncStatus = new SyncStatus(tss.getName());
            statusList = new Vector();
            sourceLOHandler = new SyncSourceLOHandler(tss, maxMsgSize, false);
        }
        
        public byte[] prepareInitializationMessage_T(int syncMode, 
                boolean requireDevInf, boolean md5Auth) {
            return prepareInitializationMessage(syncMode, requireDevInf, md5Auth);
        }
        public DevInf processInitMessage_T(SyncML message, SyncSource source)
                throws SyncException {
            return processInitMessage(message, source, null);
        }
        public byte[] prepareModificationMessage_T() throws SyncException {
            return prepareModificationMessage();
        }
    }

}


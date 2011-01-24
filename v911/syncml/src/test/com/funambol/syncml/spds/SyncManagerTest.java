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

import java.util.Vector;
import java.util.Hashtable;
import junit.framework.*;

import com.funambol.syncml.protocol.Item;
import com.funambol.syncml.protocol.Target;
import com.funambol.syncml.protocol.SyncMLCommand;
import com.funambol.syncml.protocol.Data;
import com.funambol.syncml.protocol.Status;
import com.funambol.syncml.protocol.SyncML;
import com.funambol.syncml.protocol.SyncHdr;
import com.funambol.syncml.protocol.Sync;
import com.funambol.syncml.client.TestSyncSource;

import com.funambol.util.Log;

public class SyncManagerTest extends TestCase {

    private SyncManager sm = null;
    private SyncSource  src = null;

    public SyncManagerTest(String name) {
        super(name);
    }

    public void setUp() throws Exception {
        SourceConfig sourceConfig = new SourceConfig("test", "application/*", "test");
        src = new TestSyncSource(sourceConfig);
        SyncConfig sc = new SyncConfig();

        sm = new SyncManager(sc);

        SyncSourceLOHandler handler = new SyncSourceLOHandler(src, 65536, false);
        sm.sourceLOHandler = handler;
        sm.source = src;
        sm.syncStatus = new SyncStatus(src.getName());
    }

    public void tearDown() throws Exception {
    }

    public void testNoRespInSyncHdr() throws Exception {
    }

    public void testNoRespInSyncBody() throws Exception {
    }

    /**
     * This test checks that the processSyncItem does not return any Status if
     * the incoming command has the No Resp property set.
     */
    public void testProcessSyncItemWithNoResp() throws Exception {
        SyncMLCommand command = SyncMLCommand.newInstance(SyncML.TAG_REPLACE);
        command.setCmdId(0);
        command.setNoResp(true);

        Item item = Item.newInstance();
        Target tgt = Target.newInstance();
        tgt.setLocURI("briefcase");
        item.setTarget(tgt);

        Data data = Data.newInstance("Test");
        item.setData(data);

        Vector items = new Vector();
        command.setItems(items);

        sm.itemsToProcess = new ItemsList();
        sm.statusList = new Vector();
        sm.processCommand(command, "1");
        sm.applySourceItems("1");

        Vector statusCommands = new Vector();
        sm.prepareStatus(statusCommands);
        assertTrue(statusCommands.size() == 0);
    }

    /**
     * This test checks that the processSyncItem method returns a proper status
     */
    public void testProcessSyncItem1() throws Exception {
        SyncMLCommand command = SyncMLCommand.newInstance(SyncML.TAG_REPLACE);
        command.setCmdId(10);

        Item item = Item.newInstance();
        Target tgt = Target.newInstance();
        tgt.setLocURI("briefcase");
        item.setTarget(tgt);

        Data data = Data.newInstance("Test");
        item.setData(data);

        Vector items = new Vector();
        items.addElement(item);
        command.setItems(items);

        sm.statusList = new Vector();
        sm.itemsToProcess = new ItemsList();
        sm.processCommand(command, "1");
        sm.applySourceItems("1");

        Vector statusCommands = new Vector();
        sm.prepareStatus(statusCommands);

        System.out.println("MARCO:" + statusCommands.size());
        assertTrue(statusCommands.size() == 1);
        Status status = (Status)statusCommands.elementAt(0);
        assertTrue(status != null);
        assertTrue(SyncML.TAG_REPLACE.equals(status.getCmd()));
        assertTrue("10".equals(status.getCmdRef()));
        assertTrue("200".equals(status.getData().getData()));
    }

    public void testProcessSyncCommandWithNoResp() throws Exception {

        sm.statusList = new Vector();

        Sync syncCommand = new Sync();
        Target tgt = Target.newInstance();
        tgt.setLocURI("test");
        syncCommand.setCmdID("1");
        syncCommand.setNoResp(new Boolean(true));

        syncCommand.setTarget(tgt);

        sm.processSyncCommand(syncCommand, "1");
        assertTrue(sm.statusList.size() == 0);
    }

    public void testProcessSyncCommand1() throws Exception {

        sm.statusList = new Vector();

        Sync syncCommand = new Sync();
        Target tgt = Target.newInstance();
        tgt.setLocURI("test");
        syncCommand.setCmdID("1");

        syncCommand.setTarget(tgt);

        sm.processSyncCommand(syncCommand, "1");
        assertTrue(sm.statusList.size() == 1);

        Status status = (Status)sm.statusList.elementAt(0);
        assertTrue(status != null);
        assertTrue(SyncML.TAG_SYNC.equals(status.getCmd()));
        assertTrue("1".equals(status.getCmdRef()));
        assertTrue("200".equals(status.getData().getData()));
    }

    public void testProcessInitMessageWithNoResp() throws Exception {

        sm.statusList = new Vector();
        SyncML msg = new SyncML();

        SyncHdr hdr = new SyncHdr();
        hdr.setNoResp(new Boolean(true));
        msg.setSyncHdr(hdr);

        sm.processInitMessage(msg, src);

        assertTrue(sm.statusList.size() == 0);
        assertTrue(sm.globalNoResp);

        // Now check that other methods do not generate a status
        // since a global one was set
        Sync syncCommand = new Sync();
        Target tgt = Target.newInstance();
        tgt.setLocURI("test");
        syncCommand.setCmdID("1");

        syncCommand.setTarget(tgt);

        sm.processSyncCommand(syncCommand, "1");
        assertTrue(sm.statusList.size() == 0);

        // Now check that other methods do not generate a status
        // since a global one was set
        SyncMLCommand command = SyncMLCommand.newInstance(SyncML.TAG_REPLACE);
        command.setCmdId(10);

        Item item = Item.newInstance();
        tgt = Target.newInstance();
        tgt.setLocURI("briefcase");
        item.setTarget(tgt);

        Data data = Data.newInstance("Test");
        item.setData(data);

        Vector items = new Vector();
        command.setItems(items);

        sm.itemsToProcess = new ItemsList();
        sm.processCommand(command, "1");
        sm.applySourceItems("1");

        Vector statusCommands = new Vector();
        sm.prepareStatus(statusCommands);

        assertTrue(statusCommands.size() == 0);
    }
}

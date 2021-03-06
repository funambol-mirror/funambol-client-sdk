/**
 * Funambol is a mobile platform developed by Funambol, Inc. 
 * Copyright (C) 2011 Funambol, Inc.
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

import java.io.DataInputStream;
import java.io.DataOutputStream;
import java.io.File;
import java.io.FileInputStream;
import java.io.FileOutputStream;

import com.funambol.sync.SourceConfig;
import com.funambol.util.ConsoleAppender;
import com.funambol.util.Log;

import junit.framework.TestCase;

public class SyncMLSourceConfigTest extends TestCase  {
    private static final String TAG_LOG = "SyncMLSourceConfigTest";
    
    private SyncMLSourceConfig syncSourceConfig;

    public SyncMLSourceConfigTest(String name) {
        super(name);
        Log.initLog(new ConsoleAppender(), Log.TRACE);
    }
    
    protected void setUp() throws Exception {
        super.setUp();
        
        syncSourceConfig = new SyncMLSourceConfig("briefcase", SourceConfig.BRIEFCASE_TYPE, "briefcase"); 
        SyncMLAnchor anchor = new SyncMLAnchor();
        syncSourceConfig.setSyncAnchor(anchor);
}

    
    public void testSerializeDeserialize() throws Exception {
        Log.debug(TAG_LOG, "Starting testSerializeDeserialize...");
        final String hash = "AAAAAAAAA";
        
        //create a temp file
        File tempFile = File.createTempFile("funambol", "syncmltest");
        tempFile.deleteOnExit();
        
        syncSourceConfig.setLastDevInfHash(hash);
        DataOutputStream fos = new DataOutputStream(new FileOutputStream(tempFile));
        syncSourceConfig.serialize(fos);

        //create a new source
        SyncMLSourceConfig syncSourceConfig2 = new SyncMLSourceConfig("briefcase", SourceConfig.BRIEFCASE_TYPE, "briefcase"); 
        SyncMLAnchor anchor = new SyncMLAnchor();
        syncSourceConfig2.setSyncAnchor(anchor);

        //and read saved data
        assertFalse("Wrong property value", hash.equals(syncSourceConfig2.getLastDevInfHash()));
        fos.close();
        fos = null;
        
        DataInputStream fis = new DataInputStream(new FileInputStream(tempFile));
        syncSourceConfig2.deserialize(fis);
        fis.close();
        fis = null;
        assertEquals("Wrong property value", hash, syncSourceConfig2.getLastDevInfHash());
        
        tempFile.delete();
    }
}

/*
 * Copyright (C) 2007 Funambol
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

package com.funambol.syncml.client;

import com.funambol.util.Log;
import com.funambol.syncml.spds.SyncListener;
import com.funambol.syncml.spds.BasicSyncListener;
import com.funambol.syncml.spds.SyncItem;

/**
 * Check the interface method for documentation
 */
public class TestSyncListener extends BasicSyncListener {
    public static boolean run;
    public static int result;
    public static int counter;
    
    /** Creates a new instance of TestSyncListener */
    public TestSyncListener() {
    }
    
    public void startSession() {
        run = true;
    }
    
    public void endSession(int i) {
        Log.info("######Listener###### END SESSION WITH STATUS: " + i);
        result = i;
        run = false;
    }
    
    public boolean startSyncing(int i) {
        return true;
    }
}

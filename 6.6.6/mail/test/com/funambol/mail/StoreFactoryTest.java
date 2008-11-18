/*
 * Copyright (C) 2006-2007 Funambol
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

package com.funambol.mail;

import com.funambol.mail.*;
import com.funambol.util.Log;
import jmunit.framework.cldc10.*;

public class StoreFactoryTest extends TestCase {

    private static final int STORE_NUMBER = 100;
    
    private Store[] store;
    
    /**
     * Test of getStore method, of class com.funambol.mail.StoreFactory.
     */
    public void testgetStore() throws AssertionFailedException {
        Log.debug("Test Get Store");
        //tests if the instance of store follows the Singleton pattern
        boolean result = false;
        
        for (int i=0; i<STORE_NUMBER; i++) {
            store[i] = StoreFactory.getStore();
        }
        
        for (int i=1; i<STORE_NUMBER; i++) {
            result = store[i].equals(store[i-1]);
            if (!result) {
                break;
            }
        }
        
        assertTrue(result);
    }

    public StoreFactoryTest() {
        super(1,"StoreFactoryTest");
        Log.setLogLevel(Log.DEBUG);
    }

    public void setUp() {
        store = new Store[STORE_NUMBER];
    }

    public void tearDown() {
        store = null;
    }

    public void test(int testNumber) throws Throwable {
        switch(testNumber) {
            case 0:testgetStore();break;
            default: break;
        }
    }
}

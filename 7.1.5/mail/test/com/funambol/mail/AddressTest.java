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

public class AddressTest extends TestCase {

    Address a;
    
    /**
     * Test of getType method, of class com.funambol.mail.Address.
     */
    public void testType() throws AssertionFailedException {
        Log.debug("Type Accessor Methods");
        a.setType(Address.TO);
        assertEquals(Address.TO, a.getType());
    }

    /**
     * Test of getVisibleName method, of class com.funambol.mail.Address.
     */
    public void testVisibleName() throws AssertionFailedException {
        Log.debug("Visible name");
        a.setEmail("email@funambol.com");
        boolean result = "email@funambol.com".equals(a.getVisibleName());
        if (!result) {
            fail("Test failed: visible name is not the email");
        }
        a.setName("addressName");
        result = "addressName".equals(a.getVisibleName());
        assertTrue(result);
    }

    /**
     * Test of getName method, of class com.funambol.mail.Address.
     */
    public void testName() throws AssertionFailedException {
        Log.debug("Name Accessor Methods");
        a.setName("addressName");
        assertEquals("addressName", a.getName());
    }

    /**
     * Test of getEmail method, of class com.funambol.mail.Address.
     */
    public void testEmail() throws AssertionFailedException {
        Log.debug("Email Accessor Methods");
        a.setEmail("addressMail@funambol.com");
        assertEquals("addressMail@funambol.com", a.getEmail());
    }

    
    /**
     * Test of toString method, of class com.funambol.mail.Address.
     */
    public void testtoString() throws AssertionFailedException {
        Log.debug("To String");
        a.setName("John Doe");
        a.setEmail("john.doe@mail.com");
        a.setType(Address.TO);
        
        StringBuffer expected = new StringBuffer();
        expected.append("\"").append("John Doe").append("\"").
                    append(" <").append("john.doe@mail.com").append(">");
        
        assertEquals(expected.toString(), a.toString());
    }

    /**
     * Test of parse method, of class com.funambol.mail.Address.
     */
    public void testparse() throws AssertionFailedException, Exception {
        Log.debug("Parse Address");
        Address expectedAddress = new Address();
        expectedAddress.setName("John Doe");
        expectedAddress.setEmail("john.doe@mail.com");
        expectedAddress.setType(Address.TO);
        
        a.setName("John Doe");
        a.setEmail("john.doe@mail.com");
        a.setType(Address.TO);
        a = Address.parse(Address.TO, a.toString())[0];
        
        Log.debug(a.toString());
        assertEquals(expectedAddress.toString(), a.toString());
    }

    
    public AddressTest() {
        super(6,"AddressTest");
        Log.setLogLevel(Log.DEBUG);
    }

    public void setUp() {
        a = new Address();
    }

    public void tearDown() {
        a = null;
    }
    
    public void test(int testNumber) throws Throwable {
        switch(testNumber) {
            case 0:testType();break;
            case 1:testVisibleName();break;
            case 2:testName();break;
            case 3:testEmail();break;
            case 4:testtoString();break;
            case 5:testparse();break;
            default: break;
        }
    }
}

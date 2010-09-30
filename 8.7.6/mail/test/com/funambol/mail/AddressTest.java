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
import com.funambol.util.ConsoleAppender;
import junit.framework.*;

public class AddressTest extends TestCase {

    private Address a;

    public AddressTest(String name) {
        super(name);
        Log.initLog(new ConsoleAppender());
        Log.setLogLevel(Log.INFO);
    }

    public void setUp() {
        a = new Address();
    }

    public void tearDown() {
        a = null;
    }

    /**
     * Test of getType method, of class com.funambol.mail.Address.
     */
    public void testSetType() throws Exception {
        Log.info("AddressTest: testSetType");
        a.setType(Address.TO);
        assertEquals(Address.TO, a.getType());
        Log.info("AddressTest: testSetType successful");
    }

    /**
     * Test of getVisibleName method, of class com.funambol.mail.Address.
     */
    public void testGetVisibleName() throws Exception {
        Log.info("AddressTest: testGetVisibleName");
        a.setEmail("email@funambol.com");
        boolean result = "email@funambol.com".equals(a.getVisibleName());
        if (!result) {
            fail("Test failed: visible name is not the email");
        }
        a.setName("addressName");
        result = "addressName".equals(a.getVisibleName());
        assertTrue(result);
        Log.info("AddressTest: testGetVisibleName successful");
    }

    /**
     * Test of getName method, of class com.funambol.mail.Address.
     */
    public void testGetName() throws Exception {
        Log.info("AddressTest: testGetName");
        a.setName("addressName");
        assertEquals("addressName", a.getName());
        Log.info("AddressTest: testGetName successful");
    }

    /**
     * Test of getEmail method, of class com.funambol.mail.Address.
     */
    public void testGetEmail() throws Exception {
        Log.info("AddressTest: testGetEmail");
        a.setEmail("addressMail@funambol.com");
        assertEquals("addressMail@funambol.com", a.getEmail());
        Log.info("AddressTest: testGetEmail successful");
    }

    /**
     * Test of toString method, of class com.funambol.mail.Address.
     */
    public void testToString() throws Exception {
        Log.info("AddressTest: testToString");
        a.setName("John Doe");
        a.setEmail("john.doe@mail.com");
        a.setType(Address.TO);

        StringBuffer expected = new StringBuffer();
        expected.append("\"").append("John Doe").append("\"").
                append(" <").append("john.doe@mail.com").append(">");

        assertEquals(expected.toString(), a.toString());
        Log.info("AddressTest: testToString successful");
    }

    /**
     * Test of parse method, of class com.funambol.mail.Address.
     */
    public void testParse() throws Exception {
        Log.info("AddressTest: testParse");
        Address expectedAddress = new Address();
        expectedAddress.setName("John Doe");
        expectedAddress.setEmail("john.doe@mail.com");
        expectedAddress.setType(Address.TO);

        a.setName("John Doe");
        a.setEmail("john.doe@mail.com");
        a.setType(Address.TO);
        a = Address.parse(Address.TO, a.toString())[0];

        assertEquals(expectedAddress.toString(), a.toString());
        Log.info("AddressTest: testParse successful");
    }
}

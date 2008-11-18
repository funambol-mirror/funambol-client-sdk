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

package com.funambol.mail;

import jmunit.framework.cldc10.AssertionFailedException;
import jmunit.framework.cldc10.TestCase;


/**
 * Public class to test message flags behavior
 */
public class  MessageFlagsTest extends TestCase {

    /** The object to be populated with the unit testing */
    private MessageFlags flags = null;

    /**
     * Public default constructor for the test class
     */ 
    public MessageFlagsTest() {
        super(19, "MessageFlagsTest");
    }

    /**
     * Sets up the  environment for the unit test
     */
    public void setUp() {
        //Create the new object to test
        flags = new MessageFlags();

        //Initialize all flags to active
        flags.setFlags(flags.ANSWERED|flags.FORWARDED|flags.OPENED
                       |flags.DRAFT|flags.FLAGGED|flags.DELETED
                       |flags.TX_SENDING|flags.TX_SENT|flags.TX_ERROR);
    }

    /**
     * Tears down the  environment for the unit test
     */
    public void tearDown() {
        flags = null;
    }

    /**
     * Core method for the unit test:
     * Begins with all the flag mask's values set to active
     * 0-8) Test if flag mask's values are correctly set
     * 9) Clear up flag mask's values setting them all to inactive
     * 10-18) Sets all the flags one by one and test if they are correctly set
     * invoking the isSet() method
     * @throws Throwable if any test fails
     */
    public void test(int testNumber) throws Throwable {
        switch (testNumber) {
            case 0:
                testIsDeletedFlagSet();
                break;
            case 1:
                testIsDraftFlagSet();
                break;
            case 2:
                testIsFlaggedFlagSet();
                break;
            case 3:
                testIsForwardedFlagSet();
                break;
            case 4:
                testIsOpenedFlagSet();
                break;
            case 5:
                testIsAnsweredFlagSet();
                break;    
            case 6:
                testIsTxErrorFlagSet();
                break;
            case 7:
                testIsTxSendingFlagSet();
                break;
            case 8:
                testIsTxSentFlagSet();
                break;
            case 9:
                testClearFlags();
                break;
            //Set the value of ht
            case 10:
                testSetDeletedFlag();
                break;
            case 11:
                testSetDraftFlag();
                break;
            case 12:
                testSetFlaggedFlag();
                break;
            case 13:
                testSetForwardedFlag();
                break;
            case 14:
                testSetOpenedFlag();
                break;
            case 15:
                testSetAnsweredFlag();
                break;
            case 16:
                testSetTxErrorFlag();
                break;
            case 17:
                testSetTxSendingFlag();
                break;
            case 18:
                testSetTxSentFlag();
                break;
            default:
                break;
        }
    }
    
    
    /**
     * Test of clearFlags method, of class MessageFlags
     * @param flags is the given flags mask
     * @throws AssertionFailedException if the assertion fails
     */
    public void testClearFlags()
                                               throws AssertionFailedException {
        //clear up all flags in the given mask
        flags.clearFlags();

        assertFalse(flags.isSet(flags.ANSWERED)
                    &&flags.isSet(flags.FORWARDED)
                    &&flags.isSet(flags.OPENED)
                    &&flags.isSet(flags.DRAFT)
                    &&flags.isSet(flags.FLAGGED)
                    &&flags.isSet(flags.DELETED)
                    &&flags.isSet(flags.TX_ERROR)
                    &&flags.isSet(flags.TX_SENDING)
                    &&flags.isSet(flags.TX_SENT));
    }

    /**
     * Test the setting method for DELETED flag
     * @throws AssertionFailedException if the assertion fails
     */
    public void testSetDeletedFlag() throws AssertionFailedException {
        
        //Set the DELETED flag of the given mask to active 
        flags.setFlag(flags.DELETED, true);
        assertTrue(flags.isSet(flags.DELETED));
    }
    
    /**
     * Test the setting method for DRAFT flag
     * @throws AssertionFailedException if the assertion fails
     */
    private void testSetDraftFlag() throws AssertionFailedException {
        //Set the DRAFT flag of the given mask to active 
        flags.setFlag(flags.DRAFT, true);
        assertTrue(flags.isSet(flags.DRAFT));
    }

    /**
     * Test the setting method for FLAGGED flag
     * @throws AssertionFailedException if the assertion fails
     */
    private void testSetFlaggedFlag() throws AssertionFailedException {
        //Set the FLAGGED flag of the given mask to active 
        flags.setFlag(flags.FLAGGED, true);
        assertTrue(flags.isSet(flags.FLAGGED));
    }

    /**
     * Test the setting method for FORWARDED flag
     * @throws AssertionFailedException if the assertion fails
     */
    private void testSetForwardedFlag() throws AssertionFailedException {
        //Set the FORWARDED flag of the given mask to active 
        flags.setFlag(flags.FORWARDED, true);
        assertTrue(flags.isSet(flags.FORWARDED));
    }

    /**
     * Test the setting method for OPENED flag
     * @throws AssertionFailedException if the assertion fails
     */
    private void testSetOpenedFlag() throws AssertionFailedException {
        //Set the OPENED flag of the given mask to active 
        flags.setFlag(flags.OPENED, true);
        assertTrue(flags.isSet(flags.OPENED));
    }

    /**
     * Test the setting method for ANSWERED flag
     * @throws AssertionFailedException if the assertion fails
     */
    private void testSetAnsweredFlag() throws AssertionFailedException {
        //Set the ANSWERED flag of the given mask to active 
        flags.setFlag(flags.ANSWERED, true);
        assertTrue(flags.isSet(flags.ANSWERED));
    }

    /**
     * Test the setting method for TX_ERROR flag
     * @throws AssertionFailedException if the assertion fails
     */
    private void testSetTxErrorFlag() throws AssertionFailedException {
        //Set the TX_ERROR flag of the given mask to active 
        flags.setFlag(flags.TX_ERROR, true);
        assertTrue(flags.isSet(flags.TX_ERROR));
    }

    /**
     * Test the setting method for TX_SENDING flag
     * @throws AssertionFailedException if the assertion fails
     */
    private void testSetTxSendingFlag() throws AssertionFailedException {
        //Set the TX_SENDING flag of the given mask to active 
        flags.setFlag(flags.TX_SENDING, true);
        assertTrue(flags.isSet(flags.TX_SENDING));
    }

    /**
     * Test the setting method for TX_SENT flag
     * @throws AssertionFailedException if the assertion fails
     */
    private void testSetTxSentFlag() throws AssertionFailedException {
        //Set the TX_SENT flag of the given mask to active 
        flags.setFlag(flags.TX_SENT, true);
        assertTrue(flags.isSet(flags.TX_SENT));
    }

    /**
     * Test of isSet method for DELETED flag
     * @throws AssertionFailedException if the assertion fails
     */
    public void testIsDeletedFlagSet() throws AssertionFailedException {
        assertTrue(flags.isSet(flags.DELETED));
    }

    /**
     * Test of isSet method for DRAFT flag
     * @throws AssertionFailedException if the assertion fails
     */
    public void testIsDraftFlagSet() throws AssertionFailedException {
        assertTrue(flags.isSet(flags.DRAFT));
    }

    /**
     * Test of isSet method for FLAGGED flag
     * @throws AssertionFailedException if the assertion fails
     */
    public void testIsFlaggedFlagSet() throws AssertionFailedException {
        assertTrue(flags.isSet(flags.FLAGGED));
    }

    /**
     * Test of isSet method for FORWARDED flag
     * @throws AssertionFailedException if the assertion fails
     */
    public void testIsForwardedFlagSet() throws AssertionFailedException {
        assertTrue(flags.isSet(flags.FORWARDED));
    }

    /**
     * Test of isSet method for OPENED flag
     * @throws AssertionFailedException if the assertion fails
     */
    public void testIsOpenedFlagSet() throws AssertionFailedException {
        assertTrue(flags.isSet(flags.OPENED));
    }

    /**
     * Test of isSet method for ANSWERED flag
     * @throws AssertionFailedException if the assertion fails
     */
    public void testIsAnsweredFlagSet() throws AssertionFailedException {
        assertTrue(flags.isSet(flags.ANSWERED));
    }

    /**
     * Test of isSet method for TX_ERROR flag
     * @throws AssertionFailedException if the assertion fails
     */
    public void testIsTxErrorFlagSet() throws AssertionFailedException {
        assertTrue(flags.isSet(flags.TX_ERROR));
    }
    
    /**
     * Test of isSet method for TX_SENDING flag
     * @throws AssertionFailedException if the assertion fails
     */
    public void testIsTxSendingFlagSet() throws AssertionFailedException {
        assertTrue(flags.isSet(flags.TX_SENDING));
    }

    /**
     * Test of isSet method for TX_SENT flag
     * @throws AssertionFailedException if the assertion fails
     */
    public void testIsTxSentFlagSet() throws AssertionFailedException {
        assertTrue(flags.isSet(flags.TX_SENT));
    }
}

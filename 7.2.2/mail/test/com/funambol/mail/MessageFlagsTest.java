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

import com.funambol.util.ConsoleAppender;
import com.funambol.util.FunBasicTest;
import com.funambol.util.Log;
import java.io.ByteArrayInputStream;
import java.io.ByteArrayOutputStream;
import java.io.DataInputStream;
import java.io.DataOutputStream;
import java.io.IOException;
import j2meunit.framework.*;


/**
 * Public class to test message flags behavior
 */
public class  MessageFlagsTest extends FunBasicTest {

    private static final String FLAGS_STRING= "[MessageFlags.toString]\n" +
                                "Open flag: true\n" +
                                "Reply flag: true\n" +
                                "Flag flag: true\n" +
                                "Forward flag: true\n" +
                                "Delete flag: true\n" +
                                "TxSending flag: true\n" +
                                "TxSent flag: true\n" +
                                "TxError flag: true\n" +
                                "Partial flag: true\n" +
                                "Draft flag: true";
    /** The object to be populated with the unit testing */
    private MessageFlags flags = null;

    /**
     * Public default constructor for the test class
     */ 
    public MessageFlagsTest() {
        super(24, "MessageFlagsTest");
        Log.initLog(new ConsoleAppender());
        Log.setLogLevel(Log.DEBUG);
    }

    /**
     * Sets up the  environment for the unit test
     */
    public void setUp() {
        //Create the new object to test
        flags = new MessageFlags();

        //Initialize all flags to active
        flags.setFlags(
                MessageFlags.ANSWERED|
                MessageFlags.FORWARDED|
                MessageFlags.OPENED|
                MessageFlags.DRAFT|
                MessageFlags.FLAGGED|
                MessageFlags.DELETED|
                MessageFlags.TX_SENDING|
                MessageFlags.TX_SENT|
                MessageFlags.TX_ERROR|
                MessageFlags.PARTIAL
                );
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
            case 19: 
                testMerge();
                break;
            case 20: 
                testToString();
                break;
            case 21: 
                testSerialize();
                break;
            case 22: 
                testDeserialize();
                break;
            case 23:
                testSetPartialFlag();
                break;
            case 24:
                testIsPartialFlagSet();
                break;
            default:
                break;
                
        }
    }
    
    
    /**
     * Test of clearFlags method, of class MessageFlags
     * @param flags is the given flags mask
     * @throws Exception if the assertion fails
     */
    public void testClearFlags()
                                               throws Exception {
        //clear up all flags in the given mask
        flags.clearFlags();

        assertTrue(!(flags.isSet(flags.ANSWERED)
                    &&flags.isSet(flags.FORWARDED)
                    &&flags.isSet(flags.OPENED)
                    &&flags.isSet(flags.DRAFT)
                    &&flags.isSet(flags.FLAGGED)
                    &&flags.isSet(flags.DELETED)
                    &&flags.isSet(flags.TX_ERROR)
                    &&flags.isSet(flags.TX_SENDING)
                    &&flags.isSet(flags.TX_SENT)));
    }

    /**
     * Test the setting method for DELETED flag
     * @throws Exception if the assertion fails
     */
    public void testSetDeletedFlag() throws Exception {
        Log.info("testSetDeletedFlag");
        //Set the DELETED flag of the given mask to active 
        flags.setFlag(flags.DELETED, true);
        assertTrue(flags.isSet(flags.DELETED));
        Log.info("Succesfull");
    }
    
    /**
     * Test the setting method for DRAFT flag
     * @throws Exception if the assertion fails
     */
    private void testSetDraftFlag() throws Exception {
        Log.info("testSetDraftFlag");
        //Set the DRAFT flag of the given mask to active 
        flags.setFlag(flags.DRAFT, true);
        assertTrue(flags.isSet(flags.DRAFT));
        Log.info("Succesfull");
    }

    /**
     * Test the setting method for FLAGGED flag
     * @throws Exception if the assertion fails
     */
    private void testSetFlaggedFlag() throws Exception {
        Log.info("testSetFlaggedFlag");
        //Set the FLAGGED flag of the given mask to active 
        flags.setFlag(flags.FLAGGED, true);
        assertTrue(flags.isSet(flags.FLAGGED));
        Log.info("Succesfull");
    }

    /**
     * Test the setting method for FORWARDED flag
     * @throws Exception if the assertion fails
     */
    private void testSetForwardedFlag() throws Exception {
        Log.info("testSetForwardedFlag");
        //Set the FORWARDED flag of the given mask to active 
        flags.setFlag(flags.FORWARDED, true);
        assertTrue(flags.isSet(flags.FORWARDED));
        Log.info("Succesfull");
    }

    /**
     * Test the setting method for OPENED flag
     * @throws Exception if the assertion fails
     */
    private void testSetOpenedFlag() throws Exception {
        Log.info("testSetOpenedFlag");
        //Set the OPENED flag of the given mask to active 
        flags.setFlag(flags.OPENED, true);
        assertTrue(flags.isSet(flags.OPENED));
        Log.info("Succesfull");
    }

    /**
     * Test the setting method for ANSWERED flag
     * @throws Exception if the assertion fails
     */
    private void testSetAnsweredFlag() throws Exception {
        Log.info("testSetAnsweredFlag");
        //Set the ANSWERED flag of the given mask to active 
        flags.setFlag(flags.ANSWERED, true);
        assertTrue(flags.isSet(flags.ANSWERED));
        Log.info("Succesfull");
    }

    /**
     * Test the setting method for TX_ERROR flag
     * @throws Exception if the assertion fails
     */
    private void testSetTxErrorFlag() throws Exception {
        Log.info("testSetTxErrorFlag");
        //Set the TX_ERROR flag of the given mask to active 
        flags.setFlag(flags.TX_ERROR, true);
        assertTrue(flags.isSet(flags.TX_ERROR));
        Log.info("Succesfull");
    }

    /**
     * Test the setting method for TX_SENDING flag
     * @throws Exception if the assertion fails
     */
    private void testSetTxSendingFlag() throws Exception {
        Log.info("testSetTxSendingFlag");
        //Set the TX_SENDING flag of the given mask to active 
        flags.setFlag(flags.TX_SENDING, true);
        assertTrue(flags.isSet(flags.TX_SENDING));
        Log.info("Succesfull");
    }

    /**
     * Test the setting method for TX_SENT flag
     * @throws Exception if the assertion fails
     */
    private void testSetTxSentFlag() throws Exception {
        Log.info("testSetTxSentFlag");
        //Set the TX_SENT flag of the given mask to active 
        flags.setFlag(flags.TX_SENT, true);
        assertTrue(flags.isSet(flags.TX_SENT));
        Log.info("Succesfull");
    }

    /**
     * Test of isSet method for DELETED flag
     * @throws Exception if the assertion fails
     */
    public void testIsDeletedFlagSet() throws Exception {
        Log.info("testIsDeletedFlagSet");
        assertTrue(flags.isSet(flags.DELETED));
        Log.info("Succesfull");
    }

    /**
     * Test of isSet method for DRAFT flag
     * @throws Exception if the assertion fails
     */
    public void testIsDraftFlagSet() throws Exception {
        Log.info("testIsDraftFlagSet");
        assertTrue(flags.isSet(flags.DRAFT));
        Log.info("Succesfull");
    }

    /**
     * Test of isSet method for FLAGGED flag
     * @throws Exception if the assertion fails
     */
    public void testIsFlaggedFlagSet() throws Exception {
        Log.info("testIsFlaggedFlagSet");
        assertTrue(flags.isSet(flags.FLAGGED));
        Log.info("Succesfull");
    }

    /**
     * Test of isSet method for FORWARDED flag
     * @throws Exception if the assertion fails
     */
    public void testIsForwardedFlagSet() throws Exception {
        Log.info("testIsForwardedFlagSet");
        assertTrue(flags.isSet(flags.FORWARDED));
        Log.info("Succesfull");
    }

    /**
     * Test of isSet method for OPENED flag
     * @throws Exception if the assertion fails
     */
    public void testIsOpenedFlagSet() throws Exception {
        Log.info("testIsOpenedFlagSet");
        assertTrue(flags.isSet(flags.OPENED));
        Log.info("Succesfull");
    }

    /**
     * Test of isSet method for ANSWERED flag
     * @throws Exception if the assertion fails
     */
    public void testIsAnsweredFlagSet() throws Exception {
        Log.info("testIsAnsweredFlagSet");
        assertTrue(flags.isSet(flags.ANSWERED));
        Log.info("Succesfull");
    }

    /**
     * Test of isSet method for TX_ERROR flag
     * @throws Exception if the assertion fails
     */
    public void testIsTxErrorFlagSet() throws Exception {
        Log.info("testIsTxErrorFlagSet");
        assertTrue(flags.isSet(flags.TX_ERROR));
        Log.info("Succesfull");
    }
    
    /**
     * Test of isSet method for TX_SENDING flag
     * @throws Exception if the assertion fails
     */
    public void testIsTxSendingFlagSet() throws Exception {
        Log.info("testIsTxSendingFlagSet");
        assertTrue(flags.isSet(flags.TX_SENDING));
        Log.info("Succesfull");
    }

    /**
     * Test of isSet method for TX_SENT flag
     * @throws Exception if the assertion fails
     */
    public void testIsTxSentFlagSet() throws Exception {
        Log.info("testIsTxSentFlagSet");
        assertTrue(flags.isSet(flags.TX_SENT));
        Log.info("Succesfull");
    }
    
    /**
     * Test the merge method using to flags masks and merging them together
     * @throws jmunit.framework.cldc10.Exception
     */
    public void testMerge() throws Exception {
        Log.info("testMerge");
        MessageFlags mf1 = new MessageFlags();
        MessageFlags mf2 = new MessageFlags();
        mf1.setFlag(MessageFlags.OPENED, true);
        mf1.setFlag(MessageFlags.ANSWERED, true);
        mf1.setFlag(MessageFlags.FORWARDED, true);
        
        mf2.setFlag(MessageFlags.TX_ERROR, true);
        mf2.setFlag(MessageFlags.TX_SENDING, true);
        mf2.setFlag(MessageFlags.TX_SENT, true);
        
        Log.debug(mf1.toString());
        Log.debug(mf2.toString());
                
        mf1.merge(mf2);
        Log.debug(mf1.toString());
        assertTrue(
                mf1.isSet(MessageFlags.OPENED)&&
                mf1.isSet(MessageFlags.ANSWERED)&&
                mf1.isSet(MessageFlags.FORWARDED)&&
                mf1.isSet(MessageFlags.TX_ERROR)&&
                mf1.isSet(MessageFlags.TX_SENDING)&&
                mf1.isSet(MessageFlags.TX_SENT)&&
                !mf1.isSet(MessageFlags.FLAGGED)&&
                !mf1.isSet(MessageFlags.DELETED)&&
                !mf1.isSet(MessageFlags.DRAFT)&&
                !mf1.isSet(MessageFlags.PARTIAL)
                );
        Log.info("Succesfull");
    }

    /**
     * Test the toString Method
     * @throws jmunit.framework.cldc10.Exception
     */
    public void testToString() throws Exception {
        Log.info("testToString");
        Log.debug(FLAGS_STRING);
        Log.debug(flags.toString());
        assertTrue(flags.toString().equals(FLAGS_STRING));
        Log.info("Succesfull");
    }
    
    /**
     * Test the Serialize Method
     * @throws jmunit.framework.cldc10.Exception
     */
    public void testSerialize() throws Exception, IOException {
        Log.info("testSerialize");
        ByteArrayOutputStream exbaos = new ByteArrayOutputStream();
        DataOutputStream exdos = new DataOutputStream(exbaos);
        
        int exp = 
                MessageFlags.ANSWERED|
                MessageFlags.DELETED|
                MessageFlags.DRAFT|
                MessageFlags.FLAGGED|
                MessageFlags.FORWARDED|
                MessageFlags.OPENED|
                MessageFlags.PARTIAL|
                MessageFlags.TX_ERROR|
                MessageFlags.TX_SENDING|
                MessageFlags.TX_SENT;
                
        exdos.writeInt(exp);
        
        ByteArrayOutputStream resbaos = new ByteArrayOutputStream();
        DataOutputStream resdos = new DataOutputStream(resbaos);
        
        flags.serialize(resdos);
        
        byte[] expected = resbaos.toByteArray();
        byte[] result = exbaos.toByteArray();
        boolean areStreamEquals = false;
        for (int i=0; i<expected.length; i++) {
            areStreamEquals=expected[i]==result[i];
        } 
        
        assertTrue(areStreamEquals);
        Log.info("Succesfull");
    }
    
    /**
     * Test the Deserialize Method
     * @throws jmunit.framework.cldc10.Exception
     */
    public void testDeserialize() throws Exception, IOException {
        Log.info("testDeserialize");
        ByteArrayOutputStream baos = new ByteArrayOutputStream();
        DataOutputStream dos = new DataOutputStream(baos);
        flags.serialize(dos);
        
        ByteArrayInputStream resbais = new ByteArrayInputStream(baos.toByteArray());
        DataInputStream resdin = new DataInputStream(resbais);
        flags.deserialize(resdin);
        
        int exp = 
                MessageFlags.ANSWERED|
                MessageFlags.DELETED|
                MessageFlags.DRAFT|
                MessageFlags.FLAGGED|
                MessageFlags.FORWARDED|
                MessageFlags.OPENED|
                MessageFlags.PARTIAL|
                MessageFlags.TX_ERROR|
                MessageFlags.TX_SENDING|
                MessageFlags.TX_SENT;
        
        assertEquals(exp, flags.getFlags());
        Log.info("Succesfull");
    }
    
    /**
     * Test the setting method for PARTIAL flag
     * @throws Exception if the assertion fails
     */
    private void testSetPartialFlag() throws Exception {
        Log.info("testSetFlaggedFlag");
        //Set the FLAGGED flag of the given mask to active 
        flags.setFlag(flags.PARTIAL, true);
        assertTrue(flags.isSet(flags.PARTIAL));
        Log.info("Succesfull");
    }

    /**
     * Test of isSet method for PARTIAL flag
     * @throws Exception if the assertion fails
     */
    public void testIsPartialFlagSet() throws Exception {
        Log.info("testIsForwardedFlagSet");
        assertTrue(flags.isSet(flags.PARTIAL));
        Log.info("Succesfull");
    }

}

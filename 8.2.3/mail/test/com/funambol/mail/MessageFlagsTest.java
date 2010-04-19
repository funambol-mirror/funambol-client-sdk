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

import java.io.ByteArrayInputStream;
import java.io.ByteArrayOutputStream;
import java.io.DataInputStream;
import java.io.DataOutputStream;
import java.io.IOException;
import junit.framework.*;

import com.funambol.util.ConsoleAppender;
import com.funambol.util.Log;

/**
 * Public class to test message flags behavior
 */
public class  MessageFlagsTest extends TestCase {

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
    public MessageFlagsTest(String name) {
        super(name);
        Log.initLog(new ConsoleAppender());
        Log.setLogLevel(Log.INFO);
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
     * Test of clearFlags method, of class MessageFlags
     * @param flags is the given flags mask
     * @throws Exception if the assertion fails
     */
    public void testClearFlags() throws Exception {
        Log.info("MessageFlagsTest: testClearFlags");
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
        Log.info("MessageFlagsTest: testClearFlags successful");
    }

    /**
     * Test the setting method for DELETED flag
     * @throws Exception if the assertion fails
     */
    public void testSetDeletedFlag() throws Exception {
        Log.info("MessageFlagsTest: testSetDeletedFlag");
        //Set the DELETED flag of the given mask to active 
        flags.setFlag(flags.DELETED, true);
        assertTrue(flags.isSet(flags.DELETED));
        Log.info("MessageFlagsTest: testSetDeletedFlag successful");
    }
    
    /**
     * Test the setting method for DRAFT flag
     * @throws Exception if the assertion fails
     */
    public void testSetDraftFlag() throws Exception {
        Log.info("MessageFlagsTest: testSetDraftFlag");
        //Set the DRAFT flag of the given mask to active 
        flags.setFlag(flags.DRAFT, true);
        assertTrue(flags.isSet(flags.DRAFT));
        Log.info("MessageFlagsTest: testSetDraftFlag successful");
    }

    /**
     * Test the setting method for FLAGGED flag
     * @throws Exception if the assertion fails
     */
    public void testSetFlaggedFlag() throws Exception {
        Log.info("MessageFlagsTest: testSetFlaggedFlag");
        //Set the FLAGGED flag of the given mask to active 
        flags.setFlag(flags.FLAGGED, true);
        assertTrue(flags.isSet(flags.FLAGGED));
        Log.info("MessageFlagsTest: testSetFlaggedFlag successful");
    }

    /**
     * Test the setting method for FORWARDED flag
     * @throws Exception if the assertion fails
     */
    public void testSetForwardedFlag() throws Exception {
        Log.info("MessageFlagsTest: testSetForwardedFlag");
        //Set the FORWARDED flag of the given mask to active 
        flags.setFlag(flags.FORWARDED, true);
        assertTrue(flags.isSet(flags.FORWARDED));
        Log.info("MessageFlagsTest: testSetForwardedFlag successful");
    }

    /**
     * Test the setting method for OPENED flag
     * @throws Exception if the assertion fails
     */
    public void testSetOpenedFlag() throws Exception {
        Log.info("MessageFlagsTest: testSetOpenedFlag");
        //Set the OPENED flag of the given mask to active 
        flags.setFlag(flags.OPENED, true);
        assertTrue(flags.isSet(flags.OPENED));
        Log.info("MessageFlagsTest: testSetOpenedFlag successful");
    }

    /**
     * Test the setting method for ANSWERED flag
     * @throws Exception if the assertion fails
     */
    public void testSetAnsweredFlag() throws Exception {
        Log.info("MessageFlagsTest: testSetAnsweredFlag");
        //Set the ANSWERED flag of the given mask to active 
        flags.setFlag(flags.ANSWERED, true);
        assertTrue(flags.isSet(flags.ANSWERED));
        Log.info("MessageFlagsTest: testSetAnsweredFlag successful");
    }

    /**
     * Test the setting method for TX_ERROR flag
     * @throws Exception if the assertion fails
     */
    public void testSetTxErrorFlag() throws Exception {
        Log.info("MessageFlagsTest: testSetTxErrorFlag");
        //Set the TX_ERROR flag of the given mask to active 
        flags.setFlag(flags.TX_ERROR, true);
        assertTrue(flags.isSet(flags.TX_ERROR));
        Log.info("MessageFlagsTest: testSetTxErrorFlag successful");
    }

    /**
     * Test the setting method for TX_SENDING flag
     * @throws Exception if the assertion fails
     */
    public void testSetTxSendingFlag() throws Exception {
        Log.info("MessageFlagsTest: testSetTxSendingFlag");
        //Set the TX_SENDING flag of the given mask to active 
        flags.setFlag(flags.TX_SENDING, true);
        assertTrue(flags.isSet(flags.TX_SENDING));
        Log.info("MessageFlagsTest: testSetTxSendingFlag successful");
    }

    /**
     * Test the setting method for TX_SENT flag
     * @throws Exception if the assertion fails
     */
    public void testSetTxSentFlag() throws Exception {
        Log.info("MessageFlagsTest: testSetTxSentFlag");
        //Set the TX_SENT flag of the given mask to active 
        flags.setFlag(flags.TX_SENT, true);
        assertTrue(flags.isSet(flags.TX_SENT));
        Log.info("MessageFlagsTest: testSetTxSentFlag successful");
    }

    /**
     * Test of isSet method for DELETED flag
     * @throws Exception if the assertion fails
     */
    public void testIsDeletedFlagSet() throws Exception {
        Log.info("MessageFlagsTest: testIsDeletedFlagSet");
        assertTrue(flags.isSet(flags.DELETED));
        Log.info("MessageFlagsTest: testIsDeletedFlagSet successful");
    }

    /**
     * Test of isSet method for DRAFT flag
     * @throws Exception if the assertion fails
     */
    public void testIsDraftFlagSet() throws Exception {
        Log.info("MessageFlagsTest: testIsDraftFlagSet");
        assertTrue(flags.isSet(flags.DRAFT));
        Log.info("MessageFlagsTest: testIsDraftFlagSet successful");
    }

    /**
     * Test of isSet method for FLAGGED flag
     * @throws Exception if the assertion fails
     */
    public void testIsFlaggedFlagSet() throws Exception {
        Log.info("MessageFlagsTest: testIsFlaggedFlagSet");
        assertTrue(flags.isSet(flags.FLAGGED));
        Log.info("MessageFlagsTest: testIsFlaggedFlagSet successful");
    }

    /**
     * Test of isSet method for FORWARDED flag
     * @throws Exception if the assertion fails
     */
    public void testIsForwardedFlagSet() throws Exception {
        Log.info("MessageFlagsTest: testIsForwardedFlagSet");
        assertTrue(flags.isSet(flags.FORWARDED));
        Log.info("MessageFlagsTest: testIsForwardedFlagSet successful");
    }

    /**
     * Test of isSet method for OPENED flag
     * @throws Exception if the assertion fails
     */
    public void testIsOpenedFlagSet() throws Exception {
        Log.info("MessageFlagsTest: testIsOpenedFlagSet");
        assertTrue(flags.isSet(flags.OPENED));
        Log.info("MessageFlagsTest: testIsOpenedFlagSet successful");
    }

    /**
     * Test of isSet method for ANSWERED flag
     * @throws Exception if the assertion fails
     */
    public void testIsAnsweredFlagSet() throws Exception {
        Log.info("MessageFlagsTest: testIsAnsweredFlagSet");
        assertTrue(flags.isSet(flags.ANSWERED));
        Log.info("MessageFlagsTest: testIsAnsweredFlagSet successful");
    }

    /**
     * Test of isSet method for TX_ERROR flag
     * @throws Exception if the assertion fails
     */
    public void testIsTxErrorFlagSet() throws Exception {
        Log.info("MessageFlagsTest: testIsTxErrorFlagSet");
        assertTrue(flags.isSet(flags.TX_ERROR));
        Log.info("MessageFlagsTest: testIsTxErrorFlagSet successful");
    }
    
    /**
     * Test of isSet method for TX_SENDING flag
     * @throws Exception if the assertion fails
     */
    public void testIsTxSendingFlagSet() throws Exception {
        Log.info("MessageFlagsTest: testIsTxSendingFlagSet");
        assertTrue(flags.isSet(flags.TX_SENDING));
        Log.info("MessageFlagsTest: testIsTxSendingFlagSet successful");
    }

    /**
     * Test of isSet method for TX_SENT flag
     * @throws Exception if the assertion fails
     */
    public void testIsTxSentFlagSet() throws Exception {
        Log.info("MessageFlagsTest: testIsTxSentFlagSet");
        assertTrue(flags.isSet(flags.TX_SENT));
        Log.info("MessageFlagsTest: testIsTxSentFlagSet successful");
    }
    
    /**
     * Test the merge method using to flags masks and merging them together
     * @throws jmunit.framework.cldc10.Exception
     */
    public void testMerge() throws Exception {
        Log.info("MessageFlagsTest: testMerge");
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
                !mf1.isSet(MessageFlags.OPENED)&&
                !mf1.isSet(MessageFlags.ANSWERED)&&
                !mf1.isSet(MessageFlags.FORWARDED)&&
                mf1.isSet(MessageFlags.TX_ERROR)&&
                mf1.isSet(MessageFlags.TX_SENDING)&&
                mf1.isSet(MessageFlags.TX_SENT)&&
                !mf1.isSet(MessageFlags.FLAGGED)&&
                !mf1.isSet(MessageFlags.DELETED)&&
                !mf1.isSet(MessageFlags.DRAFT)&&
                !mf1.isSet(MessageFlags.PARTIAL)
                );
        Log.info("MessageFlagsTest: testMerge successful");
    }

    /**
     * Test the toString Method
     * @throws jmunit.framework.cldc10.Exception
     */
    public void testToString() throws Exception {
        Log.info("MessageFlagsTest: testToString");
        Log.debug(FLAGS_STRING);
        Log.debug(flags.toString());
        assertTrue(flags.toString().equals(FLAGS_STRING));
        Log.info("MessageFlagsTest: testToString successful");
    }
    
    /**
     * Test the Serialize Method
     * @throws jmunit.framework.cldc10.Exception
     */
    public void testSerialize() throws Exception, IOException {
        Log.info("MessageFlagsTest: testSerialize");
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
        Log.info("MessageFlagsTest: testSerialize successful");
    }
    
    /**
     * Test the Deserialize Method
     * @throws jmunit.framework.cldc10.Exception
     */
    public void testDeserialize() throws Exception, IOException {
        Log.info("MessageFlagsTest: testDeserialize");
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
        Log.info("MessageFlagsTest: testDeserialize successful");
    }
    
    /**
     * Test the setting method for PARTIAL flag
     * @throws Exception if the assertion fails
     */
    public void testSetPartialFlag() throws Exception {
        Log.info("MessageFlagsTest: testSetFlaggedFlag");
        //Set the FLAGGED flag of the given mask to active 
        flags.setFlag(flags.PARTIAL, true);
        assertTrue(flags.isSet(flags.PARTIAL));
        Log.info("MessageFlagsTest: testSetFlaggedFlag successful");
    }

    /**
     * Test of isSet method for PARTIAL flag
     * @throws Exception if the assertion fails
     */
    public void testIsPartialFlagSet() throws Exception {
        Log.info("MessageFlagsTest: testIsForwardedFlagSet");
        assertTrue(flags.isSet(flags.PARTIAL));
        Log.info("MessageFlagsTest: testIsForwardedFlagSet successful");
    }

}

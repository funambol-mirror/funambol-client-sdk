/*
 * Funambol is a mobile platform developed by Funambol, Inc. 
 * Copyright (C) 2008 Funambol, Inc.
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

# include <cppunit/extensions/TestFactoryRegistry.h>
# include <cppunit/extensions/HelperMacros.h>

#include "base/util/StringBuffer.h"

#include "syncml/formatter/Formatter.h"
#include "syncml/parser/Parser.h"
#include "base/util/utils.h"
#include "base/globalsdef.h"

#include "syncml/core/ObjectDel.h"

USE_NAMESPACE

/**
 * This is the test class for the SyncML Parser.
 */
class ObjectDelTest : public CppUnit::TestFixture {

    CPPUNIT_TEST_SUITE(ObjectDelTest);
    CPPUNIT_TEST(testDelSingleStringBuffer);
    CPPUNIT_TEST(testDelMultipleStringBuffer);
    CPPUNIT_TEST(testSingleNotNullCheck);
    CPPUNIT_TEST(testMultipleNotNullCheck);
    CPPUNIT_TEST(testSingleNotZeroCheck);
    CPPUNIT_TEST(testMultipleNotZeroCheck);
    CPPUNIT_TEST(testSingleNotZeroArrayLength);
    CPPUNIT_TEST(testMultipleNotZeroArrayLength);
    CPPUNIT_TEST(testSingleNotZeroStringBufferLength);
    CPPUNIT_TEST(testMultipleNotZeroStringBufferLength);
    CPPUNIT_TEST_SUITE_END();

private:

    void testDelSingleStringBuffer() {
        StringBuffer* sb = new StringBuffer();
        deleteAllStringBuffer(1, &sb);
        CPPUNIT_ASSERT(sb == NULL);
    }

    void testDelMultipleStringBuffer() {
        StringBuffer* sb[16];
        
        for(int i=0;i<16;++i) {
            sb[i] = new StringBuffer();
        }

        deleteAllStringBuffer(16, &sb[0], &sb[1], &sb[2], &sb[3], &sb[4], &sb[5],
                                  &sb[6], &sb[7], &sb[8], &sb[9], &sb[10], &sb[11],
                                  &sb[12], &sb[13], &sb[14], &sb[15]);

        for(int i=0;i<16;++i) {
            CPPUNIT_ASSERT(sb[i] == NULL);
        }
    }

    void testSingleNotNullCheck() {
        char c;

        CPPUNIT_ASSERT(!NotNullCheck(1, NULL));
        CPPUNIT_ASSERT(NotNullCheck(1, &c));
    }

    void testMultipleNotNullCheck() {
        char c;

        CPPUNIT_ASSERT(!NotNullCheck(4, NULL, NULL, NULL, NULL));
        CPPUNIT_ASSERT(NotNullCheck(4, &c, NULL, NULL, NULL));
        CPPUNIT_ASSERT(NotNullCheck(4, NULL, NULL, &c, NULL));
        CPPUNIT_ASSERT(NotNullCheck(4, NULL, NULL, NULL, &c));
    }

    void testSingleNotZeroCheck() {
        CPPUNIT_ASSERT(!NotZeroCheck(1, 0));
        CPPUNIT_ASSERT(NotZeroCheck(1, 1));
    }

    void testMultipleNotZeroCheck() {
        CPPUNIT_ASSERT(!NotZeroCheck(4, 0, 0, 0, 0));
        CPPUNIT_ASSERT(NotZeroCheck(4, 1, 0, 0, 0));
        CPPUNIT_ASSERT(NotZeroCheck(4, 0, 0, 1, 0));
        CPPUNIT_ASSERT(NotZeroCheck(4, 0, 0, 0, 1));
    }

    void testSingleNotZeroArrayLength() {
        ArrayList emptyList;
        ArrayList list;
        StringBuffer sb;
        list.add(sb);

        CPPUNIT_ASSERT(!NotZeroArrayLength(1, &emptyList));
        CPPUNIT_ASSERT(NotZeroArrayLength(1, &list));
    }

    void testMultipleNotZeroArrayLength() {

        ArrayList emptyList;
        ArrayList list;
        StringBuffer sb;
        list.add(sb);

        CPPUNIT_ASSERT(!NotZeroArrayLength(3, &emptyList, &emptyList, &emptyList));
        CPPUNIT_ASSERT(NotZeroArrayLength(3, &list, &emptyList, &emptyList));
        CPPUNIT_ASSERT(NotZeroArrayLength(3, &emptyList, &list, &emptyList));
        CPPUNIT_ASSERT(NotZeroArrayLength(3, &emptyList, &emptyList, &list));
    }

    void testSingleNotZeroStringBufferLength() {
        StringBuffer emptySb("");
        StringBuffer zeroSb("");
        StringBuffer sb("StringBuffer");

        CPPUNIT_ASSERT(!NotZeroStringBufferLength(1, &emptySb));
        CPPUNIT_ASSERT(!NotZeroStringBufferLength(1, &zeroSb));
        CPPUNIT_ASSERT(NotZeroStringBufferLength(1, &sb));
    }

    void testMultipleNotZeroStringBufferLength() {

        StringBuffer emptySb("");
        StringBuffer zeroSb("");
        StringBuffer sb("StringBuffer");

        CPPUNIT_ASSERT(!NotZeroStringBufferLength(3, &emptySb, &emptySb, &emptySb));
        CPPUNIT_ASSERT(!NotZeroStringBufferLength(3, &zeroSb, &zeroSb, &zeroSb));
        CPPUNIT_ASSERT(!NotZeroStringBufferLength(3, &emptySb, &zeroSb, &zeroSb));

        CPPUNIT_ASSERT(NotZeroStringBufferLength(3, &sb, &emptySb, &emptySb));
        CPPUNIT_ASSERT(NotZeroStringBufferLength(3, &zeroSb, &sb, &emptySb));
        CPPUNIT_ASSERT(NotZeroStringBufferLength(3, &emptySb, &zeroSb, &sb));
    }
};

CPPUNIT_TEST_SUITE_REGISTRATION( ObjectDelTest );



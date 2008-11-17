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

#include "base/util/utils.h"
#include "base/util/StringBuffer.h"

#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/extensions/HelperMacros.h>
#include "base/globalsdef.h"

USE_NAMESPACE


/**
 * Test case for the class StringBuffer.
 */
class StringBufferTest : public CppUnit::TestFixture {
    CPPUNIT_TEST_SUITE(StringBufferTest);
    CPPUNIT_TEST(testConstruct);
    CPPUNIT_TEST(testCompare);
    CPPUNIT_TEST(testAssign);
    CPPUNIT_TEST(testConvert);
    CPPUNIT_TEST(testSprintf);
    CPPUNIT_TEST(testReset);
    CPPUNIT_TEST(testLength);
    CPPUNIT_TEST(testEndsWith);
    CPPUNIT_TEST_SUITE_END();

private:

    ///////////////////////////////////////////////////////// Test /////
    // test the different constructor and the c_str() method.
    void testConstruct() {
        StringBuffer s1;
        CPPUNIT_ASSERT(s1.c_str() == NULL);

        StringBuffer s2("");
        CPPUNIT_ASSERT(strcmp(s2.c_str(), "") == 0);

        StringBuffer s3("test");
        CPPUNIT_ASSERT(strcmp(s3.c_str(), "test") == 0);

        StringBuffer s4("0123456789", 4);
        CPPUNIT_ASSERT(strcmp(s4.c_str(), "0123") == 0);

        StringBuffer copy(s3);
        CPPUNIT_ASSERT(strcmp(copy, s3) == 0);
    }


    //////////////////////////////////////////////////////// Test /////
    // test the comparison operators and methods
    void testCompare() {
        StringBuffer s("Test");

        CPPUNIT_ASSERT(s == "Test");
        CPPUNIT_ASSERT(s != "tEST");
        CPPUNIT_ASSERT(s.icmp("tEST"));
    }

    //////////////////////////////////////////////////////// Test /////
    // test assignment operators and methods
    void testAssign() {
        StringBuffer s;

        s.assign("First test string");
        CPPUNIT_ASSERT(s == "First test string");

        s = "Second test string";
        CPPUNIT_ASSERT(s == "Second test string");
    }

    //////////////////////////////////////////////////////// Test /////
    // test WCHAR converison

#define TEST_STRING  "Quant'è bella giovinezza.."

    void testConvert() {

        StringBuffer str(TEST_STRING);

        char* toUtf8   = toMultibyte(TEXT(TEST_STRING));
        char* toLatin1 = toMultibyte(TEXT(TEST_STRING), "iso_8859-1");

        StringBuffer cnv;
        cnv.convert(TEXT(TEST_STRING));
        fprintf(stderr, "\nConverted string: %s\n", cnv.c_str());
        CPPUNIT_ASSERT((strcmp(cnv.c_str(), toUtf8) == 0));

        cnv.convert(TEXT(TEST_STRING), "iso_8859-1");
        fprintf(stderr, "\nConverted string: %s\n", cnv.c_str());
        CPPUNIT_ASSERT((strcmp(cnv.c_str(), toLatin1) == 0));
    }

    //////////////////////////////////////////////////////// Test /////
    void testSprintf() {
        StringBuffer buf;

        buf.sprintf("foo %s %d", "bar", 42);
        CPPUNIT_ASSERT(buf == "foo bar 42");

        buf = doSprintf("foo %s %d", "bar", 42);
        CPPUNIT_ASSERT(buf == "foo bar 42");

        for (unsigned long size = 1; size < (1<<10); size *= 2) {
            buf.sprintf("%*s", (int)size, "");
            CPPUNIT_ASSERT_EQUAL(size, buf.length());
        }
    }

    //////////////////////////////////////////////////////// Test /////
    void testReset() {
        StringBuffer s("Test reset");
        s.reset();
        CPPUNIT_ASSERT(s.c_str() == NULL);
    }

    //////////////////////////////////////////////////////// Test /////
    void testLength() {
        StringBuffer s("Test length");
        CPPUNIT_ASSERT_EQUAL((unsigned long)strlen(s.c_str()), s.length());
    }
    
    //////////////////////////////////////////////////////// Test /////
    void testUpperCase() {
        StringBuffer s("Test Uppercase");
        StringBuffer &ref = s.upperCase();

        CPPUNIT_ASSERT(s == "TEST UPPERCASE");
        CPPUNIT_ASSERT(ref == s);
    }

    //////////////////////////////////////////////////////// Test /////
    void testLowerCase() {
        StringBuffer s("Test LowerCase");
        StringBuffer &ref = s.lowerCase();

        CPPUNIT_ASSERT(s == "test lowercase");
        CPPUNIT_ASSERT(ref == s);
    }

    //////////////////////////////////////////////////////// Test /////
    // Test null() and empty() behavior
    void testEmpty() {
        StringBuffer s;
        CPPUNIT_ASSERT(s.empty());
        CPPUNIT_ASSERT(s.null());

        s = "";

        CPPUNIT_ASSERT( s.empty() );
        CPPUNIT_ASSERT( !s.null() );

    }

    //////////////////////////////////////////////////////// Test /////
    //
    void testClone() {
        StringBuffer s("Test Clone");

        StringBuffer* cloned = (StringBuffer*)s.clone();

        CPPUNIT_ASSERT_EQUAL( s, *cloned);

        delete cloned;
    }

    //////////////////////////////////////////////////////// Test /////
    // Test null() and empty() behavior
    void testEndsWith() {
        StringBuffer s("Test Ends With");

        CPPUNIT_ASSERT(s.endsWith('h'));
        CPPUNIT_ASSERT(s.endsWith(" With"));
        CPPUNIT_ASSERT(!s.endsWith('t'));
        CPPUNIT_ASSERT(!s.endsWith("with"));

        StringBuffer s2("th");

        CPPUNIT_ASSERT(!s2.endsWith("with"));
        CPPUNIT_ASSERT(!s2.endsWith(""));

        StringBuffer s3;
        CPPUNIT_ASSERT(!s3.endsWith('t'));
        CPPUNIT_ASSERT(!s3.endsWith("t"));
    }


    //----------------------------------------- Utility functions
    StringBuffer doSprintf(const char* format, ...) {
        va_list ap;
        StringBuffer buf;

        va_start(ap, format);
        buf.vsprintf(format, ap);
        va_end(ap);

        return buf;
    }
};

CPPUNIT_TEST_SUITE_REGISTRATION( StringBufferTest );


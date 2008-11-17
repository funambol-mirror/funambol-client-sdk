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

#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/extensions/HelperMacros.h>

#include "base/fscapi.h"
#include "base/util/StringMap.h"

BEGIN_NAMESPACE

/**
 * This is the test class for StringMap.
 */
class StringMapTest : public CppUnit::TestFixture {
    CPPUNIT_TEST_SUITE(StringMapTest);
    CPPUNIT_TEST(testGet);
    CPPUNIT_TEST(testInsert);
    CPPUNIT_TEST(testUpdate);
    CPPUNIT_TEST(testErase);
    CPPUNIT_TEST(testClear);
    CPPUNIT_TEST(testManyItems);
    CPPUNIT_TEST_SUITE_END();

public:

#define TESTKEY "testkey"
#define TESTVAL "testvalue"

    void setUp() {
		sm.put("key1", "value1");
		sm.put("key2", "value2");
		sm.put("key3", "value3");
    }

    void tearDown() {
		sm.clear();
    }
    
    /** Test the retrieval of elements from the StringMap. */
	void testGet() {
		CPPUNIT_ASSERT(sm.get("key1") == "value1");
		CPPUNIT_ASSERT(sm["key2"] == "value2");
		CPPUNIT_ASSERT(sm["bogus"].null());
		CPPUNIT_ASSERT(sm[NULL].null());
		CPPUNIT_ASSERT(sm[""].null());
	}

    /* Test the insertion a new item in the StringMap */
    void testInsert() {
		sm.put(TESTKEY, TESTVAL);
		CPPUNIT_ASSERT(sm[TESTKEY] == TESTVAL);
    }

    /* Test the modification an item in the StringMap */
	void testUpdate() {
		sm.put(TESTKEY, "newvalue");
		CPPUNIT_ASSERT(sm[TESTKEY] == "newvalue");
	}

    /* Test the deletion of an item from the StringMap */
    void testErase() {
		sm.remove(TESTKEY);
		CPPUNIT_ASSERT(sm[TESTKEY].null());
    }

	/* Test the clear method of the StringMap */
	void testClear() {
		sm.clear();
		CPPUNIT_ASSERT(sm["key1"].null());
	}

	void testManyItems() {
		const int max = 2000;
		int i;
		StringBuffer k, v;
		clock_t start = clock();

		// insert many items
		for (i=1; i<max; i++) {
			k.sprintf("key%d", i);
			v.sprintf("val%d", i);

			sm.put(k, v);
			CPPUNIT_ASSERT(sm[k] == v);
		}
		printf("\n\tInsert: %fs", (float)(clock()-start)/CLOCKS_PER_SEC);

		start=clock();
		for (i=5; i<max; i+=5) {
			// change some of them (5,10,15,...)
			k.sprintf("key%d", i);
			v.sprintf("newval%d", i);
			sm.put(k, v);
			CPPUNIT_ASSERT(sm[k] == v);
					
			// remove some others (3,8,13,...)
			k.sprintf("key%d", i-2);
			sm.remove(k);
			CPPUNIT_ASSERT(sm[k].null());

			// and add again (50,100,150,...)
			k.sprintf("key%d", i*10);
			v.sprintf("val%d", i*10);
			sm.put(k, v);
			CPPUNIT_ASSERT(sm[k] == v);
		}
		printf("\n\tCheck: %fs\n", (float)(clock()-start)/CLOCKS_PER_SEC);

	}

private:

	StringMap sm;

};

CPPUNIT_TEST_SUITE_REGISTRATION( StringMapTest );

END_NAMESPACE



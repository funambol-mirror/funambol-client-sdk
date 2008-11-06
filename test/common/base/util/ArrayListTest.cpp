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

# include <cppunit/extensions/TestFactoryRegistry.h>
# include <cppunit/extensions/HelperMacros.h>

#include "base/util/StringBuffer.h"
#include "base/util/ArrayList.h"
#include "base/globalsdef.h"

USE_NAMESPACE

class ArrayListTest : public CppUnit::TestFixture {
    CPPUNIT_TEST_SUITE(ArrayListTest);
    CPPUNIT_TEST(removeFirst);
    CPPUNIT_TEST(removeLast);
    CPPUNIT_TEST(removeMiddle);
    CPPUNIT_TEST(removeAll);
    CPPUNIT_TEST(getLast);
    CPPUNIT_TEST_SUITE_END();

public:
    void setUp() {
        StringBuffer a("a"), b("b"), c("c");

        abc.add(a);
        abc.add(b);
        abc.add(c);

        bc.add(b);
        bc.add(c);

        ac.add(a);
        ac.add(c);
        
        ab.add(a);
        ab.add(b);
    }
    void tearDown() {
        abc.clear();
        bc.clear();
        ac.clear();
        ab.clear();
    }

private:
    void removeFirst() {
        ArrayList l = abc;

        l.removeElementAt(0);
        CPPUNIT_ASSERT_EQUAL(2, l.size());
        CPPUNIT_ASSERT(equal(l, bc));
    }

    void removeMiddle() {
        ArrayList l = abc;

        l.removeElementAt(1);
        CPPUNIT_ASSERT_EQUAL(2, l.size());
        CPPUNIT_ASSERT(equal(l, ac));
    }

    void removeLast() {
        ArrayList l = abc;

        l.removeElementAt(2);
        CPPUNIT_ASSERT_EQUAL(2, l.size());
        CPPUNIT_ASSERT(equal(l, ab));
    }

    void removeAll() {
        ArrayList l = abc;
        CPPUNIT_ASSERT_EQUAL(0, l.removeElementAt(0));
        CPPUNIT_ASSERT_EQUAL(1, l.removeElementAt(1));
        CPPUNIT_ASSERT_EQUAL(0, l.removeElementAt(0));
        CPPUNIT_ASSERT(equal(l, empty));
    }

    void getLast() {
        ArrayList l = abc;       
        CPPUNIT_ASSERT_EQUAL(false, l.last());
        l.next();
        CPPUNIT_ASSERT_EQUAL(false, l.last());
        l.next();
        CPPUNIT_ASSERT_EQUAL(false, l.last());
        l.next();
        CPPUNIT_ASSERT_EQUAL(true, l.last());       
    }
    
    bool equal(ArrayList &first, ArrayList &second) {
        ArrayElement *first_e = first.front();
        int index = 0;
        ArrayElement *second_e = second.get(index);
        while (true) {
            if (!first_e && !second_e) {
                // end of both lists
                return true;
            }

            if (first_e && !second_e ||
                !first_e && second_e) {
                // different length
                return false;
            }

            if (*(StringBuffer *)first_e != *(StringBuffer *)second_e) {
                // different content
                return false;
            }

            first_e = first.next();
            index++;
            second_e = second.get(index);
        }
    }

    ArrayList abc, bc, ac, ab, empty;
};

CPPUNIT_TEST_SUITE_REGISTRATION( ArrayListTest );

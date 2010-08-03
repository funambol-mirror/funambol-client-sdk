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
    CPPUNIT_TEST(testAddGet);
    CPPUNIT_TEST(removeFirst);
    CPPUNIT_TEST(removeLast);
    CPPUNIT_TEST(removeMiddle);
    CPPUNIT_TEST(removeAll);
    CPPUNIT_TEST(getLast);
    CPPUNIT_TEST(iterateAndDelete);
    CPPUNIT_TEST(iterateAndDelete2);
    CPPUNIT_TEST(iterateAndAddDelete);
    CPPUNIT_TEST(testManyItems);
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

    void testAddGet() {
        ArrayList al;
        StringBuffer test("test");

        al.add(test);
        CPPUNIT_ASSERT(*(StringBuffer *)al.get(0) == test);
        CPPUNIT_ASSERT(*(StringBuffer *)al[0] == test);
        CPPUNIT_ASSERT(al[1] == NULL);
    }

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

    void iterateAndDelete() {
        StringBuffer a("a"), b("b"), c("c");
        ArrayList al;
        al.add(a);
        al.add(b);
        al.add(c);

        // Now iterate and delete the first and the last one
        ArrayElement* item = al.front();
        al.removeElementAt(0);
        item = al.next();
        al.next();
        al.removeElementAt(1);
        CPPUNIT_ASSERT(al.last());

        // Now remove also the last one
        al.removeElementAt(0);
        CPPUNIT_ASSERT(al.isEmpty());
    }

    void iterateAndDelete2() {
        StringBuffer a("a"), b("b"), c("c");
        ArrayList al;
        al.add(a);
        al.add(b);
        al.add(c);

        // Now iterate and delete the second item
        ArrayElement* item = al.front();
        item = al.next();
        al.removeElementAt(1);
        // We should have a previous and next
        item = al.prev();
        CPPUNIT_ASSERT(item && *((StringBuffer *)item) == a);
        item = al.next();
        CPPUNIT_ASSERT(item && *((StringBuffer *)item) == c);

        // Now remove the head
        item = al.front();
        al.removeElementAt(0);
        item = al.prev();
        CPPUNIT_ASSERT(item == NULL);
        item = al.next();
        CPPUNIT_ASSERT(item && *((StringBuffer *)item) == c);

        // Now remove the tail
        al.removeElementAt(0);
        item = al.prev();
        CPPUNIT_ASSERT(item == NULL);
        item = al.next();
        CPPUNIT_ASSERT(item == NULL);
        CPPUNIT_ASSERT(al.last());
    }

    void iterateAndAddDelete() {
        StringBuffer a("a"), b("b"), c("c");
        ArrayList al;
        al.add(a);
        al.add(b);
        al.add(c);

        // Now iterate and delete the second item
        ArrayElement* item = al.front();
        item = al.next();
        al.removeElementAt(1);
        // Replace it
        al.add(1, a);
        // Now the iterator should we on the second "a"
        // (we have "aac")
        item = al.prev();
        CPPUNIT_ASSERT(item && *((StringBuffer *)item) == a);
        item = al.next();
        CPPUNIT_ASSERT(item && *((StringBuffer *)item) == c);

        // Replace the first "a" and get "bac"
        al.front();
        al.next();
        al.removeElementAt(0);
        al.add(0, b);
        // Check that the iterator is still OK
        item = al.prev();
        CPPUNIT_ASSERT(item && *((StringBuffer *)item) == b);
        item = al.next();
        CPPUNIT_ASSERT(item && *((StringBuffer *)item) == c);

        // Now replace the last item (and get bab)
        al.front();
        al.next();
        al.removeElementAt(2);
        al.add(b);
        item = al.prev();
        CPPUNIT_ASSERT(item && *((StringBuffer *)item) == b);
        item = al.next();
        CPPUNIT_ASSERT(item && *((StringBuffer *)item) == b);
    }

    void testManyItems() {
        ArrayList many;

        const int max = 10000;
        int i;
        StringBuffer s;
        clock_t start = clock();

        // insert many items
        for (i=0; i<max; i++) {
            s.sprintf("item%d", i);
            many.add(s);
        }
        printf("\n\tInsert: %fs", (float)(clock()-start)/CLOCKS_PER_SEC);

        // access them using index
        start=clock();
        for (i=0; i<max; i++) {
            s.sprintf("item%d", i);
            CPPUNIT_ASSERT(*(StringBuffer *)many[i] == s);
        }
        printf("\n\tAccess index: %fs", (float)(clock()-start)/CLOCKS_PER_SEC);

        // access them using iterator methods
        StringBuffer *p;
        start=clock();
        for (i=0, p=(StringBuffer*)many.front(); p; p=(StringBuffer*)many.next(), i++) {
            s.sprintf("item%d", i);
            CPPUNIT_ASSERT(*p == s);
        }
        printf("\n\tAccess iterator methods: %fs", (float)(clock()-start)/CLOCKS_PER_SEC);

        // remove them 
        start=clock();
        for (i=1; i<max; i++) {
            many.removeElementAt(i);
        }
        printf("\n\tRemove: %fs", (float)(clock()-start)/CLOCKS_PER_SEC);
    }

    ArrayList abc, bc, ac, ab, empty;
};

CPPUNIT_TEST_SUITE_REGISTRATION( ArrayListTest );

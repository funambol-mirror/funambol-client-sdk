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

#include "base/util/BasicTime.h"
#include "base/globalsdef.h"

USE_NAMESPACE

# include <cppunit/extensions/TestFactoryRegistry.h>
# include <cppunit/extensions/HelperMacros.h>

class BasicTimeTest : public CppUnit::TestFixture {
    CPPUNIT_TEST_SUITE(BasicTimeTest);
    CPPUNIT_TEST(testEqual);
    CPPUNIT_TEST(testConversion);
    CPPUNIT_TEST_SUITE_END();

public:
    void setUp() {
        // millenium.set(2000, 01, 01, 6,
        //              00, 00, 00,
        //              00, 00);
        millenium.setYear(2000);
        buffer = NULL;
    }
    void tearDown() {
        if (buffer) {
            delete [] buffer;
        }
    }

protected:
    void testEqual() {
        BasicTime empty;
        CPPUNIT_ASSERT(empty != millenium);

        BasicTime copy(millenium);
        CPPUNIT_ASSERT(millenium == copy);
        copy = millenium;
        CPPUNIT_ASSERT(millenium == copy);
    }

    void testConversion() {
        buffer = millenium.formatRfc822();

        BasicTime copy;
        CPPUNIT_ASSERT_EQUAL(0, copy.parseRfc822(buffer));
        CPPUNIT_ASSERT(millenium == copy);
        delete [] buffer; buffer = NULL;

        CPPUNIT_ASSERT_EQUAL(-1, copy.parseRfc822("this is garbage"));

        const char convertStr[] = "Mon, 6 Nov 2006 20:30:15 +0100";
        BasicTime convert;
        CPPUNIT_ASSERT_EQUAL(0, convert.parseRfc822(convertStr));
        buffer = convert.formatRfc822();
        CPPUNIT_ASSERT(!strcmp(buffer, convertStr));
        delete [] buffer; buffer = NULL;

        const char convertStr2[] = "Mon,12 Feb 2007 09:00:01 +0100";
        BasicTime convert2;
        CPPUNIT_ASSERT_EQUAL(0, convert2.parseRfc822(convertStr2));
        buffer = convert.formatRfc822();
        CPPUNIT_ASSERT(!strcmp(buffer, convertStr));
        delete [] buffer; buffer = NULL;
    }


private:
    BasicTime millenium;
    char *buffer;
};

CPPUNIT_TEST_SUITE_REGISTRATION( BasicTimeTest );

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

#include "base/fscapi.h"
#include "base/Log.h"
#include "base/quoted-printable.h"
#include "base/util/StringBuffer.h"
#include "base/globalsdef.h"

USE_NAMESPACE

class QPTest : public CppUnit::TestFixture {

    CPPUNIT_TEST_SUITE(QPTest);
        CPPUNIT_TEST(testQP1_isNeeded); 
        CPPUNIT_TEST(testQP2);
        CPPUNIT_TEST(testQP3);
        CPPUNIT_TEST(testQP4);
        CPPUNIT_TEST(testQP5);
        CPPUNIT_TEST(testQP6);
        CPPUNIT_TEST(testQP7);
        CPPUNIT_TEST(testQP8);
        CPPUNIT_TEST(testQP9);
        CPPUNIT_TEST(testQP10);
        CPPUNIT_TEST(testQP11);
        CPPUNIT_TEST(testQP12);
    CPPUNIT_TEST_SUITE_END();

public:
    void setUp() { }

    void tearDown() { }

private:
   

    void testQP1_isNeeded(){
        StringBuffer r("r 9  ");
        bool ret = false;
        ret = qp_isNeed(r.c_str());
        CPPUNIT_ASSERT(ret == true);

        r = "r 9";
        ret = qp_isNeed(r.c_str());
        CPPUNIT_ASSERT(ret == true);

        r = "r9=";
        ret = qp_isNeed(r.c_str());
        CPPUNIT_ASSERT(ret == true);

        r = "r9";
        ret = qp_isNeed(r.c_str());
        CPPUNIT_ASSERT(ret == false);

        r = "r9\r\n";
        ret = qp_isNeed(r.c_str());
        CPPUNIT_ASSERT(ret == true);
    }

    void testQP2(){
        StringBuffer s("$ ");
        char* ret = qp_encode(s.c_str(), 0);
        StringBuffer result("$=20");
        CPPUNIT_ASSERT(strcmp(ret, result.c_str()) == 0);
    }
    void testQP3(){
        StringBuffer s("$ !");
        char* ret = qp_encode(s.c_str(), 0);
        StringBuffer result("$ !");
        CPPUNIT_ASSERT(strcmp(ret, result.c_str()) == 0);
    }
    void testQP4(){
        StringBuffer s("abcd &%$а ");
        char* ret = qp_encode(s.c_str(), 0);
        StringBuffer result("abcd &%$=E0=20");        
        CPPUNIT_ASSERT(strcmp(ret, result.c_str()) == 0);
    }
    void testQP5(){
        StringBuffer s("!а");
        char* ret = qp_encode(s.c_str(), 0);
        StringBuffer result("!=E0");        
        CPPUNIT_ASSERT(strcmp(ret, result.c_str()) == 0);
    }
    void testQP6(){
        StringBuffer s("       ");
        char* ret = qp_encode(s.c_str(), 0);
        StringBuffer result("=20=20=20=20=20=20=20");        
        CPPUNIT_ASSERT(strcmp(ret, result.c_str()) == 0);
    }
    void testQP7(){
        StringBuffer s("=  ");
        char* ret = qp_encode(s.c_str(), 0);
        StringBuffer result("=3D=20=20");        
        CPPUNIT_ASSERT(strcmp(ret, result.c_str()) == 0);
    }
    void testQP8(){
        StringBuffer s("  =  ");
        char* ret = qp_encode(s.c_str(), 0);
        StringBuffer result("  =3D=20=20");        
        CPPUNIT_ASSERT(strcmp(ret, result.c_str()) == 0);
    }
    void testQP9(){
        StringBuffer s("  =  ");
        char* ret = qp_encode(s.c_str(), 0);
        StringBuffer result("  =3D=20=20");        
        CPPUNIT_ASSERT(strcmp(ret, result.c_str()) == 0);
    }
    void testQP10(){
        StringBuffer s("аим  ");
        char* ret = qp_encode(s.c_str(), 0);
        StringBuffer result("=E0=E8=EC=20=20");        
        CPPUNIT_ASSERT(strcmp(ret, result.c_str()) == 0);
    }
    void testQP11(){
        StringBuffer s("\t \t ");
        char* ret = qp_encode(s.c_str(), 0);
        StringBuffer result("=09=20=09=20"); 
        CPPUNIT_ASSERT(strcmp(ret, result.c_str()) == 0);
    }
    void testQP12() {
        StringBuffer s("=123rt5=  т");
        char* ret = qp_encode(s.c_str(), 0);
        StringBuffer result("=3D123rt5=3D  =F2"); 
        CPPUNIT_ASSERT(strcmp(ret, result.c_str()) == 0);
    }
};
        
CPPUNIT_TEST_SUITE_REGISTRATION( QPTest );

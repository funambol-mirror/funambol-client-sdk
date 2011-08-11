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

#include <stdlib.h>

#include "base/fscapi.h"
#include "vocl/WinContact.h"
#include "base/util/utils.h"
#include "base/globalsdef.h"
#include "testUtils.h"


USE_NAMESPACE

class WinContactTest : public CppUnit::TestFixture {

    CPPUNIT_TEST_SUITE(WinContactTest);
    CPPUNIT_TEST(testParse21);
    CPPUNIT_TEST(testParse212);
    CPPUNIT_TEST(testParse213);
    CPPUNIT_TEST(testParse30);
    CPPUNIT_TEST_SUITE_END();

public:
    void setUp(){
    }

    void tearDown(){
    }

private:


    void loadFile(const char* fileName, StringBuffer& ret) {
        char* message = loadTestFile("WinContactTest", fileName, false);
        ret = message;
        delete [] message;
    }

    void testVcard(const char* vcardname){
        WinContact* wcontact = new WinContact();
        StringBuffer vcard;
        loadFile(vcardname, vcard);
        WCHAR* temp = toWideChar(vcard.c_str());
        wstring wcard(temp);
        wcontact->parse(wcard);
        vcard.replaceAll("\r\n","");
        vcard.replaceAll("\n","");
        wstring result = wcontact->toString();
        const char* tempc = toMultibyte(result.c_str());
        StringBuffer resultc(tempc);
        resultc.replaceAll("\r\n", "");
        resultc.replaceAll("\n", "");
        delete [] tempc; tempc = NULL;
        delete wcontact;
        delete [] temp; temp = NULL;
        
        CPPUNIT_ASSERT( strcmp(resultc.c_str(), vcard.c_str()) == 0 );  
    }

    void testParse30(){    
        testVcard("vcard4WContact30.vcf");
    }

    void testParse212(){
        testVcard("vcard4WContact21-2.vcf");
    }

    void testParse213(){
        testVcard("vcard4WContact21-3.vcf");
    }

    void testParse21(){    
        testVcard("vcard4WContact21.vcf");
    }

};

CPPUNIT_TEST_SUITE_REGISTRATION( WinContactTest );

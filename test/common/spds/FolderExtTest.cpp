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
#include "base/messages.h"
#include "base/Log.h"
#include "base/util/StringBuffer.h"
#include "base/util/ArrayList.h"
#include "base/globalsdef.h"
#include "spds/FolderExt.h"

#define FOLDER_STRING   "<Ext><XNam>EmailAddress</XNam><XVal>Name.Surname@gmail.com</XVal><XVal>Name.Surname@email.com</XVal></Ext>"
#define NAME            "EmailAddress"
#define EMAILADDRESS1   "Name.Surname@email.com"
#define EMAILADDRESS2   "Name.Surname@gmail.com"

USE_NAMESPACE


class FolderExtTest : public CppUnit::TestFixture {
    CPPUNIT_TEST_SUITE(FolderExtTest);
        CPPUNIT_TEST(testParse);
        CPPUNIT_TEST(testFormat);
    CPPUNIT_TEST_SUITE_END();

public:
    void setUp(){

    }

    void tearDown(){

    }

private:

    void testFormat(){
        FolderExt ext;
        ArrayList xvals;
        StringBuffer xVal1 = EMAILADDRESS2;
        StringBuffer xVal2 = EMAILADDRESS1;
        xvals.add(xVal1);
        xvals.add(xVal2);

        ext.setXNam(NAME);
        ext.setXVals(xvals);

        StringBuffer folderString(ext.format());
        folderString.replaceAll("\n", "");
        CPPUNIT_ASSERT(strcmp(folderString.c_str(), FOLDER_STRING) == 0);
    }

    void testParse(){
        FolderExt ext;
        ext.parse(FOLDER_STRING);
        CPPUNIT_ASSERT(strcmp(ext.getXNam(),NAME) == 0);

        ArrayList xvals = ext.getXVals();

        StringBuffer* xval1 = (StringBuffer*)(xvals.get(0));
        StringBuffer* xval2 = (StringBuffer*)(xvals.get(1));

        CPPUNIT_ASSERT(strcmp(xval1->c_str(), EMAILADDRESS1) == 0 || strcmp(xval2->c_str(), EMAILADDRESS1) == 0 );
        CPPUNIT_ASSERT(strcmp(xval1->c_str(), EMAILADDRESS2) == 0 || strcmp(xval2->c_str(), EMAILADDRESS2) == 0 );
      
    }

};

CPPUNIT_TEST_SUITE_REGISTRATION( FolderExtTest );
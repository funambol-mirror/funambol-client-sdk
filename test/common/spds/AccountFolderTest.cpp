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
#include "base/globalsdef.h"
#include "spds/FolderData.h"
#include "spds/FolderExt.h"

#ifdef _WIN32
#define FOLDER_NAME     L"Email Home"
#define CREATED         L"20090428T162654Z"
#define ROLE            L"account"
#else
#define FOLDER_NAME     "Email Home"
#define CREATED         "20090428T162654Z"
#define ROLE            "account"
#endif

#define XNAM_VN         "VisibleName"
#define XNAM_EA         "EmailAddress"
#define VISIBLENAME     "Name Surname"
#define EMAILADDRESS    "Name.Surname@email.com"
#define FOLDER_STRING   "<Folder><name>Email Home</name><created>20090428T162654Z</created><role>account</role><Ext><XNam>VisibleName</XNam><XVal>Name Surname</XVal></Ext><Ext><XNam>EmailAddress</XNam><XVal>Name.Surname@email.com</XVal></Ext></Folder>"

USE_NAMESPACE


class AccountFolderTest : public CppUnit::TestFixture {
    CPPUNIT_TEST_SUITE(AccountFolderTest);
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
        FolderData folder;
        folder.setName(FOLDER_NAME);
        folder.setCreated(CREATED);
        folder.setRole(ROLE);
        FolderExt ext;
        ArrayList xvals, exts;
        StringBuffer xVal1 = VISIBLENAME;
        xvals.add(xVal1);

        ext.setXNam(XNAM_VN);
        ext.setXVals(xvals);
        exts.add(ext);

        xVal1 = EMAILADDRESS;
        xvals.clear();
        xvals.add(xVal1);

        ext.setXNam(XNAM_EA);
        ext.setXVals(xvals);
        exts.add(ext);

        folder.setExtList(exts);
        char* temp = folder.format();
        StringBuffer folderString(temp);
        delete temp;
        folderString.replaceAll("\n", "");
        CPPUNIT_ASSERT(strcmp(folderString.c_str(), FOLDER_STRING) == 0);
    }

    void testParse(){
        FolderData folder;
        folder.parse(FOLDER_STRING);
        CPPUNIT_ASSERT(wcscmp(folder.getName(),FOLDER_NAME) == 0);
        CPPUNIT_ASSERT(wcscmp(folder.getCreated(),CREATED) == 0);
        CPPUNIT_ASSERT(wcscmp(folder.getRole(),ROLE) == 0);
        /*ArrayList* list = folder.getExtList();
        const char* value1 = ((KeyValuePair*)(list->get(0)))->getValue();
        const char* value2 = ((KeyValuePair*)(list->get(1)))->getValue();
        CPPUNIT_ASSERT(strcmp(value1,VISIBLENAME) == 0 );
        CPPUNIT_ASSERT(strcmp(value2,EMAILADDRESS) == 0 );
*/
    }

};

CPPUNIT_TEST_SUITE_REGISTRATION( AccountFolderTest );

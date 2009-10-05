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
#include "spds/FolderData.h"
#include "spds/MailAccount.h"

#define ACCOUNT_FOLDER  "<Folder><name>Email Home</name><created>20090428T162654Z<created><role>account</role><Ext><XNam>VisibleName</XNam> <XVal>Name Surname</XVal></Ext><Ext><XNam>EmailAddress</XNam> <XVal>Name.Surname@email.com</XVal></Ext></Folder>"
#define INBOX_FOLDER    "<Folder><name>Inbox</name><created>20090428T162654Z<created><role>inbox</role></Folder>"
#define OUTBOX_FOLDER   "<Folder><name>Outbox</name><created>20090428T162654Z<created><role>outbox</role></Folder>"
#define VISIBLENAME     "Name Surname"
#define EMAILADDRESS    "Name.Surname@email.com"

#ifdef _WIN32
#define CREATED         L"20090428T162654Z"
#define NAME            L"Email Home"
#define INBOX           L"Inbox"
#define OUTBOX          L"Outbox"
#else
#define CREATED         "20090428T162654Z"
#define NAME            "Email Home"
#define INBOX           "Inbox"
#define OUTBOX          "Outbox"
#endif

USE_NAMESPACE


class MailAccountTest : public CppUnit::TestFixture {
    CPPUNIT_TEST_SUITE(MailAccountTest);
        CPPUNIT_TEST(testFillUpAccount);
    CPPUNIT_TEST_SUITE_END();

public:
    void setUp(){

    }

    void tearDown(){

    }

private:


    void testFillUpAccount(){
       FolderData account, inbox, outbox;
       account.parse(ACCOUNT_FOLDER);
       inbox.parse(INBOX_FOLDER);
       outbox.parse(OUTBOX_FOLDER);

       MailAccount mailaccount;
       mailaccount.setAccount(account);
       mailaccount.setInbox(inbox);
       mailaccount.setOutBox(outbox);

       CPPUNIT_ASSERT( wcscmp(mailaccount.getAccountName(), NAME) == 0);
       //CPPUNIT_ASSERT( wcscmp(mailaccount.getAccountCreated(), CREATED) == 0);
       CPPUNIT_ASSERT( wcscmp(mailaccount.getInboxName(), INBOX) == 0);
       //CPPUNIT_ASSERT( wcscmp(mailaccount.getInboxCreated(), CREATED) == 0);
       CPPUNIT_ASSERT( wcscmp(mailaccount.getOutboxName(), OUTBOX) == 0);
       //CPPUNIT_ASSERT( wcscmp(mailaccount.getOutboxCreated(), CREATED) == 0);

       const char* val1 = mailaccount.getVisibleName();
       const char* val2 = mailaccount.getEmailAddress();

       CPPUNIT_ASSERT( strcmp(val1, VISIBLENAME) == 0);
       CPPUNIT_ASSERT( strcmp(val2, EMAILADDRESS) == 0);
       delete [] val1;
       delete [] val2;
    }

};

CPPUNIT_TEST_SUITE_REGISTRATION( MailAccountTest );

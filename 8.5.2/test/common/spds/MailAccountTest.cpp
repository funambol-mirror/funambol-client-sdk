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
#include "base/util/utils.h"
#include "base/util/StringBuffer.h"
#include "base/util/ArrayList.h"
#include "base/globalsdef.h"
#include "spds/FolderData.h"
#include "mail/MailAccount.h"

#define ACCOUNT_FOLDER  "<Folder><name>Email Home</name><created>20090428T162654Z</created><role>account</role><Ext><XNam>VisibleName</XNam> <XVal>Name Surname</XVal></Ext><Ext><XNam>EmailAddress</XNam> <XVal>Name.Surname@email.com</XVal></Ext></Folder>"
#define INBOX_FOLDER    "<Folder><name>Inbox</name><created>20090428T162654Z</created><role>inbox</role></Folder>"
#define OUTBOX_FOLDER   "<Folder><name>Outbox</name><created>20090428T162654Z</created><role>outbox</role></Folder>"
#define VISIBLE_NAME     "Name Surname"
#define EMAIL_ADDRESS    "Name.Surname@email.com"
#define ACCOUNT_PROTOCOL "POP3"
#define CREATED         "20090428T162654Z"
#define NAME            "Email Home"
#define INBOX           "Inbox"
#define OUTBOX          "Outbox"



#define _VISIBLENAME     "Elvis Presley"
#define _EMAILADDRESS    "elvis@iamthepelvis.com"
#define _PROTOCOL        "IMAP4"
#define _USERNAME        "elvis"
#define _PASSWORD        "TheKing"
#define _IN_SERVER       "mail.iamthepelvis.com"
#define _OUT_SERVER      "smtp.iamthepelvis.com"
#define _IN_PORT         "143"
#define _OUT_PORT        "25"
#define _IN_SSL          "0"
#define _OUT_SSL         "0"
#define _SIGNATURE       "Elvis is alive!"
#define _DOMAINNAME      "iamthepelvis.com"
#define _ID              "ID"


USE_NAMESPACE


class MailAccountTest : public CppUnit::TestFixture {
    CPPUNIT_TEST_SUITE(MailAccountTest);
        CPPUNIT_TEST(testFillUpAccount);
        CPPUNIT_TEST(testExtSetters);
    CPPUNIT_TEST_SUITE_END();

public:
    void setUp(){

    }

    void tearDown(){

    }

private:


    void testFillUpAccount(){
       MailAccount account;
       account.parse(ACCOUNT_FOLDER);
       
       CPPUNIT_ASSERT( strcmp(account.getName(), NAME) == 0);

       const char* val1 = account.getVisibleName();
       const char* val2 = account.getEmailAddress();

       CPPUNIT_ASSERT( strcmp(val1, VISIBLE_NAME) == 0);
       CPPUNIT_ASSERT( strcmp(val2, EMAIL_ADDRESS) == 0);

       account.setProtocol(ACCOUNT_PROTOCOL);
       const char* val3 = account.getProtocol();
       CPPUNIT_ASSERT(val3 != NULL);  
       CPPUNIT_ASSERT( strcmp(val3, ACCOUNT_PROTOCOL) == 0);
    }


    void testExtSetters(){
	   const WCHAR* accountIdw = NULL;
	   const char* accountId = NULL;
       MailAccount account;

       account.parse(ACCOUNT_FOLDER);
       account.setVisibleName(_VISIBLENAME);
       account.setEmailAddress(_EMAILADDRESS);
       account.setProtocol(_PROTOCOL);
       account.setUsername(_USERNAME);
       account.setPassword(_PASSWORD);
       account.setInServer(_IN_SERVER);
       account.setOutServer(_OUT_SERVER);
       account.setInPort(_IN_PORT);
       account.setOutPort(_OUT_PORT);
       account.setInSSL(_IN_SSL);
       account.setOutSSL(_OUT_SSL);
       account.setSignature(_SIGNATURE);
       account.setDomainName(_DOMAINNAME);

	   accountIdw = toWideChar(_ID);
       account.setID(accountIdw);
	   delete [] accountIdw;

	   accountId = toMultibyte(account.getID());

       CPPUNIT_ASSERT( strcmp(account.getVisibleName(),     _VISIBLENAME)   == 0 );
       CPPUNIT_ASSERT( strcmp(account.getEmailAddress(),    _EMAILADDRESS)  == 0 );
       CPPUNIT_ASSERT( strcmp(account.getProtocol(),        _PROTOCOL)      == 0 );
       CPPUNIT_ASSERT( strcmp(account.getUsername(),        _USERNAME)      == 0 );
       CPPUNIT_ASSERT( strcmp(account.getPassword(),        _PASSWORD)      == 0 );
       CPPUNIT_ASSERT( strcmp(account.getInServer(),        _IN_SERVER)     == 0 );
       CPPUNIT_ASSERT( strcmp(account.getOutServer(),       _OUT_SERVER)    == 0 );
       CPPUNIT_ASSERT( strcmp(account.getInPort(),          _IN_PORT)       == 0 );
       CPPUNIT_ASSERT( strcmp(account.getOutPort(),         _OUT_PORT)      == 0 );
       CPPUNIT_ASSERT( strcmp(account.getInSSL(),           _IN_SSL)        == 0 );
       CPPUNIT_ASSERT( strcmp(account.getOutSSL(),          _OUT_SSL)       == 0 );
       CPPUNIT_ASSERT( strcmp(account.getSignature(),       _SIGNATURE)     == 0 );
       CPPUNIT_ASSERT( strcmp(account.getDomainName(),      _DOMAINNAME)    == 0 );
       CPPUNIT_ASSERT( strcmp(accountId,					_ID)            == 0 );

	   delete [] accountId;
    }
};

CPPUNIT_TEST_SUITE_REGISTRATION( MailAccountTest );

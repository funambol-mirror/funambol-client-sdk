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
#include "base/messages.h"
#include "base/Log.h"
#include "base/util/ArrayList.h"
#include "base/util/StringBuffer.h"
#include "client/SyncClient.h"
#include "client/DMTClientConfig.h"
#include "client/MailSourceManagementNode.h"
#include "examples/TestSyncSource.h"
#include "examples/TestSyncSource2.h"
#include "filter/AllClause.h"
#include "filter/ClauseUtil.h"
#include "filter/LogicalClause.h"
#include "filter/FieldClause.h"
#include "filter/sourceFilter.h"
#include "filter/WhereClause.h"
#include "spdm/constants.h"
#include "spds/spdsutils.h"
#include "spds/constants.h"
#include "mail/MailSyncSourceConfig.h"
#include "syncml/core/core.h"
#include "syncml/formatter/Formatter.h"
#include "base/globalsdef.h"

USE_NAMESPACE

class ConfigFilterTest : public CppUnit::TestFixture {

    CPPUNIT_TEST_SUITE(ConfigFilterTest);
    CPPUNIT_TEST(testNoFilter);
    CPPUNIT_TEST(subjectOnly);
    CPPUNIT_TEST(ten);
    CPPUNIT_TEST(tentotwentyfive);
    CPPUNIT_TEST(today);
    CPPUNIT_TEST_SUITE_END();

public:
    void setUp(){
        {

        MailSyncSourceConfig* config = new MailSyncSourceConfig();

        config->setDownloadAge(-1);
        config->setBodySize(-1);
        config->setAttachSize(-1);

        MailSourceManagementNode node("sources", "mail", *config);

        }
    }

    void tearDown(){
        if (node){
            delete node;
            node = NULL;
        }
        if (filter){
            delete filter;
            filter = NULL;
        }
        if (compare && f){
            delete f;
            f = NULL;
        }
        if (compare && sb){
            delete sb;
            sb = NULL;
        }
        if (compare && compareTo){
            delete compareTo;
            compareTo = NULL;
        }
    }

private:
    MailSourceManagementNode* node;
    SourceFilter* filter;
    Filter* f ;
    StringBuffer* sb;
    StringBuffer* compareTo;
    bool compare;

    void testNoFilter(){

        compare = false;
        node = new MailSourceManagementNode("sources", "mail");
        MailSyncSourceConfig& config = node->getMailSourceConfig(TRUE);


        //
        // No filters!
        //

        filter =
            ClauseUtil::createSourceFilter(NULL, config.getBodySize(),config.getAttachSize());
        CPPUNIT_ASSERT ( filter == NULL );

    }

    void subjectOnly(){
        compare = true;
        node = new MailSourceManagementNode("sources", "mail");
        MailSyncSourceConfig& config = node->getMailSourceConfig(TRUE);
        filter =
            ClauseUtil::createSourceFilter(NULL, 0, -1);
        f = ClauseUtil::toFilter(*filter);
        sb = Formatter::getFilter(f);

        compareTo = new StringBuffer("<Filter><Field><Item><Meta><Type xmlns=\"syncml:metinf\">application/vnd.syncml-devinf+xml</Type></Meta><Data><Property><PropName>emailitem</PropName></Property></Data></Item></Field></Filter>");
        sb->replaceAll("\n","");

        CPPUNIT_ASSERT ( *sb == *compareTo );
    }

    void ten(){
        compare = true;
        node = new MailSourceManagementNode("sources", "mail");
        MailSyncSourceConfig& config = node->getMailSourceConfig(TRUE);
        filter =
            ClauseUtil::createSourceFilter(NULL, 10, 0);
        f = ClauseUtil::toFilter(*filter);
        sb = Formatter::getFilter(f);
        sb->replaceAll("\n","");
        compareTo = new StringBuffer("<Filter><Field><Item><Meta><Type xmlns=\"syncml:metinf\">application/vnd.syncml-devinf+xml</Type></Meta><Data><Property><PropName>emailitem</PropName><PropParam><ParamName>texttype</ParamName></PropParam></Property></Data></Item></Field></Filter>");
        CPPUNIT_ASSERT ( *sb == *compareTo );
    }

    void tentotwentyfive(){
        compare = true;
        node = new MailSourceManagementNode("sources", "mail");
        MailSyncSourceConfig& config = node->getMailSourceConfig(TRUE);
        filter =
            ClauseUtil::createSourceFilter(NULL, 10, 25);
        f = ClauseUtil::toFilter(*filter);
        sb = Formatter::getFilter(f);
        sb->replaceAll("\n","");
        compareTo = new StringBuffer("<Filter><Field><Item><Meta><Type xmlns=\"syncml:metinf\">application/vnd.syncml-devinf+xml</Type></Meta><Data><Property><PropName>emailitem</PropName><MaxSize>25600</MaxSize><PropParam><ParamName>texttype</ParamName></PropParam></Property></Data></Item></Field></Filter>");
        CPPUNIT_ASSERT ( *sb == *compareTo );
    }
    
    void today(){
        compare = true;
        WCHAR* TODAY = TEXT("20051211T000000Z");
        node = new MailSourceManagementNode("sources", "mail");
        MailSyncSourceConfig& config = node->getMailSourceConfig(TRUE);
        filter =
            ClauseUtil::createSourceFilter(TODAY, -1, 0);
        f = ClauseUtil::toFilter(*filter);
        sb = Formatter::getFilter(f);
        sb->replaceAll("\n","");
        compareTo = new StringBuffer("<Filter><Record><Item><Meta><Type xmlns=\"syncml:metinf\">syncml:filtertype-cgi</Type></Meta><Data><![CDATA[modified&iGE;20051211T000000Z]]></Data></Item></Record></Filter>");
        CPPUNIT_ASSERT ( *sb == *compareTo );
    }
};

CPPUNIT_TEST_SUITE_REGISTRATION( ConfigFilterTest );
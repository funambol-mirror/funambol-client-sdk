/*
 * Copyright (C) 2003-2007 Funambol, Inc.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY, TITLE, NONINFRINGEMENT or FITNESS FOR A PARTICULAR
 * PURPOSE.  See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA
 * 02111-1307  USA
 */

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
#include "spds/MailSyncSourceConfig.h"
#include "syncml/core/core.h"
#include "syncml/formatter/Formatter.h"


void testConfigFilter() {

    //
    // Let's create the DM subtrees first
    //
    {
        MailSyncSourceConfig config;

        config.setDownloadAge(-1);
        config.setBodySize(-1);
        config.setAttachSize(-1);

        MailSourceManagementNode node("sources", "mail", config);
    }

    //
    // Now we can read the config and than create the needed clause
    //
    MailSourceManagementNode node("sources", "mail");
    MailSyncSourceConfig& config = node.getMailSourceConfig(TRUE);


    //
    // No filters!
    //
    MessageBox(0, TEXT("No filter"), TEXT("Config to filter"), MB_OK);

    SourceFilter* filter =
        ClauseUtil::createSourceFilter(NULL, config.getBodySize(),config.getAttachSize());

    if (filter == NULL) {
        MessageBox(0, TEXT("No filter is required!"), TEXT("Config to filter"), MB_OK);
    } else {
        MessageBox(0, TEXT("There must be an erro..."), TEXT("Config to filter"), MB_OK);
    }

    //
    // Subject only
    //
    MessageBox(0, TEXT("Subject only"), TEXT("Config to filter"), MB_OK);

    filter = ClauseUtil::createSourceFilter(NULL, 0, -1);

    if (filter == NULL) {
        MessageBox(0, TEXT("There must be an error..."), TEXT("Config to filter"), MB_OK);
    } else {
        Filter* f = ClauseUtil::toFilter(*filter);
        if (f == NULL) {
            MessageBox(0, TEXT("There must be an error..."), TEXT("Config to filter"), MB_OK);
        } else {
            StringBuffer* sb = Formatter::getFilter(f);
            WCHAR *msg = toWideChar(sb->c_str());

            MessageBox(0, msg, TEXT("Config to filter"), MB_OK);

            delete sb;
            delete [] msg;
        }

        if (filter) {
            delete filter; filter = NULL;
        }
        if (f) {
            delete f; f = NULL;
        }
    }


    //
    // 10Kb body, no attachment
    //
    MessageBox(0, TEXT("10Kb body, no attachment"), TEXT("Config to filter"), MB_OK);

    filter = ClauseUtil::createSourceFilter(NULL, 10, 0);

    if (filter == NULL) {
        MessageBox(0, TEXT("There must be an error..."), TEXT("10Kb body, no attachment"), MB_OK);
    } else {
        Filter* f = ClauseUtil::toFilter(*filter);
        if (f == NULL) {
            MessageBox(0, TEXT("There must be an error..."), TEXT("10Kb body, no attachment"), MB_OK);
        } else {
            StringBuffer* sb = Formatter::getFilter(f);
            WCHAR *msg = toWideChar(sb->c_str());
            MessageBox(0, msg, TEXT("10Kb body, no attachment"), MB_OK);

            delete sb;
            delete [] msg;
        }

        if (filter) {
            delete filter; filter = NULL;
        }
        if (f) {
            delete f; f = NULL;
        }
    }

    //
    // 10Kb body, attachment up to 25Kb
    //
    MessageBox(0, TEXT("10Kb body, attachment up to 25Kb"), TEXT("Config to filter"), MB_OK);

    filter = ClauseUtil::createSourceFilter(NULL, 10, 25);

    if (filter == NULL) {
        MessageBox(0, TEXT("There must be an error..."), TEXT("10Kb body, attachment up to 25Kb"), MB_OK);
    } else {
        Filter* f = ClauseUtil::toFilter(*filter);
        if (f == NULL) {
            MessageBox(0, TEXT("There must be an error..."), TEXT("10Kb body, attachment up to 25Kb"), MB_OK);
        } else {
            StringBuffer* sb = Formatter::getFilter(f);
            WCHAR *msg = toWideChar(sb->c_str());

            MessageBox(0, msg, TEXT("10Kb body, attachment up to 25Kb"), MB_OK);

            delete sb;
            delete [] msg;
        }

        if (filter) {
            delete filter; filter = NULL;
        }
        if (f) {
            delete f; f = NULL;
        }
    }

    //
    // TODAY's emails, no attachments
    //
    WCHAR* TODAY = TEXT("20051211T000000Z");
    MessageBox(0, TEXT("TODAY's emails, no attachments"), TEXT("Config to filter"), MB_OK);

    filter = ClauseUtil::createSourceFilter(TODAY, -1, 0);

    if (filter == NULL) {
        MessageBox(0, TEXT("There must be an error..."), TEXT("TODAY's emails, no attachments"), MB_OK);
    } else {
        Filter* f = ClauseUtil::toFilter(*filter);
        if (f == NULL) {
            MessageBox(0, TEXT("There must be an error..."), TEXT("TODAY's emails, no attachments"), MB_OK);
        } else {
            StringBuffer* sb = Formatter::getFilter(f);
            WCHAR *msg = toWideChar(sb->c_str());

            MessageBox(0, msg, TEXT("TODAY's emails, no attachments"), MB_OK);

            if (sb) {
                delete sb; sb = NULL;
            }
            delete [] msg;
        }

        if (filter) {
            delete filter; filter = NULL;
        }
        if (f) {
            delete f; f = NULL;
        }
    }

    //
    // All in one reading from the DM
    //
    MessageBox(0, TEXT("All in one"), TEXT("Config to filter"), MB_OK);
    {
        MailSyncSourceConfig config;

        config.setDownloadAge(3);
        config.setBodySize(5);
        config.setAttachSize(30);

        MailSourceManagementNode node("sources", "mail", config);
    }

    config = node.getMailSourceConfig(TRUE); // reread from the DM

    filter = ClauseUtil::createSourceFilter(TODAY, config.getBodySize(), config.getAttachSize());

    if (filter == NULL) {
        MessageBox(0, TEXT("There must be an error..."), TEXT("All in one"), MB_OK);
    } else {
        Filter* f = ClauseUtil::toFilter(*filter);
        if (f == NULL) {
            MessageBox(0, TEXT("There must be an error..."), TEXT("All in one"), MB_OK);
        } else {
            StringBuffer* sb = Formatter::getFilter(f);
            WCHAR *msg = toWideChar(sb->c_str());

            MessageBox(0, msg, TEXT("All in one"), MB_OK);

            if (sb) {
                delete sb; sb = NULL;
            }
            delete [] msg;
        }

        if (filter) {
            delete filter; filter = NULL;
        }
        if (f) {
            delete f; f = NULL;
        }
    }
}
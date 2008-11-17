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

#include "base/fscapi.h"
#include "base/messages.h"
#include "base/Log.h"
#include "base/util/ArrayList.h"
#include "base/util/StringBuffer.h"
#include "spds/spdsutils.h"
#include "spds/constants.h"
#include "client/SyncClient.h"
#include "client/DMTClientConfig.h"
#include "examples/TestSyncSource.h"
#include "examples/TestSyncSource2.h"
#include "filter/AllClause.h"
#include "filter/ClauseUtil.h"
#include "filter/LogicalClause.h"
#include "filter/FieldClause.h"
#include "filter/sourceFilter.h"
#include "filter/WhereClause.h"
#include "syncml/core/core.h"
#include "syncml/formatter/Formatter.h"


void testClause() {
    SourceFilter filter;

    WhereClause where1("CompanyName", "funambol", EQ, FALSE);
    WhereClause where2("FirstName", "ste", CONTAIN, TRUE);

    ArrayList operands;

    operands.add(where1);
    operands.add(where2);

    LogicalClause record(AND, operands);
    AllClause all;

    //
    // Record only filter
    //
    operands.clear();
    operands.add(all);
    operands.add(record);

    LogicalClause recordOnly(AND, operands);

    filter.setClause(recordOnly);
    filter.setInclusive(FALSE);
    filter.setType("text/x-s4j-sifc");

    Filter* f = ClauseUtil::toFilter(filter);

    StringBuffer* sb = Formatter::getFilter(f);
    LOG.info("Record only filter");
    LOG.info(sb->c_str());

    delete f; delete sb;

    //
    // Field only filter
    //
    Property fieldProperty;
    ArrayList fieldProperties, fieldParameters;
    PropParam param;

    param.setParamName("texttype");
    fieldParameters.add(param);
    param.setParamName("attachtype");
    fieldParameters.add(param);

    fieldProperty.setPropName("emailitem");
    fieldProperty.setMaxSize(20000);
    fieldProperty.setPropParams(&fieldParameters);
    fieldProperties.add(fieldProperty);

    FieldClause field(&fieldProperties);

    operands.clear();
    operands.add(field);
    operands.add(all);

    LogicalClause fieldOnly(AND, operands);

    filter.setClause(fieldOnly);
    filter.setInclusive(TRUE);

    f = ClauseUtil::toFilter(filter);
    sb = Formatter::getFilter(f);

    LOG.info("Field only filter");
    LOG.info(sb->c_str());

    delete f; delete sb;

    //
    // Both record and field filters
    //
    operands.clear();
    operands.add(field);
    operands.add(record);

    LogicalClause fieldAndRecord(AND, operands);

    filter.setClause(fieldAndRecord);

    f = ClauseUtil::toFilter(filter);
    sb = Formatter::getFilter(f);

    LOG.info("Field and record filter");
    LOG.info(sb->c_str());

    delete f; delete sb;
}
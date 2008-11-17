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

void testFilter() {
	Meta meta;
    meta.setType("text/v-card");

    //
    // Record filter
    //
    Item record;
    Meta recordMeta;
    ComplexData recordData("modified&EQ;all");

    recordMeta.setType("syncml:filtertype-cgi");
    record.setMeta(&recordMeta);
    record.setData(&recordData);

    //
    // Item filter
    //
    Item field;
    Meta fieldMeta;
    Property fieldProperty;
    ArrayList fieldProperties, fieldParameters;
    ComplexData fieldData;
    PropParam param;

    param.setParamName("texttype");
    fieldParameters.add(param);
    param.setParamName("attachtype");
    fieldParameters.add(param);

    fieldProperty.setPropName("emailitem");
    fieldProperty.setMaxSize(20000);
    fieldProperty.setPropParams(&fieldParameters);
    fieldProperties.add(fieldProperty);

    fieldMeta.setType("application/vnd.syncml-devinf+xml");
    fieldData.setProperties(&fieldProperties);

    field.setMeta(&fieldMeta);
    field.setData(&fieldData);

    Filter filter(&meta, &field, &record, "filter-type");

    StringBuffer* sb = Formatter::getFilter(&filter);
    WCHAR *msg = toWideChar(sb->c_str());

    MessageBox(0, msg, TEXT("Filter"), MB_OK);

    delete [] msg;
    delete [] sb;
}

/*
 * Copyright (C) 2003-2007 Funambol, Inc
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

#ifndef INCL_FILE_CLIENT
#define INCL_FILE_CLIENT
/** @cond API */
/** @addtogroup Client */
/** @{ */

#include "base/fscapi.h"
#include "base/messages.h"
#include "base/Log.h"
#include "base/util/ArrayList.h"
#include "base/util/StringBuffer.h"
#include "spds/spdsutils.h"
#include "spds/constants.h"
#include "client/SyncClient.h"
#include "client/DMTClientConfig.h"
#include "spds/DefaultConfigFactory.h"

#include "client/FileSyncSource.h"

//#include "filter/AllClause.h"
//#include "filter/ClauseUtil.h"
//#include "filter/LogicalClause.h"
//#include "filter/FieldClause.h"
//#include "filter/SourceFilter.h"
//#include "filter/WhereClause.h"
//#include "syncml/core/core.h"
//#include "syncml/formatter/Formatter.h"
//#include "spds/DefaultConfigFactory.h"
//
//#include "examples/MySyncListener.h"
//#include "examples/MySyncSourceListener.h"
//#include "examples/MySyncStatusListener.h"
//#include "examples/MySyncItemListener.h"
//#include "examples/MyTransportListener.h"
//#include "event/SetListener.h"


#define APPLICATION_URI         "Funambol/SyncclientFILE"
#define LOG_TITLE		        "Funambol FILEClient Log"
#define LOG_PATH		        "."
#define LOG_LEVEL		        LOG_LEVEL_DEBUG
#define SOURCE_NAME             "briefcase"
#define WSOURCE_NAME            TEXT("briefcase")
#define DEVICE_ID               "Funambol FILEClient"
#define SW_VERSION              "1.0"
//#define FILE_USER_AGENT         "Funambol FILEClient 1.0"


// Function to create a default config.
void createConfig(DMTClientConfig& config);

/** @} */
/** @endcond */
#endif // INCL_FILE_CLIENT

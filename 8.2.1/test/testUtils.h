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

#ifndef INCL_TEST_UTILS
#define INCL_TEST_UTILS

/** @cond API */
/** @addtogroup Test */
/** @{ */

#include "base/fscapi.h"
#include <cppunit/TestSuite.h>
#include <cppunit/TestAssert.h>
#include <cppunit/TestFixture.h>
#include "base/globalsdef.h"

#include "base/Log.h"
#include "spds/DefaultConfigFactory.h"
#include "base/util/StringBuffer.h"
#include "client/DMTClientConfig.h"


USE_NAMESPACE

//
// ---- Collect here util functions that may be shared between different tests ----
//

/**
 * Initializes the PlatformAdapter for a given test.
 * To avoid spamming the config folder, the root context is composed so that
 * all the tests will work under "Funambol/client-test", then the test name.
 */
const StringBuffer initAdapter(const char* testName);

/**
 * Generates and returns a NEW ALLOCATED default configuration (SyncManagerConfig)
 * @param testName           the name of the test which called this method (for deviceID)
 * @param setClientDefaults  if true, will set the default deviceConfig and accessConfig
 * @param defaultSources     ArrayList of source names (StringBuffer), to create default SyncSourceConfig.
 *                           If not specified, non is created. Default is NULL.
 * @return                   the (new allocated) SyncManagerConfig
 */
SyncManagerConfig* getNewSyncManagerConfig(const char* testName, const bool setClientDefaults, ArrayList* defaultSources = NULL);

/**
 * Generates and returns a NEW ALLOCATED default configuration (DMTClientConfig)
 * @param testName           the name of the test which called this method (for deviceID)
 * @param setClientDefaults  if true, will set the default deviceConfig and accessConfig
 * @param defaultSources     ArrayList of source names (StringBuffer), to create default SyncSourceConfig.
 *                           If not specified, non is created. Default is NULL.
 * @return                   the (new allocated) DMTClientConfig
 */
DMTClientConfig* getNewDMTClientConfig(const char* testName, const bool setClientDefaults, ArrayList* defaultSources = NULL);


/*
 * Loads and returns (NEW ALLOCATED) the content of a test file.
 * The test files are located under the dir "testcases/<test name>".
 *
 * @param testName the test name
 * @param fileName the file name
 * @return a new allocated buffer with the file content
 */
char* loadTestFile(const char* testName, const char* fileName);


/** @} */
/** @endcond */
#endif // INCL_TEST_UTILS

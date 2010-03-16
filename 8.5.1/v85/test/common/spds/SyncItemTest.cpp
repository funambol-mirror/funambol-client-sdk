/*
 * Funambol is a mobile platform developed by Funambol, Inc. 
 * Copyright (C) 2008 Funambol, Inc.
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
#include "base/util/StringBuffer.h"
#include "spds/SyncManagerConfig.h"
#include "spds/SyncItem.h"

#include "common/inputStream/InputStreamTest.h"
#include "testUtils.h"


// 80 chars
#define SMALL_BUFFER    "12345678901234567890123456789012345678901234567890123456789012345678901234567890"


USE_NAMESPACE

class SyncItemTest : public CppUnit::TestFixture {

    CPPUNIT_TEST_SUITE(SyncItemTest);
    CPPUNIT_TEST(testSyncItemInputStream);
    CPPUNIT_TEST_SUITE_END();

public:

    void setUp() {}
    void tearDown() {}

private:

    /**
     * 1. Creates a SyncItem, and reads the stream from there (it's a BufferInputStream)
     * 2. Tests the SyncItem setting 2 different data
     * 3. Clones the SyncItem, then reads agin the stream
     * Use the inputStream tests defined in InputStreamTest class.
     */
    void testSyncItemInputStream() {

        SyncItem item(TEXT("key"));
        InputStreamTest test;

        // Input buffers
        StringBuffer smallBuffer = SMALL_BUFFER;
        void* smallData = (void*)(smallBuffer.c_str());
        int smallDataSize = smallBuffer.length();


        // Set data
        item.setData(smallData, smallDataSize);
        InputStream* stream = item.getInputStream();
        CPPUNIT_ASSERT (stream);

        if (stream) {
            test.testReadManyChunks(*stream, smallData, smallDataSize);
        }
        
        // Set another data inside the same SyncItem
        StringBuffer smallBuffer2("test another data inside the same SyncItem");
        void* smallData2 = (void*)(smallBuffer2.c_str());
        int smallDataSize2 = smallBuffer2.length();

        item.setData(smallData2, smallDataSize2);
        stream = item.getInputStream();
        CPPUNIT_ASSERT (stream);

        if (stream) {
            test.testReadManyChunks(*stream, smallData2, smallDataSize2);
        }

        // Clone the item
        SyncItem* itemCloned = (SyncItem*)item.clone();
        stream = itemCloned->getInputStream();
        CPPUNIT_ASSERT (stream);

        if (stream) {
            test.testReadManyChunks(*stream, smallData2, smallDataSize2);
        }
        delete itemCloned;
    }

};

CPPUNIT_TEST_SUITE_REGISTRATION( SyncItemTest );

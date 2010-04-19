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

#include "InputStreamTest.h"

USE_NAMESPACE


/**
 * ------------------------------------------------------------------------------
 * Reads a big chunk (size > stream size)
 * ------------------------------------------------------------------------------
 */
void InputStreamTest::testReadBigChunk(InputStream& stream, const void* expectedData, const int expectedDataSize) {

    // Output buffer
    const int outBufferSize = expectedDataSize + 10;
    char* outBuffer = new char[outBufferSize + 1];
    memset(outBuffer, '-', outBufferSize);

    // Reset stream
    stream.reset();
    CPPUNIT_ASSERT (stream.getPosition() == 0);
    CPPUNIT_ASSERT (!stream.eof());


    // Test read
    int bytesRead = stream.read(outBuffer, expectedDataSize + 5);

    CPPUNIT_ASSERT (bytesRead == expectedDataSize);
    CPPUNIT_ASSERT (memcmp(outBuffer, expectedData, expectedDataSize) == 0);
    CPPUNIT_ASSERT (stream.getPosition() == expectedDataSize);
    CPPUNIT_ASSERT (stream.eof());

    // no more fileContent expected...
    bytesRead = stream.read(outBuffer, 10);
    CPPUNIT_ASSERT (bytesRead == 0);

    delete [] outBuffer;
}


/**
 * ------------------------------------------------------------------------------
 * Reads in 2 chunks
 * ------------------------------------------------------------------------------
 */
void InputStreamTest::testReadTwoChunks(InputStream& stream, const void* expectedData, const int expectedDataSize) {

    int firstChunkSize  = expectedDataSize - 15;
    int secondChunkSize = 15;

    // Output buffer
    const int outBufferSize = expectedDataSize + 10;
    char* outBuffer = new char[outBufferSize + 1];
    memset(outBuffer, '-', outBufferSize);


    // Test read 1st chunk
    stream.reset();
    int bytesRead = stream.read(outBuffer, firstChunkSize);

    CPPUNIT_ASSERT (bytesRead == firstChunkSize);
    CPPUNIT_ASSERT (memcmp(outBuffer, expectedData, firstChunkSize) == 0);
    CPPUNIT_ASSERT (stream.getPosition() == firstChunkSize);
    CPPUNIT_ASSERT (!stream.eof());

    // Test read 2nd chunk
    memset(outBuffer, '-', outBufferSize);
    bytesRead = stream.read(outBuffer, expectedDataSize);
    
    CPPUNIT_ASSERT (bytesRead == secondChunkSize);
    CPPUNIT_ASSERT (memcmp(outBuffer, (byte*)expectedData + firstChunkSize, secondChunkSize) == 0);
    CPPUNIT_ASSERT (stream.getPosition() == expectedDataSize);
    CPPUNIT_ASSERT (stream.eof());

    delete [] outBuffer;
}



/**
 * ------------------------------------------------------------------------------
 * Reads in many small chuncks
 * ------------------------------------------------------------------------------
 */
void InputStreamTest::testReadManyChunks(InputStream& stream, const void* expectedData, const int expectedDataSize, const int chunkSize) {

    // Output buffer
    const int outBufferSize = expectedDataSize + 10;
    char* outBuffer = new char[outBufferSize + 1];
    memset(outBuffer, '-', outBufferSize);


    // Test read
    stream.reset();
    while (!stream.eof()) {
        memset(outBuffer, '-', outBufferSize);
        int offset = stream.getPosition();

        int bytesRead = stream.read(outBuffer, chunkSize);

        CPPUNIT_ASSERT (bytesRead <= chunkSize);
        CPPUNIT_ASSERT (memcmp(outBuffer, (byte*)expectedData + offset, bytesRead) == 0);
    }

    delete [] outBuffer;
}


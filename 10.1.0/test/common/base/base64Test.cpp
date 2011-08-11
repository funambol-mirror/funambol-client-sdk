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

#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/extensions/HelperMacros.h>

#include "base/util/StringBuffer.h"
#include "base/base64.h"

USE_NAMESPACE

/**
 * This is the test class for the SyncML Parser.
 */
class Base64Test : public CppUnit::TestFixture {

    CPPUNIT_TEST_SUITE(Base64Test);
    CPPUNIT_TEST(round);
    CPPUNIT_TEST(twoEncodes);
    CPPUNIT_TEST(twoDecodes);
    CPPUNIT_TEST_SUITE_END();

public:
    void setUp() {
        rawData = new char[2048];
        for(int i=0;i<2048;++i) {
            rawData[i] = (char)(i % 255);
        }
        rawDataSize = 2048;
    }

    void tearDown() {
        delete[] rawData;
    }

private:

    void round() {
        StringBuffer encoded;
        int len;
        b64_encode(encoded, (void *)rawData, rawDataSize);
        char* decoded = (char*)b64_decode(len, encoded.c_str());
        CPPUNIT_ASSERT(equal(rawData, decoded, rawDataSize));
        CPPUNIT_ASSERT(len == rawDataSize);
        delete [] decoded;
    }

    void twoEncodes() {
        StringBuffer encoded;
        b64_encode(encoded, (void *)rawData, rawDataSize);

        char* dest = new char[encoded.length()];
 
        int destSize = b64_encode(dest, rawData, rawDataSize);
        CPPUNIT_ASSERT(destSize == (int)encoded.length());
        CPPUNIT_ASSERT(equal(encoded.c_str(), dest, destSize));
        delete []dest;
    }

    void twoDecodes() {
        StringBuffer encoded;
        int len;
        b64_encode(encoded, (void *)rawData, rawDataSize);
        char* decoded1 = (char*)b64_decode(len, encoded.c_str());
        char* decoded2 = new char[len];
        int len2 = b64_decode(decoded2, encoded.c_str());
        CPPUNIT_ASSERT(len == len2);
        CPPUNIT_ASSERT(equal(decoded1, decoded2, len));
        delete []decoded1;
        delete []decoded2;
    }

    bool equal(const char* s1, const char* s2, int len) {
        for(int i=0;i<len;++i) {
            if (s1[i] != s2[i]) {
                return false;
            }
        }
        return true;
    }

private:

    char* rawData;
    int   rawDataSize;

};

CPPUNIT_TEST_SUITE_REGISTRATION( Base64Test );


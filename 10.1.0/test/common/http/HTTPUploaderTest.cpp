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
#include "http/constants.h"
#include "ioStream/BufferInputStream.h"
#include "http/HttpUploader.h"

#include "testUtils.h"

#define TEST_SERVER_URL      "http://testServer/funambol/ds"
#define TEST_SERVER_UPLOAD   "http://testServer:80/" UPLOAD_URL_RESOURCE
#define TEST_SOURCE_URI      "testSourceURI"
#define TEST_USER_AGENT      "testUserAgent"
#define TEST_DEVICE_ID       "testDeviceId"
#define TEST_USERNAME        "testUsername"
#define TEST_PASSWORD        "testPassword"

#define TEST_SESSION_ID      "AA123123123"
#define TEST_COOKIE_HDR      "JSESSIONID=" TEST_SESSION_ID

USE_FUNAMBOL_NAMESPACE


// headers of the last request done.
static StringMap lastRequestHeaders;

// if true, the (fake) connection is open
static bool connectionOpen = false;


/**
 * Fake HTTPConnection, to test the HttpUploader::upload().
 */
class FakeHttpConnection : public HttpConnection {

public:

    FakeHttpConnection(const char* user_agent, int status) : HttpConnection(user_agent), responseStatus(status) {
        connectionOpen = false;
    }
    ~FakeHttpConnection() {}

    int open(const StringBuffer& url, RequestMethod method = MethodPost, bool log_request=true) { 
        connectionOpen = true;
        return 0; 
    }
    int close() {
        connectionOpen = false;
        return 0;
    }
    int request(InputStream& data, OutputStream& response, bool log_request=true) {
        
        // copy the request headers, to check them later
        lastRequestHeaders.clear();
        lastRequestHeaders = requestHeaders;

        // fake set the Set-cookie response header
        responseHeaders.put(HTTP_HEADER_SET_COOKIE, TEST_COOKIE_HDR);
        return responseStatus;
    }

private:
    int responseStatus;
};



/**
 * Extends HTTPUploader, used to test its protected members.
 */
class TestHttpUploader : public HttpUploader {

public:

    TestHttpUploader() : responseStatus(HTTP_OK) {}

    // proxy methods
    StringBuffer testComposeURL() { 
        return composeURL(); 
    }
    StringBuffer testParseJSessionId(const StringBuffer& input) {
        return getHttpConnection()->parseJSessionId(input);
    }
    HttpConnection* getHttpConnection() {
        // this is a fake HttpConnection: empty implementation.
        return new FakeHttpConnection("ua", responseStatus);
    }

    // to set a different response status to the request method.
    void setFakeResponseStatus(int status) {
        responseStatus = status;
    }

private:
    int responseStatus;
};



class HttpUploaderTest : public CppUnit::TestFixture  {

    CPPUNIT_TEST_SUITE(HttpUploaderTest);
    CPPUNIT_TEST(testComposeURL);
    CPPUNIT_TEST(testParseJSessionId);
    CPPUNIT_TEST(testUploadOk);
    CPPUNIT_TEST(testUploadKo);
    CPPUNIT_TEST(testUploadBadInput);
    CPPUNIT_TEST(testUploadRequestHeaders);
    CPPUNIT_TEST(testUploadResponseHeaders);
    CPPUNIT_TEST_SUITE_END();

public:

    void setUp() {

        httpUploader = new TestHttpUploader();

        httpUploader->setUsername (TEST_USERNAME);
        httpUploader->setPassword (TEST_PASSWORD);
        httpUploader->setSyncUrl  (TEST_SERVER_URL);
        httpUploader->setDeviceID (TEST_DEVICE_ID);
        httpUploader->setSourceURI(TEST_SOURCE_URI);
        httpUploader->setUserAgent(TEST_USER_AGENT);
        httpUploader->setUseSessionID(false);
    }

    void tearDown() {
        delete httpUploader;
    }

private:

    TestHttpUploader* httpUploader;



    //
    ///////////////////////////////////////// TESTS /////////////////////////////////////////
    //

    /**
     * Tests the composeURL method.
     */
    void testComposeURL() {

        StringBuffer expected(TEST_SERVER_UPLOAD);
        expected += "/";
        expected += TEST_SOURCE_URI;
        expected += "?action=content-upload";

        // no sessionId
        httpUploader->setUseSessionID(false);
        StringBuffer url = httpUploader->testComposeURL();
        CPPUNIT_ASSERT(url == expected);

        // no sessionId (empty)
        httpUploader->setUseSessionID(true);
        url = httpUploader->testComposeURL();
        CPPUNIT_ASSERT(url == expected);
    }

    /**
     * Tests the parseJSessionId method.
     */
    void testParseJSessionId() {

        StringBuffer jsession = httpUploader->testParseJSessionId("JSESSIONID=ABCDEFG");
        CPPUNIT_ASSERT(jsession == "ABCDEFG");

        jsession = httpUploader->testParseJSessionId("JsEsSIONid=12341234");
        CPPUNIT_ASSERT(jsession == "12341234");

        jsession = httpUploader->testParseJSessionId("wrong-input");
        CPPUNIT_ASSERT(jsession.empty());

        // Test more attributes in the same header
        jsession = httpUploader->testParseJSessionId("JSESSIONID=12345; Path=/");
        CPPUNIT_ASSERT(jsession == "12345");

        jsession = httpUploader->testParseJSessionId("domain=.example.org; JSESSIONID=12345; Path=/");
        CPPUNIT_ASSERT(jsession == "12345");
    }

    /**
     * Tests the upload method: uses a FakeHttpConnection object.
     * Tests a response OK (200)
     */
    void testUploadOk() {

        BufferInputStream inputStream("test-request-body");
        int status = httpUploader->upload("testLuid", &inputStream);
        CPPUNIT_ASSERT(status == HTTP_OK);
        CPPUNIT_ASSERT(!connectionOpen);
    }

    /**
     * Tests the upload method: uses a FakeHttpConnection object.
     * Tests a response KO (500)
     */
    void testUploadKo() {

        BufferInputStream inputStream("test-request-body");
        httpUploader->setFakeResponseStatus(HTTP_SERVER_ERROR);
        int status = httpUploader->upload("testLuid", &inputStream);
        CPPUNIT_ASSERT(status == HTTP_SERVER_ERROR);
        CPPUNIT_ASSERT(!connectionOpen);
    }

    /**
     * Tests the upload method: uses a FakeHttpConnection object.
     * Tests bad input stream passed.
     */
    void testUploadBadInput() {

        httpUploader->setFakeResponseStatus(HTTP_OK);
        BufferInputStream emptyStream("");
        int status = httpUploader->upload("testLuid", &emptyStream);
        CPPUNIT_ASSERT(status != HTTP_OK);

        status = httpUploader->upload("testLuid", NULL);
        CPPUNIT_ASSERT(status != HTTP_OK);

        BufferInputStream inputStream("test-request-body");
        status = httpUploader->upload("", &inputStream);
        CPPUNIT_ASSERT(status != HTTP_OK);
    }

    /**
     * Tests the upload method: uses a FakeHttpConnection object.
     * Tests the request headers are correctly set:
     * they are set by HTTPUploader before submitting the request 
     * (and copied by the FakeHttpConnection)
     */
    void testUploadRequestHeaders() {

        BufferInputStream inputStream("test-request-body");
        int status = httpUploader->upload("testLuid", &inputStream);
        
        StringBuffer hdr;
        hdr = lastRequestHeaders.get(HTTP_HEADER_X_FUNAMBOL_DEVICE_ID); CPPUNIT_ASSERT(hdr == TEST_DEVICE_ID);
        hdr = lastRequestHeaders.get(HTTP_HEADER_X_FUNAMBOL_LUID);      CPPUNIT_ASSERT(hdr == "testLuid");
        hdr = lastRequestHeaders.get(HTTP_HEADER_ACCEPT);               CPPUNIT_ASSERT(hdr == "*/*");
        hdr = lastRequestHeaders.get(HTTP_HEADER_CONTENT_TYPE);         CPPUNIT_ASSERT(hdr == "application/octet-stream");
        
        StringBuffer size;
        size.sprintf("%d", inputStream.getTotalSize());
        hdr = lastRequestHeaders.get(HTTP_HEADER_X_FUNAMBOL_FILE_SIZE); CPPUNIT_ASSERT(hdr == size);
        
        CPPUNIT_ASSERT(status == HTTP_OK);
        CPPUNIT_ASSERT(!connectionOpen);
    }

    /**
     * Tests the upload method: uses a FakeHttpConnection object.
     * Tests the response header "Set-Cookie": it's the only one used now
     * to set the jsession id.
     */
    void testUploadResponseHeaders() {

        BufferInputStream inputStream("test-request-body");
        httpUploader->setUseSessionID(true);
        int status = httpUploader->upload("testLuid", &inputStream);
        
        StringBuffer sessionId = httpUploader->getSessionID();
        CPPUNIT_ASSERT(sessionId == TEST_SESSION_ID);
        CPPUNIT_ASSERT(!connectionOpen);
    }

};

CPPUNIT_TEST_SUITE_REGISTRATION( HttpUploaderTest );

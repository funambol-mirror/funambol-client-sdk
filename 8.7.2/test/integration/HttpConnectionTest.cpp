/*
 * Funambol is a mobile platform developed by Funambol, Inc.
 * Copyright (C) 2010 Funambol, Inc.
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

#ifdef HAVE_CONFIG_H
# include <config.h>
#endif

#ifdef WIN32
#define Sleep       Sleep
#define INTERVAL    1000
#else
#define Sleep       sleep
#define INTERVAL    1
#endif

#ifdef ENABLE_INTEGRATION_TESTS

#include "HttpConnectionTest.h"

#define TEST_NAME  "httpConnectionTest"

static const char* syncml_msg_fmt = 
"<?xml version=\"1.0\" encoding=\"UTF-8\"?>"
"<SyncML>"
"<SyncHdr><VerDTD>1.1</VerDTD>"
"<VerProto>SyncML/1.1</VerProto>"
"<SessionID>1194978121</SessionID>"
"<MsgID>1</MsgID>"
"<Target><LocURI>%s</LocURI>"
"</Target>"
"<Source><LocURI>fwm-50F0063006B000000</LocURI>"
"</Source>"
"<Cred><Meta><Format xmlns=\"syncml:metinf\">b64</Format>"
"<Type xmlns=\"syncml:metinf\">syncml:auth-basic</Type>"
"</Meta>"
"<Data>dGVvOnRlb2RlbW8=</Data>"
"</Cred>"
"<Meta><MaxMsgSize xmlns=\"syncml:metinf\">65536</MaxMsgSize>"
"<MaxObjSize xmlns=\"syncml:metinf\">2010000</MaxObjSize>"
"</Meta>"
"</SyncHdr>"
"<SyncBody><Alert><CmdID>1</CmdID>"
"<Data>200</Data>"
"<Item><Target><LocURI>scard</LocURI>"
"</Target>"
"<Source><LocURI>contact</LocURI>"
"</Source>"
"<Meta><Anchor xmlns=\"syncml:metinf\"><Last>1194977846</Last>"
"<Next>1194978121</Next>"
"</Anchor>"
"<MaxObjSize xmlns=\"syncml:metinf\">60000</MaxObjSize>"
"</Meta>"
"</Item>"
"</Alert>"
"<Final/></SyncBody></SyncML>";

BEGIN_NAMESPACE

HttpConnectionTest::HttpConnectionTest() : httpConnection("testHttpConnection"), serverUrl(getenv("CLIENT_TEST_SERVER_URL")) {}

void HttpConnectionTest::testCompressedHttpConnection()
{
    int ret = 0; 
    URL syncUri(serverUrl.c_str());
    
    StringBuffer syncMlMsg;
    syncMlMsg.sprintf(syncml_msg_fmt, serverUrl.c_str());

    ret = httpConnection.open(syncUri, HttpConnection::MethodPost);

    CPPUNIT_ASSERT(ret == 0);
    
    httpConnection.setRequestHeader(HTTP_HEADER_ACCEPT,  "*/*");
    httpConnection.setRequestHeader(HTTP_HEADER_CONTENT_TYPE,   "application/vnd.syncml+xml");
    
    httpConnection.setCompression(true);

    BufferInputStream inputStream(syncMlMsg);
    StringOutputStream outputStream;

    LOG.debug("sending message: %s", syncMlMsg.c_str());

    ret = httpConnection.request(inputStream, outputStream);

    CPPUNIT_ASSERT(ret == HTTP_OK);

    LOG.debug("request, ret = %d", ret);
    LOG.debug("response = \n%s", outputStream.getString().c_str());
    
    httpConnection.close();
}

END_NAMESPACE;

#endif // ENABLE_INTEGRATION_TESTS

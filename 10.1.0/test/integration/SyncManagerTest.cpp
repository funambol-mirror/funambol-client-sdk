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

#ifdef HAVE_CONFIG_H
# include <config.h>
#endif

#ifdef ENABLE_INTEGRATION_TESTS
#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/extensions/HelperMacros.h>

#include "base/fscapi.h"
#include "base/Log.h"
#include "base/adapter/PlatformAdapter.h"
#include "base/util/XMLProcessor.h"
#include "syncml/core/TagNames.h"

#include "testUtils.h"
#include "SyncManagerTest.h"
#include "TestSyncSource.h"
#include "vocl/VConverter.h"

USE_NAMESPACE

#define MIN_SYNCML_MSG_SIZE     5000    // in Byte. Smaller than 5k can be unacceptable for Server.
#define NUM_CALENDAR_ITEMS      50      // A high number is requested, to force multimessage.


/**
 * Parses a syncML message, and returns its Message ID (integer value).
 */
static int getMsgID(const StringBuffer& syncMLmsg) {

    char* value = XMLProcessor::copyElementContent(syncMLmsg.c_str(), MSG_ID, NULL);
    CPPUNIT_ASSERT(value);
    CPPUNIT_ASSERT(strlen(value));

    int msgID = atoi(value);
    delete [] value;
    return msgID;
}


/**
 * Generates and returns a default configuration, with ssources contacts and calendar
 */
static SyncManagerConfig* getConfiguration(const char* name) {

    StringBuffer sourceName;
    ArrayList sourceList;
    sourceName = "contact";    sourceList.add(sourceName);
    sourceName = "calendar";   sourceList.add(sourceName);

    SyncManagerConfig* config = getNewSyncManagerConfig(name, true, &sourceList);

    // Can be removed once vCal is the default format...
    SyncSourceConfig* ssc = config->getSyncSourceConfig("calendar");
    ssc->setType("text/x-vcalendar");
    ssc->setEncoding("bin");
    ssc->setVersion("1.0");
    ssc->setURI("event");

    return config;
}


//////////////////////////////////////////////////////////////////////////////////////////////

void SyncManagerTest::testServerError506() {

    LOG.setLevel(LOG_LEVEL_DEBUG);
    SyncManagerConfig* config = getConfiguration("testServerError506");
    
    // Must use 2 syncsources: contact (no items) + calendar (many items: 50)
    // so that the last source has to send items in multimessage, first 
    // won't have the Final tag.
    SyncSourceConfig* ssc = config->getSyncSourceConfig("contact");
    TestSyncSource ssContact(TEXT("contact"), ssc, 0);

    ssc = config->getSyncSourceConfig("calendar");
    TestSyncSource ssCalendar(TEXT("calendar"), ssc, NUM_CALENDAR_ITEMS);
    ssc->setSync("refresh-from-client");    // optional

    SyncSource* sources[3];
    sources[0] = &ssContact;
    sources[1] = &ssCalendar;
    sources[2] = NULL; 

    // Use a test transportAgent, to modify the 2nd message from the Server.
    // Set a low msg size, so that the calendar items are split at least in 2 syncML msg.
    // Note: the TransportAgent will be destroyed by the SyncManager.
    URL url(config->getSyncURL());
    Proxy proxy;
    TransportAgentTestError506* testTransportAgent = new TransportAgentTestError506(url, proxy, config->getResponseTimeout(), MIN_SYNCML_MSG_SIZE);
            
    SyncClient client;
    client.setTransportAgent(testTransportAgent);
    client.sync(*config, sources);

    StringBuffer report("");
    client.getSyncReport()->toString(report);
    LOG.info("\n%s", report.c_str());
    delete config;
}



void TransportAgentTestError506::beforeSendingMessage(StringBuffer& msgToSend) {

    if (!msgToSend) {
        return;
    }
    int msgID = getMsgID(msgToSend);

    int maxMsgID = NUM_CALENDAR_ITEMS + 2;
    CPPUNIT_ASSERT_MESSAGE("infinite loop in sync", (msgID < maxMsgID));
}


void TransportAgentTestError506::afterReceivingResponse(StringBuffer& msgReceived) {

    if (!msgReceived) {
        return;
    }

    // Modify only the 3rd message
    int msgID = getMsgID(msgReceived);
    if (msgID != 3) {
        return;
    }

    unsigned int pos = 0, previous = 0;
    StringBuffer status;
    const char* msg = msgReceived.c_str();

    XMLProcessor::copyElementContent(status, msg, STATUS, &pos);
    while ( !status.empty() ) {
        StringBuffer cmd;
        XMLProcessor::copyElementContent(cmd, status, CMD);
        if (cmd == SYNC) {
            //
            // It's the status of Sync command: replace the <Data> with a "506"
            //
            unsigned int dataPos=0, start=0, end=0;
            CPPUNIT_ASSERT(XMLProcessor::getElementContent(status, DATA, &dataPos, &start, &end));
            StringBuffer data = status.substr(start, end-start);
            CPPUNIT_ASSERT(!data.empty());

            dataPos = previous + start;     // absolute position in full msg
            msgReceived.replace(data, "506", dataPos);
            return;
        }

        pos += previous;
        previous = pos;
        XMLProcessor::copyElementContent(status, &msg[pos], STATUS, &pos);
    }
}


//////////////////////////////////////////////////////////////////////////////////////////////

void SyncManagerTest::testLargeObject2() {

    LOG.setLevel(LOG_LEVEL_DEBUG);
    SyncManagerConfig* config = getConfiguration("testLargeObject2");

    // To force multimessage
    config->getDeviceConfig().setMaxObjSize(100000);
    config->getAccessConfig().setMaxMsgSize(MIN_SYNCML_MSG_SIZE);

    // Just sync the contacts, with no items locally changed
    config->getSyncSourceConfig("calendar")->setIsEnabled(false);
    SyncSourceConfig* ssc = config->getSyncSourceConfig("contact");
    SyncSourceTestLargeObject2 ssContact(TEXT("contact"), ssc, 0);

    SyncSource* sources[2];
    sources[0] = &ssContact;
    sources[1] = NULL;

    // Use a test transportAgent, to simulate ALL the messages from the Server.
    // Set a low msg size, so that the Replace item is split at least in 2 syncML msg.
    // Note: the TransportAgent will be destroyed by the SyncManager.
    URL url(config->getSyncURL());
    Proxy proxy;
    TransportAgentTestLargeObject2* testTA = new TransportAgentTestLargeObject2(url, proxy, config->getResponseTimeout(), MIN_SYNCML_MSG_SIZE);
      
    // ------------------------------------
    SyncClient client;
    client.setTransportAgent(testTA);
    int ret = client.sync(*config, sources);
    // ------------------------------------

    StringBuffer report("");
    client.getSyncReport()->toString(report);
    LOG.info("\n%s", report.c_str());

    CPPUNIT_ASSERT_MESSAGE("Sync failed", !ret);
    

    // Check the results
    SyncSourceReport* ssr = ssContact.getReport();
    CPPUNIT_ASSERT(ssr);
    if (ssr) {
        int mod = ssr->getItemReportSuccessfulCount(CLIENT, COMMAND_REPLACE);
        CPPUNIT_ASSERT_EQUAL(1, mod);

        int del = ssr->getItemReportSuccessfulCount(CLIENT, COMMAND_DELETE);
        CPPUNIT_ASSERT_EQUAL(1, del);
    }

    delete config;
}


char* loadSyncMLFromFile(const int index) {

    // Only 5 syncML messages are expected
    if (index <= 0 || index > 5) {
        return NULL;
    }

    StringBuffer fileName;
    fileName.sprintf("syncML%d.xml", index);

    return loadTestFile("testLargeObject2", fileName.c_str());
}


char* TransportAgentTestLargeObject2::sendMessage(const char* msg) {

    if (!msg) {
        return NULL;
    }
    int msgID = getMsgID(msg); 

    char* ret = loadSyncMLFromFile(msgID);
    CPPUNIT_ASSERT(ret);

    LOG.debug("FAKE TRANSPORT AGENT - msg to send:\n%s", msg);
    LOG.debug("FAKE TRANSPORT AGENT - msg received:\n%s", ret);

    return ret;
}


int SyncSourceTestLargeObject2::updateItem(SyncItem& item) {

    int ret = STC_COMMAND_FAILED;
    CPPUNIT_ASSERT(item.getDataSize());

    char* charData = (char*)item.getData();
    CPPUNIT_ASSERT(charData);

    // Expected a vCard
    const WCHAR* wdata = toWideChar(charData);
    VObject* vo = VConverter::parse(wdata);

    // If null, the vCard was not correct
    CPPUNIT_ASSERT_MESSAGE("updateItem has a wrong vCard data", vo);
    if (vo) ret = STC_OK;

    delete vo;
    delete [] wdata;
    return ret;
}


#endif // ENABLE_INTEGRATION_TESTS

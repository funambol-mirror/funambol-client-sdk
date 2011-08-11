/*
 * Funambol is a mobile platform developed by Funambol, Inc.
 * Copyright (C) 2003 - 2011 Funambol, Inc.
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
#include "sapi/SapiSyncManager.h"
#include "event/SyncSourceListener.h"
#include "event/SetListener.h"
#include "event/ManageListener.h"
#include "ioStream/BufferInputStream.h"
#include "ioStream/BufferOutputStream.h"
#include "testUtils.h"


USE_FUNAMBOL_NAMESPACE

#define TEST_SOURCE_NAME        "picture"
#define DISTANT_FUTURE          2000000000


/// Listener for syncsource events, used for tests below
class TestSyncSourceListener : public SyncSourceListener {
public:
    TestSyncSourceListener(const char* name = "") : SyncSourceListener(name), totalClientItems(0), totalServerItems(0) {}
    void syncSourceBegin             (SyncSourceEvent &event) {}
    void syncSourceEnd               (SyncSourceEvent &event) {}
    void syncSourceSyncModeRequested (SyncSourceEvent& event) {}
    void syncSourceTotalClientItems  (SyncSourceEvent& event) { totalClientItems = event.getData(); }
    void syncSourceTotalServerItems  (SyncSourceEvent& event) { totalServerItems = event.getData(); }

    int getTotalServerItems() { return totalServerItems; }
    int getTotalClientItems() { return totalClientItems; }

private:
    int totalServerItems;
    int totalClientItems;
};


/// Listener for syncitem events, used for tests below
class TestSyncItemListener : public SyncItemListener {
public:
    TestSyncItemListener(const char *name = "") : SyncItemListener(name), firedClientItems(0), firedServerItems(0) {}
    void itemUploaded  (SyncItemEvent& event) { firedClientItems++; }
    void itemDownloaded(SyncItemEvent& event) { firedServerItems++; }

    int getFiredServerItems() { return firedServerItems; }
    int getFiredClientItems() { return firedClientItems; }

private:
    int firedServerItems;
    int firedClientItems;
};

/// Fake SapiSyncSource, used for tests below
class TestSapiSyncSource : public SapiSyncSource {

public:
    TestSapiSyncSource(SyncSourceConfig& sc, SyncSourceReport& report, size_t incomingFilterDate, size_t outgoingFilterDate) :
                      SapiSyncSource(sc, report, incomingFilterDate, outgoingFilterDate) {
        setFakeStatus(0);
        allCount = newCount = modCount = delCount = numDownloads = localQuota = 0;
        filterOut = filterIn = createItemError = insertItemError = false;
    }

    bool populateAllItemInfoList(AbstractSyncConfig& mainConfig) { return true; }
    InputStream*  createInputStream (const char* luid) { 
        return new BufferInputStream("upload item data content"); 
    }
    OutputStream* createOutputStream(SapiSyncItemInfo& itemInfo) {
        if (createItemError) return NULL;
        else return new BufferOutputStream();
    }        
    StringBuffer insertItem(DownloadSapiSyncItem* syncItem, ESapiSyncSourceError* errCode, long* modificationDate) {
        CPPUNIT_ASSERT (syncItem != NULL);
        if (insertItemError > 0) {
            if (numDownloads >= insertItemError) {
                return "";
            }
        }
        StringBuffer luid(syncItem->getSapiSyncItemInfo()->getGuid());
        luid.append("-luid");
        numDownloads ++;
        return luid;
    }

    // methods reimplemented for test purposes
    int beginSync(bool changes, AbstractSyncConfig& mainConfig) {
        isSyncingItemChanges = changes;

        if (!allItemInfo) { allItemInfo = new ArrayListEnumeration(); }
        for (int i=0; i<allCount; i++) {
            StringBuffer luid;
            luid.sprintf("luid-all-%d", i);
            SapiSyncItemInfo itemInfo("", luid.c_str(), luid.c_str(), i+1020, "", "", "", 0, i+1020);
            allItemInfo->add(itemInfo);
        }
        if (!newItemInfo) { newItemInfo = new ArrayListEnumeration(); }
        for (int i=0; i<newCount; i++) {
            StringBuffer luid;
            luid.sprintf("luid-new-%d", i);
            SapiSyncItemInfo itemInfo("", luid.c_str(), luid.c_str(), i+2000, "", "", "", 0, i+2000);
            newItemInfo->add(itemInfo);
        }
        if (!updatedItemInfo) { updatedItemInfo = new ArrayListEnumeration(); }
        for (int i=0; i<modCount; i++) {
            StringBuffer luid;
            luid.sprintf("luid-mod-%d", i);
            SapiSyncItemInfo itemInfo("", luid.c_str(), luid.c_str(), i+3000, "", "", "", 0, i+3000);
            updatedItemInfo->add(itemInfo);
        }
        if (!deletedItemInfo) { deletedItemInfo = new ArrayListEnumeration(); }
        for (int i=0; i<delCount; i++) {
            StringBuffer luid;
            luid.sprintf("luid-del-%d", i);
            SapiSyncItemInfo itemInfo("", luid.c_str(), luid.c_str(), i+4000, "", "", "", 0, i+4000);
            deletedItemInfo->add(itemInfo);
        }

        //if (changes == false) {
        //    applyFilterOutgoingItem(allItemInfo);
        //} else {
        //    applyFilterOutgoingItem(newItemInfo);
        //}
        return fakeStatus;
    }

    bool filterOutgoingItem(SapiSyncItemInfo& clientItemInfo) {
        if (filterOut) {
            if (clientItemInfo.getLuid() == "luid-all-2") { return true; }
            if (clientItemInfo.getLuid() == "luid-new-4") { return true; }
        }
        return false;
    }
    bool filterIncomingItem(SapiSyncItemInfo& serverItemInfo, time_t offsetTime) {
        if (filterIn) {
            if (serverItemInfo.getGuid() == "guid-all-1") { return true; }
            if (serverItemInfo.getGuid() == "guid-new-2") { return true; }
        }
        return false;
    }
    bool isLocalStorageAvailable(unsigned long long size, int* errorCode) {
        if (localQuota) {
            if (numDownloads >= localQuota) {
                return false;
            }
        }
        return true;
    }

    void validateLocalLists() {
        // use all: NEW+MOD+DEL
    }
    void validateRemoteLists(ArrayList* newServerItems, ArrayList* modServerItems, ArrayList* delServerItems) {
        // use all: NEW+MOD+DEL
    }

    void setFakeStatus(int s)                   { fakeStatus = s; }
    void setAllCount(const int n)                 { allCount = n; }
    void setNewCount(const int n)                 { newCount = n; }
    void setModCount(const int n)                 { modCount = n; }
    void setDelCount(const int n)                 { delCount = n; }
    void setFilterOut(bool enable)                { filterOut = enable; }
    void setFilterIn (bool enable)                { filterIn  = enable; }
    void setLocalQuota(const int n)               { localQuota = n; }
    void setCreateItemError()                     { createItemError = true; }
    void setInsertItemError(const int n)          { insertItemError = n; }

private:
    int fakeStatus;
    int allCount;
    int newCount;
    int modCount;
    int delCount;
    bool filterOut;
    bool filterIn;
    int numDownloads;
    int localQuota;
    bool createItemError;
    int insertItemError;
};


/// Fake SapiMediaRequestManager, used for tests below
class TestSapiMediaRequestManager : public SapiMediaRequestManager {

public:
    TestSapiMediaRequestManager(const char* url, SapiMediaSourceType mediaSourceType,
                                const char* user_agent, const char* username, const char* password) :
                                SapiMediaRequestManager(url, mediaSourceType, user_agent, username, password) {
    beginStatus = uploadStatus = downloadStatus = ESMRSuccess;
    allCount = newCount = modCount = delCount = 0;
    numUploads = uploadQuota = 0;
    resumingUploadOffset = 0;
    retryFailuresOnUpload = retryUpload = 0;
    }

    // methods reimplemented for test purposes
    ESapiMediaRequestStatus login(const char* device_id, time_t* serverTime = NULL, unsigned long *expiretime = NULL, 
        StringMap* sourcesStringMap = NULL, StringMap* propertyStringMap = NULL) {
            return beginStatus;
    }

    ESapiMediaRequestStatus getAllItems(ArrayList& sapiItemInfoList, time_t* responseTime, int limit = 0, int offset = 0) {
        sapiItemInfoList.clear();
        for (int i=0; i<allCount; i++) {
            StringBuffer guid;
            guid.sprintf("guid-all-%d", i);
            SapiSyncItemInfo itemInfo(guid.c_str(), "", guid.c_str(), i+1000, "", "", "", i+1000, i+DISTANT_FUTURE);
            sapiItemInfoList.add(itemInfo);
        }
        *responseTime = time(NULL);
        return beginStatus;
    }

    ESapiMediaRequestStatus getItemsChanges(ArrayList& newIDs, ArrayList& modIDs,
                                    ArrayList& delIDs, const StringBuffer& fromDate, time_t* reponseTimestamp) {
        newIDs.clear();
        for (int i=0; i<newCount; i++) {
            StringBuffer guid;
            guid.sprintf("guid-new-%d", i);
            newIDs.add(guid);
        }
        modIDs.clear();
        for (int i=0; i<modCount; i++) {
            StringBuffer guid;
            guid.sprintf("guid-mod-%d", i);
            modIDs.add(guid);
        }
        delIDs.clear();
        for (int i=0; i<delCount; i++) {
            StringBuffer guid;
            guid.sprintf("guid-del-%d", i);
            delIDs.add(guid);
        }
        this->fromDate = fromDate;
        *reponseTimestamp = time(NULL);
        return beginStatus;
    }

    ESapiMediaRequestStatus getItemsFromId(ArrayList& items, const ArrayList& itemsIDs) {

        items.clear();
        for (int i=0; i < itemsIDs.size(); i++) {
            StringBuffer* guid = (StringBuffer*)itemsIDs.get(i);
            if (guid && !guid->empty()) {
                SapiSyncItemInfo itemInfo(guid->c_str(), "", guid->c_str(), i+1000, "", "", "", i+1000, i+DISTANT_FUTURE);
                items.add(itemInfo);
            }
        }
        return beginStatus;
    }

    ESapiMediaRequestStatus uploadItemMetaData(UploadSapiSyncItem* item) {
        if (uploadQuota) {
            if (numUploads >= uploadQuota) {
                return ESMRQuotaExceeded;
            }
        }

        // set the GUID and return
        StringBuffer guid(item->getSapiSyncItemInfo()->getLuid());
        guid.append("-guid");
        item->getSapiSyncItemInfo()->setGuid(guid.c_str());
        return ESMRSuccess;
    }

    ESapiMediaRequestStatus uploadItemData(UploadSapiSyncItem* item, time_t* lastUpdate) {
        CPPUNIT_ASSERT (item != NULL);
        InputStream* is = item->getStream();
        CPPUNIT_ASSERT (is != NULL);

        if (retryFailuresOnUpload > 0) {
            if (retryUpload <= retryFailuresOnUpload) {
                retryUpload ++;
                return ESMRNetworkError;
            }
        }

        if (resumingUploadOffset > 0) {
            CPPUNIT_ASSERT_EQUAL (resumingUploadOffset + 1, is->getPosition());  // next byte!
            resumingUploadOffset = 0;
        }

        numUploads ++;
        return uploadStatus;
    }

    ESapiMediaRequestStatus downloadItem(DownloadSapiSyncItem* item) {
        CPPUNIT_ASSERT (item != NULL);
        OutputStream* os = item->getStream();
        CPPUNIT_ASSERT (os != NULL);

        // simulate the write in the output stream
        StringBuffer testString("download item data content");
        os->write(testString.c_str(), testString.length());
        return downloadStatus;
    }

    ESapiMediaRequestStatus getItemResumeInfo(UploadSapiSyncItem* item, size_t* offset) {
        CPPUNIT_ASSERT (item != NULL);
        resumingUploadOffset = 2;
        *offset = resumingUploadOffset;
        return ESMRSuccess;
    }

    void setBeginStatus   (ESapiMediaRequestStatus s) { beginStatus = s; }
    void setUploadStatus  (ESapiMediaRequestStatus s) { uploadStatus = s; }
    void setDownloadStatus(ESapiMediaRequestStatus s) { downloadStatus = s; }
    void setAllCount(const int n)                 { allCount = n; }
    void setNewCount(const int n)                 { newCount = n; }
    void setModCount(const int n)                 { modCount = n; }
    void setDelCount(const int n)                 { delCount = n; }
    StringBuffer getFromDate()                    { return fromDate; }
    void setUploadQuota(int n)                    { uploadQuota = n; }
    void setRetryFailuresOnUpload(const int n)    { retryFailuresOnUpload = n; }

private:
    ESapiMediaRequestStatus beginStatus;
    ESapiMediaRequestStatus uploadStatus;
    ESapiMediaRequestStatus downloadStatus;
    int allCount;
    int newCount;
    int modCount;
    int delCount;
    StringBuffer fromDate;
    int uploadQuota;
    int numUploads;
    int resumingUploadOffset;
    int retryFailuresOnUpload;
    int retryUpload;
};


/// Fake SapiSyncManager, used for tests below
class TestSapiSyncManager : public SapiSyncManager {

public:
    TestSapiSyncManager(SapiSyncSource& s, AbstractSyncConfig& c) : SapiSyncManager(s, c)
    {
        delete sapiMediaRequestManager;
        sapiMediaRequestManager = new TestSapiMediaRequestManager(config.getSyncURL(),
                                       getMediaSourceType(),
                                       config.getUserAgent(),
                                       config.getUsername(),
                                       config.getPassword());
    }

    TestSapiMediaRequestManager* getSapiMediaRequestManager() {
        return (TestSapiMediaRequestManager*)sapiMediaRequestManager;
    }

    ArrayList& getAllList() { return allServerItems; }
    ArrayList& getNewList() { return newServerItems; }
    ArrayList& getModList() { return modServerItems; }
    ArrayList& getDelList() { return delServerItems; }
};



/**
 * Tests the SapiSyncManager class.
 * Using fake SapiSyncSource and SapiMediaRequestManager objects.
 */
class SapiSyncManagerTest : public CppUnit::TestFixture  {

    CPPUNIT_TEST_SUITE(SapiSyncManagerTest);

    CPPUNIT_TEST(testLogin);
    CPPUNIT_TEST(testSourceBeginSyncError);

    // S2C: 2way, test getALL from Server
    CPPUNIT_TEST(testFirstSyncNoFiltering);
    CPPUNIT_TEST(testFirstSyncFilterDate);
    CPPUNIT_TEST(testNextSyncFilterNumber);
    CPPUNIT_TEST(testNextSyncFilterNumber2);
    CPPUNIT_TEST(testNextDownloadFirstUploadFilterDate);
    CPPUNIT_TEST(testNextSyncFilterUploadNumber);
    CPPUNIT_TEST(testNextSyncChangedUsername);
    CPPUNIT_TEST(testNextSyncChangedServerUrl);

    // S2C: 1way from server, test getALL from Server
    CPPUNIT_TEST(testFirstDownloadFilterDate);
    CPPUNIT_TEST(testNextDownloadFilterNumber);

    // S2C: 2way, test getCHANGES from Server
    CPPUNIT_TEST(testNextSyncNoFiltering);
    CPPUNIT_TEST(testNextSyncFilterDate);
    CPPUNIT_TEST(testNextSyncFilterDate2);

    // S2C: 1way from server, test getCHANGES from Server
    CPPUNIT_TEST(testNextDownloadNoUploadFilterDate);
    CPPUNIT_TEST(testNextDownloadNoUploadFilterNumber);

    // C2S: 2way, test getALL from Client
    CPPUNIT_TEST(testC2SFirstSyncNoFiltering);
    CPPUNIT_TEST(testC2SFirstSyncFilterDate);
    CPPUNIT_TEST(testC2SFirstSyncFilterNumber);
    CPPUNIT_TEST(testC2SFirstSyncFilterNumber2);
    CPPUNIT_TEST(testC2SNextUploadFirstDownloadFilterDate);

    // C2S: 1way from client, test getChanges from Client
    CPPUNIT_TEST(testC2SNextUploadNoDownloadFilterDate);
    CPPUNIT_TEST(testC2SNextSyncNoFiltering);

    // C2S: 2way, test getChanges from Client
    CPPUNIT_TEST(testC2SNextSyncNoFiltering2way);

    // Tests on GUID/LUID mappings
    CPPUNIT_TEST(testFirstSyncMappings);
    CPPUNIT_TEST(testFirstSyncFilterNumberMappings);

    // Test custom filtering
    CPPUNIT_TEST(testFirstSyncCustomFilters);
    CPPUNIT_TEST(testNextSyncCustomFilters);

    // Test twin detection
    CPPUNIT_TEST(testTwinDetection);

    // Test upload
    CPPUNIT_TEST(testUploadAll);
    CPPUNIT_TEST(testUploadAllFilterNumber);
    CPPUNIT_TEST(testUploadChanges);
    CPPUNIT_TEST(testUploadQuota);
    CPPUNIT_TEST(testUploadNetworkError);
    CPPUNIT_TEST(testUploadRetry);
    CPPUNIT_TEST(testUploadRetry2);

    // Test download
    CPPUNIT_TEST(testDownloadAll);
    CPPUNIT_TEST(testDownloadChanges);
    CPPUNIT_TEST(testDownloadFilterNumber);
    CPPUNIT_TEST(testDownloadAllLocalStorage);
    CPPUNIT_TEST(testDownloadChangesLocalStorage);
    CPPUNIT_TEST(testDownloadNetworkError);
    CPPUNIT_TEST(testDownloadCreateItemError);
    CPPUNIT_TEST(testDownloadInsertItemError);

    // Test resume
    CPPUNIT_TEST(testUploadResumeAll);
    CPPUNIT_TEST(testUploadResumeChanges);
    CPPUNIT_TEST(testUploadResumeOrphan);
    CPPUNIT_TEST(testDownloadResumeAll);
    CPPUNIT_TEST(testDownloadResumeChanges);
    CPPUNIT_TEST(testDownloadResumeOrphan);

    CPPUNIT_TEST_SUITE_END();

public:

    void setUp() {

        // create config
        StringBuffer sourceName;
        ArrayList sourceList;
        sourceName = TEST_SOURCE_NAME;
        sourceList.add(sourceName);
        config = getNewSyncManagerConfig("SapiSyncManagerTest", true, &sourceList);
        CPPUNIT_ASSERT (config != NULL);

        SyncSourceConfig* ssc = config->getSyncSourceConfig(TEST_SOURCE_NAME);
        CPPUNIT_ASSERT (ssc != NULL);
        ssc->setProperty(PROPERTY_LAST_SYNC_URL,      config->getSyncURL());
        ssc->setProperty(PROPERTY_LAST_SYNC_USERNAME, config->getUsername());

        // create report
        report.setSyncSourceReports(*config);
        SyncSourceReport* ssr = report.getSyncSourceReport(TEST_SOURCE_NAME);
        CPPUNIT_ASSERT (ssr != NULL);

        // cleanup maps
        TestSapiSyncSource source(*ssc, *ssr, 0, 0);
        cleanup(source);

    }

    void tearDown() {
        delete config;
        ManageListener::releaseAllListeners();
    }

private:

    SyncReport report;
    SyncManagerConfig* config;



    //
    ///////////////////////////////////////// TESTS /////////////////////////////////////////
    //

    void testLogin() {

        SyncSourceConfig* ssc = config->getSyncSourceConfig(TEST_SOURCE_NAME);
        CPPUNIT_ASSERT (ssc != NULL);
        size_t filterDate = (size_t)time(NULL);

        SyncSourceReport* ssr = report.getSyncSourceReport(TEST_SOURCE_NAME);
        CPPUNIT_ASSERT (ssr != NULL);
        TestSapiSyncSource source(*ssc, *ssr, filterDate, filterDate);

        TestSapiSyncManager manager(source, *config);
        ssc->setIntProperty(PROPERTY_SYNC_ITEM_NUMBER_FROM_SERVER, 1);

        // test wrong login
        TestSapiMediaRequestManager* reqManager = manager.getSapiMediaRequestManager();
        CPPUNIT_ASSERT (reqManager != NULL);
        reqManager->setBeginStatus(ESMRGenericHttpError);
        int err = manager.beginSync();
        CPPUNIT_ASSERT (err == ESSMNetworkError);
        ssr->setLastErrorCode(0);

        // test auth error
        reqManager->setBeginStatus(ESMRAccessDenied);
        err = manager.beginSync();
        CPPUNIT_ASSERT (err == ESSMAuthenticationError);
        ssr->setLastErrorCode(0);

        // test OK
        reqManager->setBeginStatus(ESMRSuccess);
        err = manager.beginSync();
        CPPUNIT_ASSERT (err == ESSMSuccess);
    }


    // Simulate an error in syncsource's begin sync
    void testSourceBeginSyncError() {

        SyncSourceConfig* ssc = config->getSyncSourceConfig(TEST_SOURCE_NAME);
        CPPUNIT_ASSERT (ssc != NULL);

        SyncSourceReport* ssr = report.getSyncSourceReport(TEST_SOURCE_NAME);
        CPPUNIT_ASSERT (ssr != NULL);
        TestSapiSyncSource source(*ssc, *ssr, 0, 0);

        source.setFakeStatus(1);
        TestSapiSyncManager manager(source, *config);

        int ret = manager.beginSync();
        CPPUNIT_ASSERT (ret == ESSMBeginSyncError);
    }


    //
    // ------ GET ALL, TWO WAY -------
    //

    // 1st sync with all filters = 0 -> GetALL
    void testFirstSyncNoFiltering() {
        TestSapiSyncSource* source = createSource(SYNC_MODE_TWO_WAY, 0, 0, -1, -1, 0);
        checkAllServerItems(10, *source);
        delete source;
    }

    // First sync with filter date > 0 -> GetALL
    void testFirstSyncFilterDate() {
        TestSapiSyncSource* source = createSource(SYNC_MODE_TWO_WAY, 0, 0, -1, -1, (size_t)time(NULL));
        checkAllServerItems(10, *source);
        delete source;
    }

    // Next sync with filter numbers > 0 -> GetALL
    // with filterByNumber < # of items on the server
    void testNextSyncFilterNumber() {
        TestSapiSyncSource* source = createSource(SYNC_MODE_TWO_WAY, 12341234, 12341233, 5, 5, 0);
        checkAllServerItems(10, *source);
        delete source;
    }

    // Next sync with filter numbers > 0 -> GetALL
    // with filterByNumber > # of items on the server
    void testNextSyncFilterNumber2() {
        TestSapiSyncSource* source = createSource(SYNC_MODE_TWO_WAY, 12341234, 12341233, 25, 25, 0);
        checkAllServerItems(10, *source);
        delete source;
    }

    // Next download but first upload, with filter date -> getALL (forced by C2S direction)
    void testNextDownloadFirstUploadFilterDate() {
        TestSapiSyncSource* source = createSource(SYNC_MODE_TWO_WAY, 0, 12341233, -1, -1, 12341234);
        checkAllServerItems(10, *source);
        delete source;
    }

    // Next sync, 2way, with filter number only for upload -> getALL (forced by C2S direction)
    void testNextSyncFilterUploadNumber() {
        TestSapiSyncSource* source = createSource(SYNC_MODE_TWO_WAY, 123412232, 12341233, 12, -1, 12341234);
        checkAllServerItems(10, *source);
        delete source;
    }

    // Next sync, 2way, with lastSyncUsername != username -> force getALL
    void testNextSyncChangedUsername() {
        TestSapiSyncSource* source = createSource(SYNC_MODE_TWO_WAY, 123412232, 12341233, -1, -1, 0);
        
        source->getConfig().setProperty(PROPERTY_LAST_SYNC_USERNAME, "fakeUser");
        checkAllServerItems(5, *source);
        source->getConfig().setProperty(PROPERTY_LAST_SYNC_USERNAME, config->getUsername());

        delete source;
    }

    // Next sync, 2way, with lastSyncUrl != syncUrl -> force getALL
    void testNextSyncChangedServerUrl() {
        TestSapiSyncSource* source = createSource(SYNC_MODE_TWO_WAY, 123412232, 12341233, -1, -1, 0);
        
        source->getConfig().setProperty(PROPERTY_LAST_SYNC_URL, "http://fakeUrl/sync");
        checkAllServerItems(5, *source);
        source->getConfig().setProperty(PROPERTY_LAST_SYNC_URL, config->getSyncURL());
        
        delete source;
    }

    //
    // ------ GET ALL, ONE WAY SERVER TO CLIENT -------
    //

    // First download with filter date > 0 -> GetALL
    void testFirstDownloadFilterDate() {
        TestSapiSyncSource* source = createSource(SYNC_MODE_ONE_WAY_FROM_SERVER, 12341234, 0, -1, -1, (size_t)time(NULL));
        checkAllServerItems(10, *source);
        delete source;
    }

    // Next download with filter number > 0 -> GetALL
    void testNextDownloadFilterNumber() {
        TestSapiSyncSource* source = createSource(SYNC_MODE_ONE_WAY_FROM_SERVER, 12341234, 12341233, 5, 5, 0);
        checkAllServerItems(10, *source);
        delete source;
    }


    //
    // ------ GET CHANGES, TWO WAY -------
    //

    // Next sync with all filters = 0 -> GetChanges from last tstamp
    void testNextSyncNoFiltering() {
        TestSapiSyncSource* source = createSource(SYNC_MODE_TWO_WAY, 1234, 1234, -1, -1, 0);
        checkChangedServerItems(10, 5, 3, *source);
        delete source;
    }

    // Next sync with filter date > 0 -> GetChanges from most recent date
    // using last tstamp > filterDate
    void testNextSyncFilterDate() {

        unsigned long last       = (unsigned long)time(NULL);
        unsigned long filterDate = last - 3600;

        TestSapiSyncSource* source = createSource(SYNC_MODE_TWO_WAY, 1234, last, -1, -1, filterDate);
        StringBuffer fromDate = checkChangedServerItems(5, 0, 2, *source);

        StringBuffer lastString       = unixTimeToString(last, true);
        StringBuffer filterDateString = unixTimeToString(filterDate, true);
        CPPUNIT_ASSERT (fromDate == lastString);
        delete source;
    }

    // Next sync with filter date > 0 -> GetChanges from most recent date
    // using last tstamp < filterDate
    void testNextSyncFilterDate2() {

        unsigned long last       = (unsigned long)time(NULL);
        unsigned long filterDate = last + 3600;

        TestSapiSyncSource* source = createSource(SYNC_MODE_TWO_WAY, 1234, last, -1, -1, filterDate);
        StringBuffer fromDate = checkChangedServerItems(5, 0, 2, *source);

        StringBuffer lastString       = unixTimeToString(last, true);
        StringBuffer filterDateString = unixTimeToString(filterDate, true);

        // still using the last dwnload tstamp to get server changes!!!
        // (filtering by date is done later in filterIncomingItem)
        CPPUNIT_ASSERT (fromDate == lastString);
        delete source;
    }

    //
    // ------ GET CHANGES, ONE WAY FROM SERVER -------
    //

    // Next download with filter date > 0 and upload never executed -> GetChanges (C2S ignored)
    void testNextDownloadNoUploadFilterDate() {
        TestSapiSyncSource* source = createSource(SYNC_MODE_ONE_WAY_FROM_SERVER, 0, 1234, -1, -1, 123451234);
        checkChangedServerItems(5, 5, 0, *source);
        delete source;
    }

    // Next download with no filter, upload with filter by number -> GetChanges (C2S ignored)
    void testNextDownloadNoUploadFilterNumber() {
        TestSapiSyncSource* source = createSource(SYNC_MODE_ONE_WAY_FROM_SERVER, 12345, 1234, 50, -1, 0);
        checkChangedServerItems(5, 5, 0, *source);
        delete source;
    }


    //
    // ------ C2S: GET ALL, TWO WAY -------
    //

    // 1st sync with all filters = 0 -> PopulateAll
    void testC2SFirstSyncNoFiltering() {
        TestSapiSyncSource* source = createSource(SYNC_MODE_TWO_WAY, 0, 0, -1, -1, 0);
        checkAllClientItems(10, *source);
        delete source;
    }


    // 1st sync with filter date -> PopulateAll
    void testC2SFirstSyncFilterDate() {
        TestSapiSyncSource* source = createSource(SYNC_MODE_TWO_WAY, 0, 0, -1, -1, 12341234);
        checkAllClientItems(10, *source);
        delete source;
    }

    // 1st sync with filter number -> PopulateAll
    // with filterByNumber < # of items on the client
    void testC2SFirstSyncFilterNumber() {
        TestSapiSyncSource* source = createSource(SYNC_MODE_TWO_WAY, 0, 0, 10, -1, 12341234);
        checkAllClientItems(12, *source);
        delete source;
    }

    // 1st sync with filter number -> PopulateAll
    // with filterByNumber > # of items on the client
    void testC2SFirstSyncFilterNumber2() {
        TestSapiSyncSource* source = createSource(SYNC_MODE_TWO_WAY, 0, 0, 10, -1, 12341234);
        checkAllClientItems(5, *source);
        delete source;
    }

    // Next upload with filter date, but 1st download -> PopulateAll (S2C forces)
    void testC2SNextUploadFirstDownloadFilterDate() {
        TestSapiSyncSource* source = createSource(SYNC_MODE_TWO_WAY, 12341234, 0, -1, -1, 12341234);
        checkAllClientItems(10, *source);
        delete source;
    }

    //
    // ------ C2S: GET ALL, ONE WAY FROM CLIENT -------
    //

    // Next upload with filter date, but 1st download -> PopulateChanges (S2C ignored)
    // (exactly the same as last one, but syncmode is 1way from client now)
    void testC2SNextUploadNoDownloadFilterDate() {
        TestSapiSyncSource* source = createSource(SYNC_MODE_ONE_WAY_FROM_CLIENT, 12341234, 0, -1, -1, 12341234);
        checkChangedClientItems(10, 5, 3, *source);
        delete source;
    }


    // Next sync with no filtering PopulateChanges
    void testC2SNextSyncNoFiltering() {
        TestSapiSyncSource* source = createSource(SYNC_MODE_ONE_WAY_FROM_CLIENT, 12341234, 123123, -1, -1, 12341234);
        checkChangedClientItems(2, 2, 0, *source);
        delete source;
    }

    //
    // ------ C2S: GET CHANGES, TWO WAY -------
    //

    // Next sync with no filters -> PopulateChanges
    void testC2SNextSyncNoFiltering2way() {
        TestSapiSyncSource* source = createSource(SYNC_MODE_TWO_WAY, 12345, 1234124, -1, -1, 12341234);
        checkChangedClientItems(2, 6, 5, *source);
        delete source;
    }


    //
    // ------ C2S + S2C: test GUID/LUID mappings -------
    //

    // First sync with no filters (ALL), some items already existing in GUID/LUID map
    void testFirstSyncMappings() {

        TestSapiSyncSource* source = createSource(SYNC_MODE_TWO_WAY, 0, 0, -1, -1, 0);

        // Add some entries in the GUID/LUID and cache tables (simulate these items are already in sync)
        // 2 server items, 3 client items
        source->getMappings().setPropertyValue("guid-all-2", "fakeLuid1");
        source->getMappings().setPropertyValue("guid-all-3", "luid-all-3");
        source->getMappings().setPropertyValue("fakeGuid1",  "luid-all-1");
        source->getMappings().setPropertyValue("fakeGuid2",  "luid-all-4");

        source->getCache().setPropertyValue("fakeLuid1",  "1234");
        source->getCache().setPropertyValue("luid-all-3", "1235");
        source->getCache().setPropertyValue("luid-all-1", "1236");
        source->getCache().setPropertyValue("luid-all-4", "1237");

        checkAllItems(10, 15, *source, 3, 2);
        cleanup(*source);
        delete source;
    }

    // First sync with filters on numbers (ALL), some items already existing in GUID/LUID map
    void testFirstSyncFilterNumberMappings() {

        TestSapiSyncSource* source = createSource(SYNC_MODE_TWO_WAY, 0, 0, 5, 6, 0);

        // Add some entries in the GUID/LUID and cache tables (simulate these items are already in sync)
        // 1 server item, 1 client item
        source->getMappings().setPropertyValue("guid-all-2", "luid-all-1");
        source->getCache().setPropertyValue("luid-all-1", "1236");

        checkAllItems(10, 13, *source, 1, 1);
        cleanup(*source);
        delete source;
    }

    //
    // ------ C2S + S2C: test IN/OUT custom filtering -------
    //

    // First sync with no filters (ALL), some items filtered out
    void testFirstSyncCustomFilters() {

        TestSapiSyncSource* source = createSource(SYNC_MODE_TWO_WAY, 0, 0, -1, -1, 0);

        // custom filtering inside ssource (1 server item, 1 client item)
        source->setFilterIn(true);
        source->setFilterOut(true);

        checkAllItems(10, 8, *source, 1, 1);
        cleanup(*source);
        delete source;
    }

    // Next sync with no filters (CHANGES), some items filtered out
    void testNextSyncCustomFilters() {

        TestSapiSyncSource* source = createSource(SYNC_MODE_TWO_WAY, 141414, 1321321, -1, -1, 0);

        // custom filtering inside ssource (1 new server item, 1 new client item)
        source->setFilterIn(true);
        source->setFilterOut(true);

        checkChangedItems(12, 6, *source, 1, 1);
        cleanup(*source);
        delete source;
    }

    //
    // ------ C2S + S2C: test twin detection -------
    //

    // First sync with no filters (ALL), some items already existing locally (in cache)
    void testTwinDetection() {

        TestSapiSyncSource* source = createSource(SYNC_MODE_TWO_WAY, 0, 0, -1, -1, 0);

        // All server items > 20, have the same size of the local fake items (twin)
        int numClientItems = 10;
        int numServerItems = 25;
        int numTwins = numServerItems - 20;

        checkAllItems(numClientItems, numServerItems, *source, numTwins, numTwins);

        // Expected 'numTwins' entries in the cache/mappings!!
        Enumeration& mappings = source->getMappings().getProperties();
        Enumeration& cache = source->getCache().getProperties();
        for (int i=0; i<numTwins; i++) {
            CPPUNIT_ASSERT (mappings.getNextElement() != NULL);
            CPPUNIT_ASSERT (cache.getNextElement() != NULL);
        }
        CPPUNIT_ASSERT (mappings.getNextElement() == NULL);
        CPPUNIT_ASSERT (cache.getNextElement() == NULL);

        cleanup(*source);
        delete source;
    }



    //
    // ------ test UPLOAD -------
    //

    // First sync with no filters (ALL), upload ALL items
    void testUploadAll() {

        TestSapiSyncSource* source = createSource(SYNC_MODE_TWO_WAY, 0, 0, -1, -1, 0);
        TestSapiSyncManager manager(*source, *config);

        int numUploads = 3;
        source->setAllCount(numUploads);

        checkUpload(manager, *source, numUploads);
        cleanup(*source);
        delete source;
    }


    // First sync with filter number (ALL), upload ALL items
    void testUploadAllFilterNumber() {

        int limit = 5;
        TestSapiSyncSource* source = createSource(SYNC_MODE_TWO_WAY, 0, 0, limit, -1, 0);
        TestSapiSyncManager manager(*source, *config);

        int numUploads = 10;
        source->setAllCount(numUploads);

        checkUpload(manager, *source, limit);
        cleanup(*source);
        delete source;
    }


    // Next sync with no filters (CHANGES), upload NEW items (MOD and DEL are not supported now)
    void testUploadChanges() {

        TestSapiSyncSource* source = createSource(SYNC_MODE_TWO_WAY, 1234, 1243, -1, -1, 0);
        TestSapiSyncManager manager(*source, *config);

        int numUploads = 6;
        source->setNewCount(numUploads);
        source->setModCount(numUploads); // will be ignored - will break the test once implemented! ;)
        source->setDelCount(numUploads); // will be ignored - will break the test once implemented! ;)

        checkUpload(manager, *source, numUploads);
        cleanup(*source);
        delete source;
    }

    // First sync with no filters (ALL), upload ALL items but Quota error after N item
    void testUploadQuota() {

        TestSapiSyncSource* source = createSource(SYNC_MODE_TWO_WAY, 0, 0, -1, -1, 0);
        TestSapiSyncManager manager(*source, *config);

        int numUploads = 15;
        source->setAllCount(numUploads);

        int limit = 10;
        TestSapiMediaRequestManager* reqManager = manager.getSapiMediaRequestManager();
        CPPUNIT_ASSERT (reqManager != NULL);
        reqManager->setUploadQuota(limit);  // will throw quota error after N items

        checkUpload(manager, *source, limit, ESSMServerQuotaExceeded);
        cleanup(*source);
        delete source;
    }


    // First sync with no filters (ALL), upload ALL items but network error uploading
    void testUploadNetworkError() {

        TestSapiSyncSource* source = createSource(SYNC_MODE_TWO_WAY, 0, 0, -1, -1, 0);
        TestSapiSyncManager manager(*source, *config);

        int numUploads = 15;
        source->setAllCount(numUploads);

        TestSapiMediaRequestManager* reqManager = manager.getSapiMediaRequestManager();
        CPPUNIT_ASSERT (reqManager != NULL);
        reqManager->setUploadStatus(ESMRNetworkError);

        checkUpload(manager, *source, 0, ESSMNetworkError);
        cleanup(*source);
        delete source;
    }

    // First sync with no filters (ALL), upload ALL items but network error uploading
    // Retry 3 times, 3rd attempt will work!
    void testUploadRetry() {

        TestSapiSyncSource* source = createSource(SYNC_MODE_TWO_WAY, 0, 0, -1, -1, 0);
        TestSapiSyncManager manager(*source, *config);

        int numUploads = 1;
        source->setAllCount(numUploads);

        TestSapiMediaRequestManager* reqManager = manager.getSapiMediaRequestManager();
        CPPUNIT_ASSERT (reqManager != NULL);
        
        reqManager->setRetryFailuresOnUpload(2);
        config->setSapiMaxRetriesOnError(3);
        config->setSapiSleepTimeOnRetry(10);    // 10 msec

        checkUpload(manager, *source, 1, ESSMSuccess);

        cleanup(*source);
        delete source;
    }

    // First sync with no filters (ALL), upload ALL items but network error uploading
    // Retry 4 times, never works
    void testUploadRetry2() {

        TestSapiSyncSource* source = createSource(SYNC_MODE_TWO_WAY, 0, 0, -1, -1, 0);
        TestSapiSyncManager manager(*source, *config);

        int numUploads = 1;
        source->setAllCount(numUploads);

        TestSapiMediaRequestManager* reqManager = manager.getSapiMediaRequestManager();
        CPPUNIT_ASSERT (reqManager != NULL);
        
        reqManager->setRetryFailuresOnUpload(5);
        config->setSapiMaxRetriesOnError(4);
        config->setSapiSleepTimeOnRetry(10);    // 10 msec

        checkUpload(manager, *source, 0, ESSMNetworkError);

        cleanup(*source);
        delete source;
    }


    //
    // ------ test DOWNLOAD -------
    //

    // First sync with no filters (ALL), download ALL items
    void testDownloadAll() {

        TestSapiSyncSource* source = createSource(SYNC_MODE_TWO_WAY, 0, 0, -1, -1, 0);
        TestSapiSyncManager manager(*source, *config);

        int numDownloads = 5;
        TestSapiMediaRequestManager* reqManager = manager.getSapiMediaRequestManager();
        CPPUNIT_ASSERT (reqManager != NULL);
        reqManager->setBeginStatus(ESMRSuccess);
        reqManager->setAllCount(numDownloads);

        checkDownload(manager, *source, numDownloads);
        cleanup(*source);
        delete source;
    }

    // Next sync with no filters (CHANGES), download NEW items
    void testDownloadChanges() {

        TestSapiSyncSource* source = createSource(SYNC_MODE_TWO_WAY, 1234, 12345, -1, -1, (size_t)time(NULL));
        TestSapiSyncManager manager(*source, *config);

        int numDownloads = 10;
        TestSapiMediaRequestManager* reqManager = manager.getSapiMediaRequestManager();
        CPPUNIT_ASSERT (reqManager != NULL);
        reqManager->setBeginStatus(ESMRSuccess);
        reqManager->setNewCount(numDownloads);
        reqManager->setModCount(numDownloads);
        reqManager->setDelCount(0);

        checkDownload(manager, *source, numDownloads*2);
        cleanup(*source);
        delete source;
    }

    // Next sync with filter number (ALL), download N items
    void testDownloadFilterNumber() {

        int filterNumber = 2;
        TestSapiSyncSource* source = createSource(SYNC_MODE_TWO_WAY, 1234, 1234, -1, filterNumber, (size_t)time(NULL));
        TestSapiSyncManager manager(*source, *config);

        int numDownloads = 10;
        TestSapiMediaRequestManager* reqManager = manager.getSapiMediaRequestManager();
        CPPUNIT_ASSERT (reqManager != NULL);
        reqManager->setBeginStatus(ESMRSuccess);
        reqManager->setAllCount(numDownloads);

        checkDownload(manager, *source, filterNumber);
        cleanup(*source);
        delete source;
    }

    // First sync with no filters (ALL), download items until local storage is full
    void testDownloadAllLocalStorage() {

        TestSapiSyncSource* source = createSource(SYNC_MODE_TWO_WAY, 0, 0, -1, -1, 0);
        TestSapiSyncManager manager(*source, *config);

        int numDownloads = 20;
        TestSapiMediaRequestManager* reqManager = manager.getSapiMediaRequestManager();
        CPPUNIT_ASSERT (reqManager != NULL);
        reqManager->setBeginStatus(ESMRSuccess);
        reqManager->setAllCount(numDownloads);

        int limit = 6;
        source->setLocalQuota(limit);   // will throw quota error after N items

        checkDownload(manager, *source, limit, ESSMClientQuotaExceeded);
        cleanup(*source);
        delete source;
    }

    // Next sync with filter date (Changes), download items until local storage is full
    void testDownloadChangesLocalStorage() {

        TestSapiSyncSource* source = createSource(SYNC_MODE_TWO_WAY, 343434, 12345, -1, -1, (size_t)time(NULL));
        TestSapiSyncManager manager(*source, *config);

        int numDownloads = 12;
        TestSapiMediaRequestManager* reqManager = manager.getSapiMediaRequestManager();
        CPPUNIT_ASSERT (reqManager != NULL);
        reqManager->setBeginStatus(ESMRSuccess);
        reqManager->setNewCount(numDownloads);
        reqManager->setModCount(numDownloads);
        reqManager->setDelCount(numDownloads);

        int limit = 5;
        source->setLocalQuota(limit);   // will throw quota error after N items

        checkDownload(manager, *source, limit, ESSMClientQuotaExceeded);
        cleanup(*source);
        delete source;
    }

    // Next sync with no filters (Changes), network error during download
    void testDownloadNetworkError() {

        TestSapiSyncSource* source = createSource(SYNC_MODE_TWO_WAY, 1234, 12345, -1, -1, 0);
        TestSapiSyncManager manager(*source, *config);

        int numDownloads = 4;
        TestSapiMediaRequestManager* reqManager = manager.getSapiMediaRequestManager();
        CPPUNIT_ASSERT (reqManager != NULL);
        reqManager->setBeginStatus(ESMRSuccess);
        reqManager->setNewCount(numDownloads);

        reqManager->setDownloadStatus(ESMRNetworkError);

        checkDownload(manager, *source, 0, ESSMNetworkError);
        cleanup(*source);
        delete source;
    }

    // First sync with no filters (ALL), source error creating new item locally
    void testDownloadCreateItemError() {

        TestSapiSyncSource* source = createSource(SYNC_MODE_TWO_WAY, 0, 0, -1, -1, 0);
        TestSapiSyncManager manager(*source, *config);

        int numDownloads = 5;
        TestSapiMediaRequestManager* reqManager = manager.getSapiMediaRequestManager();
        CPPUNIT_ASSERT (reqManager != NULL);
        reqManager->setBeginStatus(ESMRSuccess);
        reqManager->setAllCount(numDownloads);

        source->setCreateItemError();

        checkDownload(manager, *source, 0, ESSMSetItemError);
        cleanup(*source);
        delete source;
    }

    // First sync with no filters (ALL), source error inserting an item locally
    void testDownloadInsertItemError() {

        TestSapiSyncSource* source = createSource(SYNC_MODE_TWO_WAY, 0, 0, -1, -1, 0);
        TestSapiSyncManager manager(*source, *config);

        int numDownloads = 10;
        TestSapiMediaRequestManager* reqManager = manager.getSapiMediaRequestManager();
        CPPUNIT_ASSERT (reqManager != NULL);
        reqManager->setBeginStatus(ESMRSuccess);
        reqManager->setAllCount(numDownloads);

        int itemsAddedOk = 4;
        source->setInsertItemError(itemsAddedOk);

        checkDownload(manager, *source, itemsAddedOk, ESSMSetItemError);
        cleanup(*source);
        delete source;
    }


    //
    // ------ test RESUME -------
    //

    // First sync with no filters (ALL), resume upload of one item
    void testUploadResumeAll() {

        TestSapiSyncSource* source = createSource(SYNC_MODE_TWO_WAY, 0, 0, -1, -1, 0);
        TestSapiSyncManager manager(*source, *config);

        int numUploads = 5;
        source->setAllCount(numUploads);

        // Simulate in the last sync 1 item was interrupted in upload
        source->getResume().setPropertyValue("luid-all-3-guid", RESUME_UPLOAD "," "luid-all-3" "," "luid-all-3" "," "1023");

        checkUpload(manager, *source, numUploads);
        cleanup(*source);
        delete source;
    }

    // Next sync with no filters (Changes), resume upload of one item
    void testUploadResumeChanges() {

        TestSapiSyncSource* source = createSource(SYNC_MODE_TWO_WAY, 252525, 151515, -1, -1, 0);
        TestSapiSyncManager manager(*source, *config);

        int numUploads = 6;
        source->setNewCount(numUploads);

        // Simulate in the last sync 1 item was interrupted in upload
        source->getResume().setPropertyValue("luid-new-0-guid", RESUME_UPLOAD "," "luid-new-0" "," "luid-new-0" "," "2000");

        checkUpload(manager, *source, numUploads);
        cleanup(*source);
        delete source;
    }

    // Next sync with no filters (Changes), resume upload of an orphan item (deleted)
    void testUploadResumeOrphan() {

        TestSapiSyncSource* source = createSource(SYNC_MODE_TWO_WAY, 252525, 151515, -1, -1, 0);
        TestSapiSyncManager manager(*source, *config);

        int numUploads = 6;
        source->setNewCount(numUploads);

        // Simulate in the last sync 1 item was interrupted in upload
        source->getResume().setPropertyValue("guid", RESUME_UPLOAD "," "luid-no-more" "," "name" "," "1224");

        checkUpload(manager, *source, numUploads);
        cleanup(*source);
        delete source;
    }

    // First sync with no filters (ALL), resume download of one item
    void testDownloadResumeAll() {

        TestSapiSyncSource* source = createSource(SYNC_MODE_TWO_WAY, 0, 0, -1, -1, 0);
        TestSapiSyncManager manager(*source, *config);

        int numDownloads = 6;
        TestSapiMediaRequestManager* reqManager = manager.getSapiMediaRequestManager();
        CPPUNIT_ASSERT (reqManager != NULL);
        reqManager->setBeginStatus(ESMRSuccess);
        reqManager->setAllCount(numDownloads);

        // Simulate in the last sync 1 item was interrupted in download
        source->getResume().setPropertyValue("guid-all-4", RESUME_DOWNLOAD "," "" "," "name" "," "1224");

        checkDownload(manager, *source, numDownloads);
        cleanup(*source);
        delete source;
    }

    // Next sync with no filters (Changes), resume download of one item
    void testDownloadResumeChanges() {

        TestSapiSyncSource* source = createSource(SYNC_MODE_TWO_WAY, 123, 4321, -1, -1, 0);
        TestSapiSyncManager manager(*source, *config);

        int numDownloads = 5;
        TestSapiMediaRequestManager* reqManager = manager.getSapiMediaRequestManager();
        CPPUNIT_ASSERT (reqManager != NULL);
        reqManager->setBeginStatus(ESMRSuccess);
        reqManager->setNewCount(numDownloads);

        // Simulate the last sync 1 item was interrupted in download
        source->getResume().setPropertyValue("guid-new-2", RESUME_DOWNLOAD "," "" "," "name" "," "1224");

        checkDownload(manager, *source, numDownloads);
        cleanup(*source);
        delete source;
    }

    // Next sync with no filters (Changes), resume download of an orphan item (deleted)
    void testDownloadResumeOrphan() {

        TestSapiSyncSource* source = createSource(SYNC_MODE_TWO_WAY, 123, 4321, -1, -1, 0);
        TestSapiSyncManager manager(*source, *config);

        int numDownloads = 5;
        TestSapiMediaRequestManager* reqManager = manager.getSapiMediaRequestManager();
        CPPUNIT_ASSERT (reqManager != NULL);
        reqManager->setBeginStatus(ESMRSuccess);
        reqManager->setNewCount(numDownloads);

        // Simulate the last sync 1 item was interrupted in download
        source->getResume().setPropertyValue("guid-no-more", RESUME_DOWNLOAD "," "" "," "name" "," "1224");

        checkDownload(manager, *source, numDownloads);
        cleanup(*source);
        delete source;
    }

    // -------------------------------------------------------------------------------------------------------------

    /**
     * Used by many test methods above.
     * Creates a SapiSyncManager given the source passed and the config/report set in the setup().
     * Expects the getAllItems SAPI is called, with 'numItems' items on the Server.
     */
     void checkAllServerItems(const int numItems, TestSapiSyncSource& source) {

        SyncSourceConfig* ssc = config->getSyncSourceConfig(TEST_SOURCE_NAME);
        CPPUNIT_ASSERT (ssc != NULL);

        // Test ALL list, with num items S2C
        TestSyncSourceListener* listener = new TestSyncSourceListener("T1");
        setSyncSourceListener(listener);

        TestSapiSyncManager manager(source, *config);

        TestSapiMediaRequestManager* reqManager = manager.getSapiMediaRequestManager();
        CPPUNIT_ASSERT (reqManager != NULL);
        reqManager->setBeginStatus(ESMRSuccess);
        reqManager->setAllCount(numItems);

        int ret = manager.beginSync();
        CPPUNIT_ASSERT (ret == ESSMSuccess);
        CPPUNIT_ASSERT_EQUAL (numItems, manager.getAllList().size());


        int total = numItems;
        bool err;
        int limit = ssc->getIntProperty(PROPERTY_SYNC_ITEM_NUMBER_FROM_SERVER, &err);
        if (limit > 0) {
            total = min(limit, numItems);
        }
        CPPUNIT_ASSERT_EQUAL (total, listener->getTotalServerItems());

        unsetSyncSourceListener("T1");
    }


    /**
     * Used by many test methods above.
     * Creates a SapiSyncManager given the source passed and the config/report set in the setup().
     * Expects the getItemsChanges SAPI is called, with newItems/modItems/delItems items on the Server.
     */
    StringBuffer checkChangedServerItems(const int newItems, const int modItems, const int delItems, TestSapiSyncSource& source) {

        SyncSourceConfig* ssc = config->getSyncSourceConfig(TEST_SOURCE_NAME);
        CPPUNIT_ASSERT (ssc != NULL);

        // Test CHANGES list, with n items S2C
        TestSyncSourceListener* listener = new TestSyncSourceListener("T1");
        setSyncSourceListener(listener);

        TestSapiSyncManager manager(source, *config);
        TestSapiMediaRequestManager* reqManager = manager.getSapiMediaRequestManager();
        CPPUNIT_ASSERT (reqManager != NULL);

        reqManager->setBeginStatus(ESMRSuccess);
        reqManager->setNewCount(newItems);
        reqManager->setModCount(modItems);
        reqManager->setDelCount(delItems);

        int ret = manager.beginSync();
        CPPUNIT_ASSERT (ret == ESSMSuccess);
        CPPUNIT_ASSERT_EQUAL (newItems+modItems, manager.getNewList().size());  // mod are not found in mappings -> new
        CPPUNIT_ASSERT_EQUAL (0, manager.getModList().size());
        CPPUNIT_ASSERT_EQUAL (delItems, manager.getDelList().size());

        // TestSapiSyncSource supports NEW+MOD+DEL
        int total = newItems + modItems + delItems;
        bool err;
        int limit = ssc->getIntProperty(PROPERTY_SYNC_ITEM_NUMBER_FROM_SERVER, &err);
        if (limit > 0) {
            total = min(limit, total);
        }
        CPPUNIT_ASSERT_EQUAL (total, listener->getTotalServerItems());

        unsetSyncSourceListener("T1");

        return reqManager->getFromDate();
    }


    /**
     * Used by many test methods above.
     * Creates a SapiSyncManager given the source passed and the config/report set in the setup().
     * Expects the ssource's PopulateAllItems is called, with 'numItems' items locally.
     */
     void checkAllClientItems(const int numItems, TestSapiSyncSource& source) {

        SyncSourceConfig* ssc = config->getSyncSourceConfig(TEST_SOURCE_NAME);
        CPPUNIT_ASSERT (ssc != NULL);

        // Test ALL list, with num items S2C
        TestSyncSourceListener* listener = new TestSyncSourceListener("T1");
        setSyncSourceListener(listener);

        source.setAllCount(numItems);
        TestSapiSyncManager manager(source, *config);

        int ret = manager.beginSync();
        CPPUNIT_ASSERT (ret == ESSMSuccess);
        CPPUNIT_ASSERT_EQUAL (numItems, source.getItemsList("ALL")->size());

        int total = numItems;
        bool err;
        int limit = ssc->getIntProperty(PROPERTY_SYNC_ITEM_NUMBER_FROM_CLIENT, &err);
        if (limit > 0) {
            total = min(limit, numItems);
        }
        CPPUNIT_ASSERT_EQUAL (total, listener->getTotalClientItems());

        unsetSyncSourceListener("T1");
    }

     /**
     * Used by many test methods above.
     * Creates a SapiSyncManager given the source passed and the config/report set in the setup().
     * Expects the ssource's PopulateCHanges is called, with newItems/modItems/delItems items locally.
     */
     void checkChangedClientItems(const int newItems, const int modItems, const int delItems, TestSapiSyncSource& source) {

        SyncSourceConfig* ssc = config->getSyncSourceConfig(TEST_SOURCE_NAME);
        CPPUNIT_ASSERT (ssc != NULL);

        // Test ALL list, with num items S2C
        TestSyncSourceListener* listener = new TestSyncSourceListener("T1");
        setSyncSourceListener(listener);

        source.setNewCount(newItems);
        source.setModCount(modItems);
        source.setDelCount(delItems);
        TestSapiSyncManager manager(source, *config);

        int ret = manager.beginSync();
        CPPUNIT_ASSERT (ret == ESSMSuccess);
        CPPUNIT_ASSERT_EQUAL (newItems, source.getItemsList("NEW")->size());
        CPPUNIT_ASSERT_EQUAL (modItems, source.getItemsList("MOD")->size());
        CPPUNIT_ASSERT_EQUAL (delItems, source.getItemsList("DEL")->size());

        int total = newItems + modItems + delItems;
        bool err;
        int limit = ssc->getIntProperty(PROPERTY_SYNC_ITEM_NUMBER_FROM_CLIENT, &err);
        if (limit > 0) {
            total = min(limit, total);
        }
        CPPUNIT_ASSERT_EQUAL (total, listener->getTotalClientItems());

        unsetSyncSourceListener("T1");
    }


    /**
     * Used by many test methods above.
     * Creates a SapiSyncManager given the source passed and the config/report set in the setup().
     * Expects the ALL behavior is followed
     */
     void checkAllItems(const int clientItems, const int serverItems, TestSapiSyncSource& source,
                        const int filteredClientItems, const int filteredServerItems) {

        SyncSourceConfig* ssc = config->getSyncSourceConfig(TEST_SOURCE_NAME);
        CPPUNIT_ASSERT (ssc != NULL);

        // Test ALL list, with num items S2C
        TestSyncSourceListener* listener = new TestSyncSourceListener("T1");
        setSyncSourceListener(listener);

        source.setAllCount(clientItems);
        TestSapiSyncManager manager(source, *config);

        TestSapiMediaRequestManager* reqManager = manager.getSapiMediaRequestManager();
        CPPUNIT_ASSERT (reqManager != NULL);
        reqManager->setBeginStatus(ESMRSuccess);
        reqManager->setAllCount(serverItems);


        int ret = manager.beginSync();
        CPPUNIT_ASSERT (ret == ESSMSuccess);

        int serverTotal = serverItems - filteredServerItems;
        int clientTotal = clientItems - filteredClientItems;
        CPPUNIT_ASSERT_EQUAL (serverTotal, manager.getAllList().size());
        CPPUNIT_ASSERT_EQUAL (clientTotal, source.getItemsList("ALL")->size());

        // check the total number fired
        bool err = false;
        int clientLimit = ssc->getIntProperty(PROPERTY_SYNC_ITEM_NUMBER_FROM_CLIENT, &err);
        if (clientLimit > 0) {
            clientTotal = min(clientLimit-filteredClientItems, clientTotal);
        }
        int serverLimit = ssc->getIntProperty(PROPERTY_SYNC_ITEM_NUMBER_FROM_SERVER, &err);
        if (serverLimit > 0) {
            serverTotal = min(serverLimit-filteredServerItems, serverItems);
        }
        CPPUNIT_ASSERT_EQUAL (serverTotal, listener->getTotalServerItems());
        CPPUNIT_ASSERT_EQUAL (clientTotal, listener->getTotalClientItems());

        unsetSyncSourceListener("T1");
    }

     /**
     * Used by many test methods above.
     * Creates a SapiSyncManager given the source passed and the config/report set in the setup().
     * Expects the CHANGES behavior is followed
     */
     void checkChangedItems(const int clientItems, const int serverItems, TestSapiSyncSource& source,
                            const int filteredClientItems, const int filteredServerItems) {

        SyncSourceConfig* ssc = config->getSyncSourceConfig(TEST_SOURCE_NAME);
        CPPUNIT_ASSERT (ssc != NULL);

        // Test ALL list, with num items S2C
        TestSyncSourceListener* listener = new TestSyncSourceListener("T1");
        setSyncSourceListener(listener);

        source.setNewCount(clientItems);
        source.setModCount(clientItems);
        source.setDelCount(clientItems);
        TestSapiSyncManager manager(source, *config);

        TestSapiMediaRequestManager* reqManager = manager.getSapiMediaRequestManager();
        CPPUNIT_ASSERT (reqManager != NULL);
        reqManager->setBeginStatus(ESMRSuccess);
        reqManager->setNewCount(serverItems);
        reqManager->setModCount(serverItems);
        reqManager->setDelCount(serverItems);

        int ret = manager.beginSync();
        CPPUNIT_ASSERT (ret == ESSMSuccess);

        int serverNewTotal = serverItems - filteredServerItems;
        int serverModTotal = serverItems;
        int serverDelTotal = serverItems;
        int clientNewTotal = clientItems - filteredClientItems;
        int clientModTotal = clientItems;
        int clientDelTotal = clientItems;
        CPPUNIT_ASSERT_EQUAL (serverNewTotal+serverModTotal, manager.getNewList().size());  // mod items not found in mappings become new items
        CPPUNIT_ASSERT_EQUAL (0, manager.getModList().size());
        CPPUNIT_ASSERT_EQUAL (serverDelTotal, manager.getDelList().size());
        CPPUNIT_ASSERT_EQUAL (clientNewTotal, source.getItemsList("NEW")->size());
        CPPUNIT_ASSERT_EQUAL (clientModTotal, source.getItemsList("MOD")->size());
        CPPUNIT_ASSERT_EQUAL (clientDelTotal, source.getItemsList("DEL")->size());

        // check the total number fired
        // (testSapiSyncSource supports NEW,MOD,DEL)
        bool err = false;
        int clientTotal = clientNewTotal + clientModTotal + clientDelTotal;
        int clientLimit = ssc->getIntProperty(PROPERTY_SYNC_ITEM_NUMBER_FROM_CLIENT, &err);
        if (clientLimit > 0) {
            clientTotal = min(clientLimit-filteredClientItems, clientTotal);
        }
        int serverTotal = serverNewTotal + serverModTotal + serverDelTotal;
        int serverLimit = ssc->getIntProperty(PROPERTY_SYNC_ITEM_NUMBER_FROM_SERVER, &err);
        if (serverLimit > 0) {
            serverTotal = min(serverLimit-filteredServerItems, serverTotal);
        }
        CPPUNIT_ASSERT_EQUAL (serverTotal, listener->getTotalServerItems());
        CPPUNIT_ASSERT_EQUAL (clientTotal, listener->getTotalClientItems());

        unsetSyncSourceListener("T1");
    }

     /**
     * Used by many test methods above.
     * Tests the SapiSyncManager::upload() given the source passed and the config/report set in the setup().
     * Expects the upload of 'expectedUploads' items and an upload() returning expectedRet.
     */
    void checkUpload(TestSapiSyncManager& manager, TestSapiSyncSource& source, const int expectedUploads,
                     ESapiSyncManagerError expectedRet = ESSMSuccess) {

        // save the last tstamp before sync
        unsigned long tstampBefore = source.getConfig().getLast();

        // Test ALL list, with num items S2C
        TestSyncItemListener* listener = new TestSyncItemListener("T1");
        setSyncItemListener(listener);

        int ret = manager.beginSync();
        CPPUNIT_ASSERT (ret == ESSMSuccess);

        ret = manager.upload();
        CPPUNIT_ASSERT (ret == expectedRet);

        CPPUNIT_ASSERT_EQUAL (expectedUploads, listener->getFiredClientItems());

        // check resume map
        if (ret == ESSMSuccess ||
            ret == ESSMServerQuotaExceeded) {
            // for these codes, resume map should be empty
            CPPUNIT_ASSERT (source.getResume().getProperties().hasMoreElement() == false);
        } else {
            // for other codes, resume map should contain the last entry
            CPPUNIT_ASSERT (source.getResume().getProperties().hasMoreElement() == true);
        }

        // mappings/cache: 'numUploads' entries expected
        Enumeration& mappings = source.getMappings().getProperties();
        Enumeration& cache = source.getCache().getProperties();
        for (int i=0; i<expectedUploads; i++) {
            CPPUNIT_ASSERT (mappings.getNextElement() != NULL);
            CPPUNIT_ASSERT (cache.getNextElement() != NULL);
        }
        CPPUNIT_ASSERT (mappings.getNextElement() == NULL);
        CPPUNIT_ASSERT (cache.getNextElement() == NULL);

        // report: should contain all items we tried to upload
        int total = expectedUploads;
        if (expectedRet != ESSMSuccess) {
            total += 1; // last upload was unsuccessful, but error is reported
        }
        SyncSourceReport* ssr = report.getSyncSourceReport(TEST_SOURCE_NAME);
        CPPUNIT_ASSERT (ssr != NULL);
        CPPUNIT_ASSERT_EQUAL (total, ssr->getItemReportCount(SERVER, COMMAND_ADD));

        // last upload tstamp: should be set only if all uploads were successful
        unsigned long tstampAfter = source.getConfig().getLast();
        unsigned long tstampExpected = tstampBefore;
        if (ret == ESSMSuccess) {
            tstampExpected = (long)time(NULL);   // TODO: check here the tstamp from server
        }
        int error = tstampExpected - tstampAfter;
        CPPUNIT_ASSERT (error == 0 || error <= 1);    // no more than 1 second later

        unsetSyncItemListener("T1");
    }


     /**
     * Used by many test methods above.
     * Tests the SapiSyncManager::download() given the source passed and the config/report set in the setup().
     * Expects the download of 'expectedDownloads' items and an download() returning expectedRet.
     */
    void checkDownload(TestSapiSyncManager& manager, TestSapiSyncSource& source, const int expectedDownloads,
                     ESapiSyncManagerError expectedRet = ESSMSuccess) {

        // save the last tstamp before sync
        bool err = false;
        long tstampBefore = source.getConfig().getLongProperty(PROPERTY_DOWNLOAD_LAST_TIME_STAMP, &err);

        // Test ALL list, with num items S2C
        TestSyncItemListener* listener = new TestSyncItemListener("T1");
        setSyncItemListener(listener);

        int ret = manager.beginSync();
        CPPUNIT_ASSERT (ret == ESSMSuccess);

        ret = manager.download();
        CPPUNIT_ASSERT (ret == expectedRet);

        CPPUNIT_ASSERT_EQUAL (expectedDownloads, listener->getFiredServerItems());

        // check resume map
        if (ret == ESSMSuccess ||
            ret == ESSMClientQuotaExceeded ||
            ret == ESSMSetItemError) {
            // for these codes, resume map should be empty
            CPPUNIT_ASSERT (source.getResume().getProperties().hasMoreElement() == false);
        } else {
            // for other codes, resume map should contain the last entry
            CPPUNIT_ASSERT (source.getResume().getProperties().hasMoreElement() == true);
        }

        // mappings/cache: 'numDownloads' entries expected
        Enumeration& mappings = source.getMappings().getProperties();
        Enumeration& cache = source.getCache().getProperties();
        for (int i=0; i<expectedDownloads; i++) {
            CPPUNIT_ASSERT (mappings.getNextElement() != NULL);
            CPPUNIT_ASSERT (cache.getNextElement() != NULL);
        }
        CPPUNIT_ASSERT (mappings.getNextElement() == NULL);
        CPPUNIT_ASSERT (cache.getNextElement() == NULL);

        // report: check the number of items successfully downloaded
        SyncSourceReport* ssr = report.getSyncSourceReport(TEST_SOURCE_NAME);
        CPPUNIT_ASSERT (ssr != NULL);
        CPPUNIT_ASSERT_EQUAL (expectedDownloads, ssr->getItemReportSuccessfulCount(CLIENT, COMMAND_ADD));

        // report: check also the total number of items downloaded is at least bigger (successful or not)
        if (expectedRet != ESSMSuccess) {
            CPPUNIT_ASSERT(ssr->getItemReportCount(CLIENT, COMMAND_ADD) > expectedDownloads);
        }

        // last download tstamp: should be set only if all downloads were successful
        long tstampAfter = source.getConfig().getLongProperty(PROPERTY_DOWNLOAD_LAST_TIME_STAMP, &err);
        long tstampExpected = tstampBefore;
        if (ret == ESSMSuccess) {
            tstampExpected = (long)time(NULL);   // TODO: check here the tstamp from server
        }
        int error = tstampExpected - tstampAfter;
        CPPUNIT_ASSERT (error == 0 || error <= 1);    // no more than 1 second later

        unsetSyncItemListener("T1");
    }


    TestSapiSyncSource* createSource(const char* syncMode, unsigned long lastUpload, unsigned long lastDownload,
                                     const int clientFilterNumber, const int serverFilterNumber, size_t filterDate) {

        SyncSourceConfig* ssc = config->getSyncSourceConfig(TEST_SOURCE_NAME);
        CPPUNIT_ASSERT (ssc != NULL);

        ssc->setSync(syncMode);
        ssc->setLast(lastUpload);
        ssc->setLongProperty(PROPERTY_DOWNLOAD_LAST_TIME_STAMP,     lastDownload);
        ssc->setIntProperty (PROPERTY_SYNC_ITEM_NUMBER_FROM_SERVER, serverFilterNumber);
        ssc->setIntProperty (PROPERTY_SYNC_ITEM_NUMBER_FROM_CLIENT, clientFilterNumber);

        SyncSourceReport* ssr = report.getSyncSourceReport(TEST_SOURCE_NAME);
        CPPUNIT_ASSERT (ssr != NULL);

        return new TestSapiSyncSource(*ssc, *ssr, filterDate, filterDate);
    }


    /// Cleans up the source's cache/mappings/resume tables.
    void cleanup(TestSapiSyncSource& source) {

        source.getMappings().removeAllProperties();
        source.getMappings().close();

        source.getCache().removeAllProperties();
        source.getCache().close();

        source.getResume().removeAllProperties();
        source.getResume().close();
     }

};



CPPUNIT_TEST_SUITE_REGISTRATION( SapiSyncManagerTest );

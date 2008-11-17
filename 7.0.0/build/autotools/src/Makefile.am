#EXTRA_PROGRAMS = vcardconverter
EXTRA_PROGRAMS = briefcase
lib_LTLIBRARIES = libsync4j.la

SOURCES_BASE = \
    lLog.cpp \
    lautotoolsadapter.cpp \
    lArrayElement.cpp \
    lArrayList.cpp \
    lArrayListKeyValueStore.cpp \
    lBasicTime.cpp \
    lKeyValuePair.cpp \
    lWKeyValuePair.cpp \
    lStringBuffer.cpp \
    lPropertyFile.cpp \
    lWString.cpp \
    lXMLProcessor.cpp \
    lbaseutils.cpp \
    lbase64.cpp \
    lquoted-printable.cpp \
    lerror.cpp \
    lErrorHandler.cpp \
    lItemContainer.cpp \
    lmd5.c

SOURCES_EVENT = \
    lBaseEvent.cpp \
    lFireEvent.cpp \
    lListener.cpp \
    lManageListener.cpp \
    lSetListener.cpp \
    lSyncEvent.cpp \
    lSyncItemEvent.cpp \
    lSyncItemListener.cpp \
    lSyncListener.cpp \
    lSyncSourceEvent.cpp \
    lSyncSourceListener.cpp \
    lSyncStatusEvent.cpp \
    lSyncStatusListener.cpp \
    lTransportEvent.cpp \
    lTransportListener.cpp

SOURCES_FRAMEWORK = \
    lFormatter.cpp \
    lAbstractCommand.cpp \
    lAdd.cpp \
    lAllClause.cpp \
    lAlert.cpp \
    lAnchor.cpp \
    lAtomic.cpp \
    lAuthentication.cpp \
    lChal.cpp \
    lClause.cpp \
    lClauseUtil.cpp \
    lCmdID.cpp \
    lComplexData.cpp \
    lContentTypeInfo.cpp \
    lContentTypeParameter.cpp \
    lCopy.cpp \
    lCred.cpp \
    lCTCap.cpp \
    lCTPropParam.cpp \
    lCTTypeSupported.cpp \
    lData.cpp \
    lDataStore.cpp \
    lDelete.cpp \
    lDevInf.cpp \
    lDevInfData.cpp \
    lDevInfItem.cpp \
    lDSMem.cpp \
    lEMI.cpp \
    lExec.cpp \
    lExt.cpp \
    lFieldClause.cpp \
    lFilter.cpp \
    lGet.cpp \
    lItem.cpp \
    lItemizedCommand.cpp \
    lLogicalClause.cpp \
    lMap.cpp \
    lMapItem.cpp \
    lMem.cpp \
    lMeta.cpp \
    lMetInf.cpp \
    lModificationCommand.cpp \
    lNextNonce.cpp \
    lObjectDel.cpp \
    lParser.cpp \
    lProperty.cpp \
    lPropParam.cpp \
    lPut.cpp \
    lReplace.cpp \
    lResponseCommand.cpp \
    lResults.cpp \
    lSearch.cpp \
    lSequence.cpp \
    lSessionID.cpp \
    lSource.cpp \
    lSourceArray.cpp \
    lSourceFilter.cpp \
    lSourceRef.cpp \
    lStatus.cpp \
    lStringElement.cpp \
    lSync.cpp \
    lSyncBody.cpp \
    lSyncCap.cpp \
    lSyncHdr.cpp \
    lSyncML.cpp \
    lSyncType.cpp \
    lSyncTypeArray.cpp \
    lSyncNotification.cpp \
    lSyncAlert.cpp \
    lTarget.cpp \
    lTargetRef.cpp \
    lVerDTD.cpp \
    lVerProto.cpp \
    lWhereClause.cpp

SOURCES_HTTP = \
    lCurlTransportAgent.cpp \
    lTransportAgentFactory.cpp \
    lHTTPHeader.cpp \
    lProxy.cpp \
    lTransportAgent.cpp \
    lURL.cpp

SOURCES_SPDM = \
    lDMTree.cpp \
    lDMTreeFactory.cpp \
    lDeviceManagementNode.cpp \
    lManagementNode.cpp 

SOURCES_SPDS = \
    lAccessConfig.cpp \
    lBodyPart.cpp \
    lCredentialHandler.cpp \
    lCacheSyncSource.cpp \
    lDataTransformerFactory.cpp \
    lDataTransformer.cpp \
    lDeviceConfig.cpp \
    lB64Decoder.cpp \
    lB64Encoder.cpp \
    lDESDecoder.cpp \
    lDESEncoder.cpp \
    lDefaultConfigFactory.cpp \
    lEmailData.cpp \
    lFileSyncSource.cpp \
    lFileData.cpp \
    lItemReport.cpp \
    lMailMessage.cpp \
    lRawFileSyncSource.cpp \
    lSyncManagerConfig.cpp \
    lSyncItem.cpp \
    lSyncItemStatus.cpp \
    lSyncMLBuilder.cpp \
    lSyncMLProcessor.cpp \
    lSyncManager.cpp \
    lSyncMap.cpp \
    lSyncReport.cpp \
    lSyncSource.cpp \
    lSyncSourceConfig.cpp \
    lSyncSourceReport.cpp \
    lspdsutils.cpp 

SOURCES_CLIENT = \
    lDMTClientConfig.cpp \
    lSyncClient.cpp

SOURCES_VOCL = \
    lVConverter.cpp \
    lVObject.cpp \
    lVObjectFactory.cpp \
    lVProperty.cpp \
\
    lCalendar.cpp \
    lEvent.cpp \
    lToDo.cpp \
    liCalConverter.cpp \
    liCalProperty.cpp \
\
    lAddress.cpp \
    lBusinessDetail.cpp \
    lContact.cpp \
    lContact30.cpp \
    lContactDetail.cpp \
    lEmail.cpp \
    lName.cpp \
    lNote.cpp \
    lPersonalDetail.cpp \
    lPhone.cpp \
    lTitle.cpp \
    lTypedProperty.cpp \
    lWebPage.cpp \
    lvCardConverter.cpp \
    lvCardProperty.cpp

SOURCES_LIBSYNC4J = $(SOURCES_BASE) $(SOURCES_EVENT) $(SOURCES_FRAMEWORK) $(SOURCES_HTTP) $(SOURCES_SPDM) $(SOURCES_SPDS) $(SOURCES_CLIENT) $(SOURCES_VOCL)

# Explicitly copy each source file here.
# automake 1.10 creates rules which expect the *file* in this directory;
# a symbolic link is not enough. The "l" prefix is necessary to avoid
# a circular dependency (local copy of the same name as file in vpath).
$(SOURCES_LIBSYNC4J): l% : %
	ln -s $< $@
CLEANFILES = $(SOURCES_LIBSYNC4J)

libsync4j_la_SOURCES = $(SOURCES_LIBSYNC4J:%=l%)
libsync4j_la_LDFLAGS = -version-info 3:0 @CURL_LDFLAGS@ @CPPUNIT_LDFLAGS@
libsync4j_la_CXXFLAGS = @CURL_CFLAGS@ @CPPUNIT_CXXFLAGS@

# client_SOURCES = client.cpp TestSyncSource.cpp TestSyncSource2.cpp
# client_LDADD = libsync4j.la


briefcase_SOURCES = BriefcaseClient.cpp BriefcaseCacheSyncSource.cpp
briefcase_LDADD = libsync4j.la

#vcardconverter_SOURCES = vcardconverter.cpp
#vcardconverter_LDADD = libsync4j.la


#NotifProcessorTest_SOURCES = NotifProcessorTest.cpp
#NotifProcessorTest_LDADD = libsync4j.la

vpath %.cpp $(srcdir)/../../../src/c++/common/base             \
            $(srcdir)/../../../src/c++/common/base/util        \
            $(srcdir)/../../../src/c++/common/client           \
            $(srcdir)/../../../src/c++/common/examples         \
            $(srcdir)/../../../src/c++/common/event            \
            $(srcdir)/../../../src/c++/common/filter           \
            $(srcdir)/../../../src/c++/common/syncml/formatter \
            $(srcdir)/../../../src/c++/common/syncml/core      \
            $(srcdir)/../../../src/c++/common/syncml/parser    \
            $(srcdir)/../../../src/c++/common/http             \
            $(srcdir)/../../../src/c++/common/spdm             \
            $(srcdir)/../../../src/c++/common/spds             \
            $(srcdir)/../../../src/c++/common/test             \
            $(srcdir)/../../../src/c++/common/vocl             \
            $(srcdir)/../../../src/c++/common/vocl/iCalendar   \
            $(srcdir)/../../../src/c++/common/vocl/vCard       \
            $(srcdir)/../../../src/c++/posix/base              \
            $(srcdir)/../../../src/c++/posix/http              \
            $(srcdir)/../../../src/c++/posix/spdm              \
            $(srcdir)/../../../src/c++/posix/spds

vpath %.c   $(srcdir)/../../../src/c++/common/base

AM_CPPFLAGS = -I$(srcdir)/../../../src/include/posix -I$(srcdir)/../../../src/include/common

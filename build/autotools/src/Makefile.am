#EXTRA_PROGRAMS = vcardconverter
EXTRA_PROGRAMS = briefcase
lib_LTLIBRARIES = libfunambol.la

SOURCES_BASE = \
    lLog.cpp \
    lautotoolsadapter.cpp \
    lArrayElement.cpp \
    lArrayList.cpp \
    lMemoryKeyValueStore.cpp \
    lBasicTime.cpp \
    lStringBuffer.cpp \
    lStringMap.cpp \
    lPropertyFile.cpp \
    lWString.cpp \
    lXMLProcessor.cpp \
    lbaseutils.cpp \
    lbase64.cpp \
    lquoted-printable.cpp \
    lerror.cpp \
    lItemContainer.cpp \
    lPlatformAdapter.cpp \
    lmd5.cpp

SOURCES_EVENT = \
    lBaseEvent.cpp \
    lFireEvent.cpp \
    lManageListener.cpp \
    lSetListener.cpp \
    lSyncEvent.cpp \
    lSyncItemEvent.cpp \
    lSyncSourceEvent.cpp \
    lSyncStatusEvent.cpp \
    lTransportEvent.cpp 

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
    lMacTransportAgent.cpp \
    lMozillaTransportAgent.cpp \
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
    lDataTransformerFactory.cpp \
    lDataTransformer.cpp \
    lDeviceConfig.cpp \
    lB64Decoder.cpp \
    lB64Encoder.cpp \
    lDESDecoder.cpp \
    lDESEncoder.cpp \
    lDefaultConfigFactory.cpp \
    lEmailData.cpp \
    lFileData.cpp \
    lItemReport.cpp \
    lMailMessage.cpp \
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
    lTestFileSyncSource.cpp \
    lCacheSyncSource.cpp \
    lFileSyncSource.cpp \
    lOptionParser.cpp \
    lConfigSyncSource.cpp \
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

SOURCES_PUSH = \
    lCTPConfig.cpp \
    lCTPMessage.cpp \
    lCTPParam.cpp \
    lCTPService.cpp \
    lCTPThreadPool.cpp \
    lFThread.cpp \
    lFSocket.cpp

SOURCES_LIBFUNAMBOL = $(SOURCES_BASE) $(SOURCES_EVENT) $(SOURCES_FRAMEWORK) $(SOURCES_HTTP) $(SOURCES_SPDM) $(SOURCES_SPDS) $(SOURCES_CLIENT) $(SOURCES_VOCL) $(SOURCES_PUSH)

# Explicitly copy each source file here.
# automake 1.10 creates rules which expect the *file* in this directory;
# a symbolic link is not enough. The "l" prefix is necessary to avoid
# a circular dependency (local copy of the same name as file in vpath).
$(SOURCES_LIBFUNAMBOL): l% : %
	ln -s $< $@
CLEANFILES = $(SOURCES_LIBFUNAMBOL)

libfunambol_la_SOURCES = $(SOURCES_LIBFUNAMBOL:%=l%)
libfunambol_la_LDFLAGS = -version-info @SO_VERSION_INFO@ @CURL_LDFLAGS@ @CPPUNIT_LDFLAGS@
libfunambol_la_CXXFLAGS = @CURL_CFLAGS@ @CPPUNIT_CXXFLAGS@

# client_SOURCES = client.cpp TestSyncSource.cpp TestSyncSource2.cpp
# client_LDADD = libfunambol.la

#vcardconverter_SOURCES = vcardconverter.cpp
#vcardconverter_LDADD = libfunambol.la

vpath %.cpp $(srcdir)/../../../src/cpp/common/base             \
            $(srcdir)/../../../src/cpp/common/base/util        \
            $(srcdir)/../../../src/cpp/common/client           \
            $(srcdir)/../../../src/cpp/common/examples         \
            $(srcdir)/../../../src/cpp/common/event            \
            $(srcdir)/../../../src/cpp/common/filter           \
            $(srcdir)/../../../src/cpp/common/syncml/formatter \
            $(srcdir)/../../../src/cpp/common/syncml/core      \
            $(srcdir)/../../../src/cpp/common/syncml/parser    \
            $(srcdir)/../../../src/cpp/common/http             \
            $(srcdir)/../../../src/cpp/common/spdm             \
            $(srcdir)/../../../src/cpp/common/spds             \
            $(srcdir)/../../../src/cpp/common/push             \
            $(srcdir)/../../../src/cpp/common/test             \
            $(srcdir)/../../../src/cpp/common/vocl             \
            $(srcdir)/../../../src/cpp/common/vocl/iCalendar   \
            $(srcdir)/../../../src/cpp/common/vocl/vCard       \
            $(srcdir)/../../../src/cpp/posix/base              \
            $(srcdir)/../../../src/cpp/posix/base/adapter      \
            $(srcdir)/../../../src/cpp/posix/http              \
            $(srcdir)/../../../src/cpp/posix/spdm              \
            $(srcdir)/../../../src/cpp/posix/spds              \
            $(srcdir)/../../../src/cpp/posix/push

vpath %.c   $(srcdir)/../../../src/cpp/common/base

AM_CPPFLAGS = -DFUNAMBOL_BUILD_API -I$(srcdir)/../../../src/include/posix -I$(srcdir)/../../../src/include/common


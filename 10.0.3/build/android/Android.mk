LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_CFLAGS += -D ENABLE_FUNAMBOL_NAMESPACE
LOCAL_CFLAGS += -D FUNAMBOL_BUILD_API
#LOCAL_CFLAGS += -D HAVE_CONFIG_H
LOCAL_CFLAGS += -D FUN_TRANSPORT_AGENT=FUN_CURL_TRANSPORT_AGENT

LOCAL_CFLAGS += -g -O0

#LOCAL_CFLAGS += -O2

#LOCAL_CFLAGS += -D USE_WCHAR

LOCAL_CFLAGS += -I stlport/stlport

LOCAL_CFLAGS += -I sources/FunambolSDK/include/posix
LOCAL_CFLAGS += -I sources/FunambolSDK/include/android
LOCAL_CFLAGS += -I sources/FunambolSDK/include/common

LOCAL_CFLAGS += -I sources/FunambolSDK/include/common


LOCAL_SRC_FILES := \
cpp/common/inputStream/InputStream.cpp \
cpp/common/inputStream/BufferInputStream.cpp \
cpp/common/base/error.cpp \
cpp/common/base/md5.cpp \
cpp/common/base/quoted-printable.cpp \
cpp/common/base/util/EncodingHelper.cpp \
cpp/common/base/util/baseutils.cpp \
cpp/common/base/util/ArrayElement.cpp \
cpp/common/base/util/ArrayList.cpp \
cpp/common/base/util/BasicTime.cpp \
cpp/common/base/util/ItemContainer.cpp \
cpp/common/base/util/MemoryKeyValueStore.cpp \
cpp/common/base/util/PropertyFile.cpp \
cpp/common/base/util/StringBuffer.cpp \
cpp/common/base/util/StringMap.cpp \
cpp/common/base/util/XMLProcessor.cpp \
cpp/common/base/util/WString.cpp \
cpp/common/client/CacheSyncSource.cpp \
cpp/common/client/DMTClientConfig.cpp \
cpp/common/client/SyncClient.cpp \
cpp/common/event/BaseEvent.cpp \
cpp/common/event/FireEvent.cpp \
cpp/common/event/ManageListener.cpp \
cpp/common/event/SetListener.cpp \
cpp/common/event/SyncEvent.cpp \
cpp/common/event/SyncItemEvent.cpp \
cpp/common/event/SyncSourceEvent.cpp \
cpp/common/event/SyncStatusEvent.cpp \
cpp/common/event/TransportEvent.cpp \
cpp/common/filter/AllClause.cpp \
cpp/common/filter/Clause.cpp \
cpp/common/filter/ClauseUtil.cpp \
cpp/common/filter/FieldClause.cpp \
cpp/common/filter/LogicalClause.cpp \
cpp/common/filter/SourceFilter.cpp \
cpp/common/filter/WhereClause.cpp \
cpp/common/http/HTTPHeader.cpp \
cpp/common/http/Proxy.cpp \
cpp/common/http/TransportAgent.cpp \
cpp/common/http/URL.cpp \
cpp/common/push/CTPConfig.cpp \
cpp/common/push/CTPMessage.cpp \
cpp/common/push/CTPParam.cpp \
cpp/common/push/CTPService.cpp \
cpp/common/push/CTPThreadPool.cpp \
cpp/common/spdm/DMTree.cpp \
cpp/common/spdm/ManagementNode.cpp \
cpp/common/spds/Chunk.cpp \
cpp/common/spds/AccessConfig.cpp \
cpp/common/spds/B64Decoder.cpp \
cpp/common/spds/B64Encoder.cpp \
cpp/common/spds/BodyPart.cpp \
cpp/common/spds/CredentialHandler.cpp \
cpp/common/spds/DataTransformer.cpp \
cpp/common/spds/DataTransformerFactory.cpp \
cpp/common/spds/DefaultConfigFactory.cpp \
cpp/common/spds/DeviceConfig.cpp \
cpp/common/spds/EmailData.cpp \
cpp/common/spds/FileData.cpp \
cpp/common/spds/FolderData.cpp \
cpp/common/spds/ItemReport.cpp \
cpp/common/spds/MailMessage.cpp \
cpp/common/spds/MailSyncSourceConfig.cpp \
cpp/common/spds/spdsutils.cpp \
cpp/common/spds/SyncItem.cpp \
cpp/common/spds/SyncItemStatus.cpp \
cpp/common/spds/SyncItemKeys.cpp \
cpp/common/spds/SyncManager.cpp \
cpp/common/spds/SyncManagerConfig.cpp \
cpp/common/spds/SyncMap.cpp \
cpp/common/spds/SyncMLBuilder.cpp \
cpp/common/spds/SyncMLProcessor.cpp \
cpp/common/spds/SyncReport.cpp \
cpp/common/spds/SyncSource.cpp \
cpp/common/spds/SyncSourceConfig.cpp \
cpp/common/spds/SyncSourceReport.cpp \
cpp/common/syncml/core/AbstractCommand.cpp \
cpp/common/syncml/core/Add.cpp \
cpp/common/syncml/core/Alert.cpp \
cpp/common/syncml/core/Anchor.cpp \
cpp/common/syncml/core/Atomic.cpp \
cpp/common/syncml/core/Authentication.cpp \
cpp/common/syncml/core/Chal.cpp \
cpp/common/syncml/core/CmdID.cpp \
cpp/common/syncml/core/ComplexData.cpp \
cpp/common/syncml/core/ContentTypeInfo.cpp \
cpp/common/syncml/core/ContentTypeParameter.cpp \
cpp/common/syncml/core/Copy.cpp \
cpp/common/syncml/core/Cred.cpp \
cpp/common/syncml/core/CTCap.cpp \
cpp/common/syncml/core/CTPropParam.cpp \
cpp/common/syncml/core/CTTypeSupported.cpp \
cpp/common/syncml/core/Data.cpp \
cpp/common/syncml/core/DataStore.cpp \
cpp/common/syncml/core/Delete.cpp \
cpp/common/syncml/core/DevInf.cpp \
cpp/common/syncml/core/DevInfData.cpp \
cpp/common/syncml/core/DevInfItem.cpp \
cpp/common/syncml/core/DSMem.cpp \
cpp/common/syncml/core/EMI.cpp \
cpp/common/syncml/core/Exec.cpp \
cpp/common/syncml/core/Ext.cpp \
cpp/common/syncml/core/Filter.cpp \
cpp/common/syncml/core/Item.cpp \
cpp/common/syncml/core/ItemizedCommand.cpp \
cpp/common/syncml/core/Map.cpp \
cpp/common/syncml/core/MapItem.cpp \
cpp/common/syncml/core/Mem.cpp \
cpp/common/syncml/core/Meta.cpp \
cpp/common/syncml/core/MetInf.cpp \
cpp/common/syncml/core/ModificationCommand.cpp \
cpp/common/syncml/core/NextNonce.cpp \
cpp/common/syncml/core/ObjectDel.cpp \
cpp/common/syncml/core/Property.cpp \
cpp/common/syncml/core/PropParam.cpp \
cpp/common/syncml/core/Put.cpp \
cpp/common/syncml/core/Replace.cpp \
cpp/common/syncml/core/ResponseCommand.cpp \
cpp/common/syncml/core/Results.cpp \
cpp/common/syncml/core/Search.cpp \
cpp/common/syncml/core/Sequence.cpp \
cpp/common/syncml/core/SessionID.cpp \
cpp/common/syncml/core/Source.cpp \
cpp/common/syncml/core/SourceArray.cpp \
cpp/common/syncml/core/SourceRef.cpp \
cpp/common/syncml/core/Status.cpp \
cpp/common/syncml/core/StringElement.cpp \
cpp/common/syncml/core/Sync.cpp \
cpp/common/syncml/core/SyncAlert.cpp \
cpp/common/syncml/core/SyncBody.cpp \
cpp/common/syncml/core/SyncCap.cpp \
cpp/common/syncml/core/SyncHdr.cpp \
cpp/common/syncml/core/SyncML.cpp \
cpp/common/syncml/core/SyncNotification.cpp \
cpp/common/syncml/core/SyncType.cpp \
cpp/common/syncml/core/SyncTypeArray.cpp \
cpp/common/syncml/core/Target.cpp \
cpp/common/syncml/core/TargetRef.cpp \
cpp/common/syncml/core/VerDTD.cpp \
cpp/common/syncml/core/VerProto.cpp \
cpp/common/syncml/formatter/Formatter.cpp \
cpp/common/syncml/parser/Parser.cpp \
cpp/common/vocl/VConverter.cpp \
cpp/common/vocl/VObject.cpp \
cpp/common/vocl/VObjectFactory.cpp \
cpp/common/vocl/VProperty.cpp \
cpp/common/vocl/iCalendar/Calendar.cpp \
cpp/common/vocl/iCalendar/Event.cpp \
cpp/common/vocl/iCalendar/iCalConverter.cpp \
cpp/common/vocl/iCalendar/iCalProperty.cpp \
cpp/common/vocl/iCalendar/ToDo.cpp \
cpp/common/vocl/vCard/Address.cpp \
cpp/common/vocl/vCard/BusinessDetail.cpp \
cpp/common/vocl/vCard/Contact.cpp \
cpp/common/vocl/vCard/Contact30.cpp \
cpp/common/vocl/vCard/ContactDetail.cpp \
cpp/common/vocl/vCard/Email.cpp \
cpp/common/vocl/vCard/Name.cpp \
cpp/common/vocl/vCard/Note.cpp \
cpp/common/vocl/vCard/PersonalDetail.cpp \
cpp/common/vocl/vCard/Phone.cpp \
cpp/common/vocl/vCard/Title.cpp \
cpp/common/vocl/vCard/TypedProperty.cpp \
cpp/common/vocl/vCard/vCardConverter.cpp \
cpp/common/vocl/vCard/vCardProperty.cpp \
cpp/common/vocl/vCard/WebPage.cpp \
cpp/common/mail/MailAccount.cpp \
cpp/common/mail/MailAccountManager.cpp \
cpp/common/mail/MailData.cpp \
cpp/common/mail/MailMessage.cpp \
cpp/common/mail/MailSyncSourceConfig.cpp \
cpp/posix/base/autotoolsadapter.cpp \
cpp/posix/base/Log.cpp \
cpp/posix/base/adapter/PlatformAdapter.cpp \
cpp/posix/http/MacTransportAgent.cpp \
cpp/posix/http/MozillaTransportAgent.cpp \
cpp/posix/http/CurlTransportAgent.cpp \
cpp/posix/http/TransportAgentFactory.cpp \
cpp/posix/push/FSocket.cpp \
cpp/posix/push/FThread.cpp \
cpp/posix/spdm/DeviceManagementNode.cpp \
cpp/posix/spdm/DMTreeFactory.cpp \
cpp/posix/spds/DESDecoder.cpp \
cpp/posix/spds/DESEncoder.cpp \

LOCAL_MODULE    := funambol

include $(BUILD_STATIC_LIBRARY)

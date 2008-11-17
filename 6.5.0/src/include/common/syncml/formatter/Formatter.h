/*
 * Copyright (C) 2003-2007 Funambol, Inc
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY, TITLE, NONINFRINGEMENT or FITNESS FOR A PARTICULAR
 * PURPOSE.  See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA
 * 02111-1307  USA
 */


#ifndef INCL_FORMATTER
#define INCL_FORMATTER
/** @cond DEV */

#include "base/fscapi.h"
#include "base/util/utils.h"
#include "base/util/StringBuffer.h"
#include "base/util/XMLProcessor.h"
#include "base/util/ArrayList.h"

#include "syncml/core/TagNames.h"
#include "syncml/core/ObjectDel.h"



class Formatter {

    // ---------------------------------------------------------- Public data
    public:

        static StringBuffer*    getValue            (const char*  tagName, const char*  value, const char *params = NULL);
        static StringBuffer*    getValue            (const char*  tagName, StringBuffer* value, const char *params = NULL);
        static StringBuffer*    getValue            (const char*  tagName, long value, const char *params = NULL);
        static StringBuffer*    getValue            (const char*  tagName, BOOL value, const char *params = NULL);
        static StringBuffer*    getValueNotEmpty    (const char*  tagName, const char*  value, const char *params = NULL);

        static StringBuffer*    getSyncML           (SyncML*        syncML);
        static StringBuffer*    getSyncHdr          (SyncHdr*       syncHdr);
        static StringBuffer*    getSyncBody         (SyncBody*      syncBody);
        static StringBuffer*    getSessionID        (SessionID*     sessionID);
        static StringBuffer*    getVerDTD           (VerDTD*        verDTD);
        static StringBuffer*    getVerProto         (VerProto*      verProto);
        static StringBuffer*    getTarget           (Target*        target);
        static StringBuffer*    getSource           (Source*        source);
        static StringBuffer*    getCred             (Cred*          cred);
        static StringBuffer*    getMeta             (Meta*          meta);
        static StringBuffer*    getMsgID            (const char* value);
        static StringBuffer*    getRespURI          (const char* value);
        static StringBuffer*    getAuthentication   (Authentication* auth);
        static StringBuffer*    getAnchor           (Anchor*        anchor);
        static StringBuffer*    getMetInf           (MetInf*        metInf);
        static StringBuffer*    getNextNonce        (NextNonce*     nextNonce);
        static StringBuffer*    getMem              (Mem*           mem);
        static StringBuffer*    getCmdID            (CmdID*         cmdID);

        static StringBuffer*    getAlerts           (ArrayList*     alerts);
        static StringBuffer*    getAlert            (Alert*         alert);
        static StringBuffer*    getItems            (ArrayList*     items);
        static StringBuffer*    getItem             (Item*          item);

        static StringBuffer*    getDevInf           (DevInf* devInf);
        static StringBuffer*    getData             (ComplexData* data);
        static StringBuffer*    getData             (Data* data);
        static StringBuffer*    getDataStores       (ArrayList* dataStores);
        static StringBuffer*    getDataStore        (DataStore* dataStore);
        static StringBuffer*    getSourceRef        (SourceRef* sourceRef);
        static StringBuffer*    getTargetRef        (TargetRef* targetRef);
        static StringBuffer*    getSourceRefs       (ArrayList* sourceRefs);
        static StringBuffer*    getTargetRefs       (ArrayList* targetRefs);
        static StringBuffer*    getDSMem            (DSMem* dsMem);
        static StringBuffer*    getContentTypeInfos (ArrayList* contentTypeInfos, const char* TAG);
        static StringBuffer*    getContentTypeInfo  (ContentTypeInfo* contentTypeInfo, const char* TAG);
        static StringBuffer*    getSyncCap          (SyncCap* syncCap);
        static StringBuffer*    getSyncTypes        (ArrayList* syncTypes);
        static StringBuffer*    getSyncType         (SyncType* syncType);
        static StringBuffer*    getCTCaps           (ArrayList* ctCaps);
        static StringBuffer*    getCTCap            (CTCap* ctCap);

        static StringBuffer*    getExts             (ArrayList* exts);
        static StringBuffer*    getExt              (Ext* ext);
        static StringBuffer*    getXVals            (ArrayList* xvals);
        static StringBuffer*    getXVal             (StringElement* xval);
        static StringBuffer*    getStatusArray      (ArrayList* statusArray);
        static StringBuffer*    getStatus           (Status* status);
        static StringBuffer*    getChal             (Chal* chal);
        static StringBuffer*    getAdds             (ArrayList* adds);
        static StringBuffer*    getAdd              (Add* add);
        static StringBuffer*    getDels             (ArrayList* adds);
        static StringBuffer*    getDelete           (Delete* add);
        static StringBuffer*    getReplaces         (ArrayList* replaces);
        static StringBuffer*    getReplace          (Replace* replace);
        static StringBuffer*    getCopies           (ArrayList* copies);
        static StringBuffer*    getCopy             (Copy* copy);
        static StringBuffer*    getCommonCommandList(ArrayList* commands);
        static StringBuffer*    getSync             (Sync* sync);
        static StringBuffer*    getMapItem          (MapItem* mapItem);
        static StringBuffer*    getMapItems         (ArrayList* mapItems);
        static StringBuffer*    getMap              (Map* map);
        static StringBuffer*    getExec             (Exec* exec);
        static StringBuffer*    getGet              (Get* get);
        static StringBuffer*    getPut              (Put* put);
        static StringBuffer*    getResults          (Results* results);
        static StringBuffer*    getSearch           (Search* search);
        static StringBuffer*    getSources          (ArrayList* sources);
        static StringBuffer*    getSourceArray      (SourceArray* sourceArray);
        static StringBuffer*    getExtraCommandList (ArrayList* commands);
        static StringBuffer*    getSequence         (Sequence* sequence);
        static StringBuffer*    getSpecificCommand  (ArrayList* commands, const char* commandName);
        static StringBuffer*    getAtomic           (Atomic* atomic);
        static StringBuffer*    getFilter           (Filter* filter);
        static StringBuffer*    getProperty         (Property* property);
        static StringBuffer*    getPropParam        (PropParam* param);

};

/** @endcond */
#endif






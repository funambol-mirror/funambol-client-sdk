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

#include "syncml/core/Mem.h"
#include "syncml/parser/Parser.h"
#include "base/globalsdef.h"

BEGIN_NAMESPACE

SyncML* Parser::getSyncML(const char*xml) {
    SyncBody* syncBody = NULL;
    SyncHdr*  syncHdr  = NULL;
    SyncML*   syncML   = NULL;
    unsigned int pos = 0;
    StringBuffer t;
    XMLProcessor::copyElementContent(t, xml, SYNC_HDR, &pos);
    syncHdr  = getSyncHdr (t.c_str());
    XMLProcessor::copyElementContent(t, xml, SYNC_BODY, &pos);
    syncBody = getSyncBody(t.c_str());

    syncML = new SyncML(syncHdr, syncBody);

    deleteSyncHdr (&syncHdr);
    deleteSyncBody(&syncBody);

    return syncML;

}

SyncHdr* Parser::getSyncHdr(const char*xml) {

    SessionID*   sessionID = NULL;
    VerDTD*      verDTD    = NULL;
    VerProto*    verProto  = NULL;
    Source*      source    = NULL;
    Target*      target    = NULL;
    Cred*        cred      = NULL;
    StringBuffer respURI;
    StringBuffer msgID;
    bool         noResp    = false;
    Meta*        meta      = NULL;
    SyncHdr*     ret       = NULL;

    sessionID = getSessionID(xml);
    verDTD = getVerDTD(xml);
    verProto = getVerProto(xml);
    source = getSource(xml);
    target = getTarget(xml);
    cred = getCred(xml);

    XMLProcessor::copyElementContent(msgID, xml, MSG_ID, NULL);
    XMLProcessor::copyElementContent(respURI, xml, RESP_URI, NULL);
    meta = getMeta(xml);

    StringBuffer t;
    XMLProcessor::copyElementContent(t, xml, NO_RESP, NULL);
    if (!t.empty()) {
        wcscmpIgnoreCase(t.c_str(), "TRUE") ? noResp = true : noResp = false;
    }

    ret = new SyncHdr(verDTD, verProto, sessionID, msgID.c_str(), target,
                      source, respURI.c_str(), noResp, cred, meta);

    deleteVerDTD(&verDTD);
    deleteVerProto(&verProto);
    deleteSessionID(&sessionID);
    deleteSource(&source   );
    deleteTarget(&target);
    deleteCred(&cred);
    deleteMeta(&meta);

    return ret;
}

Cred* Parser::getCred(const char*xml, unsigned int* pos) {

    StringBuffer t;
    XMLProcessor::copyElementContent (t, xml, CRED, pos);
    Cred* ret              = NULL;
    Authentication* auth   = NULL;

    auth = getAuthentication(t.c_str());
    if (auth) {
        ret = new Cred(auth);
    }

    deleteAuthentication(&auth);

    return ret;
}

Authentication* Parser::getAuthentication(const char*xml) {
    Authentication* ret        = NULL;

    StringBuffer data;
    StringBuffer t;
    Meta*  meta       = NULL;

    XMLProcessor::copyElementContent (data, xml, DATA , NULL);

    meta = getMeta(xml);
    if (data || meta) {
        ret = new Authentication(meta, data);
    }
    deleteMeta(&meta);

    return ret;
}

Meta* Parser::getMeta(const char*xml, unsigned int* pos) {

    StringBuffer t;
    XMLProcessor::copyElementContentLevel (t, xml,  META, pos);
    return getMetaFromContent(t.c_str());
}

Meta* Parser::getMetaFromContent(const char* content) {

    Meta* ret        = NULL;
    MetInf* metInf   = NULL;

    metInf = getMetInf(content);
    if (metInf) {
        ret = new Meta();
        ret->setMetInf(metInf);
    }

    deleteMetInf(&metInf);

    return ret;
}


MetInf* Parser::getMetInf(const char*xml) {
    MetInf* ret             = NULL;

    Anchor*      anchor     = NULL;
    NextNonce*   nextNonce  = NULL;
    long         maxMsgSize = 0;
    long         maxObjSize = 0;
    long         size       = 0;
    ArrayList*   emi        = NULL;
    Mem*         mem        = NULL;

    // get all the values
    StringBuffer format;
    StringBuffer type;
    StringBuffer mark;
    StringBuffer version;
    StringBuffer maxMsgSizeW;
    StringBuffer maxObjSizeW;
    StringBuffer sizeW;

    XMLProcessor::copyElementContent (format, xml, FORMAT   , NULL);
    XMLProcessor::copyElementContent (type, xml, TYPE     , NULL);
    XMLProcessor::copyElementContent (mark, xml, MARK     , NULL);

    anchor       = getAnchor(xml);
    XMLProcessor::copyElementContent (version, xml, VERSIONSTR       , NULL);
    nextNonce    = getNextNonce(xml);

    XMLProcessor::copyElementContent (maxMsgSizeW, xml, MAX_MESSAGE_SIZE     , NULL);
    XMLProcessor::copyElementContent (maxObjSizeW, xml, MAX_OBJ_SIZE     , NULL);
    XMLProcessor::copyElementContent (sizeW, xml, SIZE             , NULL);

    if (!maxMsgSizeW.empty()) {
        maxMsgSize = strtol(maxMsgSizeW.c_str(), NULL, 10);
    }
    if (!maxObjSizeW.empty()) {
        maxObjSize = strtol(maxObjSizeW.c_str(), NULL, 10);
    }
    if (!sizeW.empty()) {
        size = strtol(sizeW.c_str(), NULL, 10);
    }

    emi          = getEMI(xml);
    mem          = getMem(xml);

    // check if someting is null, 0 or zero lenght
    bool isToCreate = false;
    bool notNull = NotNullCheck(7, format.c_str(), type.c_str(), mark.c_str(),
                                   version.c_str(), maxMsgSizeW.c_str(),
                                   maxObjSizeW.c_str(), sizeW.c_str());
 
    isToCreate = notNull
                 || NotZeroArrayLength(1, emi)
                 || (mem)
                 || (anchor)
                 || (nextNonce);

    if (isToCreate) {
        ret = new MetInf(format.c_str(), type.c_str(), mark.c_str(), size,
                         anchor, version.c_str(), nextNonce, maxMsgSize,
                         maxObjSize, emi, mem);
    }
    deleteAnchor(&anchor);
    deleteNextNonce(&nextNonce);
    delete emi;
    deleteMem(&mem);

    return ret;
}


void Parser::getSources(ArrayList& list, const char*xml) {

    Source* source = NULL;
    SourceArray* sourceArray = NULL;
    unsigned int pos = 0, previous = 0;
    StringBuffer t;

    XMLProcessor::copyElementContent(t, &xml[pos], SOURCE, &pos);
    while ((source = getSourceFromContent(t.c_str())) != NULL) {
        if (source) {
            sourceArray = new SourceArray(source);
            list.add(*sourceArray); // in the ArrayList NULL element cannot be inserted
            deleteSource(&source);
            deleteSourceArray(&sourceArray);
        }
        pos += previous;
        previous = pos;
        XMLProcessor::copyElementContent(t, &xml[pos], SOURCE, &pos);
    }
}


Source* Parser::getSource(const char* xml, unsigned int* pos) {
    StringBuffer t;
    XMLProcessor::copyElementContent (t, xml, SOURCE, pos);
    return getSourceFromContent(t.c_str());
}

Source* Parser::getSourceFromContent(const char* xml) {
    Source* ret   = NULL;
    StringBuffer locURI, locName;
    XMLProcessor::copyElementContent (locURI, xml, LOC_URI, NULL);
    XMLProcessor::copyElementContent (locName, xml, LOC_NAME, NULL);

    if (NotNullCheck(2, locURI.c_str(), locName.c_str())) {
        ret = new Source(locURI.c_str(), locName.c_str());
    }

    return ret;
}


Target* Parser::getTarget(const char*xml, unsigned int* pos) {
    StringBuffer t;
    XMLProcessor::copyElementContent (t, xml, TARGET, NULL);
    return getTargetFromContent(t.c_str());
}

Target* Parser::getTargetFromContent(const char* xml) {
    Target*  ret   = NULL;
    StringBuffer locURI, locName;
    XMLProcessor::copyElementContent (locURI, xml, LOC_URI, NULL);
    XMLProcessor::copyElementContent (locName, xml, LOC_NAME, NULL);

    if (NotNullCheck(2, locURI.c_str(), locName.c_str())) {
        ret = new Target(locURI.c_str(), locName.c_str());
    }

    return ret;
}

Anchor* Parser::getAnchor(const char*xml) {
    Anchor* ret  = NULL;
    StringBuffer last, next;
    XMLProcessor::copyElementContent (last, xml, LAST, NULL);
    XMLProcessor::copyElementContent (next, xml, NEXT, NULL);

    if (NotNullCheck(2, last.c_str(), next.c_str())) {
        ret = new Anchor(last.c_str(), next.c_str());
    }
    return ret;
}

NextNonce* Parser::getNextNonce(const char*xml) {
    NextNonce* ret   = NULL;
    StringBuffer value;
    XMLProcessor::copyElementContent (value, xml, NEXT_NONCE, NULL);

    if (NotNullCheck(1, value.c_str())) {
        ret = new NextNonce(value.c_str());
    }

    return ret;
}

Mem* Parser::getMem(const char*xml) {
    Mem*    ret         = NULL;
    bool    sharedMem   = false;
    long    freeMem     = 0;
    long    freeID      = 0;
    bool    isToCreate  = false;

    StringBuffer freeMemW;
    StringBuffer sharedMemW;
    StringBuffer freeIDW;
    XMLProcessor::copyElementContent (freeMemW, xml, FREE_MEM,   NULL);
    XMLProcessor::copyElementContent (sharedMemW, xml, SHARED_MEM, NULL);
    XMLProcessor::copyElementContent (freeIDW, xml, FREE_ID,    NULL);

    isToCreate = NotNullCheck(3, freeMemW.c_str(), sharedMemW.c_str(),
                                 freeIDW.c_str());

    if (!freeMemW.empty()) {
        freeMem = strtol(freeMemW.c_str(), NULL, 10);
    }
    if (!freeIDW.empty()) {
        freeID = strtol(freeIDW.c_str(), NULL, 10);
    }
    if (!sharedMemW.empty()) {
        sharedMem = sharedMemW != "0" ? true : false;
    }

    if (isToCreate) {
        ret = new Mem(sharedMem, freeMem, freeID);
    }
    return ret;
}


SessionID* Parser::getSessionID(const char*xml, unsigned int* pos) {

    StringBuffer t;
    XMLProcessor::copyElementContent(t, xml, SESSION_ID, pos);
    SessionID* ret = NULL;
    if (t.c_str()) {
        ret = new SessionID(t.c_str());
    }
    return ret;
}

VerDTD* Parser::getVerDTD(const char*xml, unsigned int* pos) {
    StringBuffer t;
    XMLProcessor::copyElementContent (t, xml, VER_DTD, pos);
    VerDTD* ret = NULL;
    if (t.c_str()) {
        ret = new VerDTD(t.c_str());
    }
    return ret;
}

VerProto* Parser::getVerProto(const char* xml, unsigned int* pos) {

    StringBuffer t;
    XMLProcessor::copyElementContent (t, xml, VER_PROTO, pos);
    VerProto* ret = NULL;
    if (t.c_str()) {
        ret = new VerProto(t.c_str());
    }
    return ret;
}

SyncBody* Parser::getSyncBody(const char*xml) {

    SyncBody* syncBody   = NULL;
    bool finalMsg        = false;
    ArrayList commands;
    getCommands(commands, xml);
    finalMsg = getFinalMsg(xml);
    syncBody = new SyncBody(&commands, finalMsg);
    return syncBody;
}

/*
* The sequence tag can contains the common commands (Add, Replace, Delete, Copy) and
* Alert
* Exec
* Get
* Map
*
* Atomic
* Sync
*/
Sequence* Parser::getSequence(const char*xml) {

    Sequence* ret           = NULL;

    Meta*   meta            = NULL;
    bool    noResp          = false;
    CmdID*  cmdID           = NULL;
    Sync* sync              = NULL;
    Atomic* atomic          = NULL;

    Alert* alert            = NULL;
    Map*   map              = NULL;
    Get*   get              = NULL;
    Exec* exec              = NULL;

    unsigned int pos = 0, previous = 0;

    StringBuffer t;

    cmdID = getCmdID(xml);
    meta = getMeta(xml);
    noResp   = getNoResp(xml);
    // list of commands that must not be leaf of Sync and Atomic
    ArrayList commands;
    getCommonCommandList(commands, xml, "Atomic&Sync");

    // Alert
    pos = 0, previous = 0;
    XMLProcessor::copyElementContentLevel(t, &xml[pos], ALERT, &pos);
    while ((alert = getAlert(t.c_str())) != NULL) {
        commands.add(*alert); // in the ArrayList NULL element cannot be inserted
        deleteAlert(&alert);
        pos += previous;
        previous = pos;
        XMLProcessor::copyElementContentLevel(t, &xml[pos], ALERT, &pos);
    }

    // Map
    pos = 0, previous = 0;
    XMLProcessor::copyElementContentLevel(t, &xml[pos], MAP, &pos);
    while ((map = getMap(t.c_str())) != NULL) {
        commands.add(*map); // in the ArrayList NULL element cannot be inserted
        deleteMap(&map);
        pos += previous;
        previous = pos;
        XMLProcessor::copyElementContentLevel(t, &xml[pos], MAP, &pos);
    }

    // Get
    pos = 0, previous = 0;
    XMLProcessor::copyElementContentLevel(t, &xml[pos], GET, &pos);
    while ((get = getGet(t.c_str())) != NULL) {
        commands.add(*get); // in the ArrayList NULL element cannot be inserted
        deleteGet(&get);
        pos += previous;
        previous = pos;
        XMLProcessor::copyElementContentLevel(t, &xml[pos], GET, &pos);
    }

    // Exec
    pos = 0, previous = 0;
    XMLProcessor::copyElementContentLevel(t, &xml[pos], EXEC, &pos);
    while ((exec = getExec(t.c_str())) != NULL) {
        commands.add(*exec); // in the ArrayList NULL element cannot be inserted
        deleteExec(&exec);
        pos += previous;
        previous = pos;
        XMLProcessor::copyElementContentLevel(t, &xml[pos], EXEC, &pos);
    }

    StringBuffer element;
    XMLProcessor::copyElementContentLevel (element, xml,  SYNC, NULL);

    if (!element.empty()) {
        sync = getSync(element.c_str());
        if (sync) {
            commands.add(*sync);
            deleteSync(&sync);
        }
    }

    XMLProcessor::copyElementContentLevel (element, xml,  ATOMIC, NULL);

    if (!element.empty()) {
        atomic = getAtomic(element.c_str());
        if (atomic) {
            commands.add(*atomic);
            deleteAtomic(&atomic);
        }
    }


    if ((cmdID)   ||
        (meta)    ||
        NotZeroArrayLength(1, &commands)) {

        ret = new Sequence(cmdID, noResp, meta, &commands);
    }

    deleteMeta(&meta);
    deleteCmdID(&cmdID);

    return ret;
}

/*
* The Atomic tag can contains the common commands (Add, Replace, Delete, Copy) and
* Alert
* Exec
* Get
* Map
*
* Atomic
* Sync
* Sequence
*/
Atomic* Parser::getAtomic(const char*xml) {

    Atomic* ret             = NULL;

    Meta*   meta            = NULL;
    bool    noResp          = false;
    CmdID*  cmdID           = NULL;
    Sync* sync              = NULL;
    Sequence* sequence      = NULL;

    Alert* alert            = NULL;
    Map*   map              = NULL;
    Get*   get              = NULL;
    Exec* exec              = NULL;

    unsigned int pos = 0, previous = 0;
    StringBuffer t;
    cmdID    = getCmdID(xml);
    meta     = getMeta(xml);
    noResp   = getNoResp(xml);
    // list of commands that must not be leaf of Sync and Atomic
    ArrayList commands;
    getCommonCommandList(commands, xml, "Sync&Sequence");

    // Alert
    pos = 0, previous = 0;
    XMLProcessor::copyElementContentLevel(t, &xml[pos], ALERT, &pos);
    while ((alert = getAlert(t.c_str())) != NULL) {
        commands.add(*alert); // in the ArrayList NULL element cannot be inserted
        deleteAlert(&alert);
        pos += previous;
        previous = pos;
        XMLProcessor::copyElementContentLevel(t, &xml[pos], ALERT, &pos);
    }

    // Map
    pos = 0, previous = 0;
    XMLProcessor::copyElementContentLevel(t, &xml[pos], MAP, &pos);
    while ((map = getMap(t.c_str())) != NULL) {
        commands.add(*map); // in the ArrayList NULL element cannot be inserted
        deleteMap(&map);
        pos += previous;
        previous = pos;
        XMLProcessor::copyElementContentLevel(t, &xml[pos], MAP, &pos);
    }

    // Get
    pos = 0, previous = 0;
    XMLProcessor::copyElementContentLevel(t, &xml[pos], GET, &pos);
    while ((get = getGet(t.c_str())) != NULL) {
        commands.add(*get); // in the ArrayList NULL element cannot be inserted
        deleteGet(&get);
        pos += previous;
        previous = pos;
        XMLProcessor::copyElementContentLevel(t, &xml[pos], GET, &pos);
    }

    // Exec
    pos = 0, previous = 0;
    XMLProcessor::copyElementContentLevel(t, &xml[pos], EXEC, &pos);
    while ((exec = getExec(t.c_str())) != NULL) {
        commands.add(*exec); // in the ArrayList NULL element cannot be inserted
        deleteExec(&exec);
        pos += previous;
        previous = pos;
        XMLProcessor::copyElementContentLevel(t, &xml[pos], EXEC, &pos);
    }

    StringBuffer element;
    XMLProcessor::copyElementContentLevel (element, xml,  SYNC , NULL);

    if (!element.empty()) {
        sync = getSync(element.c_str());
        if (sync) {
            commands.add(*sync);
            deleteSync(&sync);
        }
    }

    XMLProcessor::copyElementContentLevel (element, xml,  SEQUENCE, NULL);

    if (!element.empty()) {
        sequence = getSequence(element.c_str());
        if (sequence) {
            commands.add(*sequence);
            deleteSequence(&sequence);
        }
    }

    //
    // TBD: Atomic
    //


    if ((cmdID)   ||
        (meta)    ||
        NotZeroArrayLength(1, &commands)) {

        ret = new Atomic(cmdID, noResp, meta, &commands);
    }

    deleteMeta(&meta);
    deleteCmdID(&cmdID);

    return ret;
}

/*
* Contains the commands that the <sync> tag can have.
*    Add
*    Replace
*    Delete
*    Copy
*    Atomic
*    Map
*    Sync
*/

Sync* Parser::getSync(const char*xml) {

    Sync* ret               = NULL;
    Sequence* sequence      = NULL;
    Atomic* atomic          = NULL;
    Cred*   cred            = NULL;
    Meta*   meta            = NULL;
    bool    noResp          = false;
    CmdID*  cmdID           = NULL;
    Target* target          = NULL;
    Source* source          = NULL;
    long numberOfChanges    = -1;

    StringBuffer t;
    cmdID    = getCmdID      (xml);
    target   = getTarget     (xml);
    source   = getSource     (xml);
    meta     = getMeta       (xml);
    StringBuffer numberOfChangesW;
    XMLProcessor::copyElementContent (numberOfChangesW, xml,  NUMBER_OF_CHANGES ,NULL);
    if (!numberOfChangesW.empty()) {
        numberOfChanges = strtol(numberOfChangesW.c_str(), NULL, 10);
    }

    cred     = getCred      (xml);
    noResp   = getNoResp    (xml);
    ArrayList commands;
    getCommonCommandList(commands, xml, "Atomic&Sequence");

    char* element;
    element = XMLProcessor::copyElementContentExcept(xml,  SEQUENCE, "Atomic", NULL);

    if (element) {
        sequence = getSequence(element);
        if (sequence) {
            commands.add(*sequence);
            deleteSequence(&sequence);
        }
        safeDel(&element);
    }

    element = XMLProcessor::copyElementContentExcept(xml,  ATOMIC, "Atomic&Sequence", NULL);

    if (element) {
        atomic = getAtomic(element);
        if (atomic) {
            commands.add(*atomic);
            deleteAtomic(&atomic);
        }
        safeDel(&element);
    }

    if ((cmdID)   ||
        (cred)    ||
        (target)  ||
        (source)  ||
        (meta)    ||
        NotZeroArrayLength(1, &commands)) {

        ret = new Sync(cmdID, noResp, cred, target, source, meta,
                       numberOfChanges, &commands);
    }

    deleteCred(&cred);
    deleteMeta(&meta);
    deleteCmdID(&cmdID);
    deleteTarget(&target);
    deleteSource(&source);

    return ret;
}

void Parser::getCommonCommandList(ArrayList& commands, const char*xml, const char*except) {

    //
    //Delete
    //
    getAndAppendDels(commands, xml, except);

    //
    //Add
    //
    getAndAppendAdds(commands, xml, except);

    //
    //Replace
    //
    getAndAppendReplaces(commands, xml, except);

    //
    //Copy
    //
    getAndAppendCopies(commands, xml, except);
}

Copy* Parser::getCopy(const char*xml) {
    Copy* ret = NULL;

    CmdID*      cmdID   = NULL;
    bool        noResp  = false;
    Cred*       cred    = NULL;
    Meta*       meta    = NULL;

    cmdID   = getCmdID     (xml);
    meta    = getMeta      (xml);
    cred    = getCred      (xml);
    noResp  = getNoResp    (xml);

    ArrayList items;
    getItems(items, xml, COPY);

    if ((cmdID) ||
        (cred)  ||
        NotZeroArrayLength(1, &items)
        )  {

        ret = new Copy(cmdID, noResp, cred, meta, &items);
    }

    deleteCmdID(&cmdID);
    deleteMeta(&meta);
    deleteCred(&cred);

    return ret;
}


Add* Parser::getAdd(const char*xml) {
    Add* ret = NULL;

    CmdID*      cmdID   = NULL;
    bool        noResp  = false;
    Cred*       cred    = NULL;
    Meta*       meta    = NULL;

    cmdID   = getCmdID     (xml);
    meta    = getMeta      (xml);
    cred    = getCred      (xml);
    noResp  = getNoResp    (xml);

    ArrayList items;
    getItems(items, xml, ADD);

    if ((cmdID) ||
        (cred)  ||
        NotZeroArrayLength(1, &items)
        )  {

        ret = new Add(cmdID, noResp, cred, meta, &items);
    }

    deleteCmdID(&cmdID);
    deleteMeta(&meta);
    deleteCred(&cred);

    return ret;
}

Delete* Parser::getDelete(const char*xml) {
    Delete* ret = NULL;

    CmdID*      cmdID   = NULL;
    bool        noResp  = false;
    bool        archive = false;
    bool        sftDel  = false;
    Cred*       cred    = NULL;
    Meta*       meta    = NULL;

    cmdID   = getCmdID     (xml);
    meta    = getMeta      (xml);
    cred    = getCred      (xml);
    noResp  = getNoResp    (xml);

    ArrayList  items;
    getItems(items, xml, DEL);

    if ((cmdID) ||
        (cred)  ||
        NotZeroArrayLength(1, &items)
        )  {

        ret = new Delete(cmdID, noResp, archive, sftDel, cred, meta, &items);
    }

    deleteCmdID(&cmdID);
    deleteMeta(&meta);
    deleteCred(&cred);

    return ret;
}

Replace* Parser::getReplace(const char*xml) {
    Replace* ret = NULL;

    CmdID*      cmdID   = NULL;
    bool        noResp  = false;
    Cred*       cred    = NULL;
    Meta*       meta    = NULL;

    cmdID   = getCmdID     (xml);
    meta    = getMeta      (xml);
    cred    = getCred      (xml);
    noResp  = getNoResp    (xml);

    ArrayList  items;
    getItems(items, xml, REPLACE);

    if ((cmdID) ||
        (cred)  ||
        NotZeroArrayLength(1, &items)
        )  {

        ret = new Replace(cmdID, noResp, cred, meta, &items);
    }

    deleteCmdID(&cmdID);
    deleteMeta(&meta);
    deleteCred(&cred);

    return ret;
}

MapItem* Parser::getMapItem(const char*xml) {
    MapItem* ret = NULL;

    Target*    target = NULL;
    Source*    source = NULL;

    StringBuffer t;
    target   = getTarget(xml);
    source   = getSource(xml);

    if ((target)|| (source)) {
        ret = new MapItem(target, source);
    }

    deleteTarget(&target);
    deleteSource(&source);

    return ret;
}

/*
* Returns an ArrayList of mapItem command
*/
void Parser::getMapItems(ArrayList& list, const char*xml) {

    MapItem* mapItem = NULL;
    unsigned int pos = 0, previous = 0;

    StringBuffer t;
    XMLProcessor::copyElementContent(t, &xml[pos], MAP_ITEM, &pos);
    while ((mapItem = getMapItem(t.c_str())) != NULL) {
        list.add(*mapItem); // in the ArrayList NULL element cannot be inserted
        deleteMapItem(&mapItem);
        pos += previous;
        previous = pos;
        XMLProcessor::copyElementContent(t, &xml[pos], MAP_ITEM, &pos);
    }
}

Map* Parser::getMap(const char*xml) {
    Map* ret = NULL;

    CmdID*     cmdID  = NULL;
    Cred*      cred   = NULL;
    Meta*      meta   = NULL;

    Target*    target = NULL;
    Source*    source = NULL;

    cmdID   = getCmdID(xml);
    meta    = getMeta(xml);
    cred    = getCred(xml);
    target  = getTarget(xml);
    source  = getSource(xml);

    ArrayList mapItems;
    getMapItems(mapItems, xml);

    if ((cmdID) ||
        (meta)  ||
        (cred)  ||
        (target)||
        (source)||
        NotZeroArrayLength(1, &mapItems)
        )  {

        ret = new Map(cmdID, target, source, cred, meta, &mapItems);
    }

    deleteCmdID(&cmdID);
    deleteMeta(&meta);
    deleteCred(&cred);
    deleteTarget(&target);
    deleteSource(&source);

    return ret;
}


/*
* Returns an ArrayList of copy command
*/
void Parser::getAndAppendCopies(ArrayList& list, const char*xml, const char*except) {

    Copy* copy = NULL;
    unsigned int pos = 0, previous = 0;

   /*
    * except is set to SYNC if we are looking for Copy commands external from <sync> tag
    */
    char* t = XMLProcessor::copyElementContentExcept(&xml[pos], COPY, except, &pos);
    while ((copy = getCopy(t)) != NULL) {
        list.add(*copy); // in the ArrayList NULL element cannot be inserted
        deleteCopy(&copy);
        pos += previous;
        previous = pos;
        delete [] t;
        t = XMLProcessor::copyElementContentExcept(&xml[pos], COPY, except, &pos);
    }
    delete [] t;
}

/*
* Returns an ArrayList of add command
*/
void Parser::getAndAppendAdds(ArrayList& list, const char*xml, const char*except) {

    Add* add         = NULL;
    unsigned int pos = 0, previous = 0;
    /*
    * except is set to SYNC if we are looking for Add commands external from <sync> tag
    */
    char* t = XMLProcessor::copyElementContentExcept(&xml[pos], ADD, except, &pos);
    while ((add = getAdd(t)) != NULL) {
        list.add(*add); // in the ArrayList NULL element cannot be inserted
        deleteAdd(&add);
        pos += previous;
        previous = pos;
        delete [] t;
        t = XMLProcessor::copyElementContentExcept(&xml[pos], ADD, except, &pos);
    }
    delete [] t;
}

/*
* Returns an ArrayList of Replace commands
*/
void Parser::getAndAppendReplaces(ArrayList& list, const char*xml, const char*except) {

    Replace* replace = NULL;
    unsigned int pos = 0, previous = 0;

    char* t = XMLProcessor::copyElementContentExcept(&xml[pos], REPLACE, except, &pos);
    while ((replace = getReplace(t)) != NULL) {
        list.add(*replace); // in the ArrayList NULL element cannot be inserted
        deleteReplace(&replace);
        pos += previous;
        previous = pos;
        delete [] t;
        t = XMLProcessor::copyElementContentExcept(&xml[pos], REPLACE, except, &pos);
    }
    delete [] t;
}

/*
* Returns an ArrayList of Dels command
*/
void Parser::getAndAppendDels(ArrayList& list, const char*xml, const char*except) {

    Delete* del        = NULL;
    unsigned int pos   = 0, previous = 0;

    char* t = XMLProcessor::copyElementContentExcept(&xml[pos], DEL, except, &pos);
    while ((del = getDelete(t)) != NULL) {
        list.add(*del); // in the ArrayList NULL element cannot be inserted
        deleteDelete(&del);
        pos += previous;
        previous = pos;
        delete [] t;
        t = XMLProcessor::copyElementContentExcept(&xml[pos], DEL, except, &pos);
    }
    delete [] t;
}

/*
Commands of SyncBody tag
    Alert
    Add
    Atomic
    Copy
    Delete
    Exec
    Get
    Map
    Put
    Replace
    Results
    Search
    Sequence
    Status
    Sync
*/
void Parser::getCommands(ArrayList& ret, const char*xml) {
    ArrayList list;
    Alert* alert        = NULL;
    Map*   map          = NULL;
    Get*   get          = NULL;
    Put*   put          = NULL;
    Status* status      = NULL;
    Results* result     = NULL;
    Exec* exec          = NULL;
    Search* search      = NULL;

    Sequence* sequence  = NULL;
    Atomic* atomic      = NULL;
    Sync* sync          = NULL;
    unsigned int pos = 0, previous = 0;

    // Status
    StringBuffer t;
    XMLProcessor::copyElementContent(t, &xml[pos], STATUS, &pos);
    while ((status = getStatus(t.c_str())) != NULL) {
        ret.add(*status); // in the ArrayList NULL element cannot be inserted
        deleteStatus(&status);
        pos += previous;
        previous = pos;
        XMLProcessor::copyElementContent(t, &xml[pos], STATUS, &pos);
    }

    // Alert: use the copyElementContentLevel because Alert could be also in Atomic and Sequence commands
    pos = 0, previous = 0;
    XMLProcessor::copyElementContentLevel(t, &xml[pos], ALERT, &pos);
    while ((alert = getAlert(t.c_str())) != NULL) {
        ret.add(*alert); // in the ArrayList NULL element cannot be inserted
        deleteAlert(&alert);
        pos += previous;
        previous = pos;
        XMLProcessor::copyElementContentLevel(t, &xml[pos], ALERT, &pos);
    }

    // Map: use the copyElementContentLevel because Map could be also in Atomic and Sequence commands
    pos = 0, previous = 0;
    XMLProcessor::copyElementContentLevel(t, &xml[pos], MAP, &pos);
    while ((map = getMap(t.c_str())) != NULL) {
        ret.add(*map); // in the ArrayList NULL element cannot be inserted
        deleteMap(&map);
        pos += previous;
        previous = pos;
        XMLProcessor::copyElementContentLevel(t, &xml[pos], MAP, &pos);
    }

    // Get: use the copyElementContentLevel because Get could be also in Atomic and Sequence commands
    pos = 0, previous = 0;
    XMLProcessor::copyElementContent(t, &xml[pos], GET, &pos);
    while ((get = getGet(t.c_str())) != NULL) {
        ret.add(*get); // in the ArrayList NULL element cannot be inserted
        deleteGet(&get);
        pos += previous;
        previous = pos;
        XMLProcessor::copyElementContent(t, &xml[pos], GET, &pos);
    }

    // Put
    pos = 0, previous = 0;
    XMLProcessor::copyElementContent(t, &xml[pos], PUT, &pos);
    while ((put = getPut(t.c_str())) != NULL) {
        ret.add(*put); // in the ArrayList NULL element cannot be inserted
        deletePut(&put);
        pos += previous;
        previous = pos;
        XMLProcessor::copyElementContent(t, &xml[pos], PUT, &pos);
    }

    // Results
    pos = 0, previous = 0;
    XMLProcessor::copyElementContent(t, &xml[pos], RESULTS, &pos);
    while ((result = getResult(t.c_str())) != NULL) {
        ret.add(*result); // in the ArrayList NULL element cannot be inserted
        deleteResults(&result);
        pos += previous;
        previous = pos;
        XMLProcessor::copyElementContent(t, &xml[pos], RESULTS, &pos);
    }

    // Exec: use the copyElementContentLevel because Exec could be also in Atomic and Sequence commands
    pos = 0, previous = 0;
    XMLProcessor::copyElementContentLevel(t, &xml[pos], EXEC, &pos);
    while ((exec = getExec(t.c_str())) != NULL) {
        ret.add(*exec); // in the ArrayList NULL element cannot be inserted
        deleteExec(&exec);
        pos += previous;
        previous = pos;
        XMLProcessor::copyElementContentLevel(t, &xml[pos], EXEC, &pos);
    }

    // Search
    pos = 0, previous = 0;
    XMLProcessor::copyElementContent(t, &xml[pos], SEARCH, &pos);
    while ((search = getSearch(t.c_str())) != NULL) {
        ret.add(*search); // in the ArrayList NULL element cannot be inserted
        deleteSearch(&search);
        pos += previous;
        previous = pos;
        XMLProcessor::copyElementContent(t, &xml[pos], SEARCH, &pos);
    }

    // get the Sync commands. not belonging to Atomic and Sequence
    //sync = getSync(XMLProcessor::copyElementContentExcept (xml, SYNC, "Atomic&Sequence", NULL));

    //if (sync) {
    //    ret->add(*sync);
    //    deleteSync(&sync);
    //}

    // Sync
    pos = 0, previous = 0;
    char* t1 = XMLProcessor::copyElementContentExcept(&xml[pos], SYNC, "Atomic&Sequence", &pos);
    while ((sync = getSync(t1)) != NULL) {
        ret.add(*sync); // in the ArrayList NULL element cannot be inserted
        deleteSync(&sync);
        pos += previous;
        previous = pos;
        delete [] t1;
        t1 = XMLProcessor::copyElementContentExcept(&xml[pos], SYNC, "Atomic&Sequence", &pos);
    }
    delete [] t1;

    // get the Sequence commands. Not belonging to Atomic and Sync
    t1 = XMLProcessor::copyElementContentExcept(xml, SEQUENCE, "Atomic&Sync", &pos);
    sequence = getSequence(t1);
    delete [] t1;

    if (sequence) {
        ret.add(*sequence);
        deleteSequence(&sequence);
    }

    // get the Sequence commands. Not belonging to Sequence and Sync and Atomic
    t1 = XMLProcessor::copyElementContentExcept(xml, ATOMIC, "Atomic&Sync&Sequence", &pos);
    atomic = getAtomic(t1);
    delete [] t1;

    if (atomic) {
        ret.add(*atomic);
        deleteAtomic(&atomic);
    }


    ArrayList commonCommandList;
    getCommonCommandList(commonCommandList, xml, "Atomic&Sync&Sequence");

    for (int i = 0; i < commonCommandList.size(); i++) {
        ret.add(*commonCommandList.get(i));
    }
}

Status* Parser::getStatus(const char*xml) {

    if (!xml)
        return NULL;

    Status*  ret         = NULL;
    CmdID*   cmdID       = NULL;
    Cred*    cred        = NULL;
    Chal*    chal        = NULL;
    Data*    data        = NULL;

    cmdID = getCmdID(xml);

    StringBuffer msgRef, cmdRef, cmd;
    XMLProcessor::copyElementContent (msgRef, xml, MSG_REF, NULL);
    XMLProcessor::copyElementContent (cmdRef, xml, CMD_REF, NULL);
    XMLProcessor::copyElementContent (cmd, xml, CMD,     NULL);
    cred = getCred(xml);
    // get Data <Data>200</Data>
    data = getData(xml);

    ArrayList items;
    getItems(items, xml);

    ArrayList targetRefs;
    getTargetRefs(targetRefs, xml);

    ArrayList sourceRefs;
    getSourceRefs(sourceRefs, xml);

    chal = getChal(xml);

    if (NotNullCheck(2, msgRef.c_str(), cmdRef.c_str()) || (cred)
                                        || (data)
                                        || (cmdID)
                                        || (chal)
                                        || NotZeroArrayLength(3, &items, &targetRefs, &sourceRefs)
                                        )  {

        ret = new Status(cmdID, msgRef.c_str(), cmdRef.c_str(), cmd.c_str(),
                         &targetRefs, &sourceRefs, cred, chal, data, &items);
    }
    deleteCmdID(&cmdID);
    deleteCred(&cred);
    deleteData(&data);
    deleteChal(&chal);

    return ret;
}

Chal* Parser::getChal(const char* xml, unsigned int* pos) {

    StringBuffer t;
    XMLProcessor::copyElementContent (t, xml, CHAL, pos);

    Chal* ret  = NULL;
    Meta* meta = getMetaFromContent(t.c_str());

    if (meta) {
        ret = new Chal(meta);
        deleteMeta(&meta);
    }

    return ret;
}

void Parser::getTargetRefs(ArrayList& list, const char*xml) {
    TargetRef* targetRef = NULL;
    unsigned int pos = 0, previous = 0;

    StringBuffer t;
    XMLProcessor::copyElementContent(t, &xml[pos], TARGET_REF, &pos);
    while ((targetRef = getTargetRef(t.c_str())) != NULL) {
        list.add(*targetRef); // in the ArrayList NULL element cannot be inserted
        deleteTargetRef(&targetRef);
        pos += previous;
        previous = pos;
        XMLProcessor::copyElementContent(t, &xml[pos], TARGET_REF, &pos);
    }
}

void Parser::getSourceRefs(ArrayList& list, const char*xml) {
    SourceRef* sourceRef = NULL;
    unsigned int pos = 0, previous = 0;

    StringBuffer t;
    XMLProcessor::copyElementContent(t, &xml[pos], SOURCE_REF, &pos);
    while ((sourceRef = getSourceRef(t.c_str())) != NULL) {
        list.add(*sourceRef); // in the ArrayList NULL element cannot be inserted
        deleteSourceRef(&sourceRef);
        pos += previous;
        previous = pos;
        XMLProcessor::copyElementContent(t, &xml[pos], SOURCE_REF, &pos);
    }
}

SourceRef* Parser::getSourceRef(const char*xml) {
    SourceRef* ret = NULL;
    Source* source;

    source = getSourceFromContent(xml);
    if (source) {
        ret = new SourceRef(source);
    } else if (xml) {
        ret = new SourceRef(xml);
    }

    return ret;
}

TargetRef* Parser::getTargetRef(const char*xml) {
    TargetRef* ret = NULL;
    Target* target;

    target = getTargetFromContent(xml);
    if (target) {
        ret = new TargetRef(target);
    } else if (xml) {
        ret = new TargetRef(xml);
    }

    return ret;
}

Alert* Parser::getAlert(const char*xml) {

    Alert* ret = NULL;
  
    StringBuffer t;
    CmdID* cmdID     = getCmdID   (xml);
    Cred*  cred      = getCred    (xml);
    XMLProcessor::copyElementContent (t, xml, DATA   , NULL);
    int    data      = getDataCode(t.c_str());
    bool   noResp    = getNoResp  (xml);

    ArrayList items;
    getItems(items, xml);
    if (items.size() > 0) {
        ret = new Alert(cmdID, noResp, cred, data, &items); //Item[]
    }

    deleteCmdID(&cmdID);
    deleteCred(&cred);

    return ret;
}

Exec* Parser::getExec(const char*xml) {

    Exec* ret = NULL;

    CmdID* cmdID        = NULL;
    bool   noResp       = false;
    Cred*  cred         = NULL;

    cmdID     = getCmdID   (xml);
    cred      = getCred    (xml);
    noResp    = getNoResp  (xml);
    ArrayList items;
    getItems(items, xml);

    if (cmdID || NotZeroArrayLength(1, &items) || (cred)) {
        ret = new Exec(cmdID, noResp, cred, &items);
    }

    deleteCmdID(&cmdID);
    deleteCred(&cred);

    return ret;
}

Get* Parser::getGet(const char*xml) {

    Get* ret = NULL;

    CmdID* cmdID     = getCmdID   (xml);
    Cred*  cred      = getCred    (xml);
    bool   noResp    = getNoResp  (xml);
    Meta*  meta      = getMeta    (xml);
    StringBuffer lang;
    XMLProcessor::copyElementContent(lang, xml, LANG, NULL);
    ArrayList items;
    getItems(items, xml);

    if (NotNullCheck(1, lang)  || (cred)
                               || (cmdID)
                               || (meta)
                               || NotZeroArrayLength(1, &items))  {

        ret = new Get(cmdID, noResp, lang.c_str(), cred, meta, &items); //Item[]
    }

    deleteCmdID(&cmdID);
    deleteCred(&cred);
    deleteMeta(&meta);

    return ret;
}

Put* Parser::getPut(const char*xml) {

    Put* ret = NULL;

    CmdID* cmdID     = getCmdID   (xml);
    Cred*  cred      = getCred    (xml);
    bool   noResp    = getNoResp  (xml);
    Meta*  meta      = getMeta    (xml);
    StringBuffer lang;
    XMLProcessor::copyElementContent(lang, xml, LANG, NULL);
    ArrayList items;
    getItems(items, xml);

    if (NotNullCheck(1, lang)  || (cred)
                               || (cmdID)
                               || (meta)
                               || NotZeroArrayLength(1, &items))  {

        ret = new Put(cmdID, noResp, lang.c_str(), cred, meta, &items); //Item[]
    }

    deleteCmdID(&cmdID);
    deleteCred(&cred);
    deleteMeta(&meta);

    return ret;
}

Search* Parser::getSearch(const char*xml) {

    Search*     ret      = NULL;
    CmdID*      cmdID    = NULL;
    bool        noResp   = false;
    bool        noResults= false;
    Cred*       cred     = NULL;
    Target*     target   = NULL;
    Meta*       meta     = NULL;
    Data*       data     = NULL;

    cmdID     = getCmdID   (xml);
    cred      = getCred    (xml);
    noResp    = getNoResp  (xml);
    noResults = getNoResults(xml);
    target    = getTarget  (xml);

    StringBuffer lang;
    XMLProcessor::copyElementContent(lang, xml, LANG, NULL);
    meta      = getMeta    (xml);
    data      = getData    (xml);

    ArrayList sources;
    getSources (sources, xml);

    if (NotNullCheck(1, lang.c_str()) || (cmdID) || (cred)
                              || (meta)  || (target)
                              || (data)  || NotZeroArrayLength(1, &sources))  {

        ret = new Search(cmdID, noResp, noResults, cred, target, &sources,
                         lang.c_str(), meta, data);
    }

    deleteCmdID(&cmdID);
    deleteCred(&cred);
    deleteTarget(&target);
    deleteData(&data);
    deleteMeta(&meta);

    return ret;
}

Results* Parser::getResult(const char*xml) {

    if (!xml)
        return NULL;

    Results*    ret         = NULL;
    CmdID*      cmdID       = NULL;
    Meta*       meta        = NULL;

    StringBuffer t;
    cmdID           = getCmdID(xml);

    StringBuffer msgRef, cmdRef;
    XMLProcessor::copyElementContent (msgRef, xml, MSG_REF, NULL);
    XMLProcessor::copyElementContent (cmdRef, xml, CMD_REF, NULL);
    meta = getMeta(xml);

    ArrayList targetRefs;
    getTargetRefs(targetRefs, xml);

    ArrayList sourceRefs;
    getSourceRefs(sourceRefs, xml);

    ArrayList items;
    getItems(items, xml);

    if (NotNullCheck(2, msgRef.c_str(), cmdRef.c_str()) || (cmdID) || (meta)
                                        || NotZeroArrayLength(3, &items, &targetRefs, &sourceRefs)
                                        )  {

        ret = new Results(cmdID, msgRef.c_str(), cmdRef.c_str(), meta,
                          &targetRefs, &sourceRefs, &items);
    }
    deleteCmdID(&cmdID);
    deleteMeta(&meta);

    return ret;
}


//
// return and array list of items
//
void Parser::getItems(ArrayList& items, const char*xml, const char* command) {

    Item* item = NULL;
    unsigned int pos = 0, previous = 0;

    StringBuffer t;
    XMLProcessor::copyElementContent(t, &xml[pos], ITEM, &pos);
    while ((item = getItem(t.c_str(), command)) != NULL) {
        items.add(*item);    // in the ArrayList NULL element cannot be inserted
        deleteItem(&item);
        pos += previous;
        previous = pos;
        XMLProcessor::copyElementContent(t, &xml[pos], ITEM, &pos);
    }
}

Item* Parser::getItem(const char*xml, const char* command) {
    Item*   ret       = NULL;
    Target* target    = NULL;
    Source* source    = NULL;
    Meta*   meta      = NULL;
    ComplexData* data = NULL;
    bool moreData     = false;

    target   = getTarget(xml);
    source   = getSource(xml);
    meta     = getMeta(xml);
    data     = getComplexData(xml, command);

    moreData = getMoreData   (xml);
    StringBuffer targetParent, sourceParent;
    XMLProcessor::copyElementContent(targetParent, xml, TARGET_PARENT, NULL);
    XMLProcessor::copyElementContent(sourceParent, xml, SOURCE_PARENT, NULL);

    if ((target)     ||
            (source) ||
            (meta)   ||
            (data))  {
        // ret = new Item(target, source, meta, data, moreData);
        ret = new Item(target, source, targetParent.c_str(), sourceParent.c_str(),
                       meta, data, moreData);
    }

    deleteTarget     (&target);
    deleteSource     (&source);
    deleteMeta       (&meta);
    deleteComplexData(&data);

    return ret;
}

int Parser::getDataCode(const char*content) {
   int ret = 0;
   if (content) {
        ret = strtol(content, NULL, 10);
   }
   return ret;
}

Data* Parser::getData(const char* xml, unsigned int* pos) {
    StringBuffer t;
    XMLProcessor::copyElementContent (t, xml, DATA, pos);
    Data* ret = 0;
    if (t.c_str()) {
        ret = new Data(t.c_str());
    }
    return ret;
}

bool Parser::getFinalMsg(const char* xml, unsigned int* pos) {

    StringBuffer t;
    XMLProcessor::copyElementContent(t, xml, FINAL_MSG, pos);
    return !t.null();
}

CmdID* Parser::getCmdID(const char* xml, unsigned int* pos) {

    StringBuffer t;
    XMLProcessor::copyElementContent(t, xml, CMD_ID, pos);
    CmdID* ret = NULL;
    if (!t.empty()) {
        ret = new CmdID(t.c_str());
    }
    return ret;
}

ComplexData* Parser::getComplexData(const char* allxml, const char* command, unsigned int* pos) {

    StringBuffer t;
    XMLProcessor::copyElementContent(t, allxml, COMPLEX_DATA, pos);
    const char* xml = t.c_str();

    ComplexData* ret = NULL;
    Anchor* anchor   = NULL;
    DevInf* devInf   = NULL;

    if (command &&
            (strcmp(command, ADD) == 0 ||
             strcmp(command, REPLACE) == 0 ||
             strcmp(command, DEL) == 0 ||
             strcmp(command, COPY) == 0 ) ) {

        if (xml) {
            ret = new ComplexData(xml);
        }
    }
    else {
       anchor = getAnchor(xml);
       devInf = getDevInf(xml);

       if (anchor || devInf) {
           ret = new ComplexData(NULL);

           if (anchor)
               ret->setAnchor(anchor);
           if (devInf)
               ret->setDevInf(devInf);
       }
       else if (xml) {
           ret = new ComplexData(xml);
       }
       delete anchor;
       delete devInf;
    }
    return ret;
}

DevInf* Parser::getDevInf(const char*xml) {
    if (!xml) {
        return NULL;
    }

    DevInf* ret             = NULL;
    DataStore* dataStore    = NULL;
    CTCap* ctCap            = NULL;
    Ext* ext                = NULL;

    VerDTD* verDTD          = NULL;
    ArrayList dataStores;   // DataStore[]
    ArrayList ctCaps;       // CTCap[]
    ArrayList exts;         // Ext[]
    bool utc                = false;         // if present they Support UTC
    bool supportLargeObjs   = false;         // if present they Support largeObject
    bool supportNumberOfChanges = false;     // if present they Support NumberOfChanges
    SyncCap* syncCap        = NULL;

    unsigned int pos = 0;
    verDTD = getVerDTD(xml);
    StringBuffer man, mod, oem, fwV, swV, hwV, devId, devTyp;

    XMLProcessor::copyElementContent(man, xml, MAN,           NULL);
    XMLProcessor::copyElementContent(mod, xml, MOD,           NULL);
    XMLProcessor::copyElementContent(oem, xml, OEM,           NULL);
    XMLProcessor::copyElementContent(fwV, xml, FWV,           NULL);
    XMLProcessor::copyElementContent(swV, xml, SWV,           NULL);
    XMLProcessor::copyElementContent(hwV, xml, HWV,           NULL);
    XMLProcessor::copyElementContent(devId, xml, DEV_ID,        NULL);
    XMLProcessor::copyElementContent(devTyp, xml, DEV_TYP,       NULL);

    syncCap = getSyncCap(xml);

    unsigned int previous = 0;
    pos = 0;

    // DataStore
    StringBuffer t;
    XMLProcessor::copyElementContent(t, &xml[pos], DATA_STORE, &pos);
    while ((dataStore = getDataStore(t.c_str())) != NULL) {
        if (dataStore) {
            dataStores.add(*dataStore); // in the ArrayList NULL element cannot be inserted
            deleteDataStore(&dataStore);
        }
        pos += previous;
        previous = pos;
        XMLProcessor::copyElementContent(t, &xml[pos], DATA_STORE, &pos);
    }

    // ctCap
    pos = 0; previous = 0;
    XMLProcessor::copyElementContent(t, &xml[pos], CT_CAP, &pos);
    while ((ctCap = getCTCap(t.c_str())) != NULL) {
        if (ctCap) {
            ctCaps.add(*ctCap); // in the ArrayList NULL element cannot be inserted
            deleteCTCap(&ctCap);
        }
        pos += previous;
        previous = pos;
        XMLProcessor::copyElementContent(t, &xml[pos], CT_CAP, &pos);
    }

    // ext
    pos = 0; previous = 0;
    XMLProcessor::copyElementContent(t, &xml[pos], EXT, &pos);
    while ((ext = getExt(t.c_str())) != NULL) {
        if (ext) {
            exts.add(*ext); // in the ArrayList NULL element cannot be inserted
            deleteExt(&ext);
        }
        pos += previous;
        previous = pos;
        XMLProcessor::copyElementContent(t, &xml[pos], EXT, &pos);
    }

    //
    // The large object value depends on SUPPORT_LARGE_OBJECT tag.
    //
    StringBuffer value;
    pos = StringBuffer::npos;
    XMLProcessor::copyElementContent(value, xml, SUPPORT_LARGE_OBJECT, &pos);
    if (pos != StringBuffer::npos) {
        supportLargeObjs = true;
    }

    //
    // The number of changes value depends on SUPPORT_NUMBER_OF_CHANGES tag.
    //
    pos = StringBuffer::npos;
    XMLProcessor::copyElementContent(value, xml, SUPPORT_NUMBER_OF_CHANGES, &pos);
    if (pos != StringBuffer::npos) {
        supportNumberOfChanges = true;
    }

    //
    // The utc value depends on UTC tag.
    //
    pos = StringBuffer::npos;
    XMLProcessor::copyElementContent(value, xml, UTC, &pos);
    if (pos != StringBuffer::npos) {
        utc = true;
    }

    bool notNull = NotNullCheck(8, man.c_str(), mod.c_str(), oem.c_str(),
                                   fwV.c_str(), swV.c_str(), hwV.c_str(),
                                   devId.c_str(), devTyp.c_str());
    if (notNull       ||
        (verDTD)      ||
        (syncCap)     ||
        NotZeroArrayLength(3, &dataStores, &ctCaps, &exts) ) {

        ret = new DevInf(verDTD, man.c_str(), mod.c_str(), oem.c_str(), fwV.c_str(),
                         swV.c_str(), hwV.c_str(), devId.c_str(), devTyp.c_str(),
                         &dataStores, &ctCaps, &exts,
                         utc, supportLargeObjs, supportNumberOfChanges,
                         syncCap);

    }
    deleteVerDTD(&verDTD);
    deleteSyncCap(&syncCap);
    return ret;
}


/*
* TBD. There is to use the getNextTag method in xmlProcessor.
* This CTCap is no nested as a usual XML. See syncml_devinf_v11_20020215.pdf
*
*/
Ext* Parser::getExt(const char*xml) {
    Ext* ret = NULL;
    char* value        = NULL;
    ArrayList list;
    StringElement* s    = NULL;
    unsigned int pos = 0, previous = 0;

    StringBuffer XNam;
    XMLProcessor::copyElementContent(XNam, xml, XNAM, NULL);

    // XVal
    while ((value = XMLProcessor::copyElementContent(&xml[pos], XVAL, &pos)) != NULL) {
        if (value) {
            s = new StringElement(value);
            list.add(*s);
            deleteStringElement(&s);
            safeDel(&value);
        }
        pos += previous;
        previous = pos;
    }

    if ( XNam || NotZeroArrayLength(1, &list) ) {
        ret = new Ext(XNam, &list);
    }

    return ret;
}

DataStore* Parser::getDataStore(const char*xml) {
    DataStore* ret = NULL;

    SourceRef*       sourceRef      = NULL;
    long             maxGUIDSize    = 0;
    ContentTypeInfo* rxPref         = NULL;
    ContentTypeInfo* txPref         = NULL;;
    DSMem*           dsMem          = NULL;
    SyncCap*         syncCap        = NULL;
    ContentTypeInfo* x              = NULL;
    ArrayList        tx; // ContentTypeInfo[]
    ArrayList        rx; // ContentTypeInfo[]

    unsigned int pos = 0;
    StringBuffer t, displayName, maxGUIDSizeW;
    XMLProcessor::copyElementContent(t, xml, SOURCE_REF,  NULL);
    sourceRef   = getSourceRef(t.c_str());
    XMLProcessor::copyElementContent(displayName, xml, DISPLAY_NAME,             NULL);
    XMLProcessor::copyElementContent(maxGUIDSizeW, xml, MAX_GUID_SIZE,           NULL);
    if (!maxGUIDSizeW.empty()) {
        maxGUIDSize = strtol(maxGUIDSizeW.c_str(), NULL, 10);
    }
    XMLProcessor::copyElementContent(t, xml, RX_PREF,  NULL);
    rxPref = getContentTypeInfo(t.c_str());
    XMLProcessor::copyElementContent(t, xml, TX_PREF,  NULL);
    txPref = getContentTypeInfo(t.c_str());

    unsigned int previous = 0;
    pos = 0;

    // Rx
    XMLProcessor::copyElementContent(t, &xml[pos], RX, &pos);
    while ((x = getContentTypeInfo(t.c_str())) != NULL) {
        if (x) {
            rx.add(*x); // in the ArrayList NULL element cannot be inserted
            deleteContentTypeInfo(&x);
        }
        pos += previous;
        previous = pos;
        XMLProcessor::copyElementContent(t, &xml[pos], RX, &pos);
    }

    // Tx
    pos = 0, previous = 0;
    XMLProcessor::copyElementContent(t, &xml[pos], TX, &pos);
    while ((x = getContentTypeInfo(t.c_str())) != NULL) {
        if (x) {
            tx.add(*x); // in the ArrayList NULL element cannot be inserted
            deleteContentTypeInfo(&x);
        }
        pos += previous;
        previous = pos;
        XMLProcessor::copyElementContent(t, &xml[pos], TX, &pos);
    }

    dsMem = getDSMem(xml);
    syncCap = getSyncCap(xml);

    if (NotNullCheck(2, displayName.c_str(), maxGUIDSizeW.c_str()) ||
                                     (sourceRef)   ||
                                     (rxPref)      ||
                                     (txPref)      ||
                                     (dsMem)       ||
                                     (syncCap)     ||
                                     NotZeroArrayLength(2, &rx, &tx) ) {
        ret = new DataStore(sourceRef, displayName.c_str(), maxGUIDSize,
                            rxPref, &rx, txPref, &tx, NULL , dsMem, syncCap);
    }

    deleteContentTypeInfo(&rxPref);
    deleteContentTypeInfo(&txPref);
    deleteSyncCap(&syncCap);
    deleteDSMem(&dsMem);

    return ret;
}


SyncCap* Parser::getSyncCap(const char* allxml) {
  
    StringBuffer x;
    XMLProcessor::copyElementContent(x, allxml, SYNC_CAP,NULL);
    const char* xml = x.c_str();

    SyncCap* ret            = NULL;
    SyncType* syncType      = NULL;
    ArrayList list;

    unsigned int pos = 0, previous = 0;
    StringBuffer t;
    XMLProcessor::copyElementContent(t, &xml[pos], SYNC_TYPE, &pos);
    while ((syncType = getSyncType(t.c_str())) != NULL) {
        if (syncType) {
            list.add(*syncType); // in the ArrayList NULL element cannot be inserted
            deleteSyncType(&syncType);
        }
        pos += previous;
        previous = pos;
        XMLProcessor::copyElementContent(t, &xml[pos], SYNC_TYPE, &pos);
    }

    if (NotZeroArrayLength(1, &list)) {
        ret = new SyncCap(&list);
    }

    return ret;
}

SyncType* Parser::getSyncType(const char*content) {

    SyncType* ret            = NULL;
    int value                = 0;

    if (content) {
         value = strtol(content, NULL, 10);
         if (value >= 1 && value <= 7) {
             ret = new SyncType(value);
         }
    }

    return ret;
}


ContentTypeInfo* Parser::getContentTypeInfo(const char*xml) {

    ContentTypeInfo* ret = NULL;
    char* ctType      = NULL;
    char* verCT       = NULL;

    ctType = XMLProcessor::copyElementContent(xml, CT_TYPE,             NULL);
    verCT  = XMLProcessor::copyElementContent(xml, VER_CT,             NULL);

    if (NotNullCheck(2, ctType, verCT)) {
        ret = new ContentTypeInfo(ctType, verCT);
    }

    safeDel(&ctType);
    safeDel(&verCT);
    return ret;
}

DSMem* Parser::getDSMem(const char* allxml, unsigned int* pos) {

    StringBuffer x;
    XMLProcessor::copyElementContent(x, allxml, DS_MEM, pos);
    const char* xml = x.c_str();

    DSMem* ret          = NULL;
    StringBuffer maxMemW, sharedMemW, maxIDW;

    bool    sharedMem   = false;
    long    maxMem     = 0;
    long    maxID      = 0;

    bool isToCreate = false;

    XMLProcessor::copyElementContent (maxMemW, xml, MAX_MEM,   NULL);
    XMLProcessor::copyElementContent (sharedMemW, xml, SHARED_MEM, NULL);
    XMLProcessor::copyElementContent (maxIDW, xml, MAX_ID,    NULL);

    isToCreate = NotNullCheck(3, maxMemW.c_str(), sharedMemW.c_str(),
                                 maxIDW.c_str());

    if (!maxMemW.empty()) {
        maxMem = strtol(maxMemW.c_str(), NULL, 10);
    }
    if (!maxIDW.empty()) {
        maxID = strtol(maxIDW.c_str(), NULL, 10);
    }
    if (!sharedMemW.empty()) {
        sharedMem = sharedMemW != "0" ? true : false;
    }

    if (isToCreate) {
        ret = new DSMem(sharedMem, maxMem, maxID);
    }

    return ret;

}

bool Parser::getNoResp(const char* xml, unsigned int* pos) {
    StringBuffer t;
    XMLProcessor::copyElementContent (t, xml, NO_RESP, pos);
    return (!t.null());
}

bool Parser::getNoResults(const char* xml, unsigned int* pos) {
    StringBuffer t;
    XMLProcessor::copyElementContent(t, xml, NO_RESULTS, pos);
    return !t.null();
}

bool Parser::getMoreData(const char* xml, unsigned int* pos) {
    StringBuffer t;
    XMLProcessor::copyElementContent(t, xml, MORE_DATA, pos);
    return !t.null();
}

/*
* TBD. There is to use the getNextTag method in xmlProcessor.
* This CTCap is no nested as a usual XML. See syncml_devinf_v11_20020215.pdf
* TBD
*
*/
CTCap* Parser::getCTCap(const char* /* xml */) {
    CTCap* ret = NULL;
    //CTTypeSupported* ctTypeSupported = NULL;

    // ArrayList* ctTypes = new ArrayList();

    return ret;
}

//
// TBD
//
ArrayList* Parser::getEMI(const char* /*content*/) {
    ArrayList* ret = NULL;
    return ret;
}

END_NAMESPACE


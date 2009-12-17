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

#include <string.h>

#include "base/fscapi.h"
#include "base/base64.h"
#include "base/util/utils.h"
#include "base/util/StringBuffer.h"
#include "filter/ClauseUtil.h"
#include "spds/constants.h"
#include "spds/DataTransformerFactory.h"
#include "spds/SyncItem.h"
#include "spds/SyncMLBuilder.h"

#include "event/FireEvent.h"
#include "base/globalsdef.h"

USE_NAMESPACE


SyncMLBuilder::SyncMLBuilder() {
    initialize();
    set(NULL, NULL);
}

SyncMLBuilder::~SyncMLBuilder() {
    safeDelete(&target  );
    safeDelete(&device  );
}


SyncMLBuilder::SyncMLBuilder(char* t, char* d) {
    initialize();
    set(t, d);
}

void SyncMLBuilder::set(const char*t, const char*d) {

    target   = stringdup(t);
    device   = stringdup(d);
}

void SyncMLBuilder::initialize() {
    sessionID = (unsigned long)time(NULL);
    msgRef    = 0         ;
    msgID     = 0         ;
    cmdID     = 0         ;
}

void SyncMLBuilder::resetCommandID() {
    cmdID = 0;
}

void SyncMLBuilder::increaseMsgRef() {
    msgRef++;
}

void SyncMLBuilder::resetMsgRef() {
    msgRef = 0;
}

void SyncMLBuilder::addItemStatus(ArrayList* previousStatus, Status* status) {

    if (previousStatus->size() == 0) {
        previousStatus->add(*status);
        return;
    }

    bool found    = false;
    Status* s     = NULL;

    if (status == NULL)
        return;
    ArrayList* list = new ArrayList();
    for (int i = 0; i < previousStatus->size(); i++) {

        s = (Status*)previousStatus->get(i);
        if ((strcmp(s->getCmd(), status->getCmd()) == 0) &&
            (strcmp(s->getData()->getData(), status->getData()->getData()) == 0) &&
            (strcmp(s->getCmdRef(), status->getCmdRef()) == 0) ) {
                    list = s->getItems();
                    for (int j = 0; j < status->getItems()->size(); j++) {
                        list->add(*((Item*)(status->getItems())->get(j)));
                        found = true;
                    }
                }

    }
    if (!found)
        previousStatus->add(*status);

}



/*
* Return the status of the items sent by server. Used to create the status to respond
* after a add, replace or delete command
*/
Status* SyncMLBuilder::prepareItemStatus(const char* COMMAND,
                                         const char* key,
                                         const char* cmdRef,
                                         int code) {
    /*
    <Status>                                 CmdID*        cmdID     ,
        <CmdID>3</CmdID>                     char*      msgRef    ,
        <MsgRef>2</MsgRef>                   char*      cmdRef    ,
        <CmdRef>4</CmdRef>                   char*      cmd       ,
        <Cmd>Replace</Cmd>                   ArrayList*    targetRefs,
        <Data>201</Data>                     ArrayList*    sourceRefs,
        <Item>                               Cred*         cred      ,
            <Source>                         Chal*         chal      ,
            <LocURI>item0</LocURI>           Data*         data      ,
            </Source>                        ArrayList*    items     );
        </Item>
        <Item>
            <Source>
            <LocURI>item1</LocURI>
            </Source>
        </Item>
    </Status>
    */

    ++cmdID;
    char* cmdid = itow(cmdID);
    CmdID* commandID  = new CmdID(cmdid);
    ArrayList* empty  = new ArrayList();
    Data*      data   = new Data(code);
    ArrayList* list   = new ArrayList();
    Source* sou       = new Source(key);
    Item* item        = new Item(NULL, sou, NULL, NULL, false);
    list->add(*item);

    char *mRef = itow(msgRef);
    Status* s = new Status(commandID, mRef, cmdRef, COMMAND, empty, empty, NULL, NULL, data, list);
    delete [] mRef;

    safeDelete(&cmdid);
    deleteCmdID(&commandID);
    deleteData(&data);
    deleteSource(&sou);
    deleteItem(&item);
    delete empty;
    delete list;

    return s;

}

/*
* Return the status of the server authentication process
*/
Status* SyncMLBuilder::prepareSyncHdrStatus(Chal*chal, int d) {

    ++cmdID;
    char* cmdid = itow(cmdID);
    CmdID* commandID         = new CmdID(cmdid);
    ArrayList*    targetRefs = new ArrayList();
    ArrayList*    sourceRefs = new ArrayList();
    //TargetRef*    tar        = new TargetRef(target);
    //SourceRef*    sou        = new SourceRef(device);
    TargetRef*    tar        = new TargetRef(device);
    SourceRef*    sou        = new SourceRef(target);
    Data*      data          = new Data(d);
    targetRefs->add(*tar);
    sourceRefs->add(*sou);

    char* wmsgRef = itow(msgRef);
    Status* s = new Status(commandID, wmsgRef, "0", SYNC_HDR, targetRefs, sourceRefs, NULL, chal, data, NULL);
    if (wmsgRef){
        delete [] wmsgRef;
        wmsgRef = NULL;
    }
    // Fire Sync Status Event: syncHdr status from client
    fireSyncStatusEvent(SYNC_HDR, s->getStatusCode(), NULL, NULL, NULL , CLIENT_STATUS);

    safeDelete(&cmdid);
    deleteCmdID(&commandID);
    deleteTargetRef(&tar);
    deleteSourceRef(&sou);
    deleteData(&data);
    delete targetRefs;
    delete sourceRefs;

    return s;

}

Status* SyncMLBuilder::prepareSyncStatus(SyncSource& source, Sync* sync) {
    if (sync == NULL)
        return NULL;

    ++cmdID;

    char* cmdid = itow(cmdID);
    CmdID* commandID         = new CmdID(cmdid);
    delete [] cmdid;   cmdid = NULL;
    ArrayList*    targetRefs = new ArrayList();
    ArrayList*    sourceRefs = new ArrayList();
    CmdID* cmdRef            = sync->getCmdID();


    TargetRef*    tar        = new TargetRef(source.getConfig().getURI());
    SourceRef*    sou        = new SourceRef(_wcc(source.getName()));
    targetRefs->add(*tar);
    sourceRefs->add(*sou);
    Data* d                  = new Data(200);
    char* wmsgRef = itow(msgRef); 
    Status* s = new Status(commandID, wmsgRef, cmdRef->getCmdID(), SYNC, targetRefs, sourceRefs, NULL, NULL, d, NULL);
    if (wmsgRef){
        delete [] wmsgRef;
        wmsgRef = NULL;
    }

    // Fire Sync Status Event: sync status from client
    fireSyncStatusEvent(SYNC, s->getStatusCode(), source.getConfig().getName(),
                        source.getConfig().getURI(), NULL, CLIENT_STATUS);

    deleteCmdID(&commandID);
    deleteTargetRef(&tar);
    deleteSourceRef(&sou);
    deleteData(&d);
    delete targetRefs;
    delete sourceRefs;

    return s;

}


/*
* Return the status against an alert command. It could be releated on the authentication process
* or any other else
*/
Status* SyncMLBuilder::prepareAlertStatus(SyncSource& source, ArrayList* alerts, int authStatusCode) {
    /*
    next = source.getNextAnchor();
    <Status>\n
        <CmdID>2</CmdID>\n
        <MsgRef>1</MsgRef>
        <CmdRef>1</CmdRef>
        <Cmd>Alert</Cmd>\n
        <TargetRef>
            sb.append(source.getRemoteURI());
        </TargetRef>\n
        <SourceRef>
            sb.append(source.getName(NULL, -1));
        </SourceRef>\n
        <Data>200</Data>\n
        <Item>\n
            <Data>
                <Anchor xmlns='syncml:metinf'>
                    <Next>
                        sb.append(next);
                    </Next>
                </Anchor>\n
            </Data>\n
        </Item>\n
    </Status>
    */

    if (alerts == NULL || alerts->size() == 0)
        return NULL;

    Alert* a = NULL;
    Item* item = NULL;
    ArrayList* list = NULL; // new ArrayList();
    bool found = false;

    for (int i = 0; i < alerts->size(); i++) {
        a = (Alert*)alerts->get(i);
        list = a->getItems();
        if (list->size() == 1) {
            Item* it = (Item*)list->get(0);
            if (strcmp(it->getTarget()->getLocURI(), _wcc(source.getName())) == 0) {
                found = true;
                break;
            }
        }
    }

    //
    // Currently it returns null. It could return a status value if necessary
    //
    if (!found) {
        return NULL;

    }

    ++cmdID;

    char* cmdid = itow(cmdID);
    CmdID* commandID         = new CmdID(cmdid);
    delete [] cmdid;   cmdid = NULL;
    ArrayList*    targetRefs = new ArrayList();
    ArrayList*    sourceRefs = new ArrayList();

    TargetRef*    tar        = new TargetRef(source.getConfig().getURI());
    SourceRef*    sou        = new SourceRef(_wcc(source.getName()));
    targetRefs->add(*tar);
    sourceRefs->add(*sou);
    CmdID* cmdRef            = a->getCmdID();
    char* next            = NULL;
    int authStatus           = 0;

    if (authStatusCode >= 200 && authStatusCode <=299)
        authStatus = 200;
     else
        authStatus = authStatusCode;

    Data* d              = new Data(authStatus);
    ComplexData* data    = NULL;
    ArrayList* items     = new ArrayList();
    Anchor*    anchor    = NULL;

    if (authStatusCode < 400) {
        list = a->getItems();
        if (list->size() > 0) {
            for (int i = 0; i < list->size(); i++) {
                Item* it = (Item*)list->get(i);
                Meta* m = it->getMeta();
                if (m) {
                    anchor = m->getAnchor();
                    next = stringdup(anchor->getNext());
                }

            }

            anchor = new Anchor(NULL, next);
            data = new ComplexData();
            data->setAnchor(anchor);
            item           = new Item(NULL, NULL, NULL, data, false);
            items->add(*item);
        }
    }
    char* wmsgRef = itow(msgRef);
    Status* s = new Status(commandID, wmsgRef, cmdRef->getCmdID(), ALERT,
                           targetRefs, sourceRefs, NULL, NULL, d, items);
    if (wmsgRef){
        delete [] wmsgRef;
        wmsgRef = NULL;
    }

    // Fire Sync Status Event: alert status from client
    fireSyncStatusEvent(ALERT, s->getStatusCode(), source.getConfig().getName(),
                        source.getConfig().getURI(), NULL , CLIENT_STATUS);

    deleteCmdID(&commandID);
    deleteTargetRef(&tar);
    deleteSourceRef(&sou);
    deleteItem(&item);
    deleteAnchor(&anchor);
    deleteComplexData(&data);
    deleteData(&d);
    safeDel(&next);
    delete items;
    delete targetRefs;
    delete sourceRefs;
    return s;
}

/*
* Return the status against an arbitrary command.
*/
Status* SyncMLBuilder::prepareCmdStatus(AbstractCommand &cmd, int status) {
    /*
    <Status>\n
        <CmdID>2</CmdID>\n
        <MsgRef>1</MsgRef>
        <CmdRef>1</CmdRef>
        <Cmd>cmd</Cmd>\n
        <Data>status</Data>\n
    </Status>
    */

    ++cmdID;

    char* cmdid = itow(cmdID);
    CmdID commandID(cmdid);
    delete [] cmdid; cmdid = NULL;
    Data d(status);
    char *msgRefStr = itow(msgRef);
    ArrayList empty;

    Status* s = new Status(&commandID, msgRefStr, cmd.getCmdID()->getCmdID(),
                           cmd.getName(), &empty, &empty, NULL, NULL, &d, NULL);

    // Fire Sync Status Event: status from client
    fireSyncStatusEvent(s->getCmd(), s->getStatusCode(), NULL, NULL, NULL , CLIENT_STATUS);

    delete [] msgRefStr;
    return s;
}

AbstractCommand *SyncMLBuilder::prepareDevInf(AbstractCommand *cmd, DevInf &devInf)
{
    AbstractCommand *res = NULL;
    char *msgRefStr = NULL;

    Source source(DEVINF_URI);
    Meta meta;
    meta.setType(DEVINF_FORMAT);
    // meta.setFormat("xml");
    ComplexData complexData;
    complexData.setDevInf(&devInf);
    Item item(NULL,
              &source,
              NULL,
              &complexData,
              false);


    ++cmdID;
    char* cmdid = itow(cmdID);
    CmdID commandID(cmdid);
    delete [] cmdid; cmdid = NULL;
    ArrayList items;
    items.add(item);

    if (cmd) {
        /*
          <Result>
          <CmdID>2</CmdID>
          <MsgRef>1</MsgRef>
          <CmdRef>4</CmdRef>
          <Meta><Type xmlns='syncml:metinf'>application/vnd.syncml-devinf+xml</Type></Meta>
          <Item>
            <SourceRef><LocURI>./devinf11</LocURI></SourceRef>
            <Data>
              <DevInf>...</DevInf>
            </Data>
          </Item>
          </Result>
        */

        msgRefStr = itow(msgRef);
        ArrayList empty;
        res = new Results(&commandID,
                          msgRefStr,
                          cmd->getCmdID()->getCmdID(),
                          &meta,
                          &empty,
                          &empty,
                          &items);
    } else {
        /*
          <Put>
            <CmdID>2</CmdID>
            <Meta><Type xmlns='syncml:metinf'>application/vnd.syncml-devinf+xml</Type></Meta>
            <Item>
              <Source><LocURI>./devinf11</LocURI></Source>
              <Data>
                <DevInf xmlns='syncml:devinf'>...</DevInf>
              </Data>
            </Item>
          </Put>
        */

        res = new Put(&commandID,
                      false,
                      NULL,
                      NULL,
                      &meta,
                      &items);
    }
    safeDelete(&msgRefStr);
    return res;
}


AbstractCommand *SyncMLBuilder::prepareServerDevInf()
{
    AbstractCommand *res = NULL;

    Target target(DEVINF_URI);
    Meta meta;
    meta.setType(DEVINF_FORMAT);
    // meta.setFormat("xml");
    Item item(&target,
              NULL,
              NULL,
              NULL,
              false);

    ++cmdID;
    char* cmdid = itow(cmdID);
    CmdID commandID(cmdid);
    delete [] cmdid; cmdid = NULL;
    ArrayList items;
    items.add(item);

    /*
    <Get>
        <CmdID>3</CmdID>
        <Meta><Type xmlns=’syncml:metinf’>application/vnd.syncml-devinf+xml</Type></Meta>
        <Item>
            <Target><LocURI>./devinf12</LocURI></Target>
        </Item>
    </Get>
    */

    res = new Get(&commandID,
                  false,
                  NULL,
                  NULL,
                  &meta,
                  &items);

    return res;
}



Alert* SyncMLBuilder::prepareAlert(SyncSource& s, int code) {

    ++cmdID;

    char* cmdid = itow(cmdID);
    CmdID* commandID     = new CmdID(cmdid);
    delete [] cmdid; cmdid = NULL;
    Target* tar          = new Target(s.getConfig().getURI());
    Source* sou          = new Source(_wcc(s.getName()));
    Item* item           = new Item(tar, sou, NULL, NULL, false);

    ArrayList* list      = new ArrayList();
    list->add(*item);
    Alert* alert         = new Alert(commandID, false, NULL, code, list);

    deleteTarget(&tar);
    deleteSource(&sou);
    deleteItem(&item);
    deleteCmdID(&commandID);
    delete list;

    return alert;
}

Alert* SyncMLBuilder::prepareInitAlert(SyncSource& s, unsigned long maxObjSize) {

    ++cmdID;

    char* cmdid = itow(cmdID);
    CmdID* commandID     = new CmdID(cmdid);
    delete [] cmdid; cmdid = NULL;
    int data             = s.getPreferredSyncMode();
    Target* tar          = new Target(s.getConfig().getURI());
    const char*  val     =  toMultibyte(s.getName());
    Source* sou          = new Source(val);
    if ( val ){
        delete [] val;
        val = NULL;
    }

    //
    // Read the clause filter from the source and translate
    // it into a Filter. This must be assigned to the Target
    //
    SourceFilter* sourceFilter = s.getFilter();
    if (sourceFilter) {
        Filter* filter = ClauseUtil::toFilter(*sourceFilter);
        tar->setFilter(filter);
        delete filter; filter = NULL;
    }


    Anchor*    anchor    = new Anchor(s.getLastAnchor(), s.getNextAnchor());
    MetInf* metInf       = new MetInf(NULL, NULL, NULL, 0,
                            anchor, NULL, NULL, 0, maxObjSize,  NULL, NULL);
    Meta* meta           = new Meta();
    meta->setMetInf(metInf);
    Item* item           = new Item(tar, sou, meta, NULL, false);

    ArrayList* list      = new ArrayList();
    list->add(*item);
    Alert* alert         = new Alert(commandID, false, NULL, data, list);

    deleteCmdID(&commandID);
    deleteTarget(&tar);
    if(sou)
        delete sou;
    deleteAnchor(&anchor);
    deleteMetInf(&metInf);
    deleteMeta(&meta);
    deleteItem(&item);
    delete list;

    return alert;
}

Alert* SyncMLBuilder::prepareAddrChangeAlert(SyncSource& s) {

    SyncItem *syncItem;
    ArrayList list;
    for(syncItem = s.getFirstItem(); syncItem; syncItem = s.getNextItem()) {
        int size = syncItem->getDataSize();

        if( syncItem && size ) {  // only valid items

            char* syncData = new char[size + 1];
            memset(syncData, 0, size + 1);
            memcpy (syncData, (char*)syncItem->getData(), size);

            ComplexData addr( syncData );
            Target target( "" );
            Source source(_wcc(syncItem->getKey()));
            // Build Item
            Item item(&target, &source, NULL, &addr, false);
            // Add it to the list
            list.add(item);

            delete [] syncData;
        }
    }
    /*
    for(syncItem = s.getFirstItem(); syncItem; syncItem = s.getNextItem()) {
        int size = syncItem->getDataSize()/sizeof(char);
        char *syncData = (char *)syncItem->getData();

        if( syncItem && size ) {  // only valid items

            // Add the syncItem data as zero terminated string
            StringBuffer itemData(syncData, size);
            ComplexData addr( itemData.c_str() );
            Target target( "" );
            Source source(syncItem->getKey());
            // Build Item
            Item item(&target, &source, NULL, &addr, false);
            // Add it to the list
            list.add(item);
        }
    }
    */
    // If no valid items were provided by the syncsource
    // return null alert
    if(list.isEmpty())
        return NULL;

    // Ok, prepare the message
    cmdID++;

    char* cmdid = itow(cmdID);
    CmdID commandID(cmdid);
    delete [] cmdid; cmdid = NULL;
    int data = s.getPreferredSyncMode();

    Alert* alert = new Alert(&commandID, false, NULL, data, &list);

    return alert;
}

SyncHdr* SyncMLBuilder::prepareSyncHdr(Cred* cred, unsigned long maxMsgSize, unsigned long maxObjSize) {

    ++msgID;

    VerDTD*    verDTD    = new VerDTD("1.2");
    VerProto*  verProto  = new VerProto("SyncML/1.2");
    char* sid = ltow(sessionID);
    SessionID* sessID    = new SessionID(sid);
    delete [] sid;   sid = NULL;
    char*   messageID = itow(msgID);
    Target*    tar       = new Target(target);
    Source*    sou       = NULL;
    Meta* meta           = NULL;

    if (cred && strcmp(cred->getType(), AUTH_TYPE_MD5) == 0)
        sou = new Source(device, cred->getUsername());
    else
        sou = new Source(device);


    if (maxMsgSize > 0 || maxObjSize > 0) {
        MetInf* metInf = new MetInf(NULL, NULL, NULL, 0,
                                    NULL, NULL, NULL, maxMsgSize,
                                    maxObjSize, NULL, NULL);
        meta = new Meta();
        meta->setMetInf(metInf);
        deleteMetInf(&metInf);
    }
    SyncHdr* syncHdr = new SyncHdr(verDTD, verProto, sessID, messageID,
                                   tar, sou, NULL, false, cred, meta); //respUri e Meta are null

    deleteVerDTD(&verDTD);
    deleteVerProto(&verProto);
    deleteSessionID(&sessID);
    deleteSource(&sou);
    deleteTarget(&tar);
    safeDel(&messageID);
    if (meta) {
        delete meta;
        meta = NULL;
    }
    
    return syncHdr;
}

SyncML* SyncMLBuilder::prepareInitObject(Cred* cred, ArrayList* alerts,
                                         ArrayList* commands, unsigned long maxMsgSize,
                                         unsigned long maxObjSize) {

    SyncHdr* syncHdr     = prepareSyncHdr(cred, maxMsgSize, maxObjSize);
    SyncML*  syncml      = NULL;
    ArrayList* list      = NULL;
    SyncBody* syncBody   = NULL;

    // Clone commands, even if empty. The result is a list anyway.
    list = commands->clone();

    if (alerts && alerts->size() > 0) {
        for (int k = 0; k < alerts->size(); k++)
            list->add(*(Alert*)alerts->get(k));
    }

    syncBody   = new SyncBody(list, true);
    syncml       = new SyncML(syncHdr, syncBody);

    deleteSyncHdr(&syncHdr);
    deleteSyncBody(&syncBody);
    delete list;

    return syncml;
}

char* SyncMLBuilder::prepareMsg(SyncML* syncml) {
    StringBuffer *s = Formatter::getSyncML(syncml);
    char* str = stringdup(s->c_str());
    delete s;
    return str;
}


SyncML* SyncMLBuilder::prepareSyncML(ArrayList* commands, bool final) {

    SyncHdr* syncHdr = prepareSyncHdr(NULL);
    SyncBody* syncBody   = new SyncBody(commands, final);
    SyncML* syncml = new SyncML(syncHdr, syncBody);
    deleteSyncHdr(&syncHdr);
    deleteSyncBody(&syncBody);

    return syncml;
}

ComplexData* SyncMLBuilder::getComplexData(SyncItem* syncItem,
                                           long &syncItemOffset, long maxBytes, long &sentBytes) {

        char* t   = NULL;
        ComplexData* data = NULL;

        sentBytes = syncItem->getDataSize() - syncItemOffset;
        if (sentBytes && sentBytes > maxBytes) {
            sentBytes = maxBytes;
            if (sentBytes < 1) {
                // ensure that at least one byte is sent even if maxBytes is 0 or negative,
                // otherwise no progress is made
                sentBytes = 1;
            }
        }

        t = new char[sentBytes + 1];
        memset(t, 0, sentBytes + 1);
        memcpy(t, (char *)syncItem->getData() + syncItemOffset, sentBytes);
        syncItemOffset += sentBytes;

        data = new ComplexData(t);

        if (t)
            {delete [] t; t = NULL; }

        return data;
}


ArrayList* SyncMLBuilder::prepareItem(SyncItem* syncItem,
                                      long &syncItemOffset, long maxBytes,
                                      long &sentBytes,
                                      const char* /* type */, const char* COMMAND) {
    ArrayList* list = new ArrayList();

    Source* sou = new Source(_wcc(syncItem->getKey()));
    ComplexData* data = NULL;
    Meta m;
    bool hasMoreData = false;
    bool isFirstChunk = !syncItemOffset;
    if (strcmp(DELETE_COMMAND_NAME, COMMAND) != 0) {
        if (syncItem->getDataEncoding()) {
            m.setFormat(syncItem->getDataEncoding());
        }
        data = getComplexData(syncItem, syncItemOffset, maxBytes, sentBytes);
        hasMoreData = syncItemOffset < syncItem->getDataSize();
        if (isFirstChunk && hasMoreData) {
            // must send size, but only in first chunk of this item
            m.setSize(syncItem->getDataSize());
        }
    } else {
        // skip all item data for deleted items
        syncItemOffset = syncItem->getDataSize();
        sentBytes = 0;
    }

    char *tparent = toMultibyte(syncItem->getTargetParent());
    char *sparent = toMultibyte(syncItem->getSourceParent());

    Item* item = new Item(NULL, sou, tparent, sparent, &m, data, hasMoreData);
    list->add(*item);

    delete [] tparent;
    delete [] sparent;

    deleteSource(&sou);
    deleteComplexData(&data);
    deleteItem(&item);

    return list;
}


ComplexData* SyncMLBuilder::getComplexData(Chunk* chunk) {

    if (!chunk) {
        return  NULL;
    }
    ComplexData* data = new ComplexData(chunk->getData());            
    return data;
}

Item* SyncMLBuilder::prepareItemChunk(SyncItem* syncItem,
                                      Chunk* chunk,
                                      char* COMMAND) {
    
    StringBuffer key; key.convert(syncItem->getKey());
    Source* sou = new Source(key.c_str());
    ComplexData* data = NULL;
    bool hasMoreData = !chunk->isLast();
    Meta m;
    

    if (strcmp(DELETE_COMMAND_NAME, COMMAND) != 0) {
        m.setFormat(chunk->getDataEncoding().c_str());               
        data = getComplexData(chunk);        
        if (chunk->isFirst() && !chunk->isLast()) {
            // must send size, but only in first chunk of this item
            // m.setSize(syncItem->getDataSize());
            m.setSize(chunk->getTotalDataSize());            
        }
    } else {
        // skip all item data for deleted items        
    }    

    char *tparent = toMultibyte(syncItem->getTargetParent());
    char *sparent = toMultibyte(syncItem->getSourceParent());

    Item* item = new Item(NULL, sou, tparent, sparent, &m, data, hasMoreData);
    
    delete [] tparent;
    delete [] sparent;

    deleteSource(&sou);
    deleteComplexData(&data);
    
    return item;
}


long SyncMLBuilder::addChunk(ModificationCommand* &modificationCommand,                            
                            char* COMMAND, SyncItem* syncItem, Chunk* chunk, 
                            const char* defaultType) {

    if (chunk == NULL) {
         return 0;
    }

    // The item should determine its type itself.
    // Only fallback to the default type configured for its
    // source if (broken?) SyncSources do not set a in their
    // items.
    const char *type = NULL;
    type = toMultibyte(syncItem->getDataType());
    if (!type || !type[0]) {
        type = stringdup(defaultType);
    }

    if (!modificationCommand) {
        ++cmdID;
        char* cmdid = itow(cmdID);
        CmdID commandID(cmdid);
        delete [] cmdid; cmdid = NULL;
       
        MetInf metInf(NULL, (char*)type, NULL, 0,
                      NULL, NULL, NULL, 0, 0, NULL, NULL);
        Meta meta;
        meta.setMetInf(&metInf);

        if (strcmp(ADD_COMMAND_NAME, COMMAND) == 0)
            modificationCommand = new Add(&commandID, false, NULL, &meta, NULL);
        else if (strcmp(REPLACE_COMMAND_NAME, COMMAND) == 0){
            modificationCommand = new Replace(&commandID, false, NULL, &meta, NULL);
        } else if (strcmp(DELETE_COMMAND_NAME, COMMAND) == 0) {
            modificationCommand = new Delete(&commandID, false, false, false, NULL, &meta, NULL);
        }
    }

    ArrayList* list = modificationCommand->getItems();    
    Item* item = prepareItemChunk(syncItem, chunk, COMMAND);
    
    list->add(*item);
    deleteItem(&item);
    
    delete [] type;    
    
    return chunk->getDataSize();
}


long SyncMLBuilder::addItem(ModificationCommand* &modificationCommand,
                            long &syncItemOffset, long maxBytes,
                            const char* COMMAND, SyncItem* syncItem, const char* defaultType) {
    if (syncItem == NULL) {
         return 0;
    }

    // The item should determine its type itself.
    // Only fallback to the default type configured for its
    // source if (broken?) SyncSources do not set a in their
    // items.
    const char *type = _wcc(syncItem->getDataType());
    if (!type || !type[0]) {
        type = defaultType;
    }

    if (!modificationCommand) {
        ++cmdID;
        char* cmdid = itow(cmdID);
        CmdID commandID(cmdid);
        delete [] cmdid; cmdid = NULL;
        MetInf metInf(NULL, (char*)type, NULL, 0,
                      NULL, NULL, NULL, 0, 0, NULL, NULL);
        Meta meta;

        meta.setMetInf(&metInf);

        if (strcmp(ADD_COMMAND_NAME, COMMAND) == 0)
            modificationCommand = new Add(&commandID, false, NULL, &meta, NULL);
        else if (strcmp(REPLACE_COMMAND_NAME, COMMAND) == 0){
            modificationCommand = new Replace(&commandID, false, NULL, &meta, NULL);
        } else if (strcmp(DELETE_COMMAND_NAME, COMMAND) == 0) {
            modificationCommand = new Delete(&commandID, false, false, false, NULL, &meta, NULL);
        }
    }

    ArrayList* list = modificationCommand->getItems();
    /* size is effectively undefined for deleted items, don't check it */
    assert(!strcmp(DELETE_COMMAND_NAME, COMMAND) || syncItemOffset >= 0);
    assert(!strcmp(DELETE_COMMAND_NAME, COMMAND) || syncItemOffset <= syncItem->getDataSize());
    long sentBytes = 0;
    ArrayList* tmpList = prepareItem(syncItem, syncItemOffset, maxBytes, sentBytes, type, COMMAND);
    assert(!strcmp(DELETE_COMMAND_NAME, COMMAND) || syncItemOffset >= 0);
    assert(!strcmp(DELETE_COMMAND_NAME, COMMAND) || syncItemOffset <= syncItem->getDataSize());
    list->add(tmpList);
    delete tmpList;

    return sentBytes;
}

Sync* SyncMLBuilder::prepareSyncCommand(SyncSource& source) {

    ++cmdID;

    char* cmdid = itow(cmdID);
    CmdID* commandID     = new CmdID(cmdid);
    delete [] cmdid; cmdid = NULL;
    Target* tar          = new Target(source.getConfig().getURI());
    Source* sou          = new Source(_wcc(source.getName()));
    ArrayList* list      = new ArrayList();
    Sync* sync           = NULL;

    sync = new Sync(commandID, false, NULL, tar, sou, NULL, -1,  list);

    deleteCmdID(&commandID);
    deleteTarget(&tar);
    deleteSource(&sou);
    delete list;

    return sync;

}


Map* SyncMLBuilder::prepareMapCommand(SyncSource& s) {
    /*
    CmdID* cmdID,
    Target* target,
    Source* source,
    Cred* cred,
    Meta* meta,
    ArrayList* mapItems);
    */
    ++cmdID;
    char* cmdid = itow(cmdID);
    CmdID* commandID     = new CmdID(cmdid);
    delete [] cmdid; cmdid = NULL;
    Target* tar          = new Target(s.getConfig().getURI());
    Source* sou          = new Source(_wcc(s.getName()));
    ArrayList mapItems;
    Map* map = new Map(commandID, tar, sou, NULL, NULL, &mapItems);

    deleteCmdID(&commandID);
    deleteTarget(&tar);
    deleteSource(&sou);

    return map;

}

MapItem* SyncMLBuilder::prepareMapItem(SyncMap* syncMap) {

    Target* tar          = new Target(syncMap->getGUID());
    Source* sou          = new Source(syncMap->getLUID());
    MapItem* m = new MapItem(tar, sou);
    deleteTarget(&tar);
    deleteSource(&sou);

    return m;
}

void SyncMLBuilder::addMapItem(Map* map, MapItem* mapItem){
    if (mapItem == NULL || map == NULL)
        return;
    ArrayList* list = map->getMapItems();
    list->add(*mapItem);

}

void SyncMLBuilder::resetMessageID() {
    msgID = 0;
}


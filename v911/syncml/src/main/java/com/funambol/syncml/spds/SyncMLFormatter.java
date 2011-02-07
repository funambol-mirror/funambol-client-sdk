/*
 * Funambol is a mobile platform developed by Funambol, Inc.
 * Copyright (C) 2009 Funambol, Inc.
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

package com.funambol.syncml.spds;

import java.io.OutputStream;
import java.io.IOException;
import java.util.Vector;

import org.xmlpull.v1.XmlPullParserException;
import org.xmlpull.v1.XmlPullParser;
import org.xmlpull.v1.XmlSerializer;
//import org.xmlpull.v1.XmlPullParserFactory;
import com.funambol.org.kxml2.io.KXmlParser;
import com.funambol.org.kxml2.io.KXmlSerializer;

// For WBXML support
import com.funambol.org.kxml2.wap.WbxmlSerializer;
import com.funambol.org.kxml2.wap.Wbxml;


import com.funambol.syncml.protocol.*;
import com.funambol.util.Log;

/**
 * This class represents a formatter for SyncML. A formatter is intented to
 * generate SyncML messages starting from an abstract representation of the same
 * message. In this current implementation the formatter is not yet connected to
 * the abstract implementation and it performs a sort of low level formatting
 * where the bit and pieces of a message are printed together to form the
 * outgoing message.
 * The current implementation supports only plain XML but the class aims at
 * supporting WBXML at some point.
 */
public class SyncMLFormatter {

    private static final String TAG_LOG = "SyncMLFormatter";

    private static final String METINF = "syncml:metinf";

    private XmlSerializer formatter;
    private OutputStream  os;

    private boolean wbxml = true;

    private boolean prettyPrint = false;

    private boolean hideData = false;

    public SyncMLFormatter(boolean wbxml) {
        this.wbxml = wbxml;
    }

    public void setHideData(boolean hideData) {
        this.hideData = hideData;
    }

    public void setPrettyPrint(boolean prettyPrint) {
        this.prettyPrint = prettyPrint;
    }

    public void format(SyncML msg, OutputStream os, String charset) throws IOException {

        this.os = os;

        // Initialize the XmlWriter
        if (wbxml) {
            formatter = com.funambol.org.kxml2.wap.syncml.SyncML.createSerializer();
        } else {
            formatter = new KXmlSerializer();
            prettyPrint = true;
        }

        formatter.setOutput(os, charset);
        if (wbxml) {
            formatter.startDocument(null, null);
        }

        // Now print the message
        startTag(SyncML.TAG_SYNCML);

        SyncHdr hdr = msg.getSyncHdr();
        SyncBody body = msg.getSyncBody();

        if (hdr != null) {
            formatSyncHdr(hdr);
        }

        if (body != null) {
            formatSyncBody(body);
        }
        endTag(SyncML.TAG_SYNCML);

        formatter.endDocument();
    }

    public void formatXmlDevInf(DevInf devInf, OutputStream os, String charset) throws IOException {

        this.os = os;

        formatter = new KXmlSerializer();
        prettyPrint = true;

        formatter.setOutput(os, charset);
        formatDevInf(devInf);

        formatter.endDocument();
    }

    private void formatSyncHdr(SyncHdr header) throws IOException {
        startTag(SyncML.TAG_SYNCHDR);
        VerDTD verDTD = header.getVerDTD();
        if (verDTD != null) {
            formatSimpleTag(SyncML.TAG_VERDTD, verDTD.getValue());
        }
        String verProto = header.getVerProto();
        formatSimpleTag(SyncML.TAG_VERPROTO, verProto);

        String sessionId = header.getSessionID();
        formatSimpleTag(SyncML.TAG_SESSIONID, sessionId);

        String msgId = header.getMsgID();
        formatMsgId(msgId);

        Target target = header.getTarget();
        formatTarget(target);

        Source source = header.getSource();
        formatSource(source);

        String respURI = header.getRespURI();
        formatSimpleTag(SyncML.TAG_RESPURI, respURI);

        Boolean noResp = header.getNoResp();
        if (noResp != null && noResp.booleanValue() == true) {
            formatSimpleTag(SyncML.TAG_RESPURI, "");
        }

        Meta meta = header.getMeta();
        formatMeta(meta);

        Cred cred = header.getCred();
        formatCred(cred);

        endTag(SyncML.TAG_SYNCHDR);
    }



    private void formatCred(Cred cred) throws IOException {
        if (cred != null) {
            startTag(SyncML.TAG_CRED);
            Meta meta = cred.getMeta();
            formatMeta(meta);
            Data data = cred.getData();
            formatData(data);
            endTag(SyncML.TAG_CRED);
        }
    }


    private void formatSourceRef(SourceRef source) throws IOException {

        if (source != null) {
            formatSimpleTag(SyncML.TAG_SOURCEREF, source.getValue());
            Source s = source.getSource();
            formatSource(s);
        }
    }

    private void formatDSMem(DSMem dsMem) throws IOException {

        if (dsMem != null) {
            startTag(SyncML.TAG_DSMEM);
            formatSimpleLongTag(SyncML.TAG_MAXMEM, dsMem.getMaxMem());
            formatSimpleLongTag(SyncML.TAG_MAXID, dsMem.getMaxID());
            endTag(SyncML.TAG_DSMEM);
        }
    }

    private void formatTargetRef(TargetRef target) throws IOException {

        if (target != null) {
            formatSimpleTag(SyncML.TAG_TARGETREF, target.getValue());
            Target t = target.getTarget();
            formatTarget(t);
        }
    }

    private void formatSource(Source source) throws IOException {

        if (source != null) {
            String locURI = source.getLocURI();
            startTag(SyncML.TAG_SOURCE);
            formatSimpleTag(SyncML.TAG_LOC_URI, locURI);
            formatSimpleTag(SyncML.TAG_LOC_NAME, source.getLocName());
            endTag(SyncML.TAG_SOURCE);
        }
    }

    private void formatTarget(Target target) throws IOException {

        if (target != null) {
            String locURI = target.getLocURI();
            startTag(SyncML.TAG_TARGET);
            formatSimpleTag(SyncML.TAG_LOC_URI, locURI);
            formatSimpleTag(SyncML.TAG_LOC_NAME, target.getLocName());
            endTag(SyncML.TAG_TARGET);
        }
    }

    private void formatSourceParent(SourceParent sourceParent) throws IOException {

        if (sourceParent != null) {
            String locURI = sourceParent.getLocURI();
            startTag(SyncML.TAG_SOURCE_PARENT);
            formatSimpleTag(SyncML.TAG_LOC_URI, locURI);
            endTag(SyncML.TAG_SOURCE_PARENT);
        }
    }

    private void formatTargetParent(TargetParent targetParent) throws IOException {

        if (targetParent != null) {
            String locURI = targetParent.getLocURI();
            startTag(SyncML.TAG_TARGET_PARENT);
            formatSimpleTag(SyncML.TAG_LOC_URI, locURI);
            endTag(SyncML.TAG_TARGET_PARENT);
        }
    }


    private void formatSyncBody(SyncBody body) throws IOException {

        startTag(SyncML.TAG_SYNCBODY);

        Vector commands = body.getCommands();
        Boolean finalMsg = body.getFinalMsg();

        for(int i=0;i<commands.size();++i) {
            Object cmd = commands.elementAt(i);

            if (cmd instanceof Status) {
                Status status = (Status)cmd;
                formatStatus(status);
            } else if (cmd instanceof Sync) {
                Sync sync = (Sync)cmd;
                formatSync(sync);
            } else if (cmd instanceof Results) {
                Results results = (Results)cmd;
                formatResults(results);
            } else if (cmd instanceof Alert) {
                Alert alert = (Alert)cmd;
                formatAlert(alert);
            } else if (cmd instanceof Get) {
                Get get = (Get)cmd;
                formatGet(get);
            } else if (cmd instanceof Put) {
                Put put = (Put)cmd;
                formatPut(put);
            } else if (cmd instanceof Map) {
                Map map = (Map)cmd;
                formatMap(map);
            } else {
                Log.error(TAG_LOG, "Cannot format sync body command " + cmd);
            }
        }

        if (finalMsg != null && finalMsg.booleanValue()) {
            formatSimpleTag(SyncML.TAG_FINAL, "");
        }

        endTag(SyncML.TAG_SYNCBODY);
    }

    private void formatStatus(Status status) throws IOException {
        startTag(SyncML.TAG_STATUS);

        String cmdId = status.getCmdID();
        formatSimpleTag(SyncML.TAG_CMDID, cmdId);

        String msgRef = status.getMsgRef();
        formatSimpleTag(SyncML.TAG_MSGREF, msgRef);

        String cmdRef = status.getCmdRef();
        formatSimpleTag(SyncML.TAG_CMDREF, cmdRef);

        String cmd = status.getCmd();
        formatSimpleTag(SyncML.TAG_CMD, cmd);

        Vector targetRefs = status.getTargetRef();
        if (targetRefs != null) {
            for(int i=0;i<targetRefs.size();++i) {
                TargetRef tgtRef = (TargetRef)targetRefs.elementAt(i);
                formatTargetRef(tgtRef);
            }
        }

        Vector sourceRefs = status.getSourceRef();
        if (sourceRefs != null) {
            for(int i=0;i<sourceRefs.size();++i) {
                SourceRef srcRef = (SourceRef)sourceRefs.elementAt(i);
                formatSourceRef(srcRef);
            }
        }

        Cred cred = status.getCred();
        formatCred(cred);

        Chal chal = status.getChal();
        formatChal(chal);

        Data data = status.getData();
        formatData(data);

        Vector items = status.getItems();
        if (items != null) {
            for(int i=0;i<items.size();++i) {
                Item item = (Item)items.elementAt(i);
                formatItem(item);
            }
        }
        endTag(SyncML.TAG_STATUS);
    }

    private void formatChal(Chal chal) throws IOException {

    }

    private void formatNextNonce(NextNonce nextNonce) throws IOException {
        // TODO FIXME
    }

    private void formatResults(Results results) throws IOException {
        startTag(SyncML.TAG_RESULTS);
        formatSimpleTag(SyncML.TAG_CMDID, results.getCmdID());
        formatSimpleTag(SyncML.TAG_MSGREF, results.getMsgRef());
        formatSimpleTag(SyncML.TAG_CMDREF, results.getCmdRef());
        formatMeta(results.getMeta());
        Vector items = results.getItems();
        for(int i=0;i<items.size();++i) {
            Item item = (Item)items.elementAt(i);
            formatItem(item);
        }
        endTag(SyncML.TAG_RESULTS);
    }



    private void formatItem(Item item) throws IOException {

        startTag(SyncML.TAG_ITEM);

        Source source = item.getSource();
        formatSource(source);

        Target target = item.getTarget();
        formatTarget(target);

        SourceParent sourceParent = item.getSourceParent();
        formatSourceParent(sourceParent);

        TargetParent targetParent = item.getTargetParent();
        formatTargetParent(targetParent);

        Meta meta = item.getMeta();
        formatMeta(meta);

        Data data = item.getData();

        formatData(data);

        if (item.isMoreData()) {
            formatSimpleTag(SyncML.TAG_MORE_DATA, "");
        }

        endTag(SyncML.TAG_ITEM);
    }

    private void formatMap(Map map) throws IOException {
        if (map != null) {
            startTag(SyncML.TAG_MAP);
            String cmdId = map.getCmdID();
            formatSimpleTag(SyncML.TAG_CMDID, cmdId);
            Target tgt = map.getTarget();
            formatTarget(tgt);
            Source src = map.getSource();
            formatSource(src);
            Vector mapItems = map.getMapItems();
            for(int i=0;i<mapItems.size();++i) {
                MapItem mapItem = (MapItem)mapItems.elementAt(i);
                formatMapItem(mapItem);
            }
            endTag(SyncML.TAG_MAP);
        }
    }

    private void formatMapItem(MapItem mapItem) throws IOException {
        if (mapItem != null) {
            startTag(SyncML.TAG_MAPITEM);
            Source src = mapItem.getSource();
            formatSource(src);
            Target tgt = mapItem.getTarget();
            formatTarget(tgt);
            endTag(SyncML.TAG_MAPITEM);
        }
    }

    private void formatDevInf(DevInf devInf) throws IOException {

        if (devInf != null) {
            String man = devInf.getMan() != null ? devInf.getMan() : "";
            String mod = devInf.getMod() != null ? devInf.getMod() : "";
            String oem = devInf.getOEM() != null ? devInf.getOEM() : "";
            String fwv = devInf.getFwV() != null ? devInf.getFwV() : "";
            String swv = devInf.getSwV() != null ? devInf.getSwV() : "";
            String hwv = devInf.getHwV() != null ? devInf.getHwV() : "";
            String devID = devInf.getDevID() != null ? devInf.getDevID() : "";
            String devType = devInf.getDevTyp() != null ? devInf.getDevTyp() : "";
            
            startTagWithAttribute(SyncML.TAG_DEVINF, "xmlns", "syncml:devinf");
            formatSimpleTag(SyncML.TAG_VERDTD, "1.2");
            formatSimpleTag(SyncML.TAG_DEVINFMAN, man);
            formatSimpleTag(SyncML.TAG_DEVINFMOD, mod);
            formatSimpleTag(SyncML.TAG_DEVINFOEM, oem);
            formatSimpleTag(SyncML.TAG_DEVINFFWV, fwv);
            formatSimpleTag(SyncML.TAG_DEVINFSWV, swv);
            formatSimpleTag(SyncML.TAG_DEVINFHWV, hwv);
            formatSimpleTag(SyncML.TAG_DEVINFDEVID, devID);
            formatSimpleTag(SyncML.TAG_DEVINFDEVTYP, devType);

            
            //optional flag (if present, the server SHOULD send time in UTC form)
            if (devInf.isUTC()) {
                formatSimpleTag(SyncML.TAG_DEVINFUTC, "");
            }
            //optional (if present, it specifies that the device supports receiving
            //large objects)
            if (devInf.isSupportLargeObjs()) {
                formatSimpleTag(SyncML.TAG_DEVINFLO, "");
            }
            //optional: server MUST NOT send <NumberOfChanges> if the client has
            //not specified this flag
            if (devInf.isSupportNumberOfChanges()) {
                formatSimpleTag(SyncML.TAG_DEVINFNC, "");
            }

            /*
            if (devInf.isSupportHierarchicalSync()) {
                formatSimpleTag(SyncML.TAG_DEVINFHS, "");
            }
            */

            // format all exts
            Vector exts = devInf.getExts();
            if (exts != null) {
                for(int i=0;i<exts.size();++i) {
                    Ext ext = (Ext)exts.elementAt(i);
                    formatExt(ext);
                }
            }

            Vector dataStores = devInf.getDataStores();
            if (dataStores != null) {
                for(int i=0;i<dataStores.size();++i) {
                    DataStore ds = (DataStore)dataStores.elementAt(i);
                    formatDataStore(ds);
                }
            }
            endTag(SyncML.TAG_DEVINF);
        }
    }

    private void formatDataStore(DataStore ds) throws IOException {
        if (ds != null) {
            startTag(SyncML.TAG_DEVINFDATASTORE);
            SourceRef sr = ds.getSourceRef();
            formatSourceRef(sr);

            String displayName = ds.getDisplayName();
            if(displayName != null) {
                formatSimpleTag(SyncML.TAG_DISPLAYNAME, displayName);
            }

            long guidSize = ds.getMaxGUIDSize();
            if(guidSize > 0) {
                formatSimpleTag(SyncML.TAG_MAXGUIDSIZE, String.valueOf(guidSize));
            }

            CTInfo rxPref = ds.getRxPref();
            formatCTInfo(rxPref, SyncML.TAG_RXPREF);
            for (int i = 0; i < ds.getRxs().size(); i++) {
                formatCTInfo((CTInfo)ds.getRxs().elementAt(i), SyncML.TAG_RX);
            }

            CTInfo txPref = ds.getTxPref();
            formatCTInfo(txPref, SyncML.TAG_TXPREF);
            for (int i = 0; i < ds.getTxs().size(); i++) {
                formatCTInfo((CTInfo)ds.getTxs().elementAt(i), SyncML.TAG_TX);
            }

            DSMem dsMem = ds.getDSMem();
            if(dsMem != null) {
                formatDSMem(dsMem);
            }

            for (int i = 0; i < ds.getCTCaps().size(); i++) {
                formatCTCap((CTCap)ds.getCTCaps().elementAt(i));
            }

            SyncCap syncCap = ds.getSyncCap();
            formatSyncCap(syncCap);

            endTag(SyncML.TAG_DEVINFDATASTORE);
        }
    }

    private void formatCTInfo(CTInfo ctInfo, String sorround) throws IOException {
        if (ctInfo != null) {
            startTag(sorround);
            formatCTInfo(ctInfo);
            endTag(sorround);
        }
    }

    private void formatCTInfo(CTInfo ctInfo) throws IOException {
        if (ctInfo != null) {
            String type = ctInfo.getCTType();
            formatSimpleTag(SyncML.TAG_CTTYPE, type);
            String ver = ctInfo.getVerCT();
            formatSimpleTag(SyncML.TAG_VERCT, ver);

        }
    }

    private void formatSyncCap(SyncCap syncCap) throws IOException {
        if (syncCap != null) {
            startTag(SyncML.TAG_SYNCCAP);
            Vector types = syncCap.getSyncType();
            for(int i=0;i<types.size();++i) {
                SyncType type = (SyncType)types.elementAt(i);
                int t = type.getType();
                formatSimpleTag(SyncML.TAG_SYNCTYPE, "" + t);
            }
            endTag(SyncML.TAG_SYNCCAP);
        }
    }

    private void formatCTCap(CTCap ctCap) throws IOException {
        if (ctCap != null) {
            startTag(SyncML.TAG_CTCAP);
            formatCTInfo(ctCap.getCTInfo());
            for (int i = 0; i < ctCap.getProperties().size(); i++) {
                formatProperty((Property)ctCap.getProperties().elementAt(i));
            }
            endTag(SyncML.TAG_CTCAP);
        }
    }

    private void formatPropParam(PropParam param) throws IOException {
        startTag(SyncML.TAG_PROPPARAM);
        formatSimpleTag(SyncML.TAG_PARAMNAME, param.getParamName());
        formatSimpleTag(SyncML.TAG_DATATYPE, param.getDataType());
        for (int i = 0; i < param.getValEnums().size(); i++) {
            formatSimpleTag(SyncML.TAG_VALENUM, (String)param.getValEnums().elementAt(i));
        } 
        formatSimpleTag(SyncML.TAG_DISPLAYNAME, param.getDisplayName());
        endTag(SyncML.TAG_PROPPARAM);
     }
 
     private void formatProperty(Property property) throws IOException {
         startTag(SyncML.TAG_PROPERTY);
        
         formatSimpleTag(SyncML.TAG_PROPNAME, property.getPropName());
         formatSimpleTag(SyncML.TAG_DATATYPE, property.getDataType());
         formatSimpleLongTag(SyncML.TAG_MAXOCCUR, property.getMaxOccur());
         formatSimpleLongTag(SyncML.TAG_MAXSIZE,  property.getMaxSize());
         formatSimpleTag(SyncML.TAG_DATATYPE, property.getDataType());
         for (int i = 0; i < property.getValEnums().size(); i++) {
             formatSimpleTag(SyncML.TAG_VALENUM, (String)property.getValEnums().elementAt(i));
         }
         formatSimpleTag(SyncML.TAG_DISPLAYNAME, property.getDisplayName());
         for (int i = 0; i < property.getPropParams().size(); i++) {
             formatPropParam((PropParam)property.getPropParams().elementAt(i));
         }
         endTag(SyncML.TAG_PROPERTY);
     }
 
     private void formatData(Data data) throws IOException {
        if (data != null) {
            // We expect only of these four possibilities to have a valid value
            String str = data.getData();
            Anchor anchor = data.getAnchor();
            DevInf devInf = data.getDevInf();
            byte   binData[] = data.getBinData();
            if (str != null) {
                formatSimpleTag(SyncML.TAG_DATA, str);
            } else if (anchor != null) {
                startTag(SyncML.TAG_DATA);
                formatAnchor(anchor);
                endTag(SyncML.TAG_DATA);
            } else if (devInf != null) {
                startTag(SyncML.TAG_DATA);
                formatDevInf(devInf);
                endTag(SyncML.TAG_DATA);
            } else if (binData != null) {
                startTag(SyncML.TAG_DATA);
                formatBinData(binData);
                endTag(SyncML.TAG_DATA);
            }
        }
    }

    private void formatMeta(Meta meta) throws IOException {

        // We only print the field supported by the parser
        if (meta != null) {
            startTag(SyncML.TAG_META);

            Long size = meta.getSize();
            if (size != null) {
                formatSimpleTag(SyncML.TAG_SIZE, "" + size.longValue());
            }

            String format = meta.getFormat();
            String type = meta.getType();

            formatSimpleTagWithNamespace(SyncML.TAG_FORMAT, format, METINF);
            formatSimpleTagWithNamespace(SyncML.TAG_TYPE, type, METINF);

            Anchor anchor = meta.getAnchor();
            formatAnchor(anchor);

            NextNonce nextNonce = meta.getNextNonce();
            formatNextNonce(nextNonce);

            Long maxMsgSize = meta.getMaxMsgSize();
            if (maxMsgSize != null) {
                formatSimpleTag(SyncML.TAG_MAXMSGSIZE, "" + maxMsgSize.longValue());
            }

            Long maxObjSize = meta.getMaxObjSize();
            if (maxObjSize != null) {
                formatSimpleTag(SyncML.TAG_MAXOBJSIZE, "" + maxObjSize.longValue());
            }

            String version = meta.getVersion();
            if (version != null) {
                formatSimpleTag(SyncML.TAG_VERSION, version);
            }

            endTag(SyncML.TAG_META);
        }
    }

    private void formatAnchor(Anchor anchor) throws IOException {
        if (anchor != null) {
            String next = anchor.getNext();
            String last = anchor.getLast();
            startTagWithAttribute(SyncML.TAG_ANCHOR, "xmlns", METINF);
            formatSimpleTag(SyncML.TAG_NEXT, next);
            formatSimpleTag(SyncML.TAG_LAST, last);
            endTag(SyncML.TAG_ANCHOR);
        }
    }

    private void formatBinData(byte binData[]) throws IOException {
        if (binData != null) {
            if (formatter instanceof WbxmlSerializer) {
                if (Log.isLoggable(Log.TRACE)) {
                    Log.trace(TAG_LOG, "Formatting binary data: " + binData.length);
                }
                WbxmlSerializer wbxmlFormatter = (WbxmlSerializer)formatter;
                wbxmlFormatter.writeWapExtension(Wbxml.OPAQUE, binData);
            } else {
                if (prettyPrint) {
                    String textData = null;
                    if (Log.getLogLevel() >= Log.DEBUG) {
                        textData = new String(binData);
                        // Check if this string can be printed
                        if (!isPrintableString(textData)) {
                            textData = null;
                        }
                    }
                    if (textData == null) {
                        formatter.text("******* binary data ********");
                    } else {
                        formatter.text(textData);
                    }
                } else {
                    throw new IOException("Cannot write binary data to XML");
                }
            }
        }
    }

    private void formatSync(Sync sync) throws IOException {
        if (sync != null) {
            startTag(SyncML.TAG_SYNC);
            String cmdId = sync.getCmdID();
            formatSimpleTag(SyncML.TAG_CMDID, cmdId);

            boolean noResp = hasNoResp(sync.getNoResp());
            if (noResp) {
                formatSimpleTag(SyncML.TAG_NORESP, "");
            }

            Cred cred = sync.getCred();
            formatCred(cred);

            Target target = sync.getTarget();
            formatTarget(target);

            Source source = sync.getSource();
            formatSource(source);

            Meta meta = sync.getMeta();
            formatMeta(meta);

            Long numChanges = sync.getNumberOfChanges();
            if (numChanges != null) {
                formatSimpleTag(SyncML.TAG_NUMBEROFCHANGES, ""+numChanges.longValue());
            }

            Vector commands = sync.getCommands();
            for(int i=0;i<commands.size();++i) {
                SyncMLCommand command = (SyncMLCommand) commands.elementAt(i);
                formatCommand(command);
            }
            endTag(SyncML.TAG_SYNC);
        }
    }

    private void formatCommand(SyncMLCommand command) throws IOException {

        if (command != null) {
            startTag(command.getName());
            String cmdId = command.getCmdId();
            formatSimpleTag(SyncML.TAG_CMDID, cmdId);
            Meta meta = command.getMeta();
            formatMeta(meta);
            Vector items = command.getItems();
            for(int i=0;i<items.size();++i) {
                Item item = (Item)items.elementAt(i);
                formatItem(item);
            }
            endTag(command.getName());
        }
    }

    private void formatPut(Put put) throws IOException {
        if (put != null) {
            startTag(SyncML.TAG_PUT);

            formatItemizedCommand(put);

            String lang = put.getLang();
            formatSimpleTag(SyncML.TAG_LANG, lang);

            endTag(SyncML.TAG_PUT);
        }
    }

    private void formatGet(Get get) throws IOException {
        if (get != null) {
            startTag(SyncML.TAG_GET);

            formatItemizedCommand(get);

            String lang = get.getLang();
            formatSimpleTag(SyncML.TAG_LANG, lang);

            endTag(SyncML.TAG_GET);
        }
    }


    private void formatItemizedCommand(ItemizedCommand icommand) throws IOException {

        if (icommand != null) {
            String cmdId = icommand.getCmdID();
            formatSimpleTag(SyncML.TAG_CMDID, cmdId);

            boolean noResp = hasNoResp(icommand.getNoResp());
            if (noResp) {
                formatSimpleTag(SyncML.TAG_NORESP, "");
            }

            Cred cred = icommand.getCred();
            formatCred(cred);

            Meta meta = icommand.getMeta();
            formatMeta(meta);

            Vector items = icommand.getItems();
            for(int i=0;i<items.size();++i) {
                Item item = (Item)items.elementAt(i);
                formatItem(item);
            }
        }
    }

    private void formatAlert(Alert alert) throws IOException {
        if (alert != null) {
            startTag(SyncML.TAG_ALERT);
            formatItemizedCommand(alert);
            formatSimpleTag(SyncML.TAG_DATA, "" + alert.getData());
            endTag(SyncML.TAG_ALERT);
        }
    }

    private void formatExt(Ext ext) throws IOException {
        if (ext != null) {
            formatter.startTag(null, SyncML.TAG_EXT);

            String xnam = ext.getXNam();
            Vector xval = ext.getXVal();

            if (xnam != null) {
                formatSimpleTag(SyncML.TAG_XNAM, xnam);
            }

            if (xval != null) {
                for(int i=0;i<xval.size();++i) {
                    String v = (String)xval.elementAt(i);
                    formatSimpleTag(SyncML.TAG_XVAL, v);
                }
            }

            formatter.endTag(null, SyncML.TAG_EXT);
        }
    }

    private void formatSimpleTag(String tagName, String value) throws IOException {
        formatSimpleTagWithNamespace(tagName, value, null);
    }

    private void formatSimpleLongTag(String tagName, long value) throws IOException {
        if(value > 0) {
            formatSimpleTagWithNamespace(tagName, Long.toString(value), null);
        }
    }

    // Unfortunately some servers exhibit bugs at parsing CDATA in
    // some fields (e.g. MsgID), so in some cases we do the escaping
    private void formatMsgId(String msgId) throws IOException {
        if (msgId != null) {
            formatter.startTag(null, SyncML.TAG_MSGID);
            formatter.text(msgId);
            formatter.endTag(null, SyncML.TAG_MSGID);
            if (prettyPrint) {
                println();
            }
        }
    }

    private void formatSimpleTagWithNamespace(String tagName, String value,
                                              String namespace)
    throws IOException {

        if (value != null) {

            formatter.startTag(null, tagName);

            if (namespace != null) {
                formatter.attribute(null, "xmlns", namespace);
            }

            // For performance reason we always use CDATA because it does
            // not require escaping. kxml2 is really ineffecient at escaping
            // text, so we try to avoid it as much as possible.
            if (value.length() == 0) {
                formatter.text("");
            } else {
                formatter.text(value);
                //formatter.cdsect(value);
            }
            formatter.endTag(null, tagName);
            if (prettyPrint) {
                println();
            }
        }
    }

    private void println() throws IOException {
        formatter.text("\n");
    }

    private void startTag(String tagName) throws IOException {
        formatter.startTag(null, tagName);
        if (prettyPrint) {
            println();
        }
    }

    private void endTag(String tagName) throws IOException {
        formatter.endTag(null, tagName);
        if (prettyPrint) {
            println();
        }
    }

    private void startTagWithAttribute(String tagName, String attrName, String attrValue) throws IOException {
        formatter.startTag(null, tagName);
        formatter.attribute(null, attrName, attrValue);
        if (prettyPrint) {
            println();
        }
    }

    private boolean hasNoResp(Boolean nr) {
        if (nr != null && nr.booleanValue()) {
            return true;
        } else {
            return false;
        }
    }

    private boolean isPrintableString(String str) {
        return true;
    }

}

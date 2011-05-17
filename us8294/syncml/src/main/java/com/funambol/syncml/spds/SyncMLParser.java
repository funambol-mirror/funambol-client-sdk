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

import java.util.Vector;
import java.util.Date;
import java.util.Calendar;
import java.io.IOException;
import java.io.InputStream;
import java.io.ByteArrayInputStream;

import org.xmlpull.v1.XmlPullParserException;
import org.xmlpull.v1.XmlPullParser;
//import org.xmlpull.v1.XmlPullParserFactory;
import com.funambol.org.kxml2.io.KXmlParser;
import com.funambol.org.kxml2.wap.Wbxml;
import com.funambol.org.kxml2.wap.WbxmlParser;

import com.funambol.syncml.protocol.*;

import com.funambol.util.XmlUtil;
import com.funambol.util.Log;

/**
 * This class is meant to provide a SyncML parser. Such a parser reads a SyncML
 * message and builds a representation of this message based on the objects
 * provided by the com.funambol.syncml.protocol objects.
 * The implementation is based on KXml and relies on its XmlPull interface. As
 * such it is capable of parsing both XML and WBXML.
 * At the moment the implementation is restricted to some components of the
 * SyncML message. In particular the DevInf section.
 * This parser performs a relaxed parsing, allowing unknown tokens to be parsed.
 * These tokens are simply skipped and they are not reflected into the SyncML
 * representation.
 */
public class SyncMLParser {

    private static final String TAG_LOG = "SyncMLParser";

    private boolean wbxml = true;

    // This flag turns on super verbosity mode to trace parser execution
    // This is a development only feature. Leave it false on standard build.
    private boolean verbose = false;

    public SyncMLParser(boolean wbxml) {
        this.wbxml = wbxml;
    }

    public SyncML parse(byte message[]) throws SyncMLParserException {
/*
        // KXml minimal version does not have the factory parser,
        // therefore we need direct instantiation
        XmlPullParserFactory factory = XmlPullParserFactory.newInstance();
        factory.setNamespaceAware(true);
        XmlPullParser parser = factory.newPullParser();
*/

        XmlPullParser parser;
        if (wbxml) {
            parser = com.funambol.org.kxml2.wap.syncml.SyncML.createParser();
        } else {
            parser = new KXmlParser();
            try {
                parser.setFeature("NORMALIZE", false);
            } catch (Exception e) {
                Log.error(TAG_LOG, "Parser does not support unnormalized mode");
            }
        }

        if (Log.isLoggable(Log.TRACE)) {
            Log.trace(TAG_LOG, "parse");
        }

        SyncHdr header = null;
        SyncBody body  = null;
        try {
            ByteArrayInputStream is = new ByteArrayInputStream(message);
            parser.setInput(is, "UTF-8");

            // Begin parsing
            nextSkipSpaces(parser);
            // If the first tag is not the SyncML start tag, then this is an
            // invalid message
            require(parser, parser.START_TAG, null, SyncML.TAG_SYNCML);
            nextSkipSpaces(parser);

            while (parser.getEventType() == parser.START_TAG) {
                String tagName = parser.getName();
                if (SyncML.TAG_SYNCHDR.equals(tagName)) {
                    header = parseHeader(parser);
                } else if (SyncML.TAG_SYNCBODY.equals(tagName)) {
                    body = parseSyncBody(parser);
                } else {
                    String msg = "Error parsing. Skipping unexpected token: " + tagName;
                    Log.error(TAG_LOG, msg);
                    skipUnknownToken(parser, tagName);
                }
                nextSkipSpaces(parser);
            }
            require(parser, parser.END_TAG, null, SyncML.TAG_SYNCML);
        } catch (Exception e) {
            Log.error(TAG_LOG, "Error parsing command", e);
            throw new SyncMLParserException("Cannot parse command: " + e.toString());
        }
        SyncML syncML = new SyncML();
        if (header != null) {
            syncML.setSyncHdr(header);
        }
        if (body != null) {
            syncML.setSyncBody(body);
        }
        return syncML;
    }

    public DevInf parseXMLDevInf(String message) throws SyncMLParserException {
        XmlPullParser parser = new KXmlParser();
        try {
            parser.setFeature("NORMALIZE", false);
        } catch (Exception e) {
            Log.error(TAG_LOG, "Parser does not support unnormalized mode");
        }

        try {
            ByteArrayInputStream is = new ByteArrayInputStream(message.getBytes("UTF-8"));
            parser.setInput(is, "UTF-8");

            // Begin parsing
            nextSkipSpaces(parser);
            // If the first tag is not the SyncML start tag, then this is an
            // invalid message
            require(parser, parser.START_TAG, null, SyncML.TAG_DEVINF);
            return parseDevInf(parser);
        } catch (Exception e) {
            Log.error(TAG_LOG, "Cannot parse dev inf", e);
            throw new SyncMLParserException("Cannot parse dev inf");
        }
    }

    private void parseFinal(XmlPullParser parser) throws  XmlPullParserException,
                                                          IOException,
                                                          SyncMLParserException
    {
        nextSkipSpaces(parser);
        require(parser, parser.END_TAG, null, SyncML.TAG_FINAL);
    }

    private void parseNoResp(XmlPullParser parser) throws  XmlPullParserException,
                                                           IOException,
                                                           SyncMLParserException
    {
        nextSkipSpaces(parser);
        require(parser, parser.END_TAG, null, SyncML.TAG_NORESP);
    }



    private SyncHdr parseHeader(XmlPullParser parser) throws XmlPullParserException,
                                                             IOException,
                                                             SyncMLParserException
    {
        verboseLog("parseHeader");

        VerDTD verDTD    = null;
        String verProto  = null;
        String sessionID = null;
        String msgId     = null;
        Source source    = null;
        Target target    = null;
        Meta   meta      = null;
        String respUri   = null;
        Cred   cred      = null;
        boolean noResp   = false;

        nextSkipSpaces(parser);
        while (parser.getEventType() == parser.START_TAG) {
            String tagName = parser.getName();
            if (SyncML.TAG_VERDTD.equals(tagName)) {
                String verDTDvalue = parseSimpleStringTag(parser, SyncML.TAG_VERDTD);
                verDTD = new VerDTD(verDTDvalue);
            } else if (SyncML.TAG_VERPROTO.equals(tagName)) {
                verProto = parseSimpleStringTag(parser, SyncML.TAG_VERPROTO);
            } else if (SyncML.TAG_SESSIONID.equals(tagName)) {
                sessionID = parseSimpleStringTag(parser, SyncML.TAG_SESSIONID);
            } else if (SyncML.TAG_MSGID.equals(tagName)) {
                msgId = parseSimpleStringTag(parser, SyncML.TAG_MSGID);
            } else if (SyncML.TAG_SOURCE.equals(tagName)) {
                source = parseSource(parser);
            } else if (SyncML.TAG_TARGET.equals(tagName)) {
                target = parseTarget(parser);
            } else if (SyncML.TAG_META.equals(tagName)) {
                meta = parseMeta(parser);
            } else if (SyncML.TAG_RESPURI.equals(tagName)) {
                respUri = parseSimpleStringTag(parser, SyncML.TAG_RESPURI);
            } else if (SyncML.TAG_CRED.equals(tagName)) {
                cred = parseCred(parser);
            } else if (SyncML.TAG_NORESP.equals(tagName)) {
                parseNoResp(parser);
                noResp = true;
            } else {
                String msg = "Error parsing header tag. Skipping unexpected token: " + tagName;
                Log.error(TAG_LOG, msg);
                skipUnknownToken(parser, tagName);
            }
            nextSkipSpaces(parser);
        }
        require(parser, parser.END_TAG, null, SyncML.TAG_SYNCHDR);

        SyncHdr hdr = new SyncHdr();
        if (verDTD != null) {
            hdr.setVerDTD(verDTD);
        }
        if (verProto != null) {
            hdr.setVerProto(verProto);
        }
        if (sessionID != null) {
            hdr.setSessionID(sessionID);
        }
        if (msgId != null) {
            hdr.setMsgID(msgId);
        }
        if (source != null) {
            hdr.setSource(source);
        }
        if (target != null) {
            hdr.setTarget(target);
        }
        if (respUri != null) {
            hdr.setRespURI(respUri);
        }
        if (cred != null) {
            hdr.setCred(cred);
        }
        if (noResp) {
            hdr.setNoResp(new Boolean(true));
        }
        if (meta != null) {
            hdr.setMeta(meta);
        }
        return hdr;
    }

    private SyncBody parseSyncBody(XmlPullParser parser) throws XmlPullParserException,
                                                                IOException,
                                                                SyncMLParserException
    {
        verboseLog("parseSyncBody");

        Vector commands = new Vector();
        boolean lastMsg = false;

        nextSkipSpaces(parser);
        while (parser.getEventType() == parser.START_TAG) {
            String tagName = parser.getName();
            if (SyncML.TAG_STATUS.equals(tagName)) {
                Status status = parseStatus(parser);
                commands.addElement(status);
            } else if (SyncML.TAG_SYNC.equals(tagName)) {
                Sync sync = parseSync(parser);
                commands.addElement(sync);
            } else if (SyncML.TAG_RESULTS.equals(tagName)) {
                Results results = parseResults(parser);
                commands.addElement(results);
            } else if (SyncML.TAG_ALERT.equals(tagName)) {
                Alert alert = parseAlert(parser);
                commands.addElement(alert);
            } else if (SyncML.TAG_GET.equals(tagName)) {
                Get get = parseGet(parser);
                commands.addElement(get);
            } else if (SyncML.TAG_PUT.equals(tagName)) {
                Put put = parsePut(parser);
                commands.addElement(put);
            } else if (SyncML.TAG_FINAL.equals(tagName)) {
                parseFinal(parser);
                lastMsg = true;
            } else if (SyncML.TAG_MAP.equals(tagName)) {
                Map map = parseMap(parser);
                commands.addElement(map);
            } else {
                String msg = "Error parsing sync item tag. Skipping unexpected token: " + tagName;
                Log.error(TAG_LOG, msg);
                skipUnknownToken(parser, tagName);
            }
            nextSkipSpaces(parser);
        }
        require(parser, parser.END_TAG, null, SyncML.TAG_SYNCBODY);

        SyncBody syncBody = new SyncBody();
        syncBody.setCommands(commands);
        syncBody.setFinalMsg(new Boolean(lastMsg));
        return syncBody;
    }

    private Status parseStatus(XmlPullParser parser) throws XmlPullParserException,
                                                            IOException,
                                                            SyncMLParserException
    {
        verboseLog("parseStatus");

        String cmdId      = null;
        String msgRef     = null;
        String cmdRef     = null;
        String cmd        = null;
        Data   data       = null;
        Vector targetRefs = new Vector();
        Vector sourceRefs = new Vector();
        Vector items      = new Vector();
        Chal   chal       = null;

        nextSkipSpaces(parser);
        while (parser.getEventType() == parser.START_TAG) {
            String tagName = parser.getName();
            if (SyncML.TAG_CMDID.equals(tagName)) {
                cmdId = parseSimpleStringTag(parser, SyncML.TAG_CMDID); 
            } else if (SyncML.TAG_MSGREF.equals(tagName)) {
                msgRef = parseSimpleStringTag(parser, SyncML.TAG_MSGREF);
            } else if (SyncML.TAG_TARGETREF.equals(tagName)) {
                TargetRef targetRef = parseTargetRef(parser);
                targetRefs.addElement(targetRef);
            } else if (SyncML.TAG_SOURCEREF.equals(tagName)) {
                SourceRef sourceRef = parseSourceRef(parser);
                sourceRefs.addElement(sourceRef);
            } else if (SyncML.TAG_ITEM.equals(tagName)) {
                Item item = parseSyncItem(parser);
                items.addElement(item);
            } else if (SyncML.TAG_CMDREF.equals(tagName)) {
                cmdRef = parseSimpleStringTag(parser, SyncML.TAG_CMDREF);
            } else if (SyncML.TAG_CMD.equals(tagName)) {
                cmd = parseSimpleStringTag(parser, SyncML.TAG_CMD);
            } else if (SyncML.TAG_DATA.equals(tagName)) {
                String dataVal = parseSimpleStringTag(parser, SyncML.TAG_DATA);
                data = Data.newInstance(dataVal);
            } else if (SyncML.TAG_CHAL.equals(tagName)) {
                chal = parseChal(parser);
            } else {
                String msg = "Error parsing sync item tag. Skipping unexpected token: " + tagName;
                Log.error(TAG_LOG, msg);
                skipUnknownToken(parser, tagName);
            }
            nextSkipSpaces(parser);
        }
        require(parser, parser.END_TAG, null, SyncML.TAG_STATUS);

        Status status = Status.newInstance();
        if (cmdId != null) {
            status.setCmdID(cmdId);
        }
        if (msgRef != null) {
            status.setMsgRef(msgRef);
        }
        if (sourceRefs.size() > 0) {
            status.setSourceRef(sourceRefs);
        }
        if (targetRefs.size() > 0) {
            status.setTargetRef(targetRefs);
        }
        if (items.size() > 0) {
            status.setItems(items);
        }
        if (cmdRef != null) {
            status.setCmdRef(cmdRef);
        }
        if (cmd != null) {
            status.setCmd(cmd);
        }
        if (data != null) {
            status.setData(data);
        }
        if (chal != null) {
            status.setChal(chal);
        }
        return status;
    }

    private Map parseMap(XmlPullParser parser) throws XmlPullParserException,
                                                      IOException,
                                                      SyncMLParserException
    {
        verboseLog("parseMap");

        Source source = null;
        Target target = null;
        String cmdId  = null;

        Vector mappings = new Vector();

        nextSkipSpaces(parser);
        while (parser.getEventType() == parser.START_TAG) {
            String tagName = parser.getName();
            if (SyncML.TAG_SOURCE.equals(tagName)) {
                source = parseSource(parser);
            } else if (SyncML.TAG_CMDID.equals(tagName)) {
                cmdId = parseSimpleStringTag(parser, SyncML.TAG_CMDID);
            } else if (SyncML.TAG_TARGET.equals(tagName)) {
                target = parseTarget(parser);
            } else if (SyncML.TAG_MAPITEM.equals(tagName)) {
                MapItem mapItem = parseMapItem(parser);
                mappings.addElement(mapItem);
            } else {
                String msg = "Error parsing chal element. Skipping unexpected token: " + tagName;
                Log.error(TAG_LOG, msg);
                skipUnknownToken(parser, tagName);
            }
            nextSkipSpaces(parser);
        }

        Map map = new Map();
        if (cmdId != null) {
            map.setCmdID(cmdId);
        }
        if (source != null) {
            map.setSource(source);
        }
        if (target != null) {
            map.setTarget(target);
        }
        if (mappings.size() > 0) {
            map.setMapItems(mappings);
        }
        return map;
    }

    private MapItem parseMapItem(XmlPullParser parser) throws XmlPullParserException,
                                                              IOException,
                                                              SyncMLParserException
    {
        verboseLog("parseMapItem");

        Source source = null;
        Target target = null;

        nextSkipSpaces(parser);
        while (parser.getEventType() == parser.START_TAG) {
            String tagName = parser.getName();
            if (SyncML.TAG_SOURCE.equals(tagName)) {
                source = parseSource(parser);
            } else if (SyncML.TAG_TARGET.equals(tagName)) {
                target = parseTarget(parser);
            } else {
                String msg = "Error parsing chal element. Skipping unexpected token: " + tagName;
                Log.error(TAG_LOG, msg);
                skipUnknownToken(parser, tagName);
            }
            nextSkipSpaces(parser);
        }
        MapItem mapItem = new MapItem();
        if (source != null) {
            mapItem.setSource(source);
        }
        if (target != null) {
            mapItem.setTarget(target);
        }
        return mapItem;
    }



    private Chal parseChal(XmlPullParser parser) throws XmlPullParserException,
                                                        IOException,
                                                        SyncMLParserException
    {
        verboseLog("parseChal");

        Meta meta = null;

        nextSkipSpaces(parser);
        while (parser.getEventType() == parser.START_TAG) {
            String tagName = parser.getName();
            if (SyncML.TAG_META.equals(tagName)) {
                meta = parseMeta(parser);
            } else if (SyncML.TAG_CMDID.equals(tagName)) {
                String cmdId = parseSimpleStringTag(parser, SyncML.TAG_CMDID);
            } else {
                String msg = "Error parsing chal element. Skipping unexpected token: " + tagName;
                Log.error(TAG_LOG, msg);
                skipUnknownToken(parser, tagName);
            }
            nextSkipSpaces(parser);
        }

        Chal chal = new Chal();
        if (meta != null) {
            chal.setMeta(meta);
        }
        return chal;
    }

    private Cred parseCred(XmlPullParser parser) throws XmlPullParserException,
                                                        IOException,
                                                        SyncMLParserException
    {
        verboseLog("parseCred");

        Meta meta = null;
        Data data = null;

        nextSkipSpaces(parser);
        while (parser.getEventType() == parser.START_TAG) {
            String tagName = parser.getName();
            if (SyncML.TAG_META.equals(tagName)) {
                meta = parseMeta(parser);
            } else if (SyncML.TAG_DATA.equals(tagName)) {
                String dataVal = parseSimpleStringTag(parser, SyncML.TAG_DATA);
                data = Data.newInstance(dataVal);
            } else {
                String msg = "Error parsing chal element. Skipping unexpected token: " + tagName;
                Log.error(TAG_LOG, msg);
                skipUnknownToken(parser, tagName);
            }
            nextSkipSpaces(parser);
        }

        Cred cred = new Cred();
        if (meta != null) {
            cred.setMeta(meta);
        }
        if (data != null) {
            cred.setData(data);
        }
        return cred;
    }


    private Get parseGet(XmlPullParser parser) throws XmlPullParserException,
                                                      IOException,
                                                      SyncMLParserException
    {
        verboseLog("parseGet");

        Meta    meta   = null;
        Cred    cred   = null;
        String  lang   = null;
        boolean noResp = false;
        String  cmdId  = null;
        Vector  items  = new Vector();

        nextSkipSpaces(parser);
        while (parser.getEventType() == parser.START_TAG) {
            String tagName = parser.getName();
            if (SyncML.TAG_META.equals(tagName)) {
                meta = parseMeta(parser);
            } else if (SyncML.TAG_CRED.equals(tagName)) {
                cred = parseCred(parser);
            } else if (SyncML.TAG_LANG.equals(tagName)) {
                lang = parseSimpleStringTag(parser, SyncML.TAG_LANG);
            } else if (SyncML.TAG_NORESP.equals(tagName)) {
                parseNoResp(parser);
                noResp = true;
            } else if (SyncML.TAG_CMDID.equals(tagName)) {
                cmdId = parseSimpleStringTag(parser, SyncML.TAG_CMDID);
            } else if (SyncML.TAG_ITEM.equals(tagName)) {
                Item item = parseSyncItem(parser);
                items.addElement(item);
            } else {
                String msg = "Error parsing get element. Skipping unexpected token: " + tagName;
                Log.error(TAG_LOG, msg);
                skipUnknownToken(parser, tagName);
            }
            nextSkipSpaces(parser);
        }

        Get get = new Get();
        if (meta != null) {
            get.setMeta(meta);
        }
        if (cred != null) {
            get.setCred(cred);
        }
        if (lang != null) {
            get.setLang(lang);
        }
        if (cmdId != null) {
            get.setCmdID(cmdId);
        }
        get.setNoResp(new Boolean(noResp));
        if (items.size() > 0) {
            get.setItems(items);
        }
        return get;
    }

    private Put parsePut(XmlPullParser parser) throws XmlPullParserException,
                                                      IOException,
                                                      SyncMLParserException
    {
        verboseLog("parsePut");

        Meta    meta   = null;
        Cred    cred   = null;
        String  lang   = null;
        boolean noResp = false;
        String  cmdId  = null;
        Vector  items  = new Vector();

        nextSkipSpaces(parser);
        while (parser.getEventType() == parser.START_TAG) {
            String tagName = parser.getName();
            if (SyncML.TAG_META.equals(tagName)) {
                meta = parseMeta(parser);
            } else if (SyncML.TAG_CRED.equals(tagName)) {
                cred = parseCred(parser);
            } else if (SyncML.TAG_LANG.equals(tagName)) {
                lang = parseSimpleStringTag(parser, SyncML.TAG_LANG);
            } else if (SyncML.TAG_NORESP.equals(tagName)) {
                parseNoResp(parser);
                noResp = true;
            } else if (SyncML.TAG_CMDID.equals(tagName)) {
                cmdId = parseSimpleStringTag(parser, SyncML.TAG_CMDID);
            } else if (SyncML.TAG_ITEM.equals(tagName)) {
                Item item = parseSyncItem(parser);
                items.addElement(item);
            } else {
                String msg = "Error parsing get element. Skipping unexpected token: " + tagName;
                Log.error(TAG_LOG, msg);
                skipUnknownToken(parser, tagName);
            }
            nextSkipSpaces(parser);
        }

        Put put = new Put();
        if (meta != null) {
            put.setMeta(meta);
        }
        if (cred != null) {
            put.setCred(cred);
        }
        if (lang != null) {
            put.setLang(lang);
        }
        if (cmdId != null) {
            put.setCmdID(cmdId);
        }
        if (items.size() > 0) {
            put.setItems(items);
        }
        put.setNoResp(new Boolean(noResp));
        return put;
    }

    private Alert parseAlert(XmlPullParser parser) throws XmlPullParserException,
                                                          IOException,
                                                          SyncMLParserException
    {
        verboseLog("parseAlert");

        String cmdId      = null;
        int    alertCode  = 0;
        Vector items      = new Vector();

        nextSkipSpaces(parser);
        while (parser.getEventType() == parser.START_TAG) {
            String tagName = parser.getName();
            if (SyncML.TAG_CMDID.equals(tagName)) {
                cmdId = parseSimpleStringTag(parser, SyncML.TAG_CMDID); 
            } else if (SyncML.TAG_DATA.equals(tagName)) {
                Data data = parseItemData(parser);
                String dataVal = null;

                if (data.getData() != null) {
                    dataVal = data.getData();
                } else if (data.getBinData() != null) {
                    // This is not a real binary data
                    dataVal = new String(data.getBinData());
                }

                try {
                    alertCode = Integer.parseInt(dataVal);
                } catch (Exception e) {
                    throw new SyncMLParserException("Invalid alert code: " + dataVal);
                }
            } else if (SyncML.TAG_ITEM.equals(tagName)) {
                Item item = parseAlertItem(parser);
                items.addElement(item);
            } else {
                String msg = "Error parsing sync item tag. Skipping unexpected token: " + tagName;
                Log.error(TAG_LOG, msg);
                skipUnknownToken(parser, tagName);
            }
            nextSkipSpaces(parser);
        }
        require(parser, parser.END_TAG, null, SyncML.TAG_ALERT);

        Alert alert = new Alert();
        if (cmdId != null) {
            alert.setCmdID(cmdId);
        }
        alert.setData(alertCode);
        if (items.size() > 0) {
            alert.setItems(items);
        }
        return alert;
    }

    private Item parseAlertItem(XmlPullParser parser) throws XmlPullParserException,
                                                             IOException,
                                                             SyncMLParserException
    {
        verboseLog("parseAlertItem");

        Target target = null;
        Source source = null;
        Meta   meta   = null;

        nextSkipSpaces(parser);
        while (parser.getEventType() == parser.START_TAG) {
            String tagName = parser.getName();
            if (SyncML.TAG_TARGET.equals(tagName)) {
                target = parseTarget(parser);
            } else if (SyncML.TAG_SOURCE.equals(tagName)) {
                source = parseSource(parser);
            } else if (SyncML.TAG_META.equals(tagName)) {
                meta = parseMeta(parser);
            } else {
                String msg = "Error parsing sync item tag. Skipping unexpected token: " + tagName;
                Log.error(TAG_LOG, msg);
                skipUnknownToken(parser, tagName);
            }
            nextSkipSpaces(parser);
        }
        require(parser, parser.END_TAG, null, SyncML.TAG_ITEM);

        Item item = Item.newInstance();
        if (target != null) {
            item.setTarget(target);
        }
        if (source != null) {
            item.setSource(source);
        }
        if (meta != null) {
            item.setMeta(meta);
        }
        return item;
    }

    private Sync parseSync(XmlPullParser parser) throws XmlPullParserException,
                                                        IOException,
                                                        SyncMLParserException
    {
        verboseLog("parseSync");

        String cmdId      = null;
        Target target     = null;
        Source source     = null;
        Vector commands   = new Vector();
        Long   numChanges = null;
        boolean noResp    = false;

        nextSkipSpaces(parser);
        while (parser.getEventType() == parser.START_TAG) {
            String tagName = parser.getName();
            if (SyncML.TAG_CMDID.equals(tagName)) {
                cmdId = parseSimpleStringTag(parser, SyncML.TAG_CMDID); 
            } else if (SyncML.TAG_SOURCE.equals(tagName)) {
                source = parseSource(parser);
            } else if (SyncML.TAG_TARGET.equals(tagName)) {
                target = parseTarget(parser);
            } else if (SyncML.TAG_ADD.equals(tagName)) {
                SyncMLCommand command = parseCommand(parser, SyncML.TAG_ADD);
                commands.addElement(command);
            } else if (SyncML.TAG_REPLACE.equals(tagName)) {
                SyncMLCommand command = parseCommand(parser, SyncML.TAG_REPLACE);
                commands.addElement(command);
            } else if (SyncML.TAG_DELETE.equals(tagName)) {
                SyncMLCommand command = parseCommand(parser, SyncML.TAG_DELETE);
                commands.addElement(command);
            } else if (SyncML.TAG_NUMBEROFCHANGES.equals(tagName)) {
                long numChangesValue = parseSimpleLongTag(parser, SyncML.TAG_NUMBEROFCHANGES);
                numChanges = new Long(numChangesValue);
            } else if (SyncML.TAG_NORESP.equals(tagName)) {
                parseNoResp(parser);
                noResp = true;
            } else {
                String msg = "Error parsing sync item tag. Skipping unexpected token: " + tagName;
                Log.error(TAG_LOG, msg);
                skipUnknownToken(parser, tagName);
            }
            nextSkipSpaces(parser);
        }
        require(parser, parser.END_TAG, null, SyncML.TAG_SYNC);

        Sync sync = new Sync();
        if (cmdId != null) {
            sync.setCmdID(cmdId);
        }
        if (numChanges != null) {
            sync.setNumberOfChanges(numChanges);
        }
        if (commands.size() > 0) {
            sync.setCommands(commands);
        }
        if (source != null) {
            sync.setSource(source);
        }
        if (target != null) {
            sync.setTarget(target);
        }
        if (noResp) {
            sync.setNoResp(new Boolean(true));
        }
        return sync;
    }

    public SyncMLCommand parseCommand(XmlPullParser parser, String name) throws SyncMLParserException,
                                                                                IOException,
                                                                                XmlPullParserException
    {
        verboseLog("parseCommand");

        Vector items = new Vector();
        String cmdId = null;
        Meta   meta  = null;
        boolean noResp = false;

        nextSkipSpaces(parser);
        while (parser.getEventType() == parser.START_TAG) {
            String tagName = parser.getName();

            if (SyncML.TAG_CMDID.equals(tagName)) {
                cmdId = parseSimpleStringTag(parser, SyncML.TAG_CMDID);
            } else if (SyncML.TAG_ITEM.equals(tagName)) {
                Item item = parseSyncItem(parser);
                items.addElement(item);
            } else if (SyncML.TAG_META.equals(tagName)) {
                meta = parseMeta(parser);
            } else if (SyncML.TAG_NORESP.equals(tagName)) {
                parseNoResp(parser);
                noResp = true;
            } else {
                String msg = "Error parsing command tag. Skipping unexpected token: " + tagName;
                Log.error(TAG_LOG, msg);
                skipUnknownToken(parser, tagName);
            }
            nextSkipSpaces(parser);
        }
        require(parser, parser.END_TAG, null, name);

        SyncMLCommand cmd = SyncMLCommand.newInstance(name);
        cmd.setCmdId(cmdId);
        if (meta != null) {
            cmd.setMeta(meta);
        }
        if (items.size() > 0) {
            cmd.setItems(items);
        }
        if (noResp) {
            cmd.setNoResp(true);
        }
        return cmd;
    }

    private Item parseSyncItem(XmlPullParser parser) throws XmlPullParserException,
                                                            IOException,
                                                            SyncMLParserException
    {
        verboseLog("parseSyncItem");

        Source source = null;
        Target target = null;
        Data   data   = null;
        SourceParent sourceParent = null;
        TargetParent targetParent = null;
        boolean hasMoreData = false;
        Meta meta = null;

        nextSkipSpaces(parser);
        while (parser.getEventType() == parser.START_TAG) {
            String tagName = parser.getName();
            if (SyncML.TAG_TARGET.equals(tagName)) {
                target = parseTarget(parser);
            } else if (SyncML.TAG_SOURCE.equals(tagName)) {
                source = parseSource(parser);
            } else if (SyncML.TAG_TARGET_PARENT.equals(tagName)) {
                targetParent = parseTargetParent(parser);
            } else if (SyncML.TAG_SOURCE_PARENT.equals(tagName)) {
                sourceParent = parseSourceParent(parser);
            } else if (SyncML.TAG_MORE_DATA.equals(tagName)) {
                parseSimpleStringTag(parser, SyncML.TAG_MORE_DATA);
                hasMoreData = true;
            } else if (SyncML.TAG_DATA.equals(tagName)) {
                data = parseItemData(parser);
            } else if (SyncML.TAG_META.equals(tagName)) {
                meta = parseMeta(parser);
            } else {
                String msg = "Error parsing sync item tag. Skipping unexpected token: " + tagName;
                Log.error(TAG_LOG, msg);
                skipUnknownToken(parser, tagName);
            }
            nextSkipSpaces(parser);
        }
        require(parser, parser.END_TAG, null, SyncML.TAG_ITEM);

        Item item = Item.newInstance();
        if (source != null) {
            item.setSource(source);
        }
        if (target != null) {
            item.setTarget(target);
        }
        if (data != null) {
            item.setData(data);
        }
        if (hasMoreData) {
            item.setMoreData(new Boolean(hasMoreData));
        }
        if (sourceParent != null) {
            item.setSourceParent(sourceParent);
        }
        if (targetParent != null) {
            item.setTargetParent(targetParent);
        }
        if (meta != null) {
            item.setMeta(meta);
        }
        return item;
    }

    private Target parseTarget(XmlPullParser parser) throws XmlPullParserException,
                                                          IOException,
                                                          SyncMLParserException
    {
        verboseLog("parseTarget");
        nextSkipSpaces(parser);
        // TODO handle filter
        String locUri = null;
        String locName = null;
        while (parser.getEventType() == parser.START_TAG) {
            String tagName = parser.getName();
            if (SyncML.TAG_LOC_URI.equals(tagName)) {
                locUri = parseSimpleStringTag(parser, SyncML.TAG_LOC_URI);
            } else if (SyncML.TAG_LOC_NAME.equals(tagName)) {
                locName = parseSimpleStringTag(parser, SyncML.TAG_LOC_NAME);
            } else {
                String msg = "Error parsing target item tag. Skipping unexpected token: " + tagName;
                Log.error(TAG_LOG, msg);
                skipUnknownToken(parser, tagName);
            }
            nextSkipSpaces(parser);
        }
        require(parser, parser.END_TAG, null, SyncML.TAG_TARGET);

        Target target = Target.newInstance();
        if (locUri != null) {
            target.setLocURI(locUri);
        }
        if (locName != null) {
            target.setLocName(locName);
        }
        return target;
    }


    private Source parseSource(XmlPullParser parser) throws XmlPullParserException,
                                                            IOException,
                                                            SyncMLParserException
    {
        verboseLog("parseSource");
        nextSkipSpaces(parser);
        // TODO handle filter
        String locUri = null;
        String locName = null;
        while (parser.getEventType() == parser.START_TAG) {
            String tagName = parser.getName();
            if (SyncML.TAG_LOC_URI.equals(tagName)) {
                locUri = parseSimpleStringTag(parser, SyncML.TAG_LOC_URI);
            } else if (SyncML.TAG_LOC_NAME.equals(tagName)) {
                locName = parseSimpleStringTag(parser, SyncML.TAG_LOC_NAME);
            } else {
                String msg = "Error parsing target item tag. Skipping unexpected token: " + tagName;
                Log.error(TAG_LOG, msg);
                skipUnknownToken(parser, tagName);
            }
            nextSkipSpaces(parser);
        }
        require(parser, parser.END_TAG, null, SyncML.TAG_SOURCE);

        Source source = Source.newInstance();
        if (locUri != null) {
            source.setLocURI(locUri);
        }
        if (locName != null) {
            source.setLocName(locName);
        }
        return source;
    }

    private TargetParent parseTargetParent(XmlPullParser parser) throws XmlPullParserException,
                                                                        IOException,
                                                                        SyncMLParserException
    {
        verboseLog("parseTargetParent");

        nextSkipSpaces(parser);
        // We expect the LocURI only
        require(parser, parser.START_TAG, null, SyncML.TAG_LOC_URI);
        String locUri = parseSimpleStringTag(parser, SyncML.TAG_LOC_URI);
        nextSkipSpaces(parser);
        require(parser, parser.END_TAG, null, SyncML.TAG_TARGET_PARENT);

        TargetParent targetParent = new TargetParent(locUri);
        return targetParent;
    }

    private SourceParent parseSourceParent(XmlPullParser parser) throws XmlPullParserException,
                                                                        IOException,
                                                                        SyncMLParserException
    {
        verboseLog("parseSourceParent");

        nextSkipSpaces(parser);
        // We expect the LocURI only
        require(parser, parser.START_TAG, null, SyncML.TAG_LOC_URI);
        String locUri = parseSimpleStringTag(parser, SyncML.TAG_LOC_URI);
        nextSkipSpaces(parser);
        require(parser, parser.END_TAG, null, SyncML.TAG_SOURCE_PARENT);

        SourceParent sourceParent = SourceParent.newInstance();
        sourceParent.setLocURI(locUri);
        return sourceParent;
    }

    private Data parseItemData(XmlPullParser parser)  throws XmlPullParserException,
                                                             IOException,
                                                             SyncMLParserException
    {
        verboseLog("parseItemData");

        boolean done = false;
        String textData = null;
        byte   binData[] = null;
        Anchor anchor = null;
        DevInf devInf = null;
        StringBuffer preamble = new StringBuffer();

        // Use the nextToken here to catch binary data (Wbxml OPAQUE)
        parser.nextToken(); 
        if (parser.getEventType() == parser.CDSECT) {
            // This can only happen in XML, so we can ignore binary data and
            // grab the content directly (note that this can only be a leaf, so
            // this must be textData)
            textData = parser.getText();
            // Advance to the next token
            parser.nextToken();
        } else {
            while(parser.getEventType() == parser.IGNORABLE_WHITESPACE ||
                  parser.getEventType() == parser.TEXT ||
                  parser.getEventType() == parser.ENTITY_REF)
            {
                preamble.append(parser.getText());
                parser.nextToken();
            }
            textData = preamble.toString();
            if (parser.getEventType() == parser.START_TAG) {
                String tagName = parser.getName();
                if (SyncML.TAG_ANCHOR.equals(tagName)) {
                    anchor = parseAnchor(parser);
                } else if (SyncML.TAG_DEVINF.equals(tagName)) {
                    devInf = parseDevInf(parser);
                } else {
                    throw new SyncMLParserException("Unkonw tag in data: " + tagName);
                }
                nextSkipSpaces(parser);
            } else if (parser.getEventType() == WbxmlParser.WAP_EXTENSION) {
                // This is binary data
                binData = parseBinaryData(parser);
            } else if (parser.getEventType() == parser.TEXT) {
                textData = parseTextData(parser, textData);
                // We expect text plain data. Since this text is not contained
                // in CDATA section, we must unescape it
                if (!wbxml) {
                    textData = XmlUtil.unescapeXml(textData);
                }
            }
        }

        // We require the END DATA tag here
        require(parser, parser.END_TAG, null, SyncML.TAG_DATA);

        Data data;
        if (anchor != null) {
            data = Data.newInstance(anchor);
        } else if (devInf != null) {
            data = Data.newInstance(devInf);
        } else if (binData != null) {
            data = Data.newInstance(binData);
        } else {
            data = Data.newInstance(textData);
        }
        return data;
    }

    private byte[] parseBinaryData(XmlPullParser parser) throws XmlPullParserException,
                                                                SyncMLParserException,
                                                                IOException
    {
        verboseLog("parseBinaryData");

        byte binData[] = null;
        // We support the OPAQUE as binary data
        if (parser instanceof WbxmlParser) {
            WbxmlParser wbxmlParser = (WbxmlParser)parser;
            int wapId = wbxmlParser.getWapCode();
            if (wapId == Wbxml.OPAQUE) {
                binData = (byte[])wbxmlParser.getWapExtensionData();
            } else {
                throw new SyncMLParserException("Cannot parse WAP EXTENSION " + wapId);
            }
        } else {
            throw new SyncMLParserException("Cannot parse binary data in XML");
        }
        // Advance to the next token
        nextSkipSpaces(parser);
        return binData;
    }

    private String parseTextData(XmlPullParser parser, String preamble) throws XmlPullParserException,
                                                                               SyncMLParserException,
                                                                               IOException
    {
        verboseLog("parseTextData");
        StringBuffer value = new StringBuffer(preamble);

        String v = null;

        while (parser.getEventType() != parser.END_TAG) {
            // Now fetch the rest of the data tag
            parser.nextToken();
            if (parser.getEventType() == parser.TEXT ||
                parser.getEventType() == parser.IGNORABLE_WHITESPACE ||
                parser.getEventType() == parser.ENTITY_REF)
            {
                v = parser.getText();
                value.append(v);
            } else if (parser.getEventType() != parser.END_TAG) {
                throw new SyncMLParserException("Unexpected event: " + parser.getEventType());
            }
        }
        // Try to avoid redundant memory usage.
        if (v != null && v.length() == value.length()) {
            return v;
        } else if (v == null) {
            return preamble;
        } else {
            return value.toString();
        }
    }
   
    private Anchor parseAnchor(XmlPullParser parser)  throws XmlPullParserException,
                                                             IOException,
                                                             SyncMLParserException
    {
        verboseLog("parseAnchor");

        String last = null;
        String next = null;

        nextSkipSpaces(parser);
        while (parser.getEventType() == parser.START_TAG) {
            String tagName = parser.getName();
            if (SyncML.TAG_LAST.equals(tagName)) {
                last = parseSimpleStringTag(parser, SyncML.TAG_LAST);
            } else if (SyncML.TAG_NEXT.equals(tagName)) {
                next = parseSimpleStringTag(parser, SyncML.TAG_NEXT);
            } else {
                String msg = "Error parsing anchor tag. Skipping unexpected token: " + tagName;
                Log.error(TAG_LOG, msg);
                skipUnknownToken(parser, tagName);
            }
            nextSkipSpaces(parser);
        }
        require(parser, parser.END_TAG, null, SyncML.TAG_ANCHOR);

        Anchor anchor = new Anchor(last, next);
        return anchor;
    }


    /**
     * Parse the results section of the server response. At the moment this
     * method assumes this section contains only the server device info. This is
     * not true in general, but this is currently a limitation. In the future it
     * will be made more general.
     *
     * @param results is the results section of the server response. This value
     * shall not contain the &lt;Results&gt; tag
     * @return a Results object representing the XML element
     * @throws SyncMLParserException if the text cannot be parser properly. Note
     * that if the text contains unknown tags, they are simply skipped, but if
     * it has malformed xml, an exception is thrown.
     */
    public Results parseResults(XmlPullParser parser) throws SyncMLParserException,
                                                             IOException,
                                                             XmlPullParserException
    {
        String cmdId  = null;
        String msgRef = null;
        String cmdRef = null;
        Meta   meta   = null;
        Vector items  = new Vector();
        TargetRef tgtRef = null;
        SourceRef srcRef = null;

        verboseLog("parseResults");

        nextSkipSpaces(parser);
        while (parser.getEventType() == parser.START_TAG) {
            String tagName = parser.getName();
            if (SyncML.TAG_CMDID.equals(tagName)) {
                cmdId = parseSimpleStringTag(parser, SyncML.TAG_CMDID);
            } else if (SyncML.TAG_MSGREF.equals(tagName)) {
                msgRef = parseSimpleStringTag(parser, SyncML.TAG_MSGREF);
            } else if (SyncML.TAG_CMDREF.equals(tagName)) {
                cmdRef = parseSimpleStringTag(parser, SyncML.TAG_CMDREF);
            } else if (SyncML.TAG_META.equals(tagName)) {
                meta = parseMeta(parser);
            } else if (SyncML.TAG_ITEM.equals(tagName)) {
                DevInfItem devInfItem = parseDevInfItem(parser);
                items.addElement(devInfItem);
            } else if (SyncML.TAG_TARGETREF.equals(tagName)) {
                tgtRef = parseTargetRef(parser);
            } else if (SyncML.TAG_SOURCEREF.equals(tagName)) {
                srcRef = parseSourceRef(parser);
            } else {
                String msg = "Error parsing device info tag. Skipping unexpected token: " + tagName;
                Log.error(TAG_LOG, msg);
                skipUnknownToken(parser, tagName);
            }
            nextSkipSpaces(parser);
        }
        require(parser, parser.END_TAG, null, SyncML.TAG_RESULTS);

        Results results = new Results();
        if (cmdId != null) {
            results.setCmdID(cmdId);
        }
        if (msgRef != null) {
            results.setMsgRef(msgRef);
        }
        if (cmdRef != null) {
            results.setCmdRef(cmdRef);
        }
        if (meta != null) {
            results.setMeta(meta);
        }
        if (items.size() > 0) {
            results.setItems(items);
        }
        if (tgtRef != null) {
            results.setTargetRef(tgtRef);
        }
        if (srcRef != null) {
            results.setSourceRef(srcRef);
        }
        return results;
    }

    private Meta parseMeta(XmlPullParser parser) throws XmlPullParserException,
                                                        IOException,
                                                        SyncMLParserException
    {
        verboseLog("parseMeta");

        String type          = null;
        String format        = null;
        Anchor anchor        = null;
        NextNonce nextNonce  = null;
        Long      size       = null;
        Long      maxMsgSize = null;
        Long      maxObjSize = null;
        MetInf    metInf     = null;
        String    version    = null;

        nextSkipSpaces(parser);
        while (parser.getEventType() == parser.START_TAG) {
            String tagName = parser.getName();
            if (SyncML.TAG_TYPE.equals(tagName)) {
                type = parseSimpleStringTag(parser, SyncML.TAG_TYPE);
            } else if (SyncML.TAG_FORMAT.equals(tagName)) {
                format = parseSimpleStringTag(parser, SyncML.TAG_FORMAT);
            } else if (SyncML.TAG_NEXTNONCE.equals(tagName)) {
                String nextNonceVal = parseSimpleStringTag(parser, SyncML.TAG_NEXTNONCE);
                nextNonce = new NextNonce(nextNonceVal);
            } else if (SyncML.TAG_ANCHOR.equals(tagName)) {
                anchor = parseAnchor(parser);
            } else if (SyncML.TAG_SIZE.equals(tagName)) {
                long sizeVal = parseSimpleLongTag(parser, SyncML.TAG_SIZE);
                size = new Long(sizeVal);
            } else if (SyncML.TAG_MAXMSGSIZE.equals(tagName)) {
                long sizeVal = parseSimpleLongTag(parser, SyncML.TAG_MAXMSGSIZE);
                maxMsgSize = new Long(sizeVal);
            } else if (SyncML.TAG_MAXOBJSIZE.equals(tagName)) {
                long sizeVal = parseSimpleLongTag(parser, SyncML.TAG_MAXOBJSIZE);
                maxObjSize = new Long(sizeVal);
            } else if (SyncML.TAG_METAINF.equals(tagName)) {
                metInf = parseMetaInf(parser);
            } else if (SyncML.TAG_VERSION.equals(tagName)) {
                version = parseSimpleStringTag(parser, SyncML.TAG_VERSION);
            } else {
                String msg = "Error parsing META tag. Skipping unexpected token: " + tagName;
                Log.error(TAG_LOG, msg);
                skipUnknownToken(parser, tagName);
            }
            nextSkipSpaces(parser);
        }
        require(parser, parser.END_TAG, null, SyncML.TAG_META);

        Meta meta = Meta.newInstance();

        // If the metainf is specified as an attribute, then this object is
        // allowed to be null
        if (metInf == null) {
            metInf = MetInf.newInstance();
        }

        if (type != null) {
            metInf.setType(type);
        }
        if (format != null) {
            metInf.setFormat(format);
        }
        if (nextNonce != null) {
            metInf.setNextNonce(nextNonce);
        }
        meta.setMetInf(metInf);
        if (anchor != null) {
            meta.setAnchor(anchor);
        }
        if (size != null) {
            meta.setSize(size);
        }
        if (maxMsgSize != null) {
            meta.setMaxMsgSize(maxMsgSize);
        }
        if (maxObjSize != null) {
            meta.setMaxObjSize(maxObjSize);
        }
        if (version != null) {
            meta.setVersion(version);
        }
        return meta;
    }

    private MetInf parseMetaInf(XmlPullParser parser)  throws XmlPullParserException,
                                                              IOException,
                                                              SyncMLParserException
    {
        String type          = null;
        String format        = null;

        nextSkipSpaces(parser);
        while (parser.getEventType() == parser.START_TAG) {
            String tagName = parser.getName();
            if (SyncML.TAG_TYPE.equals(tagName)) {
                type = parseSimpleStringTag(parser, SyncML.TAG_TYPE);
            } else if (SyncML.TAG_FORMAT.equals(tagName)) {
                format = parseSimpleStringTag(parser, SyncML.TAG_FORMAT);
            } else {
                String msg = "Error parsing META tag. Skipping unexpected token: " + tagName;
                Log.error(TAG_LOG, msg);
                skipUnknownToken(parser, tagName);
            }
            nextSkipSpaces(parser);
        }
        require(parser, parser.END_TAG, null, SyncML.TAG_METAINF);

        MetInf metaInf = MetInf.newInstance();
        if (format != null) {
            metaInf.setFormat(format);
        }
        if (type != null) {
            metaInf.setType(type);
        }
        return metaInf;
    }

    private DevInfItem parseDevInfItem(XmlPullParser parser) throws XmlPullParserException,
                                                                    IOException,
                                                                    SyncMLParserException
    {
        DevInf devInf = null;
        Meta   meta   = null;
        Source source = null;
        Target target = null;

        verboseLog("parseDevInfItem");

        nextSkipSpaces(parser);
        while (parser.getEventType() == parser.START_TAG) {
            String tagName = parser.getName();
            if (SyncML.TAG_SOURCE.equals(tagName)) {
                source = parseItemSource(parser);
            } else if (SyncML.TAG_DATA.equals(tagName)) {
                if (source != null && SyncML.DEVINF12.equals(source.getLocURI())) {
                    devInf = parseDevInfData(parser);
                } else {
                    parseData(parser);
                }
            } else if (SyncML.TAG_META.equals(tagName)) {
                meta = parseMeta(parser);
            } else if (SyncML.TAG_TARGET.equals(tagName)) {
                target = parseTarget(parser);
            } else {
                String msg = "Error parsing ITEM tag. Skipping unexpected token: " + tagName;
                Log.error(TAG_LOG, msg);
                skipUnknownToken(parser, tagName);
            }
            nextSkipSpaces(parser);
        }
        require(parser, parser.END_TAG, null, SyncML.TAG_ITEM);

        DevInfItem item = new DevInfItem();
        if (devInf != null) {
            item.setDevInf(devInf);
        }
        if (meta != null) {
            item.setMeta(meta);
        }
        if (source != null) {
            item.setSource(source);
        }
        if (target != null) {
            item.setTarget(target);
        }
        return item;
    }

    private Source parseItemSource(XmlPullParser parser) throws XmlPullParserException,
                                                                IOException,
                                                                SyncMLParserException {
        verboseLog("parseItemSource");
        nextSkipSpaces(parser);
        Source source = Source.newInstance();
        while (parser.getEventType() == parser.START_TAG) {
            String tagName = parser.getName();
            if (SyncML.TAG_LOCURI.equals(tagName)) {
                String locUri = parseSimpleStringTag(parser, SyncML.TAG_LOCURI);
                source.setLocURI(locUri);
            } else if (SyncML.TAG_LOCNAME.equals(tagName)) {
                String locName = parseSimpleStringTag(parser, SyncML.TAG_LOCNAME);
                source.setLocName(locName);
            } else {
                String msg = "Error parsing ITEM tag. Skipping unexpected token: " + tagName;
                Log.error(TAG_LOG, msg);
                skipUnknownToken(parser, tagName);
            }
            nextSkipSpaces(parser);
        }
        require(parser, parser.END_TAG, null, SyncML.TAG_SOURCE);
        return source;
    }

    private DevInf parseDevInfData(XmlPullParser parser) throws XmlPullParserException,
                                                                IOException,
                                                                SyncMLParserException {
        verboseLog("parseDevInfData");

        DevInf devInf = null;

        // Depending on the WBXML generator, the devinf can be contained in an
        // OPAQUE section or can be just regular tokens. Here we handle both
        // cases. If it is an opaque we read it completely and then parse its
        // content.
        parser.nextToken();
        // Skip things we are not interested at
        while(parser.getEventType() == parser.IGNORABLE_WHITESPACE ||
              parser.getEventType() == parser.ENTITY_REF)
        {
            parser.nextToken();
        }

        if (parser.getEventType() == WbxmlParser.WAP_EXTENSION) {
            // The whole devinf is embedded in a single TEXT string, but
            // it can be encoded as XML or WBXML. We use the first byte to
            // discriminate the two cases.
            byte binData[] = parseBinaryData(parser);
            XmlPullParser devInfParser;
            if (binData[0] == 2 || binData[0] == 3) {
                // DevInf is in WBXML
                devInfParser = com.funambol.org.kxml2.wap.syncml.SyncML.createDevInfParser();
            } else {
                // DevInf is in XML
                devInfParser = new KXmlParser();
            }
            ByteArrayInputStream is = new ByteArrayInputStream(binData);
            devInfParser.setInput(is, "UTF-8");
            // Start parsing the DevInf
            nextSkipSpaces(devInfParser);
            require(devInfParser, parser.START_TAG, null, SyncML.TAG_DEVINF);
            devInf = parseDevInf(devInfParser);
        } else if (wbxml && parser.getEventType() == parser.TEXT) {
            // The whole devinf is embedded in a single TEXT string
            XmlPullParser devInfParser = new KXmlParser();
            byte binData[] = null;
            try {
                binData = parser.getText().getBytes("UTF-8");
            } catch (Exception e) {
                Log.error(TAG_LOG, "Cannot convert dev inf to UTF-8 encoding", e);
                throw new SyncMLParserException("Cannot convert dev inf to UTF-8");
            }
            ByteArrayInputStream is = new ByteArrayInputStream(binData);
            devInfParser.setInput(is, "UTF-8");
            // Start parsing the DevInf
            nextSkipSpaces(devInfParser);
            require(devInfParser, parser.START_TAG, null, SyncML.TAG_DEVINF);
            devInf = parseDevInf(devInfParser);
            // Move ahead
            nextSkipSpaces(parser);
        } else {
            if (parser.getEventType() == parser.TEXT) {
                nextSkipSpaces(parser);
            }
            while (parser.getEventType() == parser.START_TAG) {
                String tagName = parser.getName();

                if (SyncML.TAG_DEVINF.equals(tagName)) {
                    devInf = parseDevInf(parser);
                } else {
                    String msg = "Error parsing dev inf data tag. Skipping unexpected token: " + tagName;
                    Log.error(TAG_LOG, msg);
                    skipUnknownToken(parser, tagName);
                }
                nextSkipSpaces(parser);
            }
        }
        require(parser, parser.END_TAG, null, SyncML.TAG_DATA);
        return devInf;
    }

    private void parseData(XmlPullParser parser) throws XmlPullParserException,
                                                        IOException,
                                                        SyncMLParserException {

        // We skip everything until we find the DATA closure tag
        // TODO: we shall return the data somehow
        skipUnknownToken(parser, SyncML.TAG_DATA);
    }


    /**
     * Parse a Device Info section.
     * @param parser is the parser
     */
    private DevInf parseDevInf(XmlPullParser parser) throws XmlPullParserException,
                                                            IOException,
                                                            SyncMLParserException
    {
        // In general the item parsing depends on its type
        verboseLog("parseDevInf");

        DevInf devInf = new DevInf();

        nextSkipSpaces(parser);
        while (parser.getEventType() == parser.START_TAG) {
            String tagName = parser.getName();
            if (SyncML.TAG_VERDTD.equals(tagName)) {
                parseVerDTD(parser, devInf);
            } else if (SyncML.TAG_DEVINFMAN.equals(tagName)) {
                String man = parseSimpleStringTag(parser, SyncML.TAG_DEVINFMAN);
                devInf.setMan(man);
            } else if (SyncML.TAG_DEVINFMOD.equals(tagName)) {
                String mod = parseSimpleStringTag(parser, SyncML.TAG_DEVINFMOD);
                devInf.setMod(mod);
            } else if (SyncML.TAG_DEVINFOEM.equals(tagName)) {
                String oem = parseSimpleStringTag(parser, SyncML.TAG_DEVINFOEM);
                devInf.setOEM(oem);
            } else if (SyncML.TAG_DEVINFFWV.equals(tagName)) {
                String fwv = parseSimpleStringTag(parser, SyncML.TAG_DEVINFFWV);
                devInf.setFwV(fwv);
            } else if (SyncML.TAG_DEVINFSWV.equals(tagName)) {
                String swv = parseSimpleStringTag(parser, SyncML.TAG_DEVINFSWV);
                devInf.setSwV(swv);
            } else if (SyncML.TAG_DEVINFHWV.equals(tagName)) {
                String hwv = parseSimpleStringTag(parser, SyncML.TAG_DEVINFHWV);
                devInf.setHwV(hwv);
            } else if (SyncML.TAG_DEVINFDEVID.equals(tagName)) {
                String devId = parseSimpleStringTag(parser, SyncML.TAG_DEVINFDEVID);
                devInf.setDevID(devId);
            } else if (SyncML.TAG_DEVINFDEVTYP.equals(tagName)) {
                String devTyp = parseSimpleStringTag(parser, SyncML.TAG_DEVINFDEVTYP);
                devInf.setDevTyp(devTyp);
            } else if (SyncML.TAG_DEVINFUTC.equals(tagName)) {
                parseDevInfUtc(parser, devInf);
            } else if (SyncML.TAG_DEVINFLO.equals(tagName)) {
                parseDevInfLo(parser, devInf);
            } else if (SyncML.TAG_DEVINFNC.equals(tagName)) {
                parseDevInfNc(parser, devInf);
            } else if (SyncML.TAG_DEVINFDATASTORE.equals(tagName)) {
                DataStore ds = parseDevInfDataStore(parser, devInf);
                devInf.addDataStore(ds);
            } else if (SyncML.TAG_EXT.equals(tagName)) {
                Vector exts = parseExt(parser);
                devInf.addExts(exts);
            } else {
                String msg = "Error parsing ITEM tag. Skipping unexpected token: " + tagName;
                Log.error(TAG_LOG, msg);
                skipUnknownToken(parser, tagName);
            }
            nextSkipSpaces(parser);
        }
        require(parser, parser.END_TAG, null, SyncML.TAG_DEVINF);
        return devInf;
    }

    private void parseVerDTD(XmlPullParser parser, DevInf devInf)
                                                 throws XmlPullParserException,
                                                        IOException,
                                                        SyncMLParserException
    {
        String verDtd = parseSimpleStringTag(parser, SyncML.TAG_VERDTD);
        VerDTD ver = new VerDTD(verDtd);
        devInf.setVerDTD(ver);
    }

    private void parseDevInfUtc(XmlPullParser parser, DevInf devInf)
                                                   throws XmlPullParserException,
                                                          IOException, SyncMLParserException {
        devInf.setUTC(true);
        nextSkipSpaces(parser);
        require(parser, parser.END_TAG, null, SyncML.TAG_DEVINFUTC);
    }

    private void parseDevInfLo(XmlPullParser parser, DevInf devInf)
                                                   throws XmlPullParserException,
                                                          IOException, SyncMLParserException
    {
        verboseLog("parseDevInfLo");

        devInf.setSupportLargeObjs(true);
        nextSkipSpaces(parser);
        require(parser, parser.END_TAG, null, SyncML.TAG_DEVINFLO);
    }

    private void parseDevInfNc(XmlPullParser parser, DevInf devInf)
                                                   throws XmlPullParserException,
                                                          IOException, SyncMLParserException
    {
        verboseLog("parseDevInfNc");

        devInf.setSupportNumberOfChanges(true);
        nextSkipSpaces(parser);
        require(parser, parser.END_TAG, null, SyncML.TAG_DEVINFNC);
    }



    private DataStore parseDevInfDataStore(XmlPullParser parser, DevInf devInf)
                                                      throws SyncMLParserException,
                                                             XmlPullParserException,
                                                             IOException
    {
        verboseLog("parseDevInfDataStore");

        DataStore ds = new DataStore();

        nextSkipSpaces(parser);
        while (parser.getEventType() == parser.START_TAG) {
            String tagName = parser.getName();

            if (SyncML.TAG_SOURCEREF.equals(tagName)) {
                SourceRef sourceRef = parseSourceRef(parser);
                ds.setSourceRef(sourceRef);
            } else if (SyncML.TAG_DISPLAYNAME.equals(tagName)) {
                String displayName = parseSimpleStringTag(parser, SyncML.TAG_DISPLAYNAME);
                ds.setDisplayName(displayName);
            } else if (SyncML.TAG_MAXGUIDSIZE.equals(tagName)) {
                long size = parseSimpleLongTag(parser, SyncML.TAG_MAXGUIDSIZE);
                ds.setMaxGUIDSize(size);
            } else if (SyncML.TAG_RX.equals(tagName)) {
                parseRxs(parser, ds);
            } else if (SyncML.TAG_RXPREF.equals(tagName)) {
                parseRxPref(parser, ds);
            } else if (SyncML.TAG_TX.equals(tagName)) {
                parseTxs(parser, ds);
            } else if (SyncML.TAG_TXPREF.equals(tagName)) {
                parseTxPref(parser, ds);
            } else if (SyncML.TAG_SYNCCAP.equals(tagName)) {
                SyncCap cap = parseSyncCap(parser);
                ds.setSyncCap(cap);
            } else if (SyncML.TAG_CTCAP.equals(tagName)) {
                CTCap cap = parseCTCap(parser);
                ds.addCTCap(cap);
            } else if (SyncML.TAG_DSMEM.equals(tagName)) {
                DSMem dsMem = parseDSMem(parser);
                ds.setDSMem(dsMem);
            } else if (SyncML.TAG_DATASTOREHS.equals(tagName)) {
                parseDevInfHs(parser, devInf);
            } else {
                String msg = "Error parsing DATA STORE tag. Skipping unexpected token: " + tagName;
                Log.error(TAG_LOG, msg);
                skipUnknownToken(parser, tagName);
            }
            nextSkipSpaces(parser);
        }
        require(parser, parser.END_TAG, null, SyncML.TAG_DEVINFDATASTORE);

        return ds;
    }

    private SourceRef parseSourceRef(XmlPullParser parser) throws SyncMLParserException,
                                                                  XmlPullParserException,
                                                                  IOException
    {
        verboseLog("parseSourceRef");
        String name = parseSimpleStringTag(parser, SyncML.TAG_SOURCEREF);
        SourceRef sr = SourceRef.newInstance();
        sr.setValue(name);
        return sr;
    }

    private TargetRef parseTargetRef(XmlPullParser parser) throws SyncMLParserException,
                                                                  XmlPullParserException,
                                                                  IOException
    {
        verboseLog("parseTargetRef");
        String name = parseSimpleStringTag(parser, SyncML.TAG_TARGETREF);
        TargetRef tr = TargetRef.newInstance();
        tr.setValue(name);
        return tr;
    }



    private void parseRxs(XmlPullParser parser, DataStore ds)
                                                 throws SyncMLParserException,
                                                        XmlPullParserException,
                                                        IOException
    {
        verboseLog("parseRxs");
        CTInfo ctInfo = parseCTInfo(parser);
        ds.addRxs(ctInfo);
        require(parser, parser.END_TAG, null, SyncML.TAG_RX);
    }

    private void parseRxPref(XmlPullParser parser, DataStore ds)
                                                throws SyncMLParserException,
                                                       XmlPullParserException,
                                                       IOException
    {
        verboseLog("parseRxPref");
        CTInfo ctInfo = parseCTInfo(parser);
        ds.setRxPref(ctInfo);
        require(parser, parser.END_TAG, null, SyncML.TAG_RXPREF);
    }

    private void parseTxs(XmlPullParser parser, DataStore ds)
                                                 throws SyncMLParserException,
                                                        XmlPullParserException,
                                                        IOException
    {
        verboseLog("parseTxs");
        CTInfo ctInfo = parseCTInfo(parser);
        ds.addTxs(ctInfo);
        require(parser, parser.END_TAG, null, SyncML.TAG_TX);
    }

    private void parseTxPref(XmlPullParser parser, DataStore ds)
                                                throws SyncMLParserException,
                                                       XmlPullParserException,
                                                       IOException
    {
        verboseLog("parseTxPref");

        CTInfo ctInfo = parseCTInfo(parser);
        ds.setTxPref(ctInfo);
        require(parser, parser.END_TAG, null, SyncML.TAG_TXPREF);
    }


    private CTInfo parseCTInfo(XmlPullParser parser) throws SyncMLParserException,
                                                            XmlPullParserException,
                                                            IOException
    {
        verboseLog("parseCTInfo");

        CTInfo ctInfo = new CTInfo();

        nextSkipSpaces(parser);
        while (parser.getEventType() == parser.START_TAG) {
            String tagName = parser.getName();
            if (SyncML.TAG_CTTYPE.equals(tagName)) {
                String type = parseSimpleStringTag(parser, SyncML.TAG_CTTYPE);
                ctInfo.setCTType(type);
            } else if (SyncML.TAG_VERCT.equals(tagName)) {
                String ver = parseSimpleStringTag(parser, SyncML.TAG_VERCT);
                ctInfo.setVerCT(ver);
            } else {
                String msg = "Error parsing CTINFO tag. Skipping Unexpected token: " + tagName;
                Log.error(TAG_LOG, msg);
                skipUnknownToken(parser, tagName);
            }
            nextSkipSpaces(parser);
        }
        return ctInfo;
    }

    private DSMem parseDSMem(XmlPullParser parser) throws SyncMLParserException,
                                                          XmlPullParserException,
                                                          IOException {
        DSMem dsMem = new DSMem();

        verboseLog("parseDSMem");

        nextSkipSpaces(parser);
        while (parser.getEventType() == parser.START_TAG) {
            String tagName = parser.getName();
            if (SyncML.TAG_SHAREDMEM.equals(tagName)) {
                parseSimpleStringTag(parser, SyncML.TAG_SHAREDMEM);
                dsMem.setSharedMem(true);
            } else if (SyncML.TAG_MAXMEM.equals(tagName)) {
                long maxMem = parseSimpleLongTag(parser, SyncML.TAG_MAXMEM);
                dsMem.setMaxMem(maxMem);
            } else if (SyncML.TAG_MAXID.equals(tagName)) {
                long maxId = parseSimpleLongTag(parser, SyncML.TAG_MAXID);
                dsMem.setMaxID(maxId);
            } else {
                String msg = "Error parsing DSMEM tag. Skipping Unexpected token: " + tagName;
                Log.error(TAG_LOG, msg);
                skipUnknownToken(parser, tagName);
            }
            nextSkipSpaces(parser);
        }
        require(parser, parser.END_TAG, null, SyncML.TAG_DSMEM);
        return dsMem;
    }


    private SyncCap parseSyncCap(XmlPullParser parser) throws XmlPullParserException,
                                                              IOException,
                                                              SyncMLParserException
    {
        verboseLog("parseSyncCap");

        SyncCap syncCap = new SyncCap();

        nextSkipSpaces(parser);
        while (parser.getEventType() == parser.START_TAG) {
            String tagName = parser.getName();
            if (SyncML.TAG_SYNCTYPE.equals(tagName)) {
                SyncType type = parseSyncType(parser);
                syncCap.addSyncType(type);
            } else {
                String msg = "Error parsing DATA STORE tag. Unexpected token: " + tagName;
                Log.error(TAG_LOG, msg);
                skipUnknownToken(parser, tagName);
            }
            nextSkipSpaces(parser);
        }
        require(parser, parser.END_TAG, null, SyncML.TAG_SYNCCAP);
        return syncCap;
    }

    private CTCap parseCTCap(XmlPullParser parser) throws XmlPullParserException,
                                                          IOException,
                                                          SyncMLParserException
    {
        verboseLog("parseCTCap");

        CTCap ctCap = new CTCap();
        CTInfo ctInfo = new CTInfo();
        nextSkipSpaces(parser);
        while (parser.getEventType() == parser.START_TAG) {
            String tagName = parser.getName();
            if (SyncML.TAG_CTTYPE.equals(tagName)) {
                String type = parseSimpleStringTag(parser, SyncML.TAG_CTTYPE);
                ctInfo.setCTType(type);
            } else if (SyncML.TAG_VERCT.equals(tagName)) {
                String ver = parseSimpleStringTag(parser, SyncML.TAG_VERCT);
                ctInfo.setVerCT(ver);
            } else if (SyncML.TAG_PROPERTY.equals(tagName)) {
                Property property = parseProperty(parser);
                ctCap.addProperty(property);
            } else {
                String msg = "Error parsing CTINFO tag. Skipping Unexpected token: " + tagName;
                Log.error(TAG_LOG, msg);
                skipUnknownToken(parser, tagName);
            }
            nextSkipSpaces(parser);
        }
        ctCap.setCTInfo(ctInfo);
        require(parser, parser.END_TAG, null, SyncML.TAG_CTCAP);
        return ctCap;
    }
    
    private void parseDevInfHs(XmlPullParser parser, DevInf devInf)
                                                   throws XmlPullParserException,
                                                          IOException {
        verboseLog("parseDevInfHs");
        devInf.setSupportHierarchicalSync(true);
        parser.next();
        require(parser, parser.END_TAG, null, SyncML.TAG_DATASTOREHS);
    }


    private Property parseProperty(XmlPullParser parser) throws XmlPullParserException,
                                                                IOException,
                                                                SyncMLParserException
    {
        verboseLog("parseProperty");

        nextSkipSpaces(parser);
        Property property = new Property();

        while (parser.getEventType() == parser.START_TAG) {
            String tagName = parser.getName();
            if (SyncML.TAG_PROPNAME.equals(tagName)) {
                String name = parseSimpleStringTag(parser, SyncML.TAG_PROPNAME);
                property.setPropName(name);
            } else if (SyncML.TAG_MAXSIZE.equals(tagName)) {
                long maxSize = parseSimpleLongTag(parser, SyncML.TAG_MAXSIZE);
            } else if (SyncML.TAG_VALENUM.equals(tagName)) {
                String val = parseSimpleStringTag(parser, SyncML.TAG_VALENUM);
                property.addValEnum(val);
            } else if (SyncML.TAG_MAXOCCUR.equals(tagName)) {
                long maxOccur = parseSimpleLongTag(parser, SyncML.TAG_MAXOCCUR);
                property.setMaxOccur((int)maxOccur);
            } else if (SyncML.TAG_PROPPARAM.equals(tagName)) {
                PropParam param = parsePropertyParam(parser);
                property.addPropParam(param);
            } else if (SyncML.TAG_DATATYPE.equals(tagName)) {
                String val = parseSimpleStringTag(parser, SyncML.TAG_DATATYPE);
                property.setDataType(val);
            } else if (SyncML.TAG_DISPLAYNAME.equals(tagName)) {
                String val = parseSimpleStringTag(parser, SyncML.TAG_DISPLAYNAME);
                property.setDisplayName(val);
            } else {
                String msg = "Error parsing PROPERTY tag. Skipping unexpected token: " + tagName;
                Log.error(TAG_LOG, msg);
                skipUnknownToken(parser, tagName);
            }
            nextSkipSpaces(parser);
        }
        require(parser, parser.END_TAG, null, SyncML.TAG_PROPERTY);
        return property;
    }

    private PropParam parsePropertyParam(XmlPullParser parser) throws XmlPullParserException,
                                                                      IOException,
                                                                      SyncMLParserException
    {
        verboseLog("parsePropertyParam");
        
        nextSkipSpaces(parser);
        PropParam propParam = new PropParam();

        while (parser.getEventType() == parser.START_TAG) {
            String tagName = parser.getName();
            if (SyncML.TAG_PARAMNAME.equals(tagName)) {
                String name = parseSimpleStringTag(parser, SyncML.TAG_PARAMNAME);
                propParam.setParamName(name);
            } else if (SyncML.TAG_DISPLAYNAME.equals(tagName)) {
                String displayName = parseSimpleStringTag(parser, SyncML.TAG_DISPLAYNAME);
                propParam.setDisplayName(displayName);
            } else if (SyncML.TAG_VALENUM.equals(tagName)) {
                String val = parseSimpleStringTag(parser, SyncML.TAG_VALENUM);
                propParam.addValEnum(val);
            } else if (SyncML.TAG_DATATYPE.equals(tagName)) {
                String dataType = parseSimpleStringTag(parser, SyncML.TAG_DATATYPE);
                propParam.setDataType(dataType);
            } else {
                String msg = "Error parsing EXT tag. Skipping unexpected token: " + tagName;
                Log.error(TAG_LOG, msg);
                skipUnknownToken(parser, tagName);
            }
            nextSkipSpaces(parser);
        }
        require(parser, parser.END_TAG, null, SyncML.TAG_PROPPARAM);
        return propParam;
    }


    private SyncType parseSyncType(XmlPullParser parser) throws XmlPullParserException,
                                                                IOException,
                                                                SyncMLParserException
    {
        verboseLog("parseSyncType");

        long type = parseSimpleLongTag(parser, SyncML.TAG_SYNCTYPE);
        SyncType syncType = new SyncType((int)type);
        return syncType;
    }

    private String parseSimpleStringTag(XmlPullParser parser, String tag) throws XmlPullParserException,
                                                                                 IOException,
                                                                                 SyncMLParserException
    {
        verboseLog("parseSimpleStringTag");

        String value = "";
        parser.next();
        if (parser.getEventType() == parser.TEXT) {
            value = parser.getText();
            // We expect text plain data. Since this text is not contained
            // in CDATA section, we must unescape it
            value = XmlUtil.unescapeXml(value);
            parser.next();
        } else if (parser.getEventType() == parser.CDSECT) {
            value = parser.getText();
            parser.next();
        } else if (parser.getEventType() == WbxmlParser.WAP_EXTENSION) {
            verboseLog("string value in WAP EXTENSION");
            // We support the OPAQUE as binary data
            if (parser instanceof WbxmlParser) {
                WbxmlParser wbxmlParser = (WbxmlParser)parser;
                int wapId = wbxmlParser.getWapCode();
                if (wapId == Wbxml.OPAQUE) {
                    byte binData[] = (byte[])wbxmlParser.getWapExtensionData();
                    // this cannot be a real binary value!
                    value = new String(binData);
                    verboseLog("binary value is:" + value);
                } else {
                    verboseLog("unknown wap extension: " + wapId);
                    throw new SyncMLParserException("Unknown wapId " + wapId);
                }
            }
        }

        require(parser, parser.END_TAG, null, tag);

        return value;
    }

    private long parseSimpleLongTag(XmlPullParser parser, String tag) throws XmlPullParserException,
                                                                             IOException,
                                                                             SyncMLParserException
    {
        verboseLog("parseSimpleLongTag");
        String value = parseSimpleStringTag(parser, tag);
        try {
            long l = Long.parseLong(value);
            return l;
        } catch (Exception e) {
            String msg = "Error while parsing long " + e.toString();
            SyncMLParserException pe = new SyncMLParserException(msg);
            throw pe;
        }
    }
 
    private Vector parseExt(XmlPullParser parser) throws XmlPullParserException,
                                                         IOException,
                                                         SyncMLParserException
    {
        verboseLog("parseExt");

        Vector exts = new Vector();

        nextSkipSpaces(parser);
        Ext ext = null;
        while (parser.getEventType() == parser.START_TAG) {
            String tagName = parser.getName();
            if (SyncML.TAG_XNAM.equals(tagName)) {
                String name = parseSimpleStringTag(parser, SyncML.TAG_XNAM);
                ext = new Ext();
                ext.setXNam(name);
                exts.addElement(ext);
            } else if (SyncML.TAG_XVAL.equals(tagName)) {
                String value = parseSimpleStringTag(parser, SyncML.TAG_XVAL);
                if (ext == null) {
                    String msg = "Error parsing EXT tag. Found value without name. Skipping it"
                                  + tagName;
                    Log.error(TAG_LOG, msg);
                } else {
                    ext.addXVal(value);
                }
            } else {
                String msg = "Error parsing EXT tag. Skipping unexpected token: " + tagName;
                Log.error(TAG_LOG, msg);
                skipUnknownToken(parser, tagName);
            }
            nextSkipSpaces(parser);
        }
        require(parser, parser.END_TAG, null, SyncML.TAG_EXT);
        return exts;
    }

    private void require(XmlPullParser parser, int type, String namespace,
                         String name) throws XmlPullParserException
    {
        if (type != parser.getEventType()
            || (namespace != null && !namespace.equals(parser.getNamespace()))
            || (name != null &&  !name.equals(parser.getName())))
        {
            StringBuffer desc = new StringBuffer();
            desc.append("Expected ").append(parser.TYPES[ type ]).append(parser.getPositionDescription())
                .append(" -- Found ").append(parser.TYPES[parser.getEventType()]);
            throw new XmlPullParserException(desc.toString());
        }
    }

    private void nextSkipSpaces(XmlPullParser parser) throws SyncMLParserException,
                                                             XmlPullParserException,
                                                             IOException {
        int eventType = parser.next();

        if (eventType == parser.TEXT) {
            if (!parser.isWhitespace()) {
                String t = parser.getText();

                if (t.length() > 0) {
                    Log.error(TAG_LOG, "Unexpected text: " + t);
                    throw new SyncMLParserException("Unexpected text: " + t);
                }
            }
            parser.next();
        }
    }

    private void skipUnknownToken(XmlPullParser parser, String tagName)
                                                   throws  SyncMLParserException,
                                                           XmlPullParserException,
                                                           IOException
    {
        /*
        // Skip this subtree
        parser.skipSubTree();
        // Now we are positioned on the end tag
        require(parser, parser.END_TAG, null, tagName);
        parser.next();
        */

        do {
            parser.next();
        } while (parser.getEventType() != parser.END_TAG || !tagName.equals(parser.getName()));
    }

    private void verboseLog(String msg) {
        if (verbose) {
            if (Log.isLoggable(Log.TRACE)) {
                Log.trace(TAG_LOG, msg);
            }
        }
    }
}


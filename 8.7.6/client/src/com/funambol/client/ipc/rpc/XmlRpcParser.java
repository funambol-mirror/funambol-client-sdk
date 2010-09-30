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

package com.funambol.client.ipc.rpc;

import java.io.IOException;
import java.io.InputStream;
import java.util.Vector;

import org.xmlpull.v1.XmlPullParserException;
import org.xmlpull.v1.XmlPullParser;
import org.kxml2.io.KXmlParser;

import com.funambol.util.Log;
import com.funambol.util.XmlUtil;
import com.funambol.util.StringUtil;

/**
 * This class implements an Xml Rpc parser. This parser reads an input stream
 * and parses it, creating an XmlRpcMessage, which is a model for Xml Rpc
 * messages (both method calls and responses).
 * The parser is implemented using an XmlPullParser (KXml2).
 */
class XmlRpcParser {

    public XmlRpcParser() {
    }

    /**
     * This method parses the given input stream according to the XML RPC
     * specification, and it returns a representation of the the same message.
     * If the input is invalid, or it cannot be read, then the method can throw
     * an exception to signal the failure
     *
     * @param is is the data input stream
     * @throws XmlRpcParserException if the input stream cannot be parsed or
     * read
     */
    public XmlRpcMessage parse(InputStream is) throws XmlRpcParserException {

        XmlPullParser parser = new KXmlParser();
        XmlRpcMessage xmlRpcMsg = new XmlRpcMessage();
        try {
            parser.setInput(is, "UTF-8");
            do {
                nextSkipSpaces(parser);
                if (parser.getEventType() != parser.END_DOCUMENT) {
                    require(parser, parser.START_TAG, null, null);
                    String tagName = parser.getName();

                    if (StringUtil.equalsIgnoreCase(tagName, XmlRpc.TAG_METHODCALL)) {
                        parseMethodCall(parser, xmlRpcMsg);
                    } else if (StringUtil.equalsIgnoreCase(tagName, XmlRpc.TAG_METHODRESPONSE)) {
                        parseMethodResponse(parser, xmlRpcMsg);
                    } else {
                        String msg = "Error parsing device info tag. Skipping unexpected token: " + tagName;
                        Log.error(msg);
                        skipUnknownToken(parser, tagName);
                    }
                }
            } while(parser.getEventType() != parser.END_DOCUMENT);
        } catch (Exception e) {
            Log.error("Error parsing XmlRpc message: " + e.toString());
            throw new XmlRpcParserException("Cannot parse xml rpc: " + e.toString());
        }
        return xmlRpcMsg;
    }

    private String parseSimpleStringTag(XmlPullParser parser, String tag) throws XmlPullParserException,
                                                                                 IOException,
                                                                                 XmlRpcParserException {

        String value = "";
        parser.next();
        if (parser.getEventType() == parser.TEXT) {
            value = parser.getText();
            parser.next();
        }
        require(parser, parser.END_TAG, null, tag);

        return value;
    }

    private void parseMethodCall(XmlPullParser parser, XmlRpcMessage xmlRpcMsg)
    throws XmlPullParserException,
           IOException,
           XmlRpcParserException
    {
        nextSkipSpaces(parser);
        xmlRpcMsg.setMethodCall(true);
        while (parser.getEventType() == parser.START_TAG) {
            String tagName = parser.getName();
            if (StringUtil.equalsIgnoreCase(tagName, XmlRpc.TAG_METHODNAME)) {
                String name = parseSimpleStringTag(parser, XmlRpc.TAG_METHODNAME);
                xmlRpcMsg.setMethodName(name);
            } else if (StringUtil.equalsIgnoreCase(tagName, XmlRpc.TAG_PARAMS)) {
                parseParams(parser, xmlRpcMsg);
            } else {
                String msg = "Error parsing " + XmlRpc.TAG_METHODCALL
                             + " tag. Skipping unexpected token: " + tagName;
                Log.error(msg);
                skipUnknownToken(parser, tagName);
            }
            nextSkipSpaces(parser);
        }
        require(parser, parser.END_TAG, null, XmlRpc.TAG_METHODCALL);
    }

    private void parseMethodResponse(XmlPullParser parser, XmlRpcMessage xmlRpcMsg)
    throws XmlRpcParserException,
           IOException,
           XmlPullParserException
    {
        nextSkipSpaces(parser);
        xmlRpcMsg.setMethodCall(false);
        while (parser.getEventType() == parser.START_TAG) {
            String tagName = parser.getName();
            if (StringUtil.equalsIgnoreCase(tagName, XmlRpc.TAG_PARAMS)) {
                parseParams(parser, xmlRpcMsg);
            } else if (StringUtil.equalsIgnoreCase(tagName, XmlRpc.TAG_FAULT)) {
                parseFault(parser, xmlRpcMsg);
            } else {
                String msg = "Error parsing " + XmlRpc.TAG_METHODRESPONSE
                             + " tag. Skipping unexpected token: " + tagName;
                Log.error(msg);
                skipUnknownToken(parser, tagName);
            }
            nextSkipSpaces(parser);
        }
        require(parser, parser.END_TAG, null, XmlRpc.TAG_METHODRESPONSE);
    }


    private void parseParams(XmlPullParser parser, XmlRpcMessage xmlRpcMsg)
    throws XmlPullParserException,
           IOException,
           XmlRpcParserException
    {
        nextSkipSpaces(parser);
        while (parser.getEventType() == parser.START_TAG) {
            String tagName = parser.getName();
            if (StringUtil.equalsIgnoreCase(tagName, XmlRpc.TAG_PARAM)) {
                RPCParam param = parseParam(parser);
                xmlRpcMsg.addParam(param);
            } else {
                String msg = "Error parsing " + XmlRpc.TAG_PARAMS
                             + " tag. Skipping unexpected token: " + tagName;
                Log.error(msg);
                skipUnknownToken(parser, tagName);
            }
            nextSkipSpaces(parser);
        }
        require(parser, parser.END_TAG, null, XmlRpc.TAG_PARAMS);
    }

    private void parseFault(XmlPullParser parser, XmlRpcMessage xmlRpcMsg)
    throws XmlPullParserException,
           IOException,
           XmlRpcParserException
    {
        nextSkipSpaces(parser);
        xmlRpcMsg.setFault(true);
        while (parser.getEventType() == parser.START_TAG) {
            String tagName = parser.getName();
            if (StringUtil.equalsIgnoreCase(tagName, XmlRpc.TAG_VALUE)) {
                RPCParam param = parseValue(parser);
                xmlRpcMsg.addParam(param);
            } else {
                String msg = "Error parsing " + XmlRpc.TAG_PARAMS
                             + " tag. Skipping unexpected token: " + tagName;
                Log.error(msg);
                skipUnknownToken(parser, tagName);
            }
            nextSkipSpaces(parser);
        }
        require(parser, parser.END_TAG, null, XmlRpc.TAG_FAULT);
    }


    private int parseParamType(XmlPullParser parser, String typeName) {
        int paramType = RPCParam.TYPE_STRING;

        if (StringUtil.equalsIgnoreCase(XmlRpc.TYPE_INT, typeName) ||
            StringUtil.equalsIgnoreCase(XmlRpc.TYPE_I4, typeName))
        {
            paramType = RPCParam.TYPE_INT;
        } else if (StringUtil.equalsIgnoreCase(XmlRpc.TYPE_BOOLEAN, typeName)) {
            paramType = RPCParam.TYPE_BOOLEAN;
        } else if (StringUtil.equalsIgnoreCase(XmlRpc.TYPE_STRING, typeName)) {
            paramType = RPCParam.TYPE_STRING;
        } else if (StringUtil.equalsIgnoreCase(XmlRpc.TYPE_DOUBLE, typeName)) {
            paramType = RPCParam.TYPE_DOUBLE;
        } else if (StringUtil.equalsIgnoreCase(XmlRpc.TYPE_BASE64, typeName)) {
            paramType = RPCParam.TYPE_BASE64;
        } else if (typeName.toUpperCase().startsWith(XmlRpc.TYPE_DATETIME)) {
            paramType = RPCParam.TYPE_DATETIME;
            // TODO we could grab the format if needed
        } else if (StringUtil.equalsIgnoreCase(XmlRpc.TYPE_ARRAY, typeName)) {
            paramType = RPCParam.TYPE_ARRAY;
        } else if (StringUtil.equalsIgnoreCase(XmlRpc.TYPE_STRING, typeName)) {
            paramType = RPCParam.TYPE_STRING;
        } else if (StringUtil.equalsIgnoreCase(XmlRpc.TYPE_STRUCT, typeName)) {
            paramType = RPCParam.TYPE_STRUCT;
        } else {
            Log.error("Unknown parameter type: " + typeName);
        }
        return paramType;
    }

    private RPCParam[] parseArray(XmlPullParser parser) throws XmlPullParserException,
                                                               IOException,
                                                               XmlRpcParserException
    {
        if (parser.getEventType() == parser.TEXT && parser.isWhitespace()) {
            parser.next();
        }

        require(parser, parser.START_TAG, null, XmlRpc.TAG_DATA);

        nextSkipSpaces(parser);
        Vector values = new Vector();

        while (parser.getEventType() == parser.START_TAG) {
            String tagName = parser.getName();
            if (StringUtil.equalsIgnoreCase(tagName, XmlRpc.TAG_VALUE)) {
                RPCParam param = parseValue(parser);
                values.addElement(param);
            } else {
                String msg = "Error parsing " + XmlRpc.TAG_DATA
                             + " tag. Skipping unexpected token: " + tagName;
                Log.error(msg);
                skipUnknownToken(parser, tagName);
            }
            nextSkipSpaces(parser);
        }

        require(parser, parser.END_TAG, null, XmlRpc.TAG_DATA);
        RPCParam res[] = new RPCParam[values.size()];
        for(int i=0;i<values.size();++i) {
            res[i] = (RPCParam)values.elementAt(i);
        }
        nextSkipSpaces(parser);
        require(parser, parser.END_TAG, null, XmlRpc.TYPE_ARRAY);
        return res;
    }

    private RPCParam[] parseStruct(XmlPullParser parser) throws XmlPullParserException,
                                                                IOException,
                                                                XmlRpcParserException
    {
        if (parser.getEventType() == parser.TEXT && parser.isWhitespace()) {
            parser.next();
        }

        Vector values = new Vector();
        Vector names  = new Vector();

        while (parser.getEventType() == parser.START_TAG) {

            require(parser, parser.START_TAG, null, XmlRpc.TAG_MEMBER);
            nextSkipSpaces(parser);

            String name = null;
            RPCParam param = null;
            do {
                String tagName = parser.getName();

                if (StringUtil.equalsIgnoreCase(tagName, XmlRpc.TAG_VALUE)) {
                    param = parseValue(parser);
                    values.addElement(param);
                } else if (StringUtil.equalsIgnoreCase(tagName, XmlRpc.TAG_NAME)) {
                    name = parseSimpleStringTag(parser, XmlRpc.TAG_NAME);
                } else {
                    String msg = "Error parsing " + XmlRpc.TAG_DATA
                        + " tag. Skipping unexpected token: " + tagName;
                    Log.error(msg);
                    skipUnknownToken(parser, tagName);
                }
                nextSkipSpaces(parser);
            } while(parser.getEventType() == parser.START_TAG);
            if (param != null && name != null) {
                param.setName(name);
            }
            require(parser, parser.END_TAG, null, XmlRpc.TAG_MEMBER);
            nextSkipSpaces(parser);
        }

        RPCParam res[] = new RPCParam[values.size()];
        for(int i=0;i<values.size();++i) {
            res[i] = (RPCParam)values.elementAt(i);
        }

        require(parser, parser.END_TAG, null, XmlRpc.TYPE_STRUCT);
        return res;
    }


    private RPCParam parseValue(XmlPullParser parser) throws XmlPullParserException,
                                                             IOException,
                                                             XmlRpcParserException
    {
        RPCParam param = null;
        // We expect the param type or the value here (default type is
        // string)
        parser.next();
        if (parser.getEventType() == parser.TEXT &&
            parser.isWhitespace()) {

            parser.next();
        }
        int paramType = RPCParam.TYPE_STRING;
        boolean typeTag = false;
        if (parser.getEventType() == parser.START_TAG) {
            String typeName = parser.getName();
            paramType = parseParamType(parser, typeName);
            typeTag = true;
            parser.next();
        }
        // Now consumes the value
        if (paramType == RPCParam.TYPE_ARRAY) {
            RPCParam[] values = parseArray(parser);
            param = new RPCParam();
            param.setArrayValue(values);
        } else if (paramType == RPCParam.TYPE_STRUCT) {
            RPCParam[] members = parseStruct(parser);
            param = new RPCParam();
            param.setStructValue(members);
        } else {
            // All scalar types are handled here
            String value = parser.getText();
            require(parser, parser.TEXT, null, null);
            if (typeTag) {
                // We expect the type closure tag
                parser.next();
                if (parser.getEventType() != parser.END_TAG) {
                    throw new XmlRpcParserException("Cannot find type closure tag");
                }
            }

            // Build the param with the scalar type
            param = new RPCParam();
            switch (paramType) {
                case RPCParam.TYPE_STRING:
                    // We need to unescape strings
                    value = XmlUtil.unescapeXml(value);
                    param.setStringValue(value);
                    break;
                case RPCParam.TYPE_BASE64:
                    param.setBase64Value(value);
                    break;
                case RPCParam.TYPE_DATETIME:
                    param.setDateTimeValue(value);
                    break;
                case RPCParam.TYPE_INT:
                    try {
                        int intValue = Integer.parseInt(value);
                        param.setIntValue(intValue);
                    } catch (Exception e) {
                        throw new XmlRpcParserException("Cannot parse int parameter " + value);
                    }
                    break;
                case RPCParam.TYPE_BOOLEAN:
                    if (value.equals("0") || StringUtil.equalsIgnoreCase(value, "false")) {
                        param.setBooleanValue(false);
                    } else {
                        param.setBooleanValue(true);
                    }
                    break;
                case RPCParam.TYPE_DOUBLE:
                default:
                    throw new XmlRpcParserException("Unknown parameter type" + paramType);
            }
        }
        nextSkipSpaces(parser);
        require(parser, parser.END_TAG, null, XmlRpc.TAG_VALUE);

        return param;
    }

    private RPCParam parseParam(XmlPullParser parser) throws XmlPullParserException,
                                                             IOException,
                                                             XmlRpcParserException
    {
        nextSkipSpaces(parser);
        RPCParam param = null;
        if (parser.getEventType() == parser.START_TAG) {
            String tagName = parser.getName();
            if (StringUtil.equalsIgnoreCase(tagName, XmlRpc.TAG_VALUE)) {
                param = parseValue(parser);
            } else {
                String msg = "Error parsing " + XmlRpc.TAG_PARAM
                             + " tag. Skipping unexpected token: " + tagName;
                Log.error(msg);
                skipUnknownToken(parser, tagName);
            }
            nextSkipSpaces(parser);
        }

        if (param == null) {
            throw new XmlRpcParserException("Parameter value cannot be parsed");
        }
        require(parser, parser.END_TAG, null, XmlRpc.TAG_PARAM);
        return param;
    }

    private void require(XmlPullParser parser, int type, String namespace,
                         String name) throws XmlPullParserException
    {
        if (type != parser.getEventType()
            || (namespace != null && !StringUtil.equalsIgnoreCase(namespace,parser.getNamespace()))
            || (name != null &&  !StringUtil.equalsIgnoreCase(name,parser.getName())))
        {
            throw new XmlPullParserException("expected "+ parser.TYPES[ type ]+
                                              parser.getPositionDescription());
        }
    }

    private void nextSkipSpaces(XmlPullParser parser) throws XmlRpcParserException,
                                                             XmlPullParserException,
                                                             IOException {
        int eventType = parser.next();
        if (eventType == parser.TEXT) {
            if (!parser.isWhitespace()) {
                Log.error("Unexpected text: " + parser.getText());
                throw new XmlRpcParserException("Unexpected text: " + parser.getText());
            }
            parser.next();
        }
    }

    private void skipUnknownToken(XmlPullParser parser, String tagName)
                                                   throws  XmlRpcParserException,
                                                           XmlPullParserException,
                                                           IOException
    {
        do {
            parser.next();
        } while (parser.getEventType() != parser.END_TAG && !tagName.equals(parser.getName()));
    }
}

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
import java.io.OutputStream;
import java.util.Vector;

import com.funambol.util.Log;
import com.funambol.util.XmlUtil;

/**
 * This class implements an Xml Rpc formatter. The purpose of this class is to
 * convert an XmlRpcMessage into a textual representation as stated by the
 * XmlRpc standard.
 */
class XmlRpcFormatter {

    public XmlRpcFormatter() {
    }

    /**
     * This method converts the given XmlRpcMessage into a textual
     * representation according to the Xml Rpc specification
     * @param msg the XmlRpcMessage
     * @param os is the output stream into which the value is written
     * @throws XmlRpcFormatterException if an error occurs during the
     * serialization (including IOError while writing into the stream)
     */
    public void format(XmlRpcMessage msg, OutputStream os) throws XmlRpcFormatterException {
        try {
            if (msg.isMethodCall()) {
                formatMethodCall(msg, os);
            } else {
                formatResponse(msg, os);
            }
        } catch (Exception e) {
            String emsg = "Cannot format XmlRpc Message " + e.toString();
            Log.error(emsg);
            throw new XmlRpcFormatterException(emsg);
        }
    }
    
    private void formatMethodCall(XmlRpcMessage msg, OutputStream os)
    throws XmlRpcFormatterException,
           IOException {

        StringBuffer callMsg = new StringBuffer();
        callMsg.append("<?xml version=\"1.0\"?>\n");
        callMsg.append(formatStartTag(XmlRpc.TAG_METHODCALL)).append("\n");
        String methodName = msg.getMethodName();
        callMsg.append(formatStartTag(XmlRpc.TAG_METHODNAME));
        callMsg.append(methodName);
        callMsg.append(formatEndTag(XmlRpc.TAG_METHODNAME)).append("\n");

        callMsg.append(formatParams(msg));

        callMsg.append(formatEndTag(XmlRpc.TAG_METHODCALL)).append("\n");
        println(os, callMsg.toString());
    }

    private String formatParams(XmlRpcMessage msg) throws XmlRpcFormatterException {

        StringBuffer value = new StringBuffer();
        Vector params = msg.getParams();
        if (params != null && params.size() > 0) {
            value.append(formatStartTag(XmlRpc.TAG_PARAMS)).append("\n");
            for(int i=0;i<params.size();++i) {
                value.append(formatStartTag(XmlRpc.TAG_PARAM)).append("\n");

                RPCParam param = (RPCParam) params.elementAt(i);
                value.append(formatParam(param));
                value.append(formatEndTag(XmlRpc.TAG_PARAM)).append("\n");
            }
            value.append(formatEndTag(XmlRpc.TAG_PARAMS)).append("\n");
        }
        return value.toString();
    }

    private String formatFault(XmlRpcMessage msg) throws XmlRpcFormatterException {

        StringBuffer value = new StringBuffer();
        value.append(formatStartTag(XmlRpc.TAG_FAULT)).append("\n");
        Vector params = msg.getParams();
        if (params.size() != 1) {
            throw new XmlRpcFormatterException("Fault response shall have one struct value");
        }
        RPCParam param = (RPCParam) params.elementAt(0);
        value.append(formatParam(param));
        value.append(formatEndTag(XmlRpc.TAG_FAULT)).append("\n");
        return value.toString();
    }

    private String formatMember(RPCParam param) throws XmlRpcFormatterException {
        StringBuffer value = new StringBuffer();
        value.append(formatStartTag(XmlRpc.TAG_MEMBER)).append("\n");
        value.append(formatStartTag(XmlRpc.TAG_NAME)).append(param.getName())
             .append(formatEndTag(XmlRpc.TAG_NAME)).append("\n");
        value.append(formatParam(param)).append("\n");
        value.append(formatEndTag(XmlRpc.TAG_MEMBER)).append("\n");
        return value.toString();
    }

    private String formatParam(RPCParam param) throws XmlRpcFormatterException {
        StringBuffer value = new StringBuffer();
        value.append(formatStartTag(XmlRpc.TAG_VALUE));
        int type = param.getType();
        switch (type) {
            case RPCParam.TYPE_INT:
                value.append(formatStartTag(XmlRpc.TYPE_INT));
                value.append(param.getIntValue());
                value.append(formatEndTag(XmlRpc.TYPE_INT));
                break;
            case RPCParam.TYPE_STRING:
                String stringValue = param.getStringValue();
                stringValue = XmlUtil.escapeXml(stringValue);
                value.append(stringValue);
                break;
            case RPCParam.TYPE_DATETIME:
                value.append(formatStartTag(XmlRpc.TYPE_DATETIME));
                value.append(param.getDateTimeValue());
                value.append(formatEndTag(XmlRpc.TYPE_DATETIME));
                break;
            case RPCParam.TYPE_BASE64:
                value.append(formatStartTag(XmlRpc.TYPE_BASE64));
                value.append(param.getBase64Value());
                value.append(formatEndTag(XmlRpc.TYPE_BASE64));
                break;
            case RPCParam.TYPE_ARRAY:
                value.append(formatStartTag(XmlRpc.TYPE_ARRAY));
                value.append(formatStartTag(XmlRpc.TAG_DATA));
                RPCParam values[] = param.getArrayValue();
                for(int i=0;i<values.length;++i) {
                    RPCParam p = values[i];
                    value.append(formatParam(p));
                }
                value.append(formatEndTag(XmlRpc.TAG_DATA));
                value.append(formatEndTag(XmlRpc.TYPE_ARRAY));
                break;
            case RPCParam.TYPE_BOOLEAN:
                value.append(formatStartTag(XmlRpc.TYPE_BOOLEAN));
                if (param.getBooleanValue()) {
                    value.append("1");
                } else {
                    value.append("0");
                }
                value.append(formatEndTag(XmlRpc.TYPE_BOOLEAN));
                break;
            case RPCParam.TYPE_STRUCT:
                value.append(formatStartTag(XmlRpc.TYPE_STRUCT));
                RPCParam members[] = param.getStructValue();
                for(int i=0;i<members.length;++i) {
                    RPCParam p = members[i];
                    value.append(formatMember(p));
                }
                value.append(formatEndTag(XmlRpc.TYPE_STRUCT));
                break;
            default:
                throw new XmlRpcFormatterException("Unsupported type: " + type);
        }
        value.append(formatEndTag(XmlRpc.TAG_VALUE));
        return value.toString();
    }

    private String formatStartTag(String tag) {
        StringBuffer value = new StringBuffer();
        value.append("<").append(tag).append(">");
        return value.toString();
    }

    private String formatEndTag(String tag) {
        StringBuffer value = new StringBuffer();
        value.append("</").append(tag).append(">");
        return value.toString();
    }

    private void println(OutputStream os, String msg) throws IOException {
        msg = msg + "\n";
        os.write(msg.getBytes());
    }

    private void formatResponse(XmlRpcMessage msg, OutputStream os)
    throws XmlRpcFormatterException,
           IOException
    {
        StringBuffer value = new StringBuffer();
        value.append("<?xml version=\"1.0\"?>").append("\n");
        value.append(formatStartTag(XmlRpc.TAG_METHODRESPONSE)).append("\n");

        if (msg.isFault()) {
            value.append(formatFault(msg));
        } else {
            value.append(formatParams(msg));
        }
        value.append(formatEndTag(XmlRpc.TAG_METHODRESPONSE)).append("\n");
        println(os, value.toString());
    }
}

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

/**
 * This class contains all the tags/types used in the XmlRpc specification.
 */
class XmlRpc {

    public static final String TAG_METHODCALL     = "methodCall";
    public static final String TAG_METHODRESPONSE = "methodResponse";
    public static final String TAG_METHODNAME     = "methodName";
    public static final String TAG_PARAMS         = "params";
    public static final String TAG_PARAM          = "param";
    public static final String TYPE_INT           = "int";
    public static final String TYPE_I4            = "i4";
    public static final String TYPE_BOOLEAN       = "boolean";
    public static final String TYPE_STRING        = "string";
    public static final String TYPE_DOUBLE        = "double";
    public static final String TYPE_BASE64        = "base64";
    public static final String TYPE_DATETIME      = "dateTime";
    public static final String TYPE_ARRAY         = "array";
    public static final String TYPE_STRUCT        = "struct";
    public static final String TAG_VALUE          = "value";
    public static final String TAG_DATA           = "data";
    public static final String TAG_MEMBER         = "member";
    public static final String TAG_NAME           = "name";
    public static final String TAG_FAULT          = "fault";
    public static final String TAG_FAULT_CODE     = "faultCode";
    public static final String TAG_FAULT_STRING   = "faultString";
}
 

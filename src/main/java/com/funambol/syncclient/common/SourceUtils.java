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

package com.funambol.syncclient.common;

import java.io.ByteArrayInputStream;
import java.io.InputStream;

import java.util.HashMap;
import java.util.Iterator;

import javax.xml.parsers.DocumentBuilderFactory;
import javax.xml.parsers.DocumentBuilder;

import org.w3c.dom.Document;
import org.w3c.dom.Element;
import org.w3c.dom.Node;
import org.w3c.dom.NodeList;

/**
 *
 * Provide methods
 * to convert XML to HashMap
 * and HashMap to XML
 *
 */
public class SourceUtils {

    //---------------------------------------------------------------- Constants

    public static final String ROOT_NAME       = "__root__name__"          ;

    public static final String TAG_XML_VERSION = "<?xml version=\"1.0\" encoding=\""
                                               + System.getProperty("file.encoding")
                                               + "\"?>" ;


   //---------------------------------------------------------- Private data

    //----------------------------------------------------------- Public methods

    /**
     * Make a HashMap of fieldName - fieldValue
     *
     * tagName   -> key
     * tagValue  -> value
     *
     * @param content
     * @return HashMap of fieldName - fieldValue
     * @throws Exception
     **/
     public static HashMap xmlToHashMap(String content) throws Exception {
         content = dropTagCData(content);
         return xmlToHashMap(new ByteArrayInputStream(content.getBytes()));
     }

    /**
     * Make a HashMap of fieldName - fieldValue
     *
     * tagName   -> key
     * tagValue  -> value
     *
     * @param is
     * @return HashMap of fieldName - fieldValue
     * @throws Exception
     **/
    public static HashMap xmlToHashMap (InputStream is)
    throws Exception {

        DocumentBuilderFactory  docBuilderFactory  = null ;
        DocumentBuilder         docBuilder         = null ;
        Document                docXml             = null ;
        NodeList                lstChildren        = null ;
        Element                 el                 = null ;
        Node                    node               = null ;

        HashMap                 fields             = null ;

        String                  value              = null ;
        String                  nodeValue          = null ;
        String                  rootName           = null ;

        try {

            docBuilderFactory = DocumentBuilderFactory.newInstance   (    ) ;
            docBuilder        = docBuilderFactory.newDocumentBuilder (    ) ;
            docXml            = docBuilder.parse                     ( is ) ;
            el                = docXml.getDocumentElement            (    ) ;
            rootName          = el.getTagName                        (    ) ;
            lstChildren       = el.getChildNodes                     (    ) ;

            fields = new HashMap();

            for (int i=0, l = lstChildren.getLength(); i <  l; i++) {

                node = lstChildren.item(i);

                if (node.getNodeType() == org.w3c.dom.Node.ELEMENT_NODE) {

                    if (node.getChildNodes().item(0) != null) {
                        nodeValue = node.getChildNodes().item(0).getNodeValue();
                    } else {
                        nodeValue = "";
                    }

                    fields.put(node.getNodeName(), nodeValue);

                }

            }

            // put the rootName in the hashMap with key ROOT_NAME that is a conventional name
            // for store the name of the root
            if (rootName != null)
                fields.put(ROOT_NAME, rootName);

        } catch (Exception e) {
            throw new Exception(e.toString());
        }

        return fields;

    }

     /**
     * Make xml from an HashMap.
     * key   -> tagName
     * value -> tagValue
     *
     * @param fields
     * @return String with the xml
     * @throws Exception
     **/

    public static String hashMapToXml (HashMap fields)
    throws Exception {

        String           fieldName          = null ;
        String           fieldValue         = null ;
        String           message            = "";
        String           rootName           = null ;

        Iterator         i                  = null ;
        int j = 0;

        i = fields.keySet().iterator();

        while(i.hasNext()) {

            fieldName  = (String) i.next()              ;
            fieldValue = (String) fields.get((String)fieldName) ;

            fieldValue = StringTools.escapeXml(fieldValue);

            if (fieldName.equals (ROOT_NAME)) {
                rootName = fieldValue;
            } else {
                message =  message    +
                           "<"        +
                           fieldName  +
                           ">"        +
                           fieldValue +
                           "</"       +
                           fieldName  +
                           ">"        ;
            }
        }

        message = "<"      +
                  rootName +
                  ">"      +
                  message  ;

        message = message  +
                  "</"     +
                  rootName +
                  ">"      ;

        return TAG_XML_VERSION + message;

    }


   //--------------------------------------------------------- Private methods

    /**
     * @param content
     * @return input string by drop CData tag
     **/
    private static String dropTagCData(String content) {

        String tmp       = null ;

        int    startData = 0    ;
        int    endData   = 0    ;

        if (content.indexOf("<![CDATA[") != - 1) {

            startData = content.indexOf("<![CDATA[") + "<![CDATA[".length();

            //
            // for server bug: server create end CDATA with ]]]]>
            //
            endData   = content.lastIndexOf("]]]]>");

            if (endData == -1) {
               endData   = content.lastIndexOf("]]>");
            }

            tmp = content.substring(startData, endData);

            return content.substring(startData, endData);

        } else {
            return content;
        }

    }

    /**
     * This method is used to replace the String "=\r\n" with the
     * String "\r\n ": This replace is necessary because the are devices that
     * doesn't sent the correct line delimiting for long line.
     *
     * @param content the input content
     * @return the input content with property handling long line
     */
    public static String handleLineDelimiting(String content) {
        return content.replaceAll("=\r\n","\r\n ");
    }
}

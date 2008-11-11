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

import java.util.StringTokenizer;

/**
 * Utility class that groups string manipulation functions.
 *
 *
 * @version $Id: StringTools.java,v 1.3 2007-12-22 18:09:17 nichele Exp $
 */
public class StringTools {

    /**
     * Splits a comma separated values string into an array of strings.
     *
     * @param s the comma separated values list - NOT NULL
     *
     * @return the elements in the list as an array
     */
    public static String[] split(String s) {
        StringTokenizer st = new StringTokenizer(s, ", ");

        String[] values = new String[st.countTokens()];

        for (int i=0; i<values.length; ++i) {
            values[i] = st.nextToken();
        }

        return values;
    }

    /**
     * Joins the given Strin[] in a comma separated String
     *
     * @param array the String[] to join - NOT NULL
     *
     * @return a comma separated list as a single string
     */
    public static String join(String[] array) {
        StringBuffer sb = new StringBuffer();
        for (int i=0; (i<array.length); ++i) {
            if (i == 0) {
                sb.append(array[i]);
            } else {
                sb.append(',').append(array[i]);
            }
        }

        return sb.toString();
    }

    /**
     * Returns true if the given string is null or zero-length, false otherwise.
     *
     * @param s the string to check
     *
     * @return true if the given string is null or zero-length, false otherwise.
     */
    public static boolean isEmpty(String s) {
        return (s == null) || (s.length() == 0);
    }

    /**
     * Replaces special characters from the given string with an underscore ('_').
     *
     * @param s the string to replace.
     *
     * @return the replaced string.
     */
    public static String replaceSpecial(String s) {
        String ret = new String(s);

        char[] chars = s.toCharArray();
        for (int i=0; i < chars.length; ++i) {
            if ((chars[i] < '0' || chars[i]  > '9')  &&
                (chars[i]  < 'a' || chars[i]  > 'z') &&
                (chars[i]  < 'A' || chars[i]  > 'Z')) {
                chars[i] = '_';
            }
        }

        return new String(chars);
    }


    /**
     * <p>Escapes the characters in a <code>String</code> using XML entities.</p>
     *
     *
     * <p>Supports only the four basic XML entities (gt, lt, quot, amp).
     * Does not support DTDs or external entities.</p>
     *
     * @param str  the <code>String</code> to escape, may be null
     * @return a new escaped <code>String</code>, <code>null</code> if null string input
     * @see #unescapeXml(java.lang.String)
     **/
    public static String escapeXml(String str) {
        if (str == null) {
            return null;
        }
        return Entities.XML.escape(str);
    }

    /**
     * <p>Unescapes a string containing XML entity escapes to a string
     * containing the actual Unicode characters corresponding to the
     * escapes.</p>
     *
     * <p>Supports only the four basic XML entities (gt, lt, quot, amp).
     * Does not support DTDs or external entities.</p>
     *
     * @param str  the <code>String</code> to unescape, may be null
     * @return a new unescaped <code>String</code>, <code>null</code> if null string input
     * @see #escapeXml(String)
     **/
    public static String unescapeXml(String str) {
        if (str == null) {
            return null;
        }
        return Entities.XML.unescape(str);
    }


}

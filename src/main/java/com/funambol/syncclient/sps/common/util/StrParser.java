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

package com.funambol.syncclient.sps.common.util;


/**
 * This class implements string parser by parserDelimiter.
 *
  *
 * @version $Id: StrParser.java,v 1.3 2007-12-22 18:09:18 nichele Exp $
 **/
public class StrParser {

    // ------------------------------------------------------------ Private data

    private int position;

    private String str         = null;
    private String parserDelim = null;

    //------------------------------------------------------------- Constructors

    /**
     * Constructs a string parser for the specified string by parserDelimiter.
     *
     * @param   str            string to be parsed
     * @param   parserDelim    delimiter
     **/
    public StrParser(String str, String parserDelim) {

        position = 0;

        this.str = str;

        this.parserDelim = parserDelim;

    }

    //------------------------------------------------------------- Public methods

    /**
     * Verify if available more elements about this StrParser.
     *
     * @return  <code>true</code> if available more elements.
     *          <code>false</code> if not available more elements.
     **/

    public boolean hasMoreElements() {

        //
        // low performance
        //
        /**
        if ((position <= str.length()-1) && ( str.substring(position).indexOf(parserDelim) != -1

             || str.substring(position).length() > 0)) {

            return true;

        } else {

            return false;

        }

        */

        //
        //high performance
        //
        if (str.substring(position).indexOf(parserDelim) != -1

             || str.substring(position).length() > 0) {

            return true;

        } else {

            return false;

        }

    }


    /**
     * @return next element about this StrParser.
     */
    public String nextElement() {

        int positionOld;

        int positionTmp;

        positionOld = position;

        positionTmp = str.substring(position).indexOf(parserDelim);

        if (positionTmp != -1) {

            position = position + positionTmp + parserDelim.length();

            return str.substring(positionOld, position - parserDelim.length());

        } else {

            position = str.length();

            return str.substring(positionOld, position);

        }

    }


    /**
     * @return number of elements about this StrParser.
     */
    public int countElements() {

        int count = 0;
        int index = 0;

        String strTmp = null;

        strTmp = str;

        while (strTmp != null && strTmp.length() > 0) {

            count++;

            index = strTmp.indexOf(parserDelim);

            if (index == -1) {
                break;
            }

            strTmp = strTmp.substring(index + 1);

        }

        return count;

    }

}
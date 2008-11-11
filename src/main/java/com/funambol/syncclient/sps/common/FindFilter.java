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

package com.funambol.syncclient.sps.common;

import java.util.Vector;

import com.funambol.syncclient.sps.common.*;


/**
 * This interface implements methods
 * about search filter in database
 *
 *
 *
 * @version $Id: FindFilter.java,v 1.3 2007-12-22 18:09:17 nichele Exp $
 **/

public class FindFilter extends RecordFilter {


    // ------------------------------------------------------------ Private data

    private DataStore dataStore      =  null  ;
    private Vector    valueToFind    =  null  ;

    private boolean   include        =  false ;
    private boolean   caseSensitive  =  false ;

    //------------------------------------------------------------- Constructors

    /**
     * @param dataStore dataStore name
     * @param valueToFind vector contains fields value to find
     * @param include <code>true</code> valueToFind fields contains in record fields,
                      <code>false</code> valueToFind fields equals record fields

     * @param caseSensitive <code>true</code> case sensitive find,
     *                      <code>false</code> match case find
     *
     */

    public FindFilter (DataStore dataStore, Vector valueToFind, boolean include, boolean caseSensitive) {

        this.dataStore     = dataStore     ;
        this.valueToFind   = valueToFind   ;
        this.include       = include       ;
        this.caseSensitive = caseSensitive ;

    }

    //----------------------------------------------------------- Public methods

    /**
     * This method is about to check
     * is record about criteria in dataStore
     * @param record to check
     * @return <code>true</code>  if the record is accept about criteria
     *         <code>false</code> if the record is not accept about criteria
     */
    public boolean accept (Record record) {

        boolean test    = false ;
        String  valueR  = null  ;
        String  valueF  = null  ;

        for (int i=0, l = this.valueToFind.size(); i < l; i++) {

            valueR = record.getString(i);

            valueF = (String) this.valueToFind.elementAt(i);

            if(include && caseSensitive && valueF.length() > 0
                && (valueR.toUpperCase().indexOf(valueF.toUpperCase()) != -1)) {

                test = true;

                break;

            } else if(!include && caseSensitive && valueF.length() > 0
                && valueR.toUpperCase().equals(valueF.toUpperCase())) {

                test = true;

                break;

            } else if(!include && !caseSensitive && valueF.length() > 0
                && valueR.equals(valueF)) {

                test = true;

                break;

            } if(include && !caseSensitive && valueF.length() > 0
                && (valueR.indexOf(valueF) != -1)) {

                test = true;

                break;

            }

        }

        return test;

    }

}
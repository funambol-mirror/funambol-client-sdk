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

package com.funambol.syncclient.spds;

import java.util.Vector;

import com.funambol.syncclient.sps.common.*;


/**
 * This class provide methods about
 * control timestamp and modificationType
 * $Id: SyncRecordFunction.java,v 1.3 2007-12-22 18:09:17 nichele Exp $
 */
public class SyncRecordFunction {


    /**
     * set timestamp
     * @param record
     * @param timestamp
     **/
    public static void setTimestamp(Record record, String timestamp) {

        Vector values = null;
        values = new Vector();

        values.addElement(timestamp);

        record.setRecordField("timestamp", values);
    }



    /**
     * set modificationType
     * @param record
     * @param modificationType
     **/
    public static void setModificationType(Record record, String modificationType) {

        Vector values = null;
        values = new Vector();

        values.addElement(modificationType);

        record.setRecordField("mtype", values);
    }



    /**
     * @param record
     * @return timestamp
     **/
    public static String getTimestamp(Record record) {

        Vector values = null;

        values = record.getRecordField("timestamp");

        return (String) values.elementAt(0);
    }
    
    /**
     * @param record
     * @return modificationType
     **/
    public static String getModificationType(Record record) {

        Vector values = null;

        values = record.getRecordField("mtype");

        return (String) values.elementAt(0);

    }



    /**
     * @param record
     * @return timestamp field position, <p>-1</p> if timestamp field not present
     **/
    public static int getPositionTimestampField(Record record) {

        Vector positions = null;

        RecordMetadata recordMetadata = null;

        recordMetadata = record.getDataStore().getRecordMetadata();

        positions = recordMetadata.getRecordFieldPosition("timestamp");

        if (positions.size() > 0) {
            return Integer.parseInt(String.valueOf(positions.elementAt(0)));
        } else {
            return -1;
        }

    }



    /**
     * @param record
     * @return modificationType field position, <p>-1</p> if timestamp field not present
     **/
    public static int getPositionModificationTypeField(Record record) {

        Vector positions = null;

        RecordMetadata recordMetadata = null;

        recordMetadata = record.getDataStore().getRecordMetadata();

        positions = recordMetadata.getRecordFieldPosition("mtype");

        if (positions.size() > 0) {
            return Integer.parseInt(String.valueOf(positions.elementAt(0)));
        } else {
            return -1;
        }

    }


    /**
     * @param record
     * @return number of operation fields
     **/
    public static int getNumberOperationFields(Record record) {

        int numberOperationFields = 0;

        Vector positions = null;

        RecordMetadata recordMetadata = null;

        recordMetadata = record.getDataStore().getRecordMetadata();

        positions = recordMetadata.getRecordFieldPosition("key");

        if (positions.size()>0) {
            numberOperationFields = positions.size();
        }

        positions = recordMetadata.getRecordFieldPosition("timestamp");

        if (positions.size()>0) {
            numberOperationFields = numberOperationFields + positions.size();
        }

        positions = recordMetadata.getRecordFieldPosition("mtype");

        if (positions.size()>0) {
            numberOperationFields = numberOperationFields + positions.size();
        }

        return numberOperationFields;

    }
    
    /**
     * @param recordMetadata
     *
     * @return the key field position
     */
    public static int getKeyFieldPosition(RecordMetadata recordMetadata) {
        Vector positions = null;

        positions = recordMetadata.getRecordFieldPosition("key");

        if (positions.size() > 0) {
            return Integer.parseInt(String.valueOf(positions.elementAt(0)));
        } else {
            return -1;
        }
    }
    
     /**
     * @param recordMetadata
     *
     * @return the key field position
     */
    public static int getModificationTypeFieldPosition(RecordMetadata recordMetadata) {
        Vector positions = null;

        positions = recordMetadata.getRecordFieldPosition("mtype");

        if (positions.size() > 0) {
            return Integer.parseInt(String.valueOf(positions.elementAt(0)));
        } else {
            return -1;
        }
    }

}
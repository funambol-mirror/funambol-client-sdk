/*
 * Funambol is a mobile platform developed by Funambol, Inc. 
 * Copyright (C) 2011 Funambol, Inc.
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

package com.funambol.storage;

import android.content.Context;

import com.funambol.util.Log;

public class TableFactory {

    private static final String TAG_LOG = "TableFactory";

    private Context context;
    private String  dbName;

    private static TableFactory instance = null;

    public void init(Context context, String dbName) {
        this.context = context;
        this.dbName  = dbName;
    }

    public static TableFactory getInstance() {
        if (instance == null) {
            instance = new TableFactory();
        }
        return instance;
    }

    public Table getStringTable(String name, int colsType[], int keyIdx) {
        if (context == null || dbName == null) {
            Log.error(TAG_LOG, "Cannot create string table until the StringTableFactory is properly initialized");
            return null;
        }
        // On Android the default key value store in a SQL lite store
        return new SQLiteTable(context, dbName, name, colsType, keyIdx);
    }

    public Table getStringTable(String name, String colsName[], int colsType[], int keyIdx) {
        if (context == null || dbName == null) {
            Log.error(TAG_LOG, "Cannot create string table until the StringTableFactory is properly initialized");
            return null;
        }
        // On Android the default key value store in a SQL lite store
        return new SQLiteTable(context, dbName, name, colsName, colsType, keyIdx);
    }


    public Table getStringTable(String name, int colsType[], int keyIdx, boolean autoincrement) {
        if (context == null || dbName == null) {
            Log.error(TAG_LOG, "Cannot create string table until the StringTableFactory is properly initialized");
            return null;
        }
        // On Android the default key value store in a SQL lite store
        return new SQLiteTable(context, dbName, name, colsType, keyIdx, autoincrement);
    }

    public Table getStringTable(String name, String colsName[], int colsType[], int keyIdx, boolean autoincrement) {
        if (context == null || dbName == null) {
            Log.error(TAG_LOG, "Cannot create string table until the StringTableFactory is properly initialized");
            return null;
        }
        // On Android the default key value store in a SQL lite store
        return new SQLiteTable(context, dbName, name, colsName, colsType, keyIdx, autoincrement);
    }

}



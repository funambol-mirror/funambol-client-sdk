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

import java.util.Enumeration;
import java.util.Vector;
import java.io.IOException;

/**
 * This abstract class defines a generic data store for values orgnized
 * in a table.
 * The store is persistable and each concrete implementation is free to choose where and
 * how data is persisted.
 * Each table row is a tuple of fields, each one having its own type. Possible
 * types are:
 *
 * 1) STRING
 * 2) LONG
 *
 * Following some examples on how to use the table.
 *
 * Example 1
 *
 * In this example a table with 2 fruits is setup. Each fruit is described by
 * two values: its color and the number of days it lasts before rottening. The
 * key is the the fruit name and it is stored in the first field.
 *
 * String colsType = { Table.TYPE_STRING, Table.TYPE_STRING, Table.TYPE_LONG };
 * Table table = new Table("fruits", colsType, 0);
 * table.open(true);
 *
 * StringTuple bananRow = table.createNewRow("banana");
 * bananaRow.setField(1,"yellow");
 * bananaRow.setField(2,5); 
 * table.insert(bananaRow);
 *
 * StringTuple orangeRow = table.createNewRow("orange");
 * orangeRow.setField(1,"orange");
 * orangeRow.setField(2,3);
 * table.insert(orangeRow);
 *
 * table.save();
 * table.close();
 *
 *
 * Example 2
 *
 * Given the table created in example 1, we now query and print its content
 *
 * String colsType = { Table.TYPE_STRING, Table.TYPE_STRING, Table.TYPE_LONG };
 * StringTable table = new StringTable("fruits", colsType, 0);
 *
 * if (table.exists()) {
 *     table.open(false);
 *     QueryResult values = table.query();
 *     while(rows.hasMoreElements()) {
 *         Tuple value = (Tuple)rows.nextElement();
 *         String fruit = (String)value.getKey();
 *         String color = value.getStringField(1);
 *         int days  = (int)value.getLongField(2);
 *
 *         System.out.println("Fruit " + fruit + " is " + color + " and last " + days + " days.");
 *     }
 *     res.close();
 *     table.close();
 * } else {
 *     System.err.println("Fruits table not found");
 * }
 *
 * Example 3
 *
 * Given the table created in example 1, we now query searching for the banana fruit
 *
 * StringTable table = new StringTable("fruits");
 * String colsType = { Table.TYPE_STRING, Table.TYPE_STRING, Table.TYPE_LONG };
 *
 * if (table.exists()) {
 *     table.open(false);
 *     QueryFilter filter = table.createQueryFilter("banana");
 *     QueryResult res = table.query(filter);
 *     if (res.hasMoreElements()) {
 *         Tuple value = (Tuple)res.nextElement();
 *         String color = value.getStringField(1);
 *         int    days  = (int)value.getLongField(2);
 *         System.out.println("Banana is " + color + " and last " + days + " days.");
 *     }
 *     res.close();
 * } else {
 *     System.err.println("Fruits table not found");
 * }
 *
 * Example 4
 *
 * Given the table created in example 1, we now query searching for the fruits of a given color
 *
 * StringTable table = new StringTable("fruits");
 * if (table.exists()) {
 *     table.open(false);
 *     QueryFilter filter = table.createQueryFilter();
 *     filter.setValueFilter(1, QueryFilter.EQUAL, "yellow");
 *     QueryResult res = table.query(filter);
 *     while(res.hasMoreElements()) {
 *         Tuple value = (Tuple)res.nextElement();
 *         String key   = value.getKey();
 *         System.out.println("Found a yellow fruit " + key);
 *     }
 *     res.close();
 * } else {
 *     System.err.println("Fruits table not found");
 * }
 *
 */
public abstract class Table {

    private static final String TAG_LOG = "Table";

    public static final int TYPE_STRING  = 0;
    public static final int TYPE_LONG    = 1;

    private static final String KEY_COLUMN_NAME = "key";
    private static final String VALUE_COLUMN_NAME_PREFIX = "value";

    private String name;
    private int arity;
    private int keyIdx;
    private int colsType[];
    private String colsName[];
    protected boolean autoincrement;
    private Vector observers = new Vector();

    public Table(String name, int colsType[], int keyIdx, boolean autoincrement) {
        this.name = name;
        this.colsType = colsType;
        this.autoincrement = autoincrement;
        this.keyIdx = keyIdx;
        this.arity = colsType.length;
    }

    public Table(String name, String colsName[], int colsType[], int keyIdx, boolean autoincrement) {
        this(name, colsType, keyIdx, autoincrement);
        this.colsName = colsName;
    }

    public Table(String name, int colsType[], int keyIdx) {
        this(name, colsType, keyIdx, false);
    }

    public Tuple createNewRow(String key) {
        Tuple tuple = new Tuple(colsType, getKeyIdx());
        tuple.setField(getKeyIdx(), key);
        return tuple;
    }

    public Tuple createNewRow(Long key) {
        Tuple tuple = new Tuple(colsType, getKeyIdx());
        tuple.setField(getKeyIdx(), key);
        return tuple;
    }

    public Tuple createNewRow(long key) {
        Tuple tuple = new Tuple(colsType, getKeyIdx());
        tuple.setField(getKeyIdx(), new Long(key));
        return tuple;
    }

    public Tuple createNewRow() {
        Tuple tuple = new Tuple(colsType, getKeyIdx());
        return tuple;
    }

    public QueryFilter createQueryFilter() {
        QueryFilter filter = new QueryFilter();
        return filter;
    }

    public QueryFilter createQueryFilter(Object key) {
        QueryFilter filter = new QueryFilter(key);
        return filter;
    }

    public QueryFilter createQueryFilter(Vector keys) {
        QueryFilter filter = new QueryFilter(keys);
        return filter;
    }

    public String getName() {
        return name;
    }

    public int getArity() {
        return arity;
    }

    public int getColType(int idx) {
        if (idx >= arity) {
            throw new IllegalArgumentException("Invalid index");
        }
        return colsType[idx];
    }

    public String getColName(int i) {
        if (colsName == null) {
            if (i == keyIdx) {
                return KEY_COLUMN_NAME;
            }
            StringBuffer colName = new StringBuffer(VALUE_COLUMN_NAME_PREFIX).append("_").append(i);
            return colName.toString();
        } else {
            return colsName[i];
        }
    }

    public int[] getColsType() {
        return colsType;
    }

    public int getKeyIdx() {
        return keyIdx;
    }

    public int getColIndexOrThrow(String colName) {
        for(int i=0;i<arity;++i) {
            if (colName.equals(getColName(i))) {
                return i;
            }
        }
        throw new IllegalArgumentException("Cannot find column named " + colName);
    }


    /**
     * Open the table. Once a table is open, it is possible to perform
     * operations. Usually the first operation to be performed is "loading" the
     * table content.
     *
     * @throws IOException if the operation fails. For example if the table does
     * not exist and the user did not specify the createIfNotExist flag
     */
    public abstract void open() throws IOException;

    /**
     * Close the current table. This will release all the resources associated
     * to this resource and no other operation can be performed.
     */
    public abstract void close() throws IOException;

    /**
     * Insert a new item into the table. There is no guarantee the item is persisted
     * until the table is saved or closed.
     * null values in the tuple are NOT allowed.
     * The tuple cannot contain unchanged values as they do not make sense while
     * inserting new items.
     *
     * @param tuple the value for this item
     *
     * @throws IllegalArgumentException if at least one value is null, or the key is not unique
     * @throws IOException if the operation cannot be perfomed on the storage
     */
    public void insert(Tuple tuple) throws IOException {
        insertTuple(tuple);
        // Notify all the observers that a new tuple has been insterted
        for(int i=0;i<observers.size();++i) {
            TableObserver observer = (TableObserver)observers.elementAt(i);
            observer.tupleInserted(tuple);
        }
    }

    protected abstract void insertTuple(Tuple tuple) throws IOException;

    /**
     * Update an existing item into the table. There is no guarantee the item is persisted
     * until the table is saved or closed.
     * If a value needs to be left unchanged, then the corresponding value in
     * the tuple shall be "unchanged".
     * null values in the tuple are NOT allowed.
     *
     * @param key the unique key of the existing item
     * @param value the value to be stored
     * @throws IllegalArgumentException if at least one value is null or the value to be updated does not exist
     * @throws IOException if the operation cannot be perfomed on the storage
     */
    public void update(Tuple tuple) throws IOException {
        updateTuple(tuple);
        // Notify all the observers that a tuple has been updated
        for(int i=0;i<observers.size();++i) {
            TableObserver observer = (TableObserver)observers.elementAt(i);
            observer.tupleUpdated(tuple);
        }
    }

    protected abstract void updateTuple(Tuple tuple) throws IOException;

    /**
     * Removes an entry from the table
     *
     * @param key the item key
     * @throws IOException if the operation cannot be perfomed on the storage
     */
    public void delete(Object key) throws IOException {
        deleteTuple(key);
        // Notify all the observers that the table got reset
        for(int i=0;i<observers.size();++i) {
            TableObserver observer = (TableObserver)observers.elementAt(i);
            observer.tupleDeleted(key);
        }
    }

    protected abstract void deleteTuple(Object key) throws IOException;

    /**
     * Returns an enumeration with all the tuples in the store. The first column
     * in the returned tuples is always the key.
     *
     * The elements type is <code>StringTuple</code>.
     *
     * @throws IOException if the operation cannot be perfomed on the storage
     */
    public QueryResult query() throws IOException {
        return query(null, -1, false);
    }

    public QueryResult query(QueryFilter filter) throws IOException {
        return query(filter, -1, false);
    }

    public abstract QueryResult query(QueryFilter filter, int orderBy, boolean ascending) throws IOException;

    /**
     * Returns true iff key is contained in this store.
     */
    public boolean contains(Object key) throws IOException {
        QueryFilter filter = new QueryFilter(key);
        QueryResult res = query(filter);
        boolean r = res.hasMoreElements();
        res.close();
        return r;
    }

    /**
     * Persist this store.
     *
     * @throws IOException if the operation cannot be performed
     */
    public abstract void save() throws IOException;

    /**
     * Resets this data store. All data is lost after this call. The basic
     * implementation removes all rows, one by one. Concrete implementations
     * should override this implementation to be more efficient.
     *
     * @throws IOException if the operation fails
     */
    public void reset() throws IOException {
        resetTable();
        // Notify all the observers that the table got reset
        for(int i=0;i<observers.size();++i) {
            TableObserver observer = (TableObserver)observers.elementAt(i);
            observer.tableReset();
        }
    }

    public void registerObserver(TableObserver observer) {
        if (!observers.contains(observer)) {
            observers.addElement(observer);
        }
    }

    public void unregisterObserver(TableObserver observer) {
        if (observers.contains(observer)) {
            observers.removeElement(observer);
        }
    }

    protected void resetTable() throws IOException {
        QueryResult rows = query();
        try {
            while(rows.hasMoreElements()) {
                Tuple tuple = (Tuple)rows.nextElement();
                Object key = tuple.getKey();
                delete(key);
            }
        } finally {
            rows.close();
        }
    }

    public void drop() throws IOException {
        dropTable();
        // Notify all the observers that the table got reset
        for(int i=0;i<observers.size();++i) {
            TableObserver observer = (TableObserver)observers.elementAt(i);
            observer.tableDropped();
        }
    }

    protected abstract void dropTable() throws IOException;

}


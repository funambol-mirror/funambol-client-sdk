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

import junit.framework.TestCase;
import com.funambol.util.Platform;
import com.funambol.util.Log;

public class TableTest extends TestCase {

    TableFactory mTableFactory;
    
    public TableTest(String name) {
        super(name);
    }
    
    public void setUp() {
        mTableFactory = Platform.getInstance().getTableFactory();
    }
    
    public void tearDown() {
    }
    
    public void testBasic() throws Exception {

        int[] colsType = new int[] {
            Table.TYPE_STRING,
            Table.TYPE_STRING,
            Table.TYPE_LONG };

        Table table = mTableFactory.getStringTable("testBasic", colsType, 1);
        try {
            table.open();

            assertEquals(table.getName(), "testBasic");
            assertEquals(table.getArity(), 3);
            assertEquals(table.getColType(0), Table.TYPE_STRING);
            assertEquals(table.getColType(1), Table.TYPE_STRING);
            assertEquals(table.getColType(2), Table.TYPE_LONG);
            assertEquals(table.getColsType(), colsType);
            assertEquals(table.getColName(0), "value_0");
            assertEquals(table.getColName(1), "key");
            assertEquals(table.getColName(2), "value_2");
        } finally {
            table.close();
            table.drop();
        }
    }
    
    public void testOpenClose() throws Exception {
        Table table = mTableFactory.getStringTable("testOpenClose", new int[] {
            Table.TYPE_STRING, Table.TYPE_LONG }, 0);
        table.open();
        table.close();
        table.drop();
    }

    public void testLoadSave() throws Exception {
        Table table = mTableFactory.getStringTable("testLoadSave", new int[] {
            Table.TYPE_STRING, Table.TYPE_STRING,
            Table.TYPE_LONG, Table.TYPE_LONG }, 0);
        try {
            table.open();

            Tuple tuple = table.createNewRow("key");
            tuple.setField(1, "string value");
            tuple.setField(2, 1234567890);
            tuple.setField(3, 1234567891);
            table.insert(tuple);
            table.save();
            table.close();

            table.open();
            QueryResult res = table.query(table.createQueryFilter());
            Tuple read = res.nextElement();
            assertEquals(read.getArity(), 4);
            assertEquals(read.getStringField(1), "string value");
            assertEquals(read.getLongField(2).longValue(), 1234567890);
            assertEquals(read.getLongField(3).longValue(), 1234567891);
            res.close();
        } finally {
            table.close();
            table.drop();
        }
    }

    public void testInsert() throws Exception {
        Table table = mTableFactory.getStringTable("testInsert", new int[] {
            Table.TYPE_STRING, Table.TYPE_STRING,
            Table.TYPE_LONG}, 0);
        try {
            table.open();

            Tuple tuple = table.createNewRow("newRow");
            tuple.setField(1, "string value");
            tuple.setField(2, 1234567890);
            table.insert(tuple);

            QueryResult res = table.query(table.createQueryFilter());
            Tuple read = res.nextElement();
            assertEquals(read.getStringField(1), "string value");
            assertEquals(read.getLongField(2).longValue(), 1234567890);
            res.close();
        } finally {
            table.close();
            table.drop();
        }
    }

    public void testUpdate() throws Exception {
        Table table = mTableFactory.getStringTable("testUpdate", new int[] {
            Table.TYPE_STRING, Table.TYPE_STRING,
            Table.TYPE_LONG}, 0);
        try {
            table.open();

            Tuple tuple = table.createNewRow("key");
            tuple.setField(1, "value");
            tuple.setField(2, 1234);
            table.insert(tuple);
            table.save();

            tuple.setField(1, "new value");
            tuple.setField(2, 12345);
            table.update(tuple);
            table.save();

            QueryResult res = table.query(table.createQueryFilter());
            Tuple read = res.nextElement();
            assertEquals(read.getStringField(1), "new value");
            assertEquals(read.getLongField(2).longValue(), 12345);
            res.close();
        } finally {
            table.close();
            table.drop();
        }
    }

    public void testDelete() throws Exception {
        Table table = mTableFactory.getStringTable("testDelete", new int[] {
            Table.TYPE_STRING, Table.TYPE_STRING,
            Table.TYPE_LONG}, 0);
        try {
            table.open();

            Tuple tuple1 = table.createNewRow("key1");
            tuple1.setField(1, "value1");
            tuple1.setField(2, 12340001);

            Tuple tuple2 = table.createNewRow("key2");
            tuple2.setField(1, "value2");
            tuple2.setField(2, 12340002);

            Tuple tuple3 = table.createNewRow("key3");
            tuple3.setField(1, "value3");
            tuple3.setField(2, 12340003);

            table.insert(tuple1);
            table.insert(tuple2);
            table.insert(tuple3);
            table.save();

            table.delete("key2");
            table.save();

            QueryResult res = table.query();
            Tuple read1 = res.nextElement();
            Tuple read2 = res.nextElement();
            assertTrue(!res.hasMoreElements());

            assertTrue((read1.getStringField(1).equals("value1") && read1.getLongField(2).longValue() == 12340001 &&
                        read2.getStringField(1).equals("value3") && read2.getLongField(2).longValue() == 12340003)
                    ||
                    (read2.getStringField(1).equals("value1") && read2.getLongField(2).longValue() == 12340001 &&
                     read1.getStringField(1).equals("value3") && read1.getLongField(2).longValue() == 12340003));
            res.close();
        } finally {
            table.close();
            table.drop();
        }
    }

    public void testContains() throws Exception {
        Table table = mTableFactory.getStringTable("testContains", new int[] {
            Table.TYPE_STRING, Table.TYPE_STRING,
            Table.TYPE_LONG}, 0);
        try {
            table.open();

            Tuple tuple1 = table.createNewRow("key1");
            tuple1.setField(1, "value1");
            tuple1.setField(2, 12340001);

            Tuple tuple2 = table.createNewRow("key2");
            tuple2.setField(1, "value2");
            tuple2.setField(2, 12340002);

            Tuple tuple3 = table.createNewRow("key3");
            tuple3.setField(1, "value3");
            tuple3.setField(2, 12340003);

            table.insert(tuple1);
            table.insert(tuple2);
            table.insert(tuple3);
            table.save();

            assertTrue(table.contains("key1"));
            assertTrue(table.contains("key2"));
            assertTrue(table.contains("key3"));
            table.save();

            table.delete("key1");
            table.delete("key2");
            table.save();

            assertTrue(!table.contains("key1"));
            assertTrue(!table.contains("key2"));
            assertTrue(table.contains("key3"));
        } finally {
            table.close();
            table.drop();
        }
    }

    public void testReset() throws Exception {
        Table table = mTableFactory.getStringTable("testReset", new int[] {
            Table.TYPE_STRING, Table.TYPE_STRING,
            Table.TYPE_LONG}, 0);
        try {
            table.open();

            Tuple tuple1 = table.createNewRow("key1");
            tuple1.setField(1, "value1");
            tuple1.setField(2, 12340001);

            Tuple tuple2 = table.createNewRow("key2");
            tuple2.setField(1, "value2");
            tuple2.setField(2, 12340002);

            table.insert(tuple1);
            table.insert(tuple2);
            table.save();

            assertTrue(table.contains("key1"));
            assertTrue(table.contains("key2"));
            table.save();

            table.reset();
            table.save();

            assertTrue(!table.contains("key1"));
            assertTrue(!table.contains("key2"));
        } finally {
            table.close();
            table.drop();
        }
    }

    public void testEmptyQueryFilter() throws Exception {
        Table table = mTableFactory.getStringTable("testEmptyQueryFilter", new int[] {
            Table.TYPE_STRING, Table.TYPE_STRING}, 0);
        table.open();

        try {
            Tuple tuple1 = table.createNewRow("black");
            tuple1.setField(1, "black value");

            Tuple tuple2 = table.createNewRow("white");
            tuple2.setField(1, "white value");

            Tuple tuple3 = table.createNewRow("green");
            tuple3.setField(1, "green value");

            Tuple tuple4 = table.createNewRow("red");
            tuple4.setField(1, "red value");

            table.insert(tuple1);
            table.insert(tuple2);
            table.insert(tuple3);
            table.insert(tuple4);
            table.save();

            QueryResult res = table.query(table.createQueryFilter());
            res.nextElement();
            res.nextElement();
            res.nextElement();
            res.nextElement();
            assertTrue(!res.hasMoreElements());
            res.close();
        } finally {
            table.close();
            table.drop();
        }
    }

    public void testKeyQueryFilter() throws Exception {
        Table table = mTableFactory.getStringTable("testKeyQueryFilter", new int[] {
            Table.TYPE_STRING, Table.TYPE_STRING}, 0);
        try {
            table.open();

            Tuple tuple1 = table.createNewRow("black");
            tuple1.setField(1, "black value");

            Tuple tuple2 = table.createNewRow("white");
            tuple2.setField(1, "white value");

            Tuple tuple3 = table.createNewRow("green");
            tuple3.setField(1, "green value");

            Tuple tuple4 = table.createNewRow("red");
            tuple4.setField(1, "red value");

            table.insert(tuple1);
            table.insert(tuple2);
            table.insert(tuple3);
            table.insert(tuple4);
            table.save();

            QueryResult res = table.query(table.createQueryFilter("green"));
            Tuple read = res.nextElement();
            assertTrue(!res.hasMoreElements());

            assertEquals(read.getStringField(0), "green");
            assertEquals(read.getStringField(1), "green value");
            res.close();
        } finally {
            table.close();
            table.drop();
        }
    }

    public void testValueQueryFilter() throws Exception {
        Table table = mTableFactory.getStringTable("testValueQueryFilter", new int[] {
            Table.TYPE_STRING, Table.TYPE_STRING}, 0);
        try {
            table.open();

            Tuple tuple1 = table.createNewRow("black");
            tuple1.setField(1, "black value");

            Tuple tuple2 = table.createNewRow("white");
            tuple2.setField(1, "white value");

            Tuple tuple3 = table.createNewRow("green");
            tuple3.setField(1, "green value");

            Tuple tuple4 = table.createNewRow("red");
            tuple4.setField(1, "red value");

            table.insert(tuple1);
            table.insert(tuple2);
            table.insert(tuple3);
            table.insert(tuple4);
            table.save();

            QueryFilter filter = table.createQueryFilter();
            filter.setValueFilter(1, true, QueryFilter.EQUAL, "red value");

            QueryResult res = table.query(filter);

            assertTrue(res.hasMoreElements());

            Tuple read = res.nextElement();
            assertTrue(!res.hasMoreElements());

            assertEquals(read.getStringField(0), "red");
            assertEquals(read.getStringField(1), "red value");

            res.close();
        } finally {
            table.close();
            table.drop();
        }
    }

    public void testMultipleValueQueryFilter_OR() throws Exception {
        Table table = mTableFactory.getStringTable("testMultipleValueQueryFilter_OR", new int[] {
            Table.TYPE_STRING, Table.TYPE_STRING, Table.TYPE_STRING}, 0);
        try {
            table.open();

            Tuple tuple1 = table.createNewRow("black");
            tuple1.setField(1, "black value");
            tuple1.setField(2, "white value");

            Tuple tuple2 = table.createNewRow("white");
            tuple2.setField(1, "white value");
            tuple2.setField(2, "black value");

            Tuple tuple3 = table.createNewRow("green");
            tuple3.setField(1, "green value");
            tuple3.setField(2, "red value");

            Tuple tuple4 = table.createNewRow("red");
            tuple4.setField(1, "red value");
            tuple4.setField(2, "green value");

            table.insert(tuple1);
            table.insert(tuple2);
            table.insert(tuple3);
            table.insert(tuple4);
            table.save();

            QueryFilter filter = table.createQueryFilter();
            filter.setValueFilter(1, false, QueryFilter.EQUAL, "red value");
            filter.setValueFilter(2, false, QueryFilter.EQUAL, "white value");

            QueryResult res = table.query(filter);
            assertTrue(res.hasMoreElements());
            Tuple read1 = res.nextElement();
            assertTrue(res.hasMoreElements());
            Tuple read2 = res.nextElement();
            assertTrue(!res.hasMoreElements());

            assertTrue(read1.getStringField(0).equals("red") && read2.getStringField(0).equals("black") ||
                    read2.getStringField(0).equals("red") && read1.getStringField(0).equals("black"));
            res.close();
        } finally {
            table.close();
            table.drop();
        }
    }

    public void testMultipleValueQueryFilter_AND() throws Exception {
        Table table = mTableFactory.getStringTable("testMultipleValueQueryFilter_AND", new int[] {
            Table.TYPE_STRING, Table.TYPE_STRING,
            Table.TYPE_STRING}, 0);
        try {
            table.open();

            Tuple tuple1 = table.createNewRow("black");
            tuple1.setField(1, "black value1");
            tuple1.setField(2, "black value2");

            Tuple tuple2 = table.createNewRow("white");
            tuple2.setField(1, "white value1");
            tuple2.setField(2, "white value2");

            Tuple tuple3 = table.createNewRow("green");
            tuple3.setField(1, "green value1");
            tuple3.setField(2, "green value2");

            table.insert(tuple1);
            table.insert(tuple2);
            table.insert(tuple3);
            table.save();

            QueryFilter filter = table.createQueryFilter();
            filter.setValueFilter(1, true, QueryFilter.EQUAL, "white value1");
            filter.setValueFilter(2, true, QueryFilter.EQUAL, "white value2");

            QueryResult res = table.query(filter);
            assertTrue(res.hasMoreElements());
            Tuple read = res.nextElement();
            assertTrue(!res.hasMoreElements());

            assertEquals(read.getStringField(0), "white");
            assertEquals(read.getStringField(1), "white value1");
            assertEquals(read.getStringField(2), "white value2");

            res.close();
        } finally {
            table.close();
            table.drop();
        }
    }

    public void testOrderBy() throws Exception {
        Table table = mTableFactory.getStringTable("testOrderBy", new int[] {
            Table.TYPE_STRING, Table.TYPE_LONG}, 1);
        try {
            table.open();

            Tuple tuple1 = table.createNewRow(3000);
            tuple1.setField(0, "3000 value");

            Tuple tuple2 = table.createNewRow(1000);
            tuple2.setField(0, "1000 value");

            Tuple tuple3 = table.createNewRow(2000);
            tuple3.setField(0, "2000 value");

            table.insert(tuple1);
            table.insert(tuple2);
            table.insert(tuple3);
            table.save();

            QueryFilter filter = table.createQueryFilter();
            QueryResult res = table.query(filter, 1, true);
            assertEquals(res.nextElement().getStringField(0), "1000 value");
            assertEquals(res.nextElement().getStringField(0), "2000 value");
            assertEquals(res.nextElement().getStringField(0), "3000 value");
            assertTrue(!res.hasMoreElements());

            res.close();
        } finally {
            table.close();
            table.drop();
        }
    }

    public void testGetColIndex() throws Exception {
        Table table = mTableFactory.getStringTable("testGetColIndex",
                                                  new String[] {"description", "id" },
                                                  new int[] {Table.TYPE_STRING, Table.TYPE_LONG},
                                                  1);
        table.open();
        try {
            int idIdx = table.getColIndexOrThrow("id");
            int descIdx = table.getColIndexOrThrow("description");

            assertTrue(idIdx == 1);
            assertTrue(descIdx == 0);
        } finally {
            table.close();
            table.drop();
        }
    }

    public void testAutoincrement1() throws Exception {
        Table table = mTableFactory.getStringTable("testAutoincrement1",
                                                  new String[] {"description", "id" },
                                                  new int[] {Table.TYPE_STRING, Table.TYPE_LONG},
                                                  1,
                                                  true);

        try {
            table.open();
            int idIdx = table.getColIndexOrThrow("id");
            int descIdx = table.getColIndexOrThrow("description");

            Tuple newRow = table.createNewRow();
            newRow.setField(descIdx, "FirstRow");
            table.insert(newRow);

            Long genKey1 = (Long)newRow.getKey();
            assertTrue(genKey1.longValue() == 0);

            newRow = table.createNewRow();
            newRow.setField(descIdx, "SecondRow");
            table.insert(newRow);

            Long genKey2 = (Long)newRow.getKey();
            assertTrue(genKey2.longValue() == 1);
        } finally {
            table.close();
            table.drop();
        }
    }

    public void testAutoincrement2() throws Exception {
        Table table = mTableFactory.getStringTable("testAutoincrement2",
                                                  new String[] {"description", "id" },
                                                  new int[] {Table.TYPE_STRING, Table.TYPE_LONG},
                                                  1,
                                                  true);

        try {
            table.open();
            int idIdx = table.getColIndexOrThrow("id");
            int descIdx = table.getColIndexOrThrow("description");

            Tuple newRow = table.createNewRow();
            newRow.setField(descIdx, "FirstRow");
            table.insert(newRow);

            Long genKey1 = (Long)newRow.getKey();
            assertTrue(genKey1.longValue() == 0);
            table.save();

            table = mTableFactory.getStringTable("testAutoincrement2",
                    new String[] {"description", "id" },
                    new int[] {Table.TYPE_STRING, Table.TYPE_LONG},
                    1,
                    true);
            table.open();

            newRow = table.createNewRow();
            newRow.setField(descIdx, "SecondRow");
            table.insert(newRow);

            Long genKey2 = (Long)newRow.getKey();
            assertTrue(genKey2.longValue() == 1);
        } finally {
            table.close();
            table.drop();
        }
    }



}


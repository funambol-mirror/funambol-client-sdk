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
import java.util.Hashtable;
import java.util.Vector;
import java.io.IOException;
import java.io.OutputStream;
import java.io.InputStream;

import com.funambol.platform.FileAdapter;

import com.funambol.util.StringUtil;
import com.funambol.util.QuickSort;
import com.funambol.util.Log;

/**
 * This is an implementation of the key value store that stores items in a file.
 * There is no real journaling mechanism, but as long as new items are just
 * added we do not rewrite everything on save, but rather append the new items.
 * One limitation of this implementation is that both keys and values cannot
 * contain these sequences:
 *
 * 1) line feeds
 * 2) SEPARATOR (currently defined as =$^&amp;_~=)
 */
public class FileTable extends Table {

    private static final String TAG_LOG = "FileTable";
    
    protected Hashtable store;
    protected Hashtable pendingAdditions;

    // These are special characters that must be escaped because they are used
    // to provide a structure to the table
    private static final char LF = (char)10;
    private static final char SEPARATOR = ',';

    private static final char SPECIAL_CHARS[] = {LF, SEPARATOR};

    private boolean updated = false;
    private boolean appended = false;

    private boolean hiddenFile = false;

    private FileAdapter file;
    private String directory;

    private long nextKey = 0;
    private int numRefs = 0;

    private Object lock = new Object();

    public FileTable(String directory, String tableName, int colsType[], int keyIdx) {
        this(directory, tableName, null, colsType, keyIdx, false, false);
    }

    public FileTable(String directory, String tableName, String colsName[], int colsType[], int keyIdx) {
        this(directory, tableName, colsName, colsType, keyIdx, false, false);
    }

    public FileTable(String directory, String tableName, int colsType[], int keyIdx, boolean autoincrement) {
        this(directory, tableName, null, colsType, keyIdx, autoincrement, false);
    }

    public FileTable(String directory, String tableName, String colsName[], int colsType[], int keyIdx,
                     boolean autoincrement)
    {
        this(directory, tableName, colsName, colsType, keyIdx, autoincrement, false);
    }

    public FileTable(String directory, String tableName, String colsName[], int colsType[], int keyIdx,
                     boolean autoincrement, boolean hiddenFile)
    {
        super(tableName, colsName, colsType, keyIdx, autoincrement);

        if (autoincrement && colsType[keyIdx] != TYPE_LONG) {
            throw new IllegalArgumentException("Autoincrement key must have type long");
        }

        store = new Hashtable();
        pendingAdditions = new Hashtable();
        this.directory = directory;
        this.hiddenFile = hiddenFile;
    }

    public void open() throws IOException {
        synchronized(lock) {

            numRefs++;
            if (file != null) {
                return;
            }

            file = new FileAdapter(getName());
            if (!file.exists()) {
                file.create();
            } else {
                load();
            }
        }
    }

    public void close() throws IOException {
        if (file == null) {
            return;
        }
        synchronized(lock) {
            --numRefs;
            if (numRefs == 0) {
                file.close();
                file = null;
            }
        }
    }

    protected void insertTuple(Tuple tuple) throws IOException {
        Object key = tuple.getKey();
        synchronized(lock) {
            putInternal(key, tuple);
            pendingAdditions.put(key, tuple);
            appended = true;
            if (autoincrement) {
                tuple.setField(getKeyIdx(), new Long(nextKey++));
            }
        }
    }

    protected void updateTuple(Tuple tuple) throws IOException {
        Object key = tuple.getKey();
        synchronized(lock) {
            putInternal(key, tuple);
            pendingAdditions.put(key, tuple);
            updated = true;
        }
    }

    public boolean contains(String key) {
        return store.get(key) != null;
    }

    protected void deleteTuple(Object key) throws IOException {
        synchronized(lock) {
            if (store.remove(key) == null) {
                throw new IOException("Cannot delete item, not found " + key);
            } else {
                updated = true;
            }
        }
    }

    public void save() throws IOException {
        if (!appended && !updated) {
            return;
        }
        if (file == null) {
            throw new IOException("Table must be opened before saving it");
        }
        synchronized(lock) {
            boolean fileExists = false;
            OutputStream os  = null;
            // If there have been only additions, then we can append at the end of
            // the file. Otherwise we rewrite completely
            try {
                fileExists = file.exists();
                if (!updated) {
                    // Open in append mode
                    os = file.openOutputStream(true);
                    Enumeration keys = pendingAdditions.keys();
                    while(keys.hasMoreElements()) {
                        Object key = keys.nextElement();
                        Tuple tuple = (Tuple)pendingAdditions.get(key);
                        String newRow = encodeTuple(tuple);
                        os.write(newRow.getBytes("UTF-8"));
                        os.write((int)LF);
                    }
                } else {
                    // Rewrite all the values
                    os = file.openOutputStream();
                    Enumeration keys = store.keys();
                    while(keys.hasMoreElements()) {
                        Object key = keys.nextElement();
                        Tuple tuple = (Tuple)store.get(key);
                        String newRow = encodeTuple(tuple);
                        os.write(newRow.getBytes("UTF-8"));
                        os.write((int)LF);
                    }
                }
            } finally {
                if (os != null) {
                    try {
                        os.close();
                    } catch (IOException ioe) {
                    }
                }
                // Reset all the meta information
                pendingAdditions.clear();
                appended = false;
                updated  = false;
            }
            // Hide the file only the first time it is created
            if(hiddenFile && !fileExists) {
                hideFile(getName());
            }
        }
    }

    private void load() throws IOException {

        if (file == null) {
            throw new IOException("Table must be opened before saving it");
        }

        synchronized(lock) {
            InputStream is   = null;
            try {
                is = file.openInputStream();
                StringBuffer currentLine = new StringBuffer();
                for(long i=0, l=file.getSize(); i<l; ++i) {
                    char b = (char)is.read();
                    if (b == (char)LF) {
                        // This is the end of a line
                        String line = currentLine.toString().trim();
                        Tuple tuple = decodeTuple(line);
                        if (autoincrement) {
                            Long key = (Long)tuple.getKey();
                            if (key.longValue() >= nextKey) {
                                nextKey = key.longValue() + 1;
                            }
                        }
                        currentLine = new StringBuffer();
                    } else {
                        currentLine.append((char)b);
                    }
                }
            } finally {
                if (is != null) {
                    try {
                        is.close();
                    } catch (IOException ioe) {}
                }
            }
        }
    }

    public QueryResult query(QueryFilter filter, int orderBy, boolean ascending) throws IOException {
        Vector res = new Vector();

        synchronized(lock) {
            Enumeration keys = store.keys();
            while(keys.hasMoreElements()) {
                Object key = keys.nextElement();
                Tuple tuple = (Tuple)store.get(key);

                if (filter != null) {
                    if (filter.filterRow(tuple)) {
                        res.addElement(tuple);
                    }
                } else {
                    res.addElement(tuple);
                }
                tuple.setOrderingField(orderBy);
            }
        }

        if (orderBy != -1) {
            // Perform a quicksort
            QuickSort qs = new QuickSort();
            qs.quicksort(res, null, ascending);
        }
        return new VectorQueryResult(res);
    }

    protected void resetTable() throws IOException {

        if (file == null) {
            throw new IOException("Table must be opened before saving it");
        }

        synchronized(lock) {
            boolean fileExists = false;
            OutputStream os = null;
            try {
                fileExists = file.exists();
                // This truncate the file
                os = file.openOutputStream();
            } catch (Exception e) {
                throw new IOException(e.toString());
            } finally {
                if (os != null) {
                    try {
                        os.close();
                    } catch (IOException ioe) {
                    }
                }
            }
            // Hide the file only the first time it is created
            if(hiddenFile && !fileExists) {
                hideFile(getName());
            }
            store = new Hashtable();
        }
    }

    protected void dropTable() throws IOException {
        if (file != null) {
            throw new IOException("Table must be closed before dropping it");
        }
        synchronized(lock) {
            file = new FileAdapter(getName());
            file.delete();
        }
    }

    private String encodeTuple(Tuple tuple) {
        StringBuffer res = new StringBuffer();
        for(int i=0;i<tuple.getArity();++i) {
            String v;
            if (getColType(i) == TYPE_STRING) {
                v = encodeValue(tuple.getStringField(i));
            } else if (getColType(i) == TYPE_LONG) {
                v = "" + tuple.getLongField(i);
            } else {
                throw new IllegalStateException("Unknown field type " + getColType(i));
            }
            if (i > 0) {
                res.append(SEPARATOR);
            }
            res.append(v);
        }
        return res.toString();
    }

    private Tuple decodeTuple(String row) {
        Tuple tuple = new Tuple(getColsType(), getKeyIdx());
        String fields[] = StringUtil.split(row, "" + SEPARATOR);
        if (fields.length != getArity()) {
            throw new IllegalStateException("FileTable is not properly formatted");
        }

        for(int i=0;i<getArity();++i) {
            if (getColType(i) == TYPE_STRING) {
                String v = decodeValue(fields[i]);
                tuple.setField(i, v);
            } else if (getColType(i) == TYPE_LONG) {
                String v = decodeValue(fields[i]);
                tuple.setField(i, Long.parseLong(v));
            } else {
                throw new IllegalStateException("Unknown field type " + getColType(i));
            }
        }
        return tuple;
    }

    private String encodeValue(String value) {
        // We escape this way: any special char is preceeded by a \
        // and the slash is doubled
        // This is not very efficient, but this implementation is mostly unused
        value = StringUtil.replaceAll(value, "\\","\\\\");
        for(int i=0;i<SPECIAL_CHARS.length;++i) {
            char c = SPECIAL_CHARS[i];
            value = StringUtil.replaceAll(value, ""+c,"\\"+c);
        }
        return value;
    }

    private String decodeValue(String value) {
        // We escape this way: any special char is preceeded by a \
        // and the slash is doubled
        // This is not very efficient, but this implementation is mostly unused
        for(int i=0;i<SPECIAL_CHARS.length;++i) {
            char c = SPECIAL_CHARS[i];
            value = StringUtil.replaceAll(value, "\\"+c, ""+c);
        }
        value = StringUtil.replaceAll(value, "\\\\","\\");
        return value;
    }


    /**
     * Set the hidden attribute to true for the given filename. The hidden
     * attrobute is set only if the file already exists, since otherwise the
     * system would throw a IOException.
     *
     * @param fileName
     */
    private void hideFile(String fileName) {
        try {
            if(file.exists()) {
                boolean hidden = file.setHidden(true);
                if(!hidden) {
                    throw new RuntimeException("Cannot hide file");
                }
            }
        } catch (IOException ex) {
            Log.error(TAG_LOG, "Filed to hide file: " + fileName, ex);
        }
    }

    private Object putInternal(Object key, Tuple value) {
        return store.put(key, value);
    }

    public class VectorQueryResult implements QueryResult {

        private Vector items;
        private int idx = 0;

        public VectorQueryResult(Vector items) {
            this.items = items;
        }

        public boolean hasMoreElements() {
            return idx < items.size();
        }

        public Tuple nextElement() {
            return (idx < items.size()) ? (Tuple)items.elementAt(idx++) : null;
        }

        public int getCount() {
            return items.size();
        }

        public void close() {
            items = null;
        }
    }
}


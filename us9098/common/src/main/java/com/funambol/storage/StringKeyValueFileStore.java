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

package com.funambol.storage;

import java.util.Enumeration;
import java.util.Hashtable;
import java.util.Vector;
import java.io.IOException;
import java.io.OutputStream;
import java.io.InputStream;

import com.funambol.platform.FileAdapter;

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
public class StringKeyValueFileStore implements StringKeyValueStore {

    private static final String TAG_LOG = "StringKeyValueFileStore";
    
    protected Hashtable store;
    protected Vector    pendingAdditions;
    protected String    fileName;

    private static final int  LF = 10;
    // This is the value used to separate keys from values. Neither of them is
    // allowed to contain it.
    private static final String SEPARATOR = "=$^&_~=";

    // This is the old separator which was previously used. It is still
    // defined/used to handle backward compatibility
    private static final char OLD_SEPARATOR = '=';

    private boolean updated = false;
    private boolean appended = false;

    private boolean hiddenFile = false;

    public StringKeyValueFileStore(String fileName) {
        this(fileName, false);
    }

    public StringKeyValueFileStore(String fileName, boolean hiddenFile) {
        store = new Hashtable();
        pendingAdditions = new Vector();
        this.fileName = fileName;
        this.hiddenFile = hiddenFile;
    }

    public void add(String key, String value) {
        putInternal(key, value);
        pendingAdditions.addElement(key);
        appended = true;
    }

    public void update(String key, String value) {
        putInternal(key, value);
        updated = true;
    }

    public String put(String key, String value) {
        updated = true;
        return putInternal(key, value);
    }

    public String get(String key) {
        String res = (String)store.get(key);
        return res;
    }

    public Enumeration keys() {
        return store.keys();
    }

    public Enumeration keyValuePairs() {

        final Enumeration keys   = store.keys();
        final Enumeration values = store.elements();

        return new Enumeration () {

            boolean last = false;

            public Object nextElement() {

                String key   = (String)keys.nextElement();
                String value = (String)values.nextElement();

                return new StringKeyValuePair(key, value);
            }

            public boolean hasMoreElements() {
                return keys.hasMoreElements() && values.hasMoreElements();
            }
        };
    }

    public boolean contains(String key) {
        return store.get(key) != null;
    }

    public String remove(String key) {
        updated = true;
        return (String)store.remove(key);
    }

    public void save() throws IOException {
        if (!appended && !updated) {
            return;
        }
        boolean fileExists = false;
        FileAdapter file = null;
        OutputStream os  = null;
        // If there have been only additions, then we can append at the end of
        // the file. Otherwise we rewrite completely
        try {
            file = new FileAdapter(fileName);
            fileExists = file.exists();
            if (!updated) {
                // Open in append mode
                os = file.openOutputStream(true);
                Enumeration keys = pendingAdditions.elements();
                while(keys.hasMoreElements()) {
                    String key = (String)keys.nextElement();
                    String value = this.get(key);
                    os.write(key.getBytes("UTF-8"));
                    os.write(SEPARATOR.getBytes("UTF-8"));
                    os.write(value.getBytes("UTF-8"));
                    os.write((int)LF);
                }
            } else {
                // Rewrite all the values
                os = file.openOutputStream();
                Enumeration keys = store.keys();
                while(keys.hasMoreElements()) {
                    String key = (String)keys.nextElement();
                    String value = this.get(key);
                    os.write(key.getBytes("UTF-8"));
                    os.write(SEPARATOR.getBytes("UTF-8"));
                    os.write(value.getBytes("UTF-8"));
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
            if (file != null) {
                try {
                    file.close();
                } catch (IOException ioe) {
                }
            }
            // Reset all the meta information
            pendingAdditions.removeAllElements();
            appended = false;
            updated  = false;
        }
        // Hide the file only the first time it is created
        if(hiddenFile && !fileExists) {
            hideFile(fileName);
        }
    }

    public void load() throws IOException {

        FileAdapter file = null;
        InputStream is   = null;
        try {
            file = new FileAdapter(fileName);
            is = file.openInputStream();
            StringBuffer currentLine = new StringBuffer();
            for(long i=0, l=file.getSize(); i<l; ++i) {
                char b = (char)is.read();
                if (b == (char)LF) {
                    // This is the end of a line
                    String line = currentLine.toString().trim();
                    int pos = line.indexOf(SEPARATOR);
                    if (pos > 0) {
                        String key = line.substring(0, pos);
                        String value = line.substring(pos + SEPARATOR.length(), line.length());
                        this.put(key, value);
                    } else if (line.lastIndexOf(OLD_SEPARATOR) != -1) {
                        // This is a version with the old separator. We load and
                        // on writing we will migrate to the new one
                        pos = line.lastIndexOf(OLD_SEPARATOR);
                        String key = line.substring(0, pos);
                        String value = line.substring(pos + 1, line.length());
                        this.put(key, value);
                    } else {
                        throw new IOException("Malformed String Store file");
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
            if (file != null) {
                try {
                    file.close();
                } catch (IOException ioe) {}
            }
        }
    }

    public void reset() throws IOException {
        boolean fileExists = false;
        FileAdapter file = null;
        OutputStream os = null;
        try {
            file = new FileAdapter(fileName);
            fileExists = file.exists();
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
            if (file != null) {
                try {
                    file.close();
                } catch (IOException ioe) {
                }
            }
        }
        // Hide the file only the first time it is created
        if(hiddenFile && !fileExists) {
            hideFile(fileName);
        }
        store = new Hashtable();
    }

    /**
     * Set the hidden attribute to true for the given filename. The hidden
     * attrobute is set only if the file already exists, since otherwise the
     * system would throw a IOException.
     *
     * @param fileName
     */
    private void hideFile(String fileName) {
        FileAdapter file = null;
        try {
            file = new FileAdapter(fileName);
            if(file.exists()) {
                boolean hidden = file.setHidden(true);
                if(!hidden) {
                    throw new RuntimeException("Cannot hide file");
                }
            }
        } catch (IOException ex) {
            Log.error(TAG_LOG, "Filed to hide file: " + fileName, ex);
        } finally {
            if (file != null) {
                try {
                    file.close();
                } catch (IOException ioe) {
                }
            }
        }
    }

    private String putInternal(String key, String value) {
        return (String)store.put(key, value);
    }
}

